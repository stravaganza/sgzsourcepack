#include "Plugins.h"
#include "FXSplineEvParticles.h"


const std::string CFXSplineEvParticles::CLASSNAME("Particle Emitter Evaluator");

REGISTER_PLUGIN(CPluginFactory, CFXSplineEvParticles)


CFXSplineEvParticles::CFXSplineEvParticles()
{
	DefineVar("PS Emit Rate",     CVarInt::CLASSNAME);
	DefineVar("PS Emit Start",    CVarFloat::CLASSNAME);
	DefineVar("PS Emit Stop",     CVarFloat::CLASSNAME);
	DefineVar("PS Emit Radius",   CVarFloat::CLASSNAME);
	DefineVar("PS Emit Angle",    CVarFloat::CLASSNAME);
	DefineVar("PS Life",          CVarFloat::CLASSNAME);
	DefineVar("PS Speed",         CVarFloat::CLASSNAME);
	DefineVar("PS Size",          CVarFloat::CLASSNAME);
	DefineVar("PS Grow End",      CVarFloat::CLASSNAME);
	DefineVar("PS Fade Start",    CVarFloat::CLASSNAME);
	DefineVar("PS Fade Size",     CVarCombo::CLASSNAME);
	DefineVar("PS Accel X",       CVarFloat::CLASSNAME);
	DefineVar("PS Accel Y",       CVarFloat::CLASSNAME);
	DefineVar("PS Bubble Amp",    CVarFloat::CLASSNAME);
	DefineVar("PS Bubble Period", CVarFloat::CLASSNAME);
	DefineVar("PS Bubble Phase",  CVarFloat::CLASSNAME);
	DefineVar("PS Global Chaos",  CVarFloat::CLASSNAME);

	((CVarInt*)GetVar  ("PS Emit Rate"     ))->SetConstant(10);
	((CVarInt*)GetVar  ("PS Emit Rate"     ))->LockConstant(true);
	((CVarFloat*)GetVar("PS Emit Start"    ))->SetConstant(0.0f);
	((CVarInt*)GetVar  ("PS Emit Start"    ))->LockConstant(true);
	((CVarFloat*)GetVar("PS Emit Stop"     ))->SetConstant(20.0f);
	((CVarInt*)GetVar  ("PS Emit Stop"     ))->LockConstant(true);
	((CVarFloat*)GetVar("PS Emit Radius"   ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Emit Angle"    ))->SetConstant(180.0f);
	((CVarFloat*)GetVar("PS Life"          ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("PS Life"          ))->LockConstant(true);
	((CVarFloat*)GetVar("PS Speed"         ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Size"          ))->SetConstant(0.005f);
	((CVarFloat*)GetVar("PS Grow End"      ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Fade Start"    ))->SetConstant(0.0f);
	((CVarCombo*)GetVar("PS Fade Size"     ))->AddOption("No"); // default
	((CVarCombo*)GetVar("PS Fade Size"     ))->AddOption("Yes");
	((CVarFloat*)GetVar("PS Accel X"       ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Accel Y"       ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Bubble Amp"    ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Bubble Period" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("PS Bubble Phase"  ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("PS Global Chaos"  ))->SetConstant(0.0f);

	GetVar("PS Emit Rate"     )->ReloadFXOnChange(this);
	GetVar("PS Emit Start"    )->ReloadFXOnChange(this);
	GetVar("PS Emit Stop"     )->ReloadFXOnChange(this);
	GetVar("PS Emit Radius"   )->ReloadFXOnChange(this);
	GetVar("PS Emit Angle"    )->ReloadFXOnChange(this);
	GetVar("PS Life"          )->ReloadFXOnChange(this);
	GetVar("PS Speed"         )->ReloadFXOnChange(this);
	GetVar("PS Size"          )->ReloadFXOnChange(this);
	GetVar("PS Grow End"      )->ReloadFXOnChange(this);
	GetVar("PS Fade Start"    )->ReloadFXOnChange(this);
	GetVar("PS Fade Size"     )->ReloadFXOnChange(this);
	GetVar("PS Accel X"       )->ReloadFXOnChange(this);
	GetVar("PS Accel Y"       )->ReloadFXOnChange(this);
	GetVar("PS Bubble Amp"    )->ReloadFXOnChange(this);
	GetVar("PS Bubble Period" )->ReloadFXOnChange(this);
	GetVar("PS Bubble Phase"  )->ReloadFXOnChange(this);
	GetVar("PS Global Chaos"  )->ReloadFXOnChange(this);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);
	m_pResTexture = NULL;

	m_pPartSystem = NULL;

	m_fTime = -1.0f;
}

CFXSplineEvParticles::~CFXSplineEvParticles()
{
	Free();
}

bool CFXSplineEvParticles::OnLoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	m_pPartSystem = new CParticleSystemStd;

	OnSplineFrameBegin(NULL, 0.0f, 0.0f); // just to initiate particle system first

	CVarInt::CValueInt     valueEmitRate;
	CVarFloat::CValueFloat valueEmitStart;
	CVarFloat::CValueFloat valueEmitStop;
	CVarFloat::CValueFloat valueLife;
	CVarFloat::CValueFloat valueGlobalChaos;

	EvaluateVar("PS Emit Rate",    0.0f, &valueEmitRate);
	EvaluateVar("PS Emit Start",   0.0f, &valueEmitStart);
	EvaluateVar("PS Emit Stop",    0.0f, &valueEmitStop);
	EvaluateVar("PS Life",         0.0f, &valueLife);
	EvaluateVar("PS Global Chaos", 0.0f, &valueGlobalChaos);

	int   nNum  = valueEmitRate.GetValue() > 1      ? valueEmitRate.GetValue() : 1;
	float fLife = valueLife.GetValue()     > 0.001f ? valueLife.GetValue()     : 0.001f;

	m_pPartSystem->SetNumParticles(nNum, true);
	m_pPartSystem->SetEmitStart   (valueEmitStart.GetValue());
	m_pPartSystem->SetEmitStop    (valueEmitStop.GetValue());
	m_pPartSystem->SetPartLife    (fLife, valueGlobalChaos.GetValue());

	m_pPartSystem->InitSystem();

	return true;
}

bool CFXSplineEvParticles::OnFree()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	SAFE_DELETE(m_pPartSystem);

	return true;
}

UtilGL::Texturing::CTexture* CFXSplineEvParticles::GetTexture()
{
	if(m_pResTexture)
	{
		return (UtilGL::Texturing::CTexture*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();
	}

	return NULL;
}

bool CFXSplineEvParticles::EvaluateWholeSpline()
{
	return true;
}

void CFXSplineEvParticles::OnSplineFrameBegin(	CDemo* pDemo,
												float fEffectTime,
												float fDemoTime)
{
	CVarFloat::CValueFloat valueEmitRadius;
	CVarFloat::CValueFloat valueEmitAngle;
	CVarFloat::CValueFloat valueSpeed;
	CVarFloat::CValueFloat valueSize;
	CVarFloat::CValueFloat valueGrowEnd;
	CVarFloat::CValueFloat valueFadeStart;
	CVarCombo::CValueCombo valueFadeSize;
	CVarFloat::CValueFloat valueAccelX;
	CVarFloat::CValueFloat valueAccelY;
	CVarFloat::CValueFloat valueBubbleAmp;
	CVarFloat::CValueFloat valueBubblePeriod;
	CVarFloat::CValueFloat valueBubblePhase;
	CVarFloat::CValueFloat valueGlobalChaos;

	CVarColor::CValueColor valueColor;
	CVarCombo::CValueCombo valueBlendMode;
	CVarFloat::CValueFloat valueAlpha;

	EvaluateVar("PS Emit Radius",    fEffectTime, &valueEmitRadius);
	EvaluateVar("PS Emit Angle",     fEffectTime, &valueEmitAngle);
	EvaluateVar("PS Speed",          fEffectTime, &valueSpeed);
	EvaluateVar("PS Size",           fEffectTime, &valueSize);
	EvaluateVar("PS Grow End",       fEffectTime, &valueGrowEnd);
	EvaluateVar("PS Fade Start",     fEffectTime, &valueFadeStart);
	EvaluateVar("PS Fade Size",      fEffectTime, &valueFadeSize);
	EvaluateVar("PS Accel X",        fEffectTime, &valueAccelX);
	EvaluateVar("PS Accel Y",        fEffectTime, &valueAccelY);
	EvaluateVar("PS Bubble Amp",     fEffectTime, &valueBubbleAmp);
	EvaluateVar("PS Bubble Period",  fEffectTime, &valueBubblePeriod);
	EvaluateVar("PS Bubble Phase",   fEffectTime, &valueBubblePhase);
	EvaluateVar("PS Global Chaos",   fEffectTime, &valueGlobalChaos);

	EvaluateVar("Spline Color",      fEffectTime, &valueColor);
	EvaluateVar("Spline Blend Mode", fEffectTime, &valueBlendMode);
	EvaluateVar("Spline Alpha",      fEffectTime, &valueAlpha);

	CVector4 v4Col(valueColor.GetValue());
	v4Col.SetW(valueAlpha.GetValue());

	m_pPartSystem->SetSeed          (1337);
	m_pPartSystem->SetPartColor     (v4Col);
	m_pPartSystem->SetAffectLighting(false);
	m_pPartSystem->SetBlendMult     (valueBlendMode.GetValue() == "Mult");
	m_pPartSystem->SetEmitRadius    (valueEmitRadius.GetValue());
	m_pPartSystem->SetEmitOffPlane  (0.0f, 0.0f);
	m_pPartSystem->SetEmitOffAxis   (0.0f, valueEmitAngle.GetValue());
	m_pPartSystem->SetPartSpeed     (valueSpeed.GetValue(), valueGlobalChaos.GetValue());
	m_pPartSystem->SetPartSize      (valueSize.GetValue(), valueGlobalChaos.GetValue());
	m_pPartSystem->SetPartGrowEnd   (valueGrowEnd.GetValue());
	m_pPartSystem->SetPartFadeStart (valueFadeStart.GetValue());
	m_pPartSystem->SetPartFadeSize  (valueFadeSize.GetValue() == "Yes");
	m_pPartSystem->SetPartWorldAccel(CVector3(	valueAccelX.GetValue(),
												valueAccelY.GetValue(),
												0.0f));

	if(!IS_ZERO(valueBubbleAmp.GetValue()))
	{
		m_pPartSystem->SetBubbleAmp   (valueBubbleAmp.GetValue(),    valueGlobalChaos.GetValue());
		m_pPartSystem->SetBubblePeriod(valueBubblePeriod.GetValue(), valueGlobalChaos.GetValue());
		m_pPartSystem->SetBubblePhase (valueBubblePhase.GetValue(),  valueGlobalChaos.GetValue());
	}

	m_fTime = fEffectTime;
	m_pPartSystem->SetTexture((UtilGL::Texturing::CTexture2D*)GetTexture());

	m_posTrack.Delete();
	m_rotTrack.clear();
}

void CFXSplineEvParticles::OnSplineFrameFinish()
{
	// Do nothing
}

float CFXSplineEvParticles::GetTimeEvaluation(float fTime)
{
	// Force complete spline evaluation to build the whole animation track

	return 1.0f;
}

void CFXSplineEvParticles::OnSplineBegin(int nSplineCount, int nSplineIndex)
{
	// Do nothing
}

void CFXSplineEvParticles::OnSplineFinish(int nSplineCount, int nSplineIndex)
{
	// Do nothing
}

void CFXSplineEvParticles::OnSplinePoint(const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
										int   nPointIndex, int   nTotalPoints,
										float fEvalTime,   float fPointEvalTime, float fPointSplineTime)
{
	// Update position track

	CSplineCatmullRom::TKey posKey;

	posKey.fTime   = fPointEvalTime;
	posKey.v3Value = v3Point;

	m_posTrack.AddKey(posKey);

	// Update rotation track

	CMatrix  rotMatrix;
	CVector3 v3X, v3Y, v3Z;

	v3Z = CVector3(v3Point, v3PrevPoint);
	v3Z.Normalize();
	v3Y = CVector3(0.0f, 0.0f, -1.0f);
	v3X = v3Y ^ v3Z;

	rotMatrix.SetXAxis(v3X);
	rotMatrix.SetYAxis(v3Y);
	rotMatrix.SetZAxis(v3Z);

	CAnimRotSampled::TSample rotSample;
	rotSample.quat = rotMatrix.Quaternion();

	m_rotTrack.push_back(rotSample);
}

void CFXSplineEvParticles::OnSplineEndPoint(const CVector3& v3Point, const CVector3& v3EscapeDir, float fEvalTime, float fPointEvalTime, float fPointSplineTime)
{
	if(m_fTime < 0.0f) return;

	if(m_posTrack.GetNumKeys() > 0 && m_rotTrack.size() > 0)
	{
		int nFrequency  = 50;
		int nNumSamples = (GetEnd() - GetBegin()) * nFrequency;

		if(nNumSamples > 1)
		{
			CAnimRotSampled::TSample* splineRotTrack = new CAnimRotSampled::TSample[m_rotTrack.size()];
			CAnimRotSampled splineRotAnim;
			VECROTSAMPLES::iterator it;

			int i;

			for(i = 0, it = m_rotTrack.begin(); it != m_rotTrack.end(); ++it, i++)
			{
				splineRotTrack[i].quat = it->quat;
			}

			splineRotAnim.SetData(splineRotTrack, m_rotTrack.size(), m_rotTrack.size(), 0.0f);

			// Build Position+Rotation Track

			CAnimPosSampled::TSample* postrack = new CAnimPosSampled::TSample[nNumSamples];
			CAnimRotSampled::TSample* rottrack = new CAnimRotSampled::TSample[nNumSamples];

			for(i = 0; i < nNumSamples; i++)
			{
				float fT = i / (float)(nNumSamples - 1);

				CVarFloat::CValueFloat valueTime;
				EvaluateVar("Spline Time Value", (GetEnd() - GetBegin()) * fT, &valueTime);

				m_posTrack.Evaluate   (valueTime.GetValue(), &postrack[i].pos);
				splineRotAnim.Evaluate(valueTime.GetValue(), &rottrack[i].quat);
			}

			CAnimPosSampled* pAnimPos = new CAnimPosSampled;
			CAnimRotSampled* pAnimRot = new CAnimRotSampled;
			pAnimPos->SetData(postrack, nNumSamples, nFrequency, 0.0f);
			pAnimRot->SetData(rottrack, nNumSamples, nFrequency, 0.0f);

			m_pPartSystem->SetPosAnimation(pAnimPos);
			m_pPartSystem->SetRotAnimation(pAnimRot);

			splineRotAnim.Delete();
		}
	}
	else
	{
		m_pPartSystem->SetPosAnimation(NULL);
		m_pPartSystem->SetRotAnimation(NULL);
	}

	m_pPartSystem->SetAnimEvalTime(m_fTime, true);
	m_pPartSystem->SetEvalTime(m_fTime);
	m_pPartSystem->Render();
}
