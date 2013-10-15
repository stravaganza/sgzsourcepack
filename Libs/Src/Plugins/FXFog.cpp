#include "Plugins.h"
#include "FXFog.h"


const std::string CFXFog::CLASSNAME("Standard Fog");

REGISTER_PLUGIN(CPluginFactory, CFXFog)


CFXFog::CFXFog()
{
	DefineVar("Color",			  CVarColor::CLASSNAME);
	DefineVar("Linear Near",	  CVarFloat::CLASSNAME);
	DefineVar("Linear Far",       CVarFloat::CLASSNAME);
	DefineVar("Exp Density",      CVarFloat::CLASSNAME);
	DefineVar("Fog Type",         CVarCombo::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Linear Near"   ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Linear Far"    ))->SetConstant(1000.0f);
	((CVarFloat*)GetVar("Exp Density"   ))->SetConstant(1.0f);
	((CVarColor*)GetVar("Color"         ))->SetConstant(CVector3(0.0f, 0.0f, 0.0f));

	((CVarCombo*)GetVar("Fog Type"))->AddOption("Linear"); // default
	((CVarCombo*)GetVar("Fog Type"))->AddOption("Exp");
	((CVarCombo*)GetVar("Fog Type"))->AddOption("Exp2");
	((CVarCombo*)GetVar("Fog Type"))->AddOption("Disabled");
}

CFXFog::~CFXFog()
{
	Free();
}

void CFXFog::OnLeave(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::DISABLED);	
	UtilGL::States::Lock(UtilGL::States::FOG, false);
}

void CFXFog::OnDemoFrameEnd(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::DISABLED);	
	UtilGL::States::Lock(UtilGL::States::FOG, false);
}

bool CFXFog::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXFog::Free()
{
	return true;
}

bool CFXFog::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarFloat::CValueFloat valueNear;
	CVarFloat::CValueFloat valueFar;
	CVarFloat::CValueFloat valueDensity;
	CVarCombo::CValueCombo valueType;
	CVarColor::CValueColor valueColor;

	EvaluateVar("Color",	   fEffectTime, &valueColor);
	EvaluateVar("Linear Near", fEffectTime, &valueNear);
	EvaluateVar("Linear Far",  fEffectTime, &valueFar);
	EvaluateVar("Exp Density", fEffectTime, &valueDensity);
	EvaluateVar("Fog Type",    fEffectTime, &valueType);

	float fNear = valueNear.GetValue();
	float fFar  = valueFar.GetValue();

	if(ARE_EQUAL(fNear, fFar) || fFar < fNear)
	{
		fFar = fNear + 0.001f;
	}

	if(valueType.GetValue() == "Linear")
	{
		glFogi(GL_FOG_MODE,  GL_LINEAR);
		glFogf(GL_FOG_START, fNear);
		glFogf(GL_FOG_END,   fFar);
	}
	else if(valueType.GetValue() == "Exp")
	{
		glFogi(GL_FOG_MODE,    GL_EXP);
		glFogf(GL_FOG_DENSITY, valueDensity.GetValue());
	}
	else if(valueType.GetValue() == "Exp2")
	{
		glFogi(GL_FOG_MODE,    GL_EXP2);
		glFogf(GL_FOG_DENSITY, valueDensity.GetValue());
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::DISABLED);
		return true;
	}

	glFogfv(GL_FOG_COLOR, valueColor.GetValue().Data());
	UtilGL::States::Set(UtilGL::States::FOG,  UtilGL::States::ENABLED);
	UtilGL::States::Lock(UtilGL::States::FOG, true);

	return true;
}

