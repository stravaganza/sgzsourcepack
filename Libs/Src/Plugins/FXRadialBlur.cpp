#include "Plugins.h"
#include "FXRadialBlur.h"


const std::string CFXRadialBlur::CLASSNAME("Radial Blur 1");

REGISTER_PLUGIN(CPluginFactory, CFXRadialBlur)


CFXRadialBlur::CFXRadialBlur()
{
	DefineVar("BlurTexture Width",   CVarInt::CLASSNAME);
	DefineVar("BlurTexture Height",  CVarInt::CLASSNAME);
	DefineVar("Steps",               CVarInt::CLASSNAME);
	DefineVar("Width",               CVarFloat::CLASSNAME);
	DefineVar("Height",              CVarFloat::CLASSNAME);
	DefineVar("X Origin",            CVarFloat::CLASSNAME);
	DefineVar("Y Origin",            CVarFloat::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("BlurTexture Width" ))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Width" ))->SetConstant(512);
	((CVarInt*)GetVar("BlurTexture Height"))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Height"))->SetConstant(512);

	((CVarInt*)GetVar("Steps"))->SetConstant(10);

	((CVarFloat*)GetVar("Width"   ))->SetConstant(1.2f);
	((CVarFloat*)GetVar("Height"  ))->SetConstant(1.2f);
	((CVarFloat*)GetVar("X Origin" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin" ))->SetConstant(0.5f);

	GetVar("BlurTexture Width" )->ReloadFXOnChange(this);
	GetVar("BlurTexture Height")->ReloadFXOnChange(this);
}

CFXRadialBlur::~CFXRadialBlur()
{
	Free();
}

void CFXRadialBlur::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_texture.GetWidth(), m_texture.GetHeight(), pDemo->ScissorActive());
}

bool CFXRadialBlur::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("BlurTexture Width",  0.0f, &valueInt);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2.0f));

	EvaluateVar("BlurTexture Height", 0.0f, &valueInt);
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

bool CFXRadialBlur::Free()
{
	m_texture.Free();

	return true;
}

bool CFXRadialBlur::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Framebuffer to texture

	m_texture.SetActive();
	m_texture.CopyFromFramebuffer(0, 0, 0, 0, m_texture.GetWidth(), m_texture.GetHeight());
	m_texture.SetActive();

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Evaluate vars

	CVarInt::CValueInt     valueSteps;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueOriginX;
	CVarFloat::CValueFloat valueOriginY;

	EvaluateVar("Steps",    fEffectTime, &valueSteps);
	EvaluateVar("Width",    fEffectTime, &valueWidth);
	EvaluateVar("Height",   fEffectTime, &valueHeight);
	EvaluateVar("X Origin", fEffectTime, &valueOriginX);
	EvaluateVar("Y Origin", fEffectTime, &valueOriginY);

	// Compute blur direction

	CVector3 v3Vector;
	v3Vector.Build(CVector3(valueOriginX.GetValue(), valueOriginY.GetValue(), 0.0f), CVector3(0.5f, 0.5f, 0.0f));

	v3Vector.SetX(v3Vector.X() * (valueWidth.GetValue()  - 1.0f));
	v3Vector.SetY(v3Vector.Y() * (valueHeight.GetValue() - 1.0f));

	// Draw quads

	int nSteps = valueSteps.GetValue();

	if(nSteps < 1) nSteps = 1;

	float fAlpha = 1.0f / nSteps;

	for(int i = 0; i < nSteps; i++)
	{
		float fT       = (float)i / (nSteps <= 1 ? 1 : nSteps - 1);
		float fWidth   = ComputeLERP(1.0f, valueWidth.GetValue(),  fT);
		float fHeight  = ComputeLERP(1.0f, valueHeight.GetValue(), fT);
		float fCenterX = 0.5f + (v3Vector * fT).X();
		float fCenterY = 0.5f + (v3Vector * fT).Y();

		UtilGL::Rendering::DrawCenteredQuad(&m_texture,
											fCenterX, fCenterY,
											fWidth, fHeight,
											0.0f, CVector4(1.0f, 1.0f, 1.0f, fAlpha),
											UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_ONE);
	}

	return true;
}
