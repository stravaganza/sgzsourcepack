#include "Plugins.h"
#include "FXStencilMaskBitmap.h"


const std::string CFXStencilMaskBitmap::CLASSNAME("Mask Bitmap");

REGISTER_PLUGIN(CPluginFactory, CFXStencilMaskBitmap)


CFXStencilMaskBitmap::CFXStencilMaskBitmap()
{
	DefineVar("X Position", CVarFloat::CLASSNAME);
	DefineVar("Y Position", CVarFloat::CLASSNAME);
	DefineVar("Width",      CVarFloat::CLASSNAME);
	DefineVar("Height",     CVarFloat::CLASSNAME);
	DefineVar("Scale",      CVarFloat::CLASSNAME);
	DefineVar("Mask Bias",  CVarFloat::CLASSNAME);
	DefineVar("Angle",      CVarFloat::CLASSNAME);

	DefineResourceReference("Alpha Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("X Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Width" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Height"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Scale" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Mask Bias"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Angle" ))->SetConstant(0.0f);

	m_pResTexture = NULL;
	m_pBitmap     = NULL;
	m_pucBuffer   = NULL;
}

CFXStencilMaskBitmap::~CFXStencilMaskBitmap()
{
	Free();
}

void CFXStencilMaskBitmap::OnDemoFrameEnd(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	UtilGL::States::Set(UtilGL::States::STENCILBUFFER, UtilGL::States::DISABLED);
}

bool CFXStencilMaskBitmap::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Alpha Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find alpha texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	m_pBitmap = (CBitmap*)(((CResourceTexture2D*)m_pResTexture)->GetBitmap());
	assert(m_pBitmap);

	m_pucBuffer = new unsigned char[m_pBitmap->GetWidth() * m_pBitmap->GetHeight() * sizeof(unsigned char) * 4];

	return true;
}

bool CFXStencilMaskBitmap::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	SAFE_DELETE_A(m_pucBuffer);

	return true;
}

bool CFXStencilMaskBitmap::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("WARNING: Texture not available, effect will draw flat quad");
	}

	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueScale;
	CVarFloat::CValueFloat valueMaskBias;
	CVarFloat::CValueFloat valueAngle;

	EvaluateVar("X Position", fEffectTime, &valuePosX);
	EvaluateVar("Y Position", fEffectTime, &valuePosY);
	EvaluateVar("Width",      fEffectTime, &valueWidth);
	EvaluateVar("Height",     fEffectTime, &valueHeight);
	EvaluateVar("Scale",      fEffectTime, &valueScale);
	EvaluateVar("Mask Bias",  fEffectTime, &valueMaskBias);
	EvaluateVar("Angle",      fEffectTime, &valueAngle);

	pTexture->SetActive();

	GLint minFilter, magFilter;

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for(int i = 0; i < m_pBitmap->GetWidth() * m_pBitmap->GetHeight(); i++)
	{
		int nIndex = (i * 4);
		int nValue = (int)((unsigned char*)m_pBitmap->GetPixels())[nIndex] + (valueMaskBias.GetValue() * 255.0f);

		if(nValue > 255) nValue = 255;
		if(nValue < 0  ) nValue = 0;

		m_pucBuffer[nIndex + 0] = 255;
		m_pucBuffer[nIndex + 1] = 255;
		m_pucBuffer[nIndex + 2] = 255;
		m_pucBuffer[nIndex + 3] = nValue;
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_pucBuffer);

	CVarFloat* pVarAngle = reinterpret_cast<CVarFloat*>(GetVar("Angle"));
	assert(pVarAngle);

	CVector4 v4Color(1.0f, 1.0f, 1.0f, 1.0f);

	float fAngle = valueAngle.GetValue();

	if(IS_ZERO(fAngle) && !pVarAngle->IsConstant())
	{
		// Avoid aspect ratio problem (see UtilGL::Rendering::DrawRect())
		fAngle = fAngle < 0.0f ? fAngle - (ALMOST_ZERO * 2.0f) : fAngle + (ALMOST_ZERO * 2.0f);
	}

	UtilGL::States::Set (UtilGL::States::ALPHATEST, UtilGL::States::ENABLED);
	UtilGL::States::Lock(UtilGL::States::ALPHATEST, true);
	glAlphaFunc(GL_EQUAL, 0);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, TRUE);
	UtilGL::States::Set (UtilGL::States::STENCILBUFFER, UtilGL::States::ENABLED);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp  (GL_REPLACE, GL_REPLACE, GL_REPLACE);

	bool bResult = UtilGL::Rendering::DrawCenteredQuad(pTexture,
									valuePosX.GetValue(),  valuePosY.GetValue(),
									valueWidth.GetValue()  * valueScale.GetValue(),
									valueHeight.GetValue() * valueScale.GetValue(),
									fAngle, v4Color,
									GL_ONE, GL_ZERO);

	UtilGL::States::Lock(UtilGL::States::ALPHATEST,  false);
	UtilGL::States::Set (UtilGL::States::ALPHATEST,  UtilGL::States::DISABLED);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp  (GL_KEEP, GL_KEEP, GL_KEEP);

	// Restore filtering

	pTexture->SetActive();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	return bResult;
}
