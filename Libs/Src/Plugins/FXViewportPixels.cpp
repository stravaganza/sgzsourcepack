#include "Plugins.h"
#include "FXViewportPixels.h"


const std::string CFXViewportPixels::CLASSNAME("Change Viewport (Pixels)");

REGISTER_PLUGIN(CPluginFactory, CFXViewportPixels)


CFXViewportPixels::CFXViewportPixels()
{
	DefineVar("VP Left",   CVarInt::CLASSNAME);
	DefineVar("VP Right",  CVarInt::CLASSNAME);
	DefineVar("VP Top",    CVarInt::CLASSNAME);
	DefineVar("VP Bottom", CVarInt::CLASSNAME);
	DefineVar("Scissor",   CVarCombo::CLASSNAME);
	DefineVar("Aspect" ,   CVarFloat::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarInt*)GetVar("VP Left"  ))->SetConstant(0);
	((CVarInt*)GetVar("VP Right" ))->SetConstant(512);
	((CVarInt*)GetVar("VP Top"   ))->SetConstant(0);
	((CVarInt*)GetVar("VP Bottom"))->SetConstant(512);
	((CVarFloat*)GetVar("Aspect" ))->SetConstant(1.33f);

	((CVarCombo*)GetVar("Scissor"))->AddOption("No"); // default
	((CVarCombo*)GetVar("Scissor"))->AddOption("Yes");
}

CFXViewportPixels::~CFXViewportPixels()
{
	Free();
}

bool CFXViewportPixels::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXViewportPixels::Free()
{
	return true;
}

bool CFXViewportPixels::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarInt::CValueInt valueLeft;
	CVarInt::CValueInt valueRight;
	CVarInt::CValueInt valueTop;
	CVarInt::CValueInt valueBottom;
	CVarFloat::CValueFloat valueAspect;
	CVarCombo::CValueCombo valueScissor;

	EvaluateVar("VP Left",    fEffectTime, &valueLeft);
	EvaluateVar("VP Right",   fEffectTime, &valueRight);
	EvaluateVar("VP Top",     fEffectTime, &valueTop);
	EvaluateVar("VP Bottom",  fEffectTime, &valueBottom);
	EvaluateVar("Aspect",     fEffectTime, &valueAspect);
	EvaluateVar("Scissor",    fEffectTime, &valueScissor);

	// Change viewport

	float fAspect = valueAspect.GetValue();

	if(fAspect < 0.0f || IS_ZERO(fAspect))
	{
		fAspect = pDemo->GetAspect();
	}

	m_fAspect = pDemo->GetAspect();
	pDemo->SetAspect(fAspect);

	return pDemo->SetViewport(	valueLeft.GetValue(), valueTop.GetValue(),
								valueRight.GetValue() - valueLeft.GetValue(), valueBottom.GetValue() - valueTop.GetValue(),
								valueScissor.GetValue() == "Yes");
}
