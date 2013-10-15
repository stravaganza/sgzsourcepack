#include "Plugins.h"
#include "FXRadialBlur2Texture.h"


const std::string CFXRadialBlur2Texture::CLASSNAME("Radial Blur 2 Texture");

REGISTER_PLUGIN(CPluginFactory, CFXRadialBlur2Texture)


CFXRadialBlur2Texture::CFXRadialBlur2Texture()
{
	DefineVar("Blur Radius (Pw2)",   CVarInt::CLASSNAME);
	DefineVar("X Origin",            CVarFloat::CLASSNAME);
	DefineVar("Y Origin",            CVarFloat::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("Blur Radius (Pw2)"))->SetConstant(8);

	((CVarFloat*)GetVar("X Origin" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin" ))->SetConstant(0.5f);

	m_pResTexture = NULL;
	m_pTexture    = NULL;
}

CFXRadialBlur2Texture::~CFXRadialBlur2Texture()
{
	Free();
}

void CFXRadialBlur2Texture::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(m_pTexture)
	{
		pDemo->SetViewport(0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXRadialBlur2Texture::LoadData(CResourceList* pResourceList)
{
	assert(pResourceList);

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);
	m_pTexture = (UtilGL::Texturing::CTexture2D*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();

	return true;
}

bool CFXRadialBlur2Texture::Free()
{
	m_pTexture = NULL;

	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
		return false;
	}

	return true;
}

bool CFXRadialBlur2Texture::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pTexture) return false;

	// Evaluate vars

	CVarInt::CValueInt     valueRadius;
	CVarFloat::CValueFloat valueOriginX;
	CVarFloat::CValueFloat valueOriginY;

	EvaluateVar("Blur Radius (Pw2)", fEffectTime, &valueRadius);
	EvaluateVar("X Origin",			 fEffectTime, &valueOriginX);
	EvaluateVar("Y Origin",			 fEffectTime, &valueOriginY);

	if(valueRadius.GetValue() < 1) valueRadius.SetValue(1);

	int nRadiusPwr = MYROUND(log10f(valueRadius.GetValue()) / log10f(2));
	int nRadius    = pow(2, nRadiusPwr);

	if(nRadius < 1) nRadius = 1;

	// Compute blur direction

	CVector3 v3Vector;
	v3Vector.Build(CVector3(valueOriginX.GetValue(), valueOriginY.GetValue(), 0.0f), CVector3(0.5f, 0.5f, 0.0f));

	v3Vector = v3Vector * ((float)nRadius / (float)m_pTexture->GetWidth()) * 2.0f;

	// Set States

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_INVSRCALPHA);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), 0, 0.0f, 10.0f);

	m_pTexture->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Blur loop

	int nCurRadius = 1;

	while(nRadius >= nCurRadius)
	{
		float fPos     = (float)nCurRadius / (float)nRadius;
		float fCenterX = 0.5f + (v3Vector * fPos).X();
		float fCenterY = 0.5f + (v3Vector * fPos).Y();

		m_pTexture->CopyFromFramebuffer(0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());
		DrawQuad(fCenterX * m_pTexture->GetWidth(), fCenterY * m_pTexture->GetHeight(), nCurRadius, 0.5f);

		nCurRadius = nCurRadius * 2;
	}

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	m_pTexture->CopyFromFramebuffer(0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	
	return true;
}

bool CFXRadialBlur2Texture::DrawQuad(float fCenterX, float fCenterY, float fRadius, float fAlpha)
{
	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, fAlpha));

	// Compute some parameters

	float fHalfWidth  = m_pTexture->GetWidth()  / 2;
	float fHalfHeight = m_pTexture->GetHeight() / 2;

	float fLeft   = fCenterX - fHalfWidth  - fRadius;
	float fTop    = fCenterY - fHalfHeight - fRadius;
	float fRight  = fCenterX + fHalfWidth  + fRadius;
	float fBottom = fCenterY + fHalfHeight + fRadius;

	float fTexOffsetX = (1.0f / m_pTexture->GetWidth()) * 0.5f; // Texels are sampled from center
	float fTexOffsetY = (1.0f / m_pTexture->GetWidth()) * 0.5f;

	// Draw Quad

	glBegin(GL_QUADS);

	glTexCoord2f(fTexOffsetX, 1.0f - fTexOffsetY);
	glVertex3f  (fLeft, fTop, -1);

	glTexCoord2f(fTexOffsetX, fTexOffsetY);
	glVertex3f  (fLeft, fBottom, -1);

	glTexCoord2f(1.0f - fTexOffsetX, fTexOffsetY);
	glVertex3f  (fRight, fBottom, -1);

	glTexCoord2f(1.0f - fTexOffsetX, 1.0f - fTexOffsetY);
	glVertex3f  (fRight, fTop, -1);

	glEnd();

	return true;
}