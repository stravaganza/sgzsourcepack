#include "Plugins.h"
#include "FXRecursiveBlurTexture.h"


const std::string CFXRecursiveBlurTexture::CLASSNAME("Recursive Blur Texture");

REGISTER_PLUGIN(CPluginFactory, CFXRecursiveBlurTexture)


CFXRecursiveBlurTexture::CFXRecursiveBlurTexture()
{
	DefineVar("Intensity",          CVarFloat::CLASSNAME);
	DefineVar("Width",              CVarFloat::CLASSNAME);
	DefineVar("Height",             CVarFloat::CLASSNAME);
	DefineVar("X Origin",           CVarFloat::CLASSNAME);
	DefineVar("Y Origin",           CVarFloat::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values

	((CVarFloat*)GetVar("Intensity" ))->SetConstant(0.90f);
	((CVarFloat*)GetVar("Width"     ))->SetConstant(1.02f);
	((CVarFloat*)GetVar("Height"    ))->SetConstant(1.02f);
	((CVarFloat*)GetVar("X Origin"  ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin"  ))->SetConstant(0.5f);

	m_pResTexture = NULL;
	m_pTexture    = NULL;
}

CFXRecursiveBlurTexture::~CFXRecursiveBlurTexture()
{
	Free();
}

void CFXRecursiveBlurTexture::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(m_pTexture)
	{
		pDemo->SetViewport(0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXRecursiveBlurTexture::LoadData(CResourceList* pResourceList)
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

bool CFXRecursiveBlurTexture::Free()
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

bool CFXRecursiveBlurTexture::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pTexture) return false;

	// Evaluate vars

	CVarFloat::CValueFloat valueIntensity;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueOriginX;
	CVarFloat::CValueFloat valueOriginY;

	EvaluateVar("Intensity",  fEffectTime, &valueIntensity);
	EvaluateVar("Width",      fEffectTime, &valueWidth);
	EvaluateVar("Height",     fEffectTime, &valueHeight);
	EvaluateVar("X Origin",   fEffectTime, &valueOriginX);
	EvaluateVar("Y Origin",   fEffectTime, &valueOriginY);

	// Compute blur direction

	CVector3 v3Vector;
	v3Vector.Build(CVector3(valueOriginX.GetValue(), valueOriginY.GetValue(), 0.0f), CVector3(0.5f, 0.5f, 0.0f));

	if(!IS_ZERO(v3Vector.X()) || !IS_ZERO(v3Vector.Y()))
	{
		v3Vector.Normalize();
	}

	// Accumulate previous blur

	float fWidth  = valueWidth.GetValue();
	float fHeight = valueHeight.GetValue();

	UtilGL::Rendering::DrawCenteredQuad(m_pTexture,
										0.5f + v3Vector.X() * ((fWidth  * 0.5f) - 0.5f),
										0.5f + v3Vector.Y() * ((fHeight * 0.5f) - 0.5f),
										fWidth, fHeight,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, valueIntensity.GetValue()),
										UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);

	// Store accumulation

	m_pTexture->CopyFromFramebuffer(0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}
