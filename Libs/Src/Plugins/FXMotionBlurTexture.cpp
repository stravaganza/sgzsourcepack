#include "Plugins.h"
#include "FXMotionBlurTexture.h"


const std::string CFXMotionBlurTexture::CLASSNAME("Motion Blur Texture");

REGISTER_PLUGIN(CPluginFactory, CFXMotionBlurTexture)


CFXMotionBlurTexture::CFXMotionBlurTexture()
{
	DefineVar("Trail (0-1)", CVarFloat::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values

	((CVarFloat*)GetVar("Trail (0-1)"))->SetConstant(0.9f);

	m_pResTexture = NULL;
	m_pTexture    = NULL;
}

CFXMotionBlurTexture::~CFXMotionBlurTexture()
{
	Free();
}

void CFXMotionBlurTexture::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(m_pTexture)
	{
		pDemo->SetViewport(0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXMotionBlurTexture::LoadData(CResourceList* pResourceList)
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

bool CFXMotionBlurTexture::Free()
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

bool CFXMotionBlurTexture::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Evaluate vars

	CVarFloat::CValueFloat valueTrail;
	EvaluateVar("Trail (0-1)", fEffectTime, &valueTrail);
	float fAlpha = valueTrail.GetValue();

	// Draw previous

	UtilGL::Rendering::DrawCenteredQuad(m_pTexture,
										0.5f, 0.5f, 1.0f, 1.0f,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, fAlpha),
										UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);
	// Accumulate

	m_pTexture->SetActive();
	m_pTexture->CopyFromFramebuffer(0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return 	UtilGL::Rendering::DrawCenteredQuad(m_pTexture,
										0.5f, 0.5f, 1.0f, 1.0f,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
										UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);
}
