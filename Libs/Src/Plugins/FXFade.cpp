#include "Plugins.h"
#include "FXFade.h"


const std::string CFXFade::CLASSNAME("Standard Fade");

REGISTER_PLUGIN(CPluginFactory, CFXFade)


CFXFade::CFXFade()
{
	DefineVar("Alpha", CVarFloat::CLASSNAME);
	DefineVar("Color", CVarColor::CLASSNAME);

	// Set the default values

	((CVarFloat*)GetVar("Alpha"))->SetConstant(0.5f);
	((CVarColor*)GetVar("Color"))->SetConstant(CVector3(0.0f, 0.0f, 0.0f));

}

CFXFade::~CFXFade()
{
	Free();
}

bool CFXFade::LoadData(CResourceList* pResourceList)
{
	return true;
}

bool CFXFade::Free()
{
	return true;
}

bool CFXFade::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	CVarFloat::CValueFloat valueAlpha;
	CVarColor::CValueColor valueColor;

	EvaluateVar("Alpha", fEffectTime, &valueAlpha);
	EvaluateVar("Color", fEffectTime, &valueColor);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	CVector4 v4Color = valueColor.GetValue();
	v4Color.SetW(fAlpha);

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	if(ARE_EQUAL(fAlpha, 1.0f))
	{
		return UtilGL::Rendering::DrawRect(NULL, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, v4Color, UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);
	}

	return UtilGL::Rendering::DrawRect(NULL, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, v4Color, UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);
}
