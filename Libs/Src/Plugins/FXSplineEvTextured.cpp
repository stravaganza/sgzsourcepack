#include "Plugins.h"
#include "FXSplineEvTextured.h"


const std::string CFXSplineEvTextured::CLASSNAME("Textured Evaluator");

REGISTER_PLUGIN(CPluginFactory, CFXSplineEvTextured)


CFXSplineEvTextured::CFXSplineEvTextured()
{
	DefineVar("Thickness",      CVarFloat::CLASSNAME);
	DefineVar("Texture Offset", CVarFloat::CLASSNAME);

	((CVarFloat*)GetVar("Thickness"))->SetConstant(0.01f);
	((CVarFloat*)GetVar("Texture Offset"))->SetConstant(0.01f);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);
	m_pResTexture = NULL;
}

CFXSplineEvTextured::~CFXSplineEvTextured()
{
	Free();
}

bool CFXSplineEvTextured::OnLoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXSplineEvTextured::OnFree()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

UtilGL::Texturing::CTexture* CFXSplineEvTextured::GetTexture()
{
	if(m_pResTexture)
	{
		return (UtilGL::Texturing::CTexture*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();
	}

	return NULL;
}

bool CFXSplineEvTextured::EvaluateWholeSpline()
{
	return true;
}

void CFXSplineEvTextured::OnSplineFrameBegin(CDemo* pDemo,
											float fEffectTime,
											float fDemoTime)
{
	CVarFloat::CValueFloat valueThickness;
	CVarFloat::CValueFloat valueTexOffset;
	EvaluateVar("Thickness",       fEffectTime, &valueThickness);
	EvaluateVar("Texture Offset",  fEffectTime, &valueTexOffset);

	m_fThickness = valueThickness.GetValue();
	m_fTexOffset = valueTexOffset.GetValue();
}

void CFXSplineEvTextured::OnSplineFrameFinish()
{

}

void CFXSplineEvTextured::OnSplineBegin(int nSplineCount, int nSplineIndex)
{
	glBegin(GL_QUAD_STRIP);
}

void CFXSplineEvTextured::OnSplineFinish(int nSplineCount, int nSplineIndex)
{
	glEnd();
}

void CFXSplineEvTextured::OnSplinePoint(const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
										int   nPointIndex, int   nTotalPoints,
										float fEvalTime,   float fPointEvalTime, float fPointSplineTime)
{
	CVector3 perp(v3PrevPoint, v3NextPoint);
	perp.Set(-perp.Y(), perp.X(), 0.0f);
	perp.Normalize();
	perp = perp * (m_fThickness * 0.5f);

	glTexCoord2f(fPointSplineTime + m_fTexOffset, 0.0f);
	glVertex3fv ((v3Point + perp).Data());
	glTexCoord2f(fPointSplineTime + m_fTexOffset, 1.0f);
	glVertex3fv ((v3Point - perp).Data());
}
