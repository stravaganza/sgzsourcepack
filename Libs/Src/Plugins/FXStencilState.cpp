#include "Plugins.h"
#include "FXStencilState.h"


const std::string CFXStencilState::CLASSNAME("Stencil State");

REGISTER_PLUGIN(CPluginFactory, CFXStencilState)


CFXStencilState::CFXStencilState()
{
	DefineVar("State", CVarCombo::CLASSNAME);

	((CVarCombo*)GetVar("State"))->AddOption("Off"); // default
	((CVarCombo*)GetVar("State"))->AddOption("On");
}

CFXStencilState::~CFXStencilState()
{
	Free();
}

bool CFXStencilState::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXStencilState::Free()
{
	return true;
}

bool CFXStencilState::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarCombo::CValueCombo valueState;
	EvaluateVar("State", fEffectTime, &valueState);

	if(valueState.GetValue() == "On")
	{
		UtilGL::States::Set(UtilGL::States::STENCILBUFFER, UtilGL::States::ENABLED);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::STENCILBUFFER, UtilGL::States::DISABLED);
	}

	return true;
}
