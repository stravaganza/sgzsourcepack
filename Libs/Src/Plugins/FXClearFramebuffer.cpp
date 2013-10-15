#include "Plugins.h"
#include "FXClearFramebuffer.h"


const std::string CFXClearFramebuffer::CLASSNAME("Clear Framebuffer");

REGISTER_PLUGIN(CPluginFactory, CFXClearFramebuffer)


CFXClearFramebuffer::CFXClearFramebuffer()
{
	DefineVar("Clear Color",    CVarCombo::CLASSNAME);
	DefineVar("Clear Z-Buffer", CVarCombo::CLASSNAME);
	DefineVar("Clear Stencil",  CVarCombo::CLASSNAME);
	DefineVar("Value Color",    CVarColor::CLASSNAME);
	DefineVar("Value Z",        CVarFloat::CLASSNAME);
	DefineVar("Value Stencil",  CVarInt::CLASSNAME);

	// Add combo options (first option becomes the default active one)

	CVarCombo* pVarCombo;

	pVarCombo = (CVarCombo*)GetVar("Clear Color");
	pVarCombo->AddOption("Yes");
	pVarCombo->AddOption("No");

	pVarCombo = (CVarCombo*)GetVar("Clear Z-Buffer");
	pVarCombo->AddOption("Yes");
	pVarCombo->AddOption("No");

	pVarCombo = (CVarCombo*)GetVar("Clear Stencil");
	pVarCombo->AddOption("Yes");
	pVarCombo->AddOption("No");

	// Set the rest of default values

	((CVarColor*)GetVar("Value Color"   ))->SetConstant(CVector3(0.0f, 0.0f, 0.0f));
	((CVarFloat*)GetVar("Value Z"       ))->SetConstant(1.0f);
	((CVarInt*)GetVar  ("Value Stencil" ))->SetConstant(0);
}

CFXClearFramebuffer::~CFXClearFramebuffer()
{
	Free();
}

bool CFXClearFramebuffer::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXClearFramebuffer::Free()
{
	return true;
}

bool CFXClearFramebuffer::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	GLbitfield flags = 0;

	CVarCombo::CValueCombo valueCombo;
	CVarColor::CValueColor valueColor;
	CVarFloat::CValueFloat valueZ;
	CVarInt::CValueInt     valueStencil;

	EvaluateVar("Clear Color", fEffectTime, &valueCombo);
	flags = valueCombo.GetValue() == "Yes" ? flags | GL_COLOR_BUFFER_BIT : flags;

	EvaluateVar("Clear Z-Buffer", fEffectTime, &valueCombo);
	flags = valueCombo.GetValue() == "Yes" ? flags | GL_DEPTH_BUFFER_BIT : flags;

	EvaluateVar("Clear Stencil", fEffectTime, &valueCombo);
	flags = valueCombo.GetValue() == "Yes" ? flags | GL_STENCIL_BUFFER_BIT : flags;

	EvaluateVar("Value Color",   fEffectTime, &valueColor);
	EvaluateVar("Value Z",       fEffectTime, &valueZ);
	EvaluateVar("Value Stencil", fEffectTime, &valueStencil);

	CVector4 v4Col = valueColor.GetValue();

	glClearColor(v4Col.X(), v4Col.Y(), v4Col.Z(), v4Col.W());
	glClearDepth(valueZ.GetValue());
	glClearStencil(valueStencil.GetValue());

	glClear(flags);

	return true;
}
