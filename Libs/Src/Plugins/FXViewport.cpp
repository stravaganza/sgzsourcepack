#include "Plugins.h"
#include "FXViewport.h"


const std::string CFXViewport::CLASSNAME("Change Viewport");

REGISTER_PLUGIN(CPluginFactory, CFXViewport)


CFXViewport::CFXViewport()
{
	DefineVar("X Center", CVarFloat::CLASSNAME);
	DefineVar("Y Center", CVarFloat::CLASSNAME);
	DefineVar("Width",    CVarFloat::CLASSNAME);
	DefineVar("Height",   CVarFloat::CLASSNAME);
	DefineVar("Scissor",  CVarCombo::CLASSNAME);
	DefineVar("Aspect" ,  CVarFloat::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("X Center"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Center"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Width" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Height"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Aspect"))->SetConstant(1.33f);

	((CVarCombo*)GetVar("Scissor"))->AddOption("No"); // default
	((CVarCombo*)GetVar("Scissor"))->AddOption("Yes");
}

CFXViewport::~CFXViewport()
{
	Free();
}

bool CFXViewport::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXViewport::Free()
{
	return true;
}

bool CFXViewport::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarFloat::CValueFloat valueXCenter;
	CVarFloat::CValueFloat valueYCenter;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueAspect;
	CVarCombo::CValueCombo valueScissor;

	EvaluateVar("X Center",   fEffectTime, &valueXCenter);
	EvaluateVar("Y Center",   fEffectTime, &valueYCenter);
	EvaluateVar("Width",      fEffectTime, &valueWidth);
	EvaluateVar("Height",     fEffectTime, &valueHeight);
	EvaluateVar("Aspect",     fEffectTime, &valueAspect);
	EvaluateVar("Scissor",    fEffectTime, &valueScissor);

	// Change viewport

	float fX = valueXCenter.GetValue() - valueWidth.GetValue()  * 0.5f;
	float fY = valueYCenter.GetValue() - valueHeight.GetValue() * 0.5f;

	m_fAspect = pDemo->GetAspect();
	pDemo->SetAspect(valueAspect.GetValue());

	return pDemo->SetViewport(fX, fY, valueWidth.GetValue(), valueHeight.GetValue(), valueScissor.GetValue() == "Yes");
}
