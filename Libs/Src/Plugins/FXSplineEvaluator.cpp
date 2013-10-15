#include "Plugins.h"
#include "FXSplineEvaluator.h"


const std::string CFXSplineEvaluator::CLASSNAME("Standard Spline Evaluator");

REGISTER_PLUGIN(CPluginFactory, CFXSplineEvaluator)


CFXSplineEvaluator::CFXSplineEvaluator()
{
	DefineVar("Spline X Position", CVarFloat::CLASSNAME);
	DefineVar("Spline Y Position", CVarFloat::CLASSNAME);
	DefineVar("Spline Width",      CVarFloat::CLASSNAME);
	DefineVar("Spline Height",     CVarFloat::CLASSNAME);
	DefineVar("Spline Scale",      CVarFloat::CLASSNAME);
	DefineVar("Spline Alpha",      CVarFloat::CLASSNAME);
	DefineVar("Spline Angle",      CVarFloat::CLASSNAME);
	DefineVar("Spline Blend Mode", CVarCombo::CLASSNAME);

	DefineVar("Spline Time Value", CVarFloat::CLASSNAME);
	DefineVar("Spline Color",      CVarColor::CLASSNAME);
	DefineVar("Spline Num Steps",  CVarInt::CLASSNAME);

	// Set the default values

	((CVarFloat*)GetVar("Spline X Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Spline Y Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Spline Width" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Spline Height"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Spline Scale" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Spline Alpha" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Spline Angle" ))->SetConstant(0.0f);

	((CVarCombo*)GetVar("Spline Blend Mode"))->AddOption("Mult"); // default
	((CVarCombo*)GetVar("Spline Blend Mode"))->AddOption("Add");

	((CVarFloat*)GetVar("Spline Time Value"))->SetConstant(1.0f);
	((CVarColor*)GetVar("Spline Color"))->SetConstant(CVector3(1.0f, 1.0f, 1.0f));
	((CVarInt*)GetVar("Spline Num Steps"))->SetConstant(100);

	DefineResourceReference("Spline", CResourceSpline::CLASSNAME);
	m_pResSpline = NULL;
}

CFXSplineEvaluator::~CFXSplineEvaluator()
{
	Free();
}

bool CFXSplineEvaluator::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResSpline = this->FindResource(pResourceList, "Spline", CResourceSpline::CLASSNAME)))
	{
		FXLoadError("Can't find spline resource");
		return false;
	}

	m_pResSpline->NotifyDependency(this);

	return OnLoadData(pResourceList);
}

bool CFXSplineEvaluator::Free()
{
	if(m_pResSpline)
	{
		m_pResSpline->RemoveDependency(this);
		m_pResSpline = NULL;
	}

	return OnFree();
}

float CFXSplineEvaluator::GetTimeEvaluation(float fTime)
{
	CVarFloat::CValueFloat valueTime;
	EvaluateVar("Spline Time Value", fTime, &valueTime);

	return valueTime.GetValue();
}

void CFXSplineEvaluator::OnSplineBegin(int nSplineCount, int nSplineIndex)
{
	glBegin(GL_LINE_STRIP);
}

void CFXSplineEvaluator::OnSplineFinish(int nSplineCount, int nSplineIndex)
{
	glEnd();
}

void CFXSplineEvaluator::OnSplinePoint(	const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
										int   nPointIndex, int   nTotalPoints,
										float fEvalTime,   float fPointEvalTime, float fPointSplineTime)
{
	glVertex3fv(v3Point.Data());
}

bool CFXSplineEvaluator::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResSpline)
	{
		FXRuntimeError("Spline resource not found");
		return false;
	}

	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueScale;
	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valueAngle;
	CVarCombo::CValueCombo valueBlendMode;

	CVarFloat::CValueFloat valueTime;
	CVarColor::CValueColor valueColor;
	CVarInt::CValueInt     valueSteps;

	EvaluateVar("Spline X Position", fEffectTime, &valuePosX);
	EvaluateVar("Spline Y Position", fEffectTime, &valuePosY);
	EvaluateVar("Spline Width",      fEffectTime, &valueWidth);
	EvaluateVar("Spline Height",     fEffectTime, &valueHeight);
	EvaluateVar("Spline Scale",      fEffectTime, &valueScale);
	EvaluateVar("Spline Alpha",      fEffectTime, &valueAlpha);
	EvaluateVar("Spline Angle",      fEffectTime, &valueAngle);
	EvaluateVar("Spline Blend Mode", fEffectTime, &valueBlendMode);

	EvaluateVar("Spline Color",      fEffectTime, &valueColor);
	EvaluateVar("Spline Num Steps",  fEffectTime, &valueSteps);
	EvaluateVar("Spline Time Value", fEffectTime, &valueTime);

	int   NUMSTEPS  = valueSteps.GetValue() < 2 ? 2 : valueSteps.GetValue();
	float DELTASTEP = 1.0f / (NUMSTEPS - 1);
	bool bAlphaTexture = false;

	if(this->GetTexture())
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		this->GetTexture()->SetActive();

		if(this->GetTexture()->HasAlpha())
		{
			bAlphaTexture = true;
		}
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	}

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::DISABLED);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	CVarFloat* pVarAngle = reinterpret_cast<CVarFloat*>(GetVar("Spline Angle"));
	assert(pVarAngle);

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

	float fAlpha = valueAlpha.GetValue();

	if(fAlpha < 0.0f) fAlpha = 0.0f;
	if(fAlpha > 1.0f) fAlpha = 1.0f;

	CVector4 v4Col = valueColor.GetValue();
	v4Col.SetW(fAlpha);

	UtilGL::States::SetColor(v4Col);

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !bAlphaTexture)
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

	// Build Transformation matrix

	CMatrix mtxTransform;
	float fCenterX = valuePosX.GetValue();
	float fCenterY = valuePosY.GetValue();

	mtxTransform.BuildTranslation(-0.5f, -0.5f, 0.0f);
	mtxTransform.Scale( valueWidth.GetValue()  * valueScale.GetValue(),
						valueHeight.GetValue() * valueScale.GetValue(),
						1.0f);

	if(!IS_ZERO(fAngle))
	{
		float fAspect  = pDemo->GetAspect();
		mtxTransform.RotateZ(fAngle);
		mtxTransform.Scale(1.0f, fAspect, 1.0f);
	}

	mtxTransform.Translate(fCenterX, fCenterY, 0.0f);

	// EVALUATE

	float fValueTime     = GetTimeEvaluation(fEffectTime);
	int nSplineCount     = ((CResourceSpline*)m_pResSpline)->GetSplineCount();
	int nCompleteSplines = (int)(nSplineCount * fValueTime);
	int nLastSpline      = nCompleteSplines;

	if(nCompleteSplines < 0)            nCompleteSplines = 0;
	if(nCompleteSplines > nSplineCount) nCompleteSplines = nSplineCount;

	bool bEnd = false;

	if(ARE_EQUAL(valueTime.GetValue(), 1.0f) && nLastSpline == nSplineCount)
	{
		nLastSpline = nSplineCount - 1;
		bEnd        = true;
	}

	CVector3 v3Point, v3PrevPoint, v3NextPoint;

	OnSplineFrameBegin(pDemo, fEffectTime, fDemoTime);

	if(this->EvaluateWholeSpline())
	{
		// Draw completed splines

		for(int i = 0; i < nCompleteSplines; i++)
		{
			CSplineCatmullRom* pSpline = (CSplineCatmullRom*)((CResourceSpline*)m_pResSpline)->GetSpline(i);

			if(!pSpline)
			{
				FXRuntimeError("Spline not available");
				continue;
			}

			OnSplineBegin(nSplineCount, i);

			float t;
			int   s;

			for(s = 0; s < NUMSTEPS; s++)
			{
				t = s * DELTASTEP;

				if(s == 0)
				{
					// First

					pSpline->Evaluate(DELTASTEP, &v3NextPoint);
					pSpline->Evaluate(0.0f, &v3Point);
					v3PrevPoint = v3Point - CVector3(v3Point, v3NextPoint);

					mtxTransform.Transform(&v3PrevPoint);
					mtxTransform.Transform(&v3Point);
					mtxTransform.Transform(&v3NextPoint);
				}
				else if((s + 1) == NUMSTEPS)
				{
					// Last

					v3PrevPoint = v3Point;
					v3Point     = v3NextPoint;
					v3NextPoint = v3Point + CVector3(v3PrevPoint, v3Point);
				}
				else
				{
					// Intermediate

					v3PrevPoint = v3Point;
					v3Point     = v3NextPoint;

					float tNext = t + DELTASTEP;

					if(tNext < 0.0f) tNext = 0.0f;
					if(tNext > 1.0f) tNext = 1.0f;

					pSpline->Evaluate(tNext, &v3NextPoint);
					mtxTransform.Transform(&v3NextPoint);
				}

				// Callback

				OnSplinePoint(v3PrevPoint, v3Point, v3NextPoint,
							  s, NUMSTEPS,
							  fValueTime, ((float)i + t) / nSplineCount, t);
			}

			OnSplineFinish(nSplineCount, i);
		}

		// Draw incomplete last spline

		if(nLastSpline >= 0 && nLastSpline < nSplineCount)
		{
			CSplineCatmullRom* pSpline = (CSplineCatmullRom*)((CResourceSpline*)m_pResSpline)->GetSpline(nLastSpline);

			float fTEnd = 1.0f;

			if(bEnd == false)
			{
				fTEnd = (nSplineCount * valueTime.GetValue()) - (float)((int)(nSplineCount * valueTime.GetValue()));
			}
			
			if(pSpline)
			{
				OnSplineBegin(nSplineCount, i);

				float t;
				int   s, nSteps = fTEnd * NUMSTEPS;

				for(s = 0; s < nSteps; s++)
				{
					t = s * DELTASTEP;

					if(s == 0)
					{
						// First

						pSpline->Evaluate(DELTASTEP, &v3NextPoint);
						pSpline->Evaluate(0.0f, &v3Point);
						v3PrevPoint = v3Point - CVector3(v3Point, v3NextPoint);

						mtxTransform.Transform(&v3PrevPoint);
						mtxTransform.Transform(&v3Point);
						mtxTransform.Transform(&v3NextPoint);
					}
					else if((s + 1) == nSteps)
					{
						// Last

						v3PrevPoint = v3Point;
						v3Point     = v3NextPoint;
						v3NextPoint = v3Point + CVector3(v3PrevPoint, v3Point);
					}
					else
					{
						// Intermediate

						v3PrevPoint = v3Point;
						v3Point     = v3NextPoint;

						float tNext = t + DELTASTEP;

						if(tNext < 0.0f) tNext = 0.0f;
						if(tNext > 1.0f) tNext = 1.0f;

						pSpline->Evaluate(tNext, &v3NextPoint);
						mtxTransform.Transform(&v3NextPoint);
					}

					// Callback

					OnSplinePoint(v3PrevPoint, v3Point, v3NextPoint,
								  s, NUMSTEPS,
								  fValueTime, ((float)nLastSpline + t) / nSplineCount, t);
				}

				OnSplineFinish(nSplineCount, i);
			}
			else
			{
				FXRuntimeError("Spline not available");
			}
		}
	}

	nCompleteSplines = (int)(nSplineCount * valueTime.GetValue());
	nLastSpline      = nCompleteSplines;

	if(nLastSpline >= 0 && nLastSpline < nSplineCount)
	{
		CSplineCatmullRom* pSpline = (CSplineCatmullRom*)((CResourceSpline*)m_pResSpline)->GetSpline(nLastSpline);
		float fTEnd = 1.0f;

		if(bEnd == false)
		{
			fTEnd = (nSplineCount * valueTime.GetValue()) - (float)((int)(nSplineCount * valueTime.GetValue()));
		}

		if(pSpline)
		{
			pSpline->Evaluate(fTEnd, &v3Point);
			mtxTransform.Transform(&v3Point);

			CVector3 v3Aux(1.0f, 0.0f, 0.0f);

			if((fTEnd + 0.02f) > 1.0f)
			{
				pSpline->Evaluate(fTEnd + 0.02f, &v3Aux);
				v3Aux = CVector3(v3Point, v3Aux);
				v3Aux.Normalize();
			}
			else
			{
				pSpline->Evaluate(fTEnd - 0.02f, &v3Aux);
				v3Aux = CVector3(v3Aux, v3Point);
				v3Aux.Normalize();
			}

			OnSplineBegin(nSplineCount, nLastSpline);
			OnSplineEndPoint(v3Point, v3Aux, valueTime.GetValue(), valueTime.GetValue(), fTEnd);
			OnSplineFinish(nSplineCount, nLastSpline);
		}
		else
		{
			FXRuntimeError("Spline not available");
		}
	}

	OnSplineFrameFinish();

	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::ENABLED);

	return true;
}
