#include "Plugins.h"
#include "FXCreationRays.h"


const std::string CFXCreationRays::CLASSNAME("Creation Rays");

REGISTER_PLUGIN(CPluginFactory, CFXCreationRays)


CFXCreationRays::CFXCreationRays()
{
	DefineVar("Alpha",         CVarFloat::CLASSNAME);
	DefineVar("X Position",    CVarFloat::CLASSNAME);
	DefineVar("Y Position",    CVarFloat::CLASSNAME);
	DefineVar("Ray Size",      CVarFloat::CLASSNAME);
	DefineVar("Ray Frequency", CVarFloat::CLASSNAME);
	DefineVar("Ray Duration",  CVarFloat::CLASSNAME);
	DefineVar("Radius",        CVarFloat::CLASSNAME);
	DefineVar("Randomness",    CVarFloat::CLASSNAME);
	DefineVar("Blend Mode",    CVarCombo::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Alpha"        ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("X Position"   ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Position"   ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Ray Size"     ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Ray Frequency"))->LockConstant(true);
	((CVarFloat*)GetVar("Ray Frequency"))->SetConstant(5.0f);
	((CVarFloat*)GetVar("Ray Duration" ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Radius"       ))->SetConstant(0.6f);
	((CVarFloat*)GetVar("Randomness"   ))->SetConstant(0.3f);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");

	GetVar("X Position"   )->ReloadFXOnChange(this);
	GetVar("Y Position"   )->ReloadFXOnChange(this);
	GetVar("Ray Size"     )->ReloadFXOnChange(this);
	GetVar("Ray Frequency")->ReloadFXOnChange(this);
	GetVar("Ray Frequency")->ReloadFXOnChange(this);
	GetVar("Ray Duration" )->ReloadFXOnChange(this);
	GetVar("Radius"       )->ReloadFXOnChange(this);
	GetVar("Randomness"   )->ReloadFXOnChange(this);

	m_pResTexture = NULL;
}

CFXCreationRays::~CFXCreationRays()
{
	Free();
}

bool CFXCreationRays::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	// Create Rays

	CVarFloat::CValueFloat valueFloat;
	EvaluateVar("Ray Frequency", 0.0f, &valueFloat);

	int nNumRays = MYROUND((GetEnd() - GetBegin()) * valueFloat.GetValue());

	for(int i = 0; i < nNumRays; i++)
	{
		SRay ray;
		float fX, fY, fSize, fDuration, fRadius, fRandomness;

		EvaluateVar("X Position", (i / (float)nNumRays) * (GetEnd() - GetBegin()), &valueFloat);
		fX = valueFloat.GetValue();
		EvaluateVar("Y Position", (i / (float)nNumRays) * (GetEnd() - GetBegin()), &valueFloat);
		fY = valueFloat.GetValue();
		EvaluateVar("Ray Size", (i / (float)nNumRays) * (GetEnd() - GetBegin()), &valueFloat);
		fSize = valueFloat.GetValue();
		EvaluateVar("Ray Duration", (i / (float)nNumRays) * (GetEnd() - GetBegin()), &valueFloat);
		fDuration = valueFloat.GetValue();
		EvaluateVar("Radius", (i / (float)nNumRays) * (GetEnd() - GetBegin()), &valueFloat);
		fRadius = valueFloat.GetValue();
		EvaluateVar("Randomness", (i / (float)nNumRays) * (GetEnd() - GetBegin()), &valueFloat);
		fRandomness = valueFloat.GetValue();

		if(IS_ZERO(fDuration)) fDuration = 0.01f;

		CVector3 v3Dir(1.0f, 0.0f, 0.0f);
		CMatrix  mtxRot;
		mtxRot.BuildRotationZ(ComputeRand(0.0f, 360.0f));
		mtxRot.Transform(&v3Dir, true);

		fX        += ComputeRand(-0.5f, 0.5f) * fRandomness;
		fY        += ComputeRand(-0.5f, 0.5f) * fRandomness;
		fSize     += ComputeRand(-0.5f, 0.5f) * fRandomness;
		fDuration += ComputeRand(-0.5f, 0.5f) * fRandomness * 0.1f;
		fRadius   += ComputeRand(-0.5f, 0.5f) * fRandomness;

		v3Dir = v3Dir * fRadius;
		ray.v3Start.Set(fX - v3Dir.X() * 0.5f, fY - v3Dir.Y() * 0.5f, 0.0f);
		ray.v3End     = ray.v3Start + v3Dir;
		ray.fSize     = fSize;
		ray.fStart    = (i / (float)nNumRays) * (GetEnd() - GetBegin());
		ray.fDuration = fDuration;

		ray.v2Map1.Set(ComputeRand(0.0f, 1.0f), ComputeRand(0.0f, 1.0f));
		ray.v2Map2.Set(ComputeRand(0.0f, 1.0f), ComputeRand(0.0f, 1.0f));

		m_vecRays.push_back(ray);
	}

	std::sort(m_vecRays.begin(), m_vecRays.end(), SRay::SortPredicate);

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXCreationRays::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	m_vecRays.clear();

	return true;
}

bool CFXCreationRays::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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
	else
	{
		pTexture->SetActive();
	}

	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarCombo::CValueCombo valueBlendMode;

	EvaluateVar("Alpha",      fEffectTime, &valueAlpha);
	EvaluateVar("X Position", fEffectTime, &valuePosX);
	EvaluateVar("Y Position", fEffectTime, &valuePosY);
	EvaluateVar("Blend Mode", fEffectTime, &valueBlendMode);

	// Change states

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);

	int nSrcBlend = UtilGL::States::BLEND_SRCALPHA;
	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !pTexture->HasAlpha())
	{
		nSrcBlend = UtilGL::States::BLEND_ONE;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}

	if(!((nSrcBlend == UtilGL::States::BLEND_ONE  && nDstBlend == UtilGL::States::BLEND_ZERO) ||
		 (nSrcBlend == UtilGL::States::BLEND_ZERO && nDstBlend == UtilGL::States::BLEND_ONE)))
	{
		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND, nSrcBlend);
		UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	}

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);

	// DRAW

	glBegin(GL_LINES);

	VECRAYS::iterator it;

	for(it = m_vecRays.begin(); it < m_vecRays.end(); ++it)
	{
		if(fEffectTime < it->fStart)                 continue;
		if(fEffectTime > it->fStart + it->fDuration) continue;

		float fT = (fEffectTime - it->fStart) / it->fDuration;

		CVector3 v3Pos = it->v3Start.LERP(it->v3End, fT);
		CVector3 v3Dir = it->v3End - it->v3Start;
		v3Dir.Normalize();

		CVector3 v1 = v3Pos - (v3Dir * 0.5f * it->fSize);
		CVector3 v2 = v3Pos + (v3Dir * 0.5f * it->fSize);

		glTexCoord2fv(it->v2Map1.Data());
		glVertex3fv(v1.Data());

		glTexCoord2fv(it->v2Map2.Data());
		glVertex3fv(v2.Data());
	}

	glEnd();

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);

	return true;
}
