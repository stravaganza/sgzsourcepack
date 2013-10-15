#include "Plugins.h"
#include "FXCrossFade.h"


const std::string CFXCrossFade::CLASSNAME("Cross Fade");

REGISTER_PLUGIN(CPluginFactory, CFXCrossFade)


CFXCrossFade::CFXCrossFade()
{
	DefineVar("Alpha",      CVarFloat::CLASSNAME);
	DefineVar("Blend Mode", CVarCombo::CLASSNAME);

	// Add combo options

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");

	// Set default alpha value.
	// The desired one would have been a linear change from 0.0 to 1.0 but since we don't
	// know the effect time range at this point yet, we can't do that.

	((CVarFloat*)GetVar("Alpha"))->SetConstant(0.5f);
}

CFXCrossFade::~CFXCrossFade()
{
	Free();
}

void CFXCrossFade::OnDemoFrameEnd(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	// Unlock blending states and restore alpha scale

	UtilGL::States::SetAlphaScale(m_fOldAlphaScale);

	UtilGL::States::Set (UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	UtilGL::States::Lock(UtilGL::States::BLENDING, false);
	UtilGL::States::Lock(UtilGL::States::SRCBLEND, false);
	UtilGL::States::Lock(UtilGL::States::DSTBLEND, false);
}

bool CFXCrossFade::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXCrossFade::Free()
{
	return true;
}

bool CFXCrossFade::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarFloat::CValueFloat valueAlpha;
	CVarCombo::CValueCombo valueBlendMode;

	// Evaluate vars

	EvaluateVar("Alpha",      fEffectTime, &valueAlpha);
	EvaluateVar("Blend Mode", fEffectTime, &valueBlendMode);

	float fAlpha = valueAlpha.GetValue();
	
	if(fAlpha > 1.0f) fAlpha = 1.0f;
	if(fAlpha < 0.0f) fAlpha = 0.0f;

	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	// Set alpha scale

	m_fOldAlphaScale = UtilGL::States::GetAlphaScale();
	UtilGL::States::SetAlphaScale(fAlpha);

	// Lock blending mode

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);
	
	UtilGL::States::Lock(UtilGL::States::BLENDING, true);
	UtilGL::States::Lock(UtilGL::States::SRCBLEND, true);
	UtilGL::States::Lock(UtilGL::States::DSTBLEND, true);

	// Clear z/stencil
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}
