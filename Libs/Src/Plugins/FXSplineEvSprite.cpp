#include "Plugins.h"
#include "FXSplineEvSprite.h"


const std::string CFXSplineEvSprite::CLASSNAME("Sprite Evaluator");

REGISTER_PLUGIN(CPluginFactory, CFXSplineEvSprite)


CFXSplineEvSprite::CFXSplineEvSprite()
{
	DefineVar("Sprite Size",  CVarFloat::CLASSNAME);
	DefineVar("Sprite Angle", CVarFloat::CLASSNAME);
	DefineVar("Type",         CVarCombo::CLASSNAME);

	((CVarFloat*)GetVar("Sprite Size"))->SetConstant(0.01f);
	((CVarFloat*)GetVar("Sprite Angle"))->SetConstant(0.0f);

	((CVarCombo*)GetVar("Type"))->AddOption("Single Sprite"); // default
	((CVarCombo*)GetVar("Type"))->AddOption("Whole Spline");

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);
	m_pResTexture = NULL;

	m_bEvaluateWholeSpline = false;
}

CFXSplineEvSprite::~CFXSplineEvSprite()
{
	Free();
}

bool CFXSplineEvSprite::OnLoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXSplineEvSprite::OnFree()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

UtilGL::Texturing::CTexture* CFXSplineEvSprite::GetTexture()
{
	if(m_pResTexture)
	{
		return (UtilGL::Texturing::CTexture*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();
	}

	return NULL;
}

bool CFXSplineEvSprite::EvaluateWholeSpline()
{
	CVarCombo::CValueCombo valueType;
	EvaluateVar("Type", 0.0f, &valueType);

	m_bEvaluateWholeSpline = true;

	if(valueType.GetValue() == "Single Sprite")
	{
		m_bEvaluateWholeSpline = false;
	}

	return m_bEvaluateWholeSpline;
}

void CFXSplineEvSprite::OnSplineFrameBegin(CDemo* pDemo,
											float fEffectTime,
											float fDemoTime)
{
	CVarFloat::CValueFloat valueSpriteSize;
	CVarFloat::CValueFloat valueSpriteAngle;

	EvaluateVar("Sprite Size",  fEffectTime, &valueSpriteSize);
	EvaluateVar("Sprite Angle", fEffectTime, &valueSpriteAngle);

	m_verts[0].Set(-0.5f, -0.5f, 0.0f);
	m_verts[1].Set(-0.5f, +0.5f, 0.0f);
	m_verts[2].Set(+0.5f, +0.5f, 0.0f);
	m_verts[3].Set(+0.5f, -0.5f, 0.0f);

	CMatrix mtxTransform;

	mtxTransform.Scale( valueSpriteSize.GetValue(),
						valueSpriteSize.GetValue(),
						1.0f);

	if(!IS_ZERO(valueSpriteAngle.GetValue()))
	{
		float fAspect  = pDemo->GetAspect();
		mtxTransform.RotateZ(valueSpriteAngle.GetValue());
		mtxTransform.Scale(1.0f, fAspect, 1.0f);
	}

	mtxTransform.Transform(&m_verts[0]);
	mtxTransform.Transform(&m_verts[1]);
	mtxTransform.Transform(&m_verts[2]);
	mtxTransform.Transform(&m_verts[3]);
}

void CFXSplineEvSprite::OnSplineFrameFinish()
{

}

void CFXSplineEvSprite::OnSplineBegin(int nSplineCount, int nSplineIndex)
{
	glBegin(GL_QUADS);
}

void CFXSplineEvSprite::OnSplineFinish(int nSplineCount, int nSplineIndex)
{
	glEnd();
}

void CFXSplineEvSprite::OnSplinePoint(const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
										int   nPointIndex, int   nTotalPoints,
										float fEvalTime,   float fPointEvalTime, float fPointSplineTime)
{
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv ((v3Point + m_verts[0]).Data());

	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv ((v3Point + m_verts[1]).Data());	

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv ((v3Point + m_verts[2]).Data());

	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv ((v3Point + m_verts[3]).Data());
}

void CFXSplineEvSprite::OnSplineEndPoint(const CVector3& v3Point, const CVector3& v3EscapeDir, float fEvalTime, float fPointEvalTime, float fPointSplineTime)
{
	if(!EvaluateWholeSpline())
	{
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv ((v3Point + m_verts[0]).Data());

		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv ((v3Point + m_verts[1]).Data());	

		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv ((v3Point + m_verts[2]).Data());

		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv ((v3Point + m_verts[3]).Data());
	}
}
