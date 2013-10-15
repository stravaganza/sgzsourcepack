#include "Plugins.h"
#include "FXMotionBlur.h"


const std::string CFXMotionBlur::CLASSNAME("Motion Blur");

REGISTER_PLUGIN(CPluginFactory, CFXMotionBlur)


CFXMotionBlur::CFXMotionBlur()
{
	DefineVar("Texture Width",   CVarInt::CLASSNAME);
	DefineVar("Texture Height",  CVarInt::CLASSNAME);
	DefineVar("Trail (0-1)",     CVarFloat::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("Texture Width" ))->LockConstant(true);
	((CVarInt*)GetVar("Texture Width" ))->SetConstant(512);
	((CVarInt*)GetVar("Texture Height"))->LockConstant(true);
	((CVarInt*)GetVar("Texture Height"))->SetConstant(512);

	((CVarFloat*)GetVar("Trail (0-1)"))->SetConstant(0.9f);

	GetVar("Texture Width" )->ReloadFXOnChange(this);
	GetVar("Texture Height")->ReloadFXOnChange(this);
}

CFXMotionBlur::~CFXMotionBlur()
{
	Free();
}

void CFXMotionBlur::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_texture.GetWidth(), m_texture.GetHeight(), pDemo->ScissorActive());
}

bool CFXMotionBlur::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("Texture Width",  0.0f, &valueInt);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2.0f));

	EvaluateVar("Texture Height", 0.0f, &valueInt);
	int nHeightPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2.0f));

	int nWidth  = pow(2, nWidthPwr);
	int nHeight = pow(2, nHeightPwr);

	// Create texture

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();
	texOptions.eFilter = UtilGL::Texturing::FILTER_LINEAR;

	m_texture.LoadFlat(nWidth, nHeight, CVector4(0.0f, 0.0f, 0.0f, 1.0f), false, false, &texOptions);

	return true;
}

bool CFXMotionBlur::Free()
{
	m_texture.Free();

	return true;
}

bool CFXMotionBlur::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Evaluate vars

	CVarFloat::CValueFloat valueTrail;
	EvaluateVar("Trail (0-1)", fEffectTime, &valueTrail);
	float fAlpha = valueTrail.GetValue();

	// Draw previous

	UtilGL::Rendering::DrawCenteredQuad(&m_texture,
										0.5f, 0.5f, 1.0f, 1.0f,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, fAlpha),
										UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);
	// Accumulate

	m_texture.SetActive();
	m_texture.CopyFromFramebuffer(0, 0, 0, 0, m_texture.GetWidth(), m_texture.GetHeight());

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return 	UtilGL::Rendering::DrawCenteredQuad(&m_texture,
										0.5f, 0.5f, 1.0f, 1.0f,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
										UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);
}
