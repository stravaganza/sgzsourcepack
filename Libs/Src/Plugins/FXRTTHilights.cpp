#include "Plugins.h"
#include "FXRTTHilights.h"


const std::string CFXRTTHilights::CLASSNAME("Render Glow Hilights");

REGISTER_PLUGIN(CPluginFactory, CFXRTTHilights)


CFXRTTHilights::CFXRTTHilights()
{
	DefineVar("Glow Radius (Pw2)", CVarInt::CLASSNAME);
	DefineVar("Intensity Cut",     CVarFloat::CLASSNAME);
	DefineVar("Post Multiply",     CVarInt::CLASSNAME);

	DefineResourceReference("Texture Target", CResourceTexture2D::CLASSNAME);

	((CVarInt*)  GetVar("Glow Radius (Pw2)" ))->SetConstant(4);
	((CVarFloat*)GetVar("Intensity Cut"     ))->SetConstant(0.8f);
	((CVarInt*)  GetVar("Post Multiply"     ))->SetConstant(4);

	m_pResTexture = NULL;
}

CFXRTTHilights::~CFXRTTHilights()
{
	Free();
}

void CFXRTTHilights::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture) return;

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(pTexture)
	{
		pDemo->SetViewport(0, 0, pTexture->GetWidth(), pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXRTTHilights::LoadData(CResourceList* pResourceList)
{
	m_pResTexture = FindResource(pResourceList, "Texture Target", CResourceTexture2D::CLASSNAME);

	if(!m_pResTexture)
	{
		FXLoadError("Can't find Texture Target resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXRTTHilights::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

bool CFXRTTHilights::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not available");
		pDemo->SetDemoViewport();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("No texture target available");
		pDemo->SetDemoViewport();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return false;
	}

	// Evaluate vars

	CVarInt::CValueInt     valueRadius;
	CVarFloat::CValueFloat valueIntensityCut;
	CVarInt::CValueInt     valuePostMultiply;

	EvaluateVar("Glow Radius (Pw2)", fEffectTime, &valueRadius);
	EvaluateVar("Intensity Cut",     fEffectTime, &valueIntensityCut);
	EvaluateVar("Post Multiply",     fEffectTime, &valuePostMultiply);

	// Substract

	float fSubstract = valueIntensityCut.GetValue();

	if(fSubstract > 1.0f) fSubstract = 1.0f;

	if(UtilGL::Extensions::GetAvailableExtensions()->EXT_blend_subtract && fSubstract > 0.0f)
	{
		glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);

		UtilGL::Rendering::DrawRect(NULL, 0.0f, 0.0f, 1.0f, 1.0f,
									0.0f, CVector4(1.0f, 1.0f, 1.0f, 0.0f) * fSubstract,
									UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ONE);

		glBlendEquationEXT(GL_FUNC_ADD_EXT);
	}

	// Add

	int nPasses = valuePostMultiply.GetValue() - 1;

	if(nPasses < 0)   nPasses = 0;
	if(nPasses > 100) nPasses = 100;

	if(nPasses > 0)
	{
		pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	}

	for(int i = 0; i < nPasses; i++)
	{
		UtilGL::Rendering::DrawCenteredQuad(pTexture,
											0.5f, 0.5f, 1.0f, 1.0f,
											0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
											UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ONE);
	}

	// Blur

	UtilGL::Rendering::ComputeBoxBlur(valueRadius.GetValue(), pTexture);

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	
	return true;
}
