#include "Plugins.h"
#include "FXBitmap.h"


const std::string CFXBitmap::CLASSNAME("Standard Bitmap");

REGISTER_PLUGIN(CPluginFactory, CFXBitmap)


CFXBitmap::CFXBitmap()
{
	DefineVar("X Position", CVarFloat::CLASSNAME);
	DefineVar("Y Position", CVarFloat::CLASSNAME);
	DefineVar("Width",      CVarFloat::CLASSNAME);
	DefineVar("Height",     CVarFloat::CLASSNAME);
	DefineVar("Scale",      CVarFloat::CLASSNAME);
	DefineVar("Alpha",      CVarFloat::CLASSNAME);
	DefineVar("Angle",      CVarFloat::CLASSNAME);
	DefineVar("Blend Mode", CVarCombo::CLASSNAME);
	DefineVar("Filtering",  CVarCombo::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("X Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Width" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Height"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Scale" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Alpha" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Angle" ))->SetConstant(0.0f);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Color Mult");

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	m_pResTexture = NULL;
}

CFXBitmap::~CFXBitmap()
{
	Free();
}

bool CFXBitmap::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXBitmap::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

bool CFXBitmap::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("WARNING: Texture not available, effect will draw flat quad");
	}

	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueScale;
	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valueAngle;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueFiltering;

	EvaluateVar("X Position", fEffectTime, &valuePosX);
	EvaluateVar("Y Position", fEffectTime, &valuePosY);
	EvaluateVar("Width",      fEffectTime, &valueWidth);
	EvaluateVar("Height",     fEffectTime, &valueHeight);
	EvaluateVar("Scale",      fEffectTime, &valueScale);
	EvaluateVar("Alpha",      fEffectTime, &valueAlpha);
	EvaluateVar("Angle",      fEffectTime, &valueAngle);
	EvaluateVar("Blend Mode", fEffectTime, &valueBlendMode);
	EvaluateVar("Filtering",  fEffectTime, &valueFiltering);

	// Change filter mode

	GLint minFilter, magFilter;
	GLfloat fMaxAnisotropy;

	if(pTexture)
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		pTexture->SetActive();

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

		if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
		{
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		}

		if(valueFiltering.GetValue() == "Yes")
		{
			// If it's point filtering, change it to linear, otherwise leave it as it is

			if(minFilter == GL_NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			if(magFilter == GL_NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	CVarFloat* pVarAngle = reinterpret_cast<CVarFloat*>(GetVar("Angle"));
	assert(pVarAngle);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);

	float fAngle = valueAngle.GetValue();

	if(IS_ZERO(fAngle) && !pVarAngle->IsConstant())
	{
		// Avoid aspect ratio problem (see UtilGL::Rendering::DrawRect())
		fAngle = fAngle < 0.0f ? fAngle - (ALMOST_ZERO * 2.0f) : fAngle + (ALMOST_ZERO * 2.0f);
	}

	int nSrcBlend = UtilGL::States::BLEND_SRCALPHA;
	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;
	else if(valueBlendMode.GetValue() == "Color Mult")
	{
		nSrcBlend = UtilGL::States::BLEND_DSTCOLOR;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}

	bool bHasAlpha = false;

	if(pTexture)
	{
		if(pTexture->HasAlpha())
		{
			bHasAlpha = true;
		}
	}

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !bHasAlpha)
	{
		nSrcBlend = UtilGL::States::BLEND_ONE;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}
	
	bool bResult = UtilGL::Rendering::DrawCenteredQuad(pTexture,
									valuePosX.GetValue(),  valuePosY.GetValue(),
									valueWidth.GetValue()  * valueScale.GetValue(),
									valueHeight.GetValue() * valueScale.GetValue(),
									fAngle, v4Color,
									nSrcBlend, nDstBlend);
	// Restore filtering

	if(pTexture)
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		pTexture->SetActive();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);

		if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
		{
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fMaxAnisotropy);
		}
	}

	return bResult;
}
