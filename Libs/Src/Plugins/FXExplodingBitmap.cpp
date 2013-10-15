#include "Plugins.h"
#include "FXExplodingBitmap.h"


const std::string CFXExplodingBitmap::CLASSNAME("Exploding Bitmap");

REGISTER_PLUGIN(CPluginFactory, CFXExplodingBitmap)


CFXExplodingBitmap::CFXExplodingBitmap()
{
	DefineVar("X Position", CVarFloat::CLASSNAME);
	DefineVar("Y Position", CVarFloat::CLASSNAME);
	DefineVar("Width",      CVarFloat::CLASSNAME);
	DefineVar("Height",     CVarFloat::CLASSNAME);
	DefineVar("Scale",      CVarFloat::CLASSNAME);
	DefineVar("Alpha",      CVarFloat::CLASSNAME);
	DefineVar("Angle",      CVarFloat::CLASSNAME);
	DefineVar("Blend Mode", CVarCombo::CLASSNAME);
	DefineVar("Filtering",  CVarCombo::CLASSNAME);
	DefineVar("Explosion State", CVarFloat::CLASSNAME);

	DefineResourceReference("Texture",          CResourceTexture2D::CLASSNAME);
	DefineResourceReference("Particle Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("X Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Width" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Height"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Scale" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Alpha" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Angle" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Explosion State"))->SetConstant(0.0f);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	m_pResTexture     = NULL;
	m_pResTexturePart = NULL;

	m_nNumParticles    = 0;
	m_pv3Positions     = NULL;
	m_pv3PositionsExpl = NULL;
	m_pv3Directions    = NULL;
	m_pv4Colors        = NULL;
	m_pfSizes          = NULL;
}

CFXExplodingBitmap::~CFXExplodingBitmap()
{
	Free();
}

bool CFXExplodingBitmap::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	if(!(m_pResTexturePart = this->FindResource(pResourceList, "Particle Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find particle texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	CBitmap* pBitmap = (CBitmap*)(((CResourceTexture2D*)m_pResTexture)->GetBitmap());
	assert(pBitmap);

	unsigned char* pcPixels = (unsigned char*)pBitmap->GetPixels();
	assert(pcPixels);

	m_nNumParticles    = pBitmap->GetWidth() * pBitmap->GetHeight();
	m_pv3Positions     = new CVector3[m_nNumParticles];
	m_pv3PositionsExpl = new CVector3[m_nNumParticles];
	m_pv3Directions    = new CVector3[m_nNumParticles];
	m_pv4Colors        = new CVector4[m_nNumParticles];
	m_pfSizes          = new float   [m_nNumParticles];

	float fSize = 1.0f / pBitmap->GetWidth();

	for(int y = 0; y < pBitmap->GetHeight(); y++)
	{
		for(int x = 0; x < pBitmap->GetWidth(); x++)
		{
			int nPixelIndex = (y * pBitmap->GetPitch()) + (x * (pBitmap->GetBpp() / 8));

			int a = pBitmap->GetBpp() == 32 ? pcPixels[nPixelIndex + 3] : 255;
			int r = pcPixels[nPixelIndex + 2];
			int g = pcPixels[nPixelIndex + 1];
			int b = pcPixels[nPixelIndex + 0];

			int nPointIndex = (y * pBitmap->GetWidth()) + x;

			m_pv3Positions[nPointIndex].SetX(ComputeLERP(-0.5f,  0.5f, x / (float)pBitmap->GetWidth()));
			m_pv3Positions[nPointIndex].SetY(ComputeLERP( 0.5f, -0.5f, y / (float)pBitmap->GetHeight())); // opengl v invert
			m_pv3Positions[nPointIndex].SetZ(-5.0f);
			m_pv4Colors   [nPointIndex].Set(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
			m_pfSizes     [nPointIndex] = fSize * 2.0f;

			m_pv3Directions[nPointIndex].SetX(ComputeRand(-1.0f, 1.0f));
			m_pv3Directions[nPointIndex].SetY(ComputeRand(-1.0f, 1.0f));
			m_pv3Directions[nPointIndex].SetZ(0.0f);

			CVector3 dir = m_pv3Positions[nPointIndex];
			dir.Normalize();

			m_pv3Directions[nPointIndex] = dir + m_pv3Directions[nPointIndex] * 0.3f;
		}
	}

	return true;
}

bool CFXExplodingBitmap::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	if(m_pResTexturePart)
	{
		m_pResTexturePart->RemoveDependency(this);
		m_pResTexturePart = NULL;
	}

	m_nNumParticles = 0;
	SAFE_DELETE_A(m_pv3Positions);
	SAFE_DELETE_A(m_pv3PositionsExpl);
	SAFE_DELETE_A(m_pv3Directions);
	SAFE_DELETE_A(m_pv4Colors);
	SAFE_DELETE_A(m_pfSizes);

	return true;
}

bool CFXExplodingBitmap::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	if(!m_pResTexturePart)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());
	UtilGL::Texturing::CTexture2D* pPartTex = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexturePart)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("WARNING: Texture not available, effect will draw flat quad");
	}

	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueScale;
	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valueAngle;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueFiltering;
	CVarFloat::CValueFloat valueExplosion;

	EvaluateVar("X Position", fEffectTime, &valuePosX);
	EvaluateVar("Y Position", fEffectTime, &valuePosY);
	EvaluateVar("Width",      fEffectTime, &valueWidth);
	EvaluateVar("Height",     fEffectTime, &valueHeight);
	EvaluateVar("Scale",      fEffectTime, &valueScale);
	EvaluateVar("Alpha",      fEffectTime, &valueAlpha);
	EvaluateVar("Angle",      fEffectTime, &valueAngle);
	EvaluateVar("Blend Mode", fEffectTime, &valueBlendMode);
	EvaluateVar("Filtering",  fEffectTime, &valueFiltering);
	EvaluateVar("Explosion State", fEffectTime, &valueExplosion);

	// Change filter mode

	pTexture->SetActive();

	GLint minFilter, magFilter;

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

	if(valueFiltering.GetValue() == "Yes")
	{
		// If it's point filtering, change it to linear, otherwise leave it as it is

		if(minFilter == GL_NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if(magFilter == GL_NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	CVarFloat* pVarAngle = reinterpret_cast<CVarFloat*>(GetVar("Angle"));
	assert(pVarAngle);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);

	float fAngle = valueAngle.GetValue();

	if(IS_ZERO(fAngle) && !pVarAngle->IsConstant())
	{
		// Avoid aspect ratio problem (see UtilGL::Rendering::DrawRect())
		fAngle = fAngle < 0.0f ? fAngle - (ALMOST_ZERO * 2.0f) : fAngle + (ALMOST_ZERO * 2.0f);
	}

	int nSrcBlend = UtilGL::States::BLEND_SRCALPHA;
	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !pTexture->HasAlpha())
	{
		nSrcBlend = UtilGL::States::BLEND_ONE;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}
	
	bool bResult = true;

	if(valueExplosion.GetValue() > 0.0f)
	{
		float fAspect = 1.0f;

		if(!IS_ZERO(fAngle))
		{
			fAspect = CWindowGL::GetActiveWindow()->GetAspect();
		}

		float fScaleX = valueWidth.GetValue()  * valueScale.GetValue();
		float fScaleY = valueHeight.GetValue() * valueScale.GetValue() * fAspect;

		for(int i = 0; i < m_nNumParticles; i++)
		{
			float fX = m_pv3Positions[i].X() * fScaleX;;
			float fY = m_pv3Positions[i].Y() * fScaleY;
			m_pv3PositionsExpl[i].Set(fX, fY, m_pv3Positions[i].Z());
			m_pv3PositionsExpl[i] = m_pv3PositionsExpl[i] + (m_pv3Directions[i] * valueExplosion.GetValue());
		}

		CMatrix mtxWorld;
		mtxWorld.RotateZ(fAngle);
		mtxWorld.Translate(valuePosX.GetValue(), valuePosY.GetValue(), 0.0f);

		UtilGL::Transforming::SetMatrix  (UtilGL::Transforming::MATRIX_WORLD, mtxWorld);
		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);
		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

		UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);

		UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND,  nSrcBlend);
		UtilGL::States::Set(UtilGL::States::DSTBLEND,  nDstBlend);

		glMatrixMode(GL_PROJECTION);
		glOrtho(0.0, 1.0, 1.0, 0.0, 1.0, 10.0);

		float fAlphaScale = UtilGL::States::GetAlphaScale();
		UtilGL::States::SetAlphaScale(valueAlpha.GetValue());

		UtilGL::Rendering::DrawBillboards(	m_nNumParticles,
											pPartTex,
											m_pv3PositionsExpl,
											m_pv4Colors,
											m_pfSizes);

		UtilGL::States::SetAlphaScale(fAlphaScale);


		UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::ENABLED);
	}
	else
	{
		bResult = UtilGL::Rendering::DrawCenteredQuad(	pTexture,
														valuePosX.GetValue(),  valuePosY.GetValue(),
														valueWidth.GetValue()  * valueScale.GetValue(),
														valueHeight.GetValue() * valueScale.GetValue(),
														fAngle, v4Color,
														nSrcBlend, nDstBlend);
	}

	// Restore filtering

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0f, 1.33, 1.0f, 1000.0f);

	return bResult;
}
