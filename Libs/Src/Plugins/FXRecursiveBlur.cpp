#include "Plugins.h"
#include "FXRecursiveBlur.h"


const std::string CFXRecursiveBlur::CLASSNAME("Recursive Blur");

REGISTER_PLUGIN(CPluginFactory, CFXRecursiveBlur)


CFXRecursiveBlur::CFXRecursiveBlur()
{
	DefineVar("BlurTexture Width",  CVarInt::CLASSNAME);
	DefineVar("BlurTexture Height", CVarInt::CLASSNAME);
	DefineVar("Intensity",          CVarFloat::CLASSNAME);
	DefineVar("Width",              CVarFloat::CLASSNAME);
	DefineVar("Height",             CVarFloat::CLASSNAME);
	DefineVar("X Origin",           CVarFloat::CLASSNAME);
	DefineVar("Y Origin",           CVarFloat::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("BlurTexture Width" ))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Width" ))->SetConstant(256);
	((CVarInt*)GetVar("BlurTexture Height"))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Height"))->SetConstant(256);

	((CVarFloat*)GetVar("Intensity" ))->SetConstant(0.90f);
	((CVarFloat*)GetVar("Width"     ))->SetConstant(1.02f);
	((CVarFloat*)GetVar("Height"    ))->SetConstant(1.02f);
	((CVarFloat*)GetVar("X Origin"  ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin"  ))->SetConstant(0.5f);

	GetVar("BlurTexture Width" )->ReloadFXOnChange(this);
	GetVar("BlurTexture Height")->ReloadFXOnChange(this);
}

CFXRecursiveBlur::~CFXRecursiveBlur()
{
	Free();
}

void CFXRecursiveBlur::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_texture.GetWidth(), m_texture.GetHeight(), pDemo->ScissorActive());
}

bool CFXRecursiveBlur::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("BlurTexture Width",  0.0f, &valueInt);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	EvaluateVar("BlurTexture Height", 0.0f, &valueInt);
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

bool CFXRecursiveBlur::Free()
{
	m_texture.Free();

	return true;
}

bool CFXRecursiveBlur::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

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

	UtilGL::Rendering::DrawCenteredQuad(&m_texture,
										0.5f + v3Vector.X() * ((fWidth  * 0.5f) - 0.5f),
										0.5f + v3Vector.Y() * ((fHeight * 0.5f) - 0.5f),
										fWidth, fHeight,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, valueIntensity.GetValue()),
										UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);

	// Store accumulation

	m_texture.CopyFromFramebuffer(0, 0, 0, 0, m_texture.GetWidth(), m_texture.GetHeight());

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Draw blur

	UtilGL::Rendering::DrawCenteredQuad(&m_texture,
										0.5f, 0.5f, 1.0f, 1.0f,
										0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
										UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);
	
	return true;
}
