#include "Plugins.h"
#include "FXFadeInvert.h"


const std::string CFXFadeInvert::CLASSNAME("Invert Fade");

REGISTER_PLUGIN(CPluginFactory, CFXFadeInvert)


CFXFadeInvert::CFXFadeInvert()
{
	DefineVar("Alpha", CVarFloat::CLASSNAME);

	// Set the default values

	((CVarFloat*)GetVar("Alpha"))->SetConstant(1.0f);
}

CFXFadeInvert::~CFXFadeInvert()
{
	Free();
}

bool CFXFadeInvert::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXFadeInvert::Free()
{
	return true;
}

bool CFXFadeInvert::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarFloat::CValueFloat valueAlpha;

	EvaluateVar("Alpha", fEffectTime, &valueAlpha);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	CVector4 v4Color(fAlpha, fAlpha, fAlpha, 1.0f);

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	return UtilGL::Rendering::DrawRect(NULL, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, v4Color, UtilGL::States::BLEND_INVDSTCOLOR, UtilGL::States::BLEND_INVSRCCOLOR);
}
