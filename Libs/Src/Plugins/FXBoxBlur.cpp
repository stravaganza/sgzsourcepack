#include "Plugins.h"
#include "FXBoxBlur.h"


const std::string CFXBoxBlur::CLASSNAME("Box Blur");

REGISTER_PLUGIN(CPluginFactory, CFXBoxBlur)


CFXBoxBlur::CFXBoxBlur()
{
	DefineVar("BlurTexture Width",  CVarInt::CLASSNAME);
	DefineVar("BlurTexture Height", CVarInt::CLASSNAME);
	DefineVar("Blur Radius (Pw2)",  CVarInt::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("BlurTexture Width" ))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Width" ))->SetConstant(256);
	((CVarInt*)GetVar("BlurTexture Height"))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Height"))->SetConstant(256);
	((CVarInt*)GetVar("Blur Radius (Pw2)" ))->SetConstant(16);

	GetVar("BlurTexture Width" )->ReloadFXOnChange(this);
	GetVar("BlurTexture Height")->ReloadFXOnChange(this);
}

CFXBoxBlur::~CFXBoxBlur()
{
	Free();
}

void CFXBoxBlur::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_texture.GetWidth(), m_texture.GetHeight(), pDemo->ScissorActive());
}

bool CFXBoxBlur::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("BlurTexture Width",  0.0f, &valueInt);
	if(valueInt.GetValue() < 1) valueInt.SetValue(1);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	EvaluateVar("BlurTexture Height", 0.0f, &valueInt);
	if(valueInt.GetValue() < 1) valueInt.SetValue(1);
	int nHeightPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	int nWidth  = pow(2, nWidthPwr);
	int nHeight = pow(2, nHeightPwr);

	// Create texture

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();
	texOptions.eFilter = UtilGL::Texturing::FILTER_LINEAR;

	m_texture.LoadFlat(nWidth, nHeight, CVector4(0.0f, 0.0f, 0.0f, 1.0f), false, false, &texOptions);

	return true;
}

bool CFXBoxBlur::Free()
{
	m_texture.Free();

	return true;
}

bool CFXBoxBlur::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Evaluate vars

	CVarInt::CValueInt valueRadius;

	EvaluateVar("Blur Radius (Pw2)", fEffectTime, &valueRadius);

	// Blur

	UtilGL::Rendering::ComputeBoxBlur(valueRadius.GetValue(), &m_texture);

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Draw final blur

	UtilGL::Rendering::DrawCenteredQuad(&m_texture, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
										CVector4(1.0f, 1.0f, 1.0f, 1.0f),
										UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);
	
	return true;
}
