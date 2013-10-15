#include "DemoSystem.h"
#include "FXSplineRays.h"
/*
#define MAXBILLBOARDS 30000
#define PRECALCPOINTS 1000


// Auto registration mechanism:

const std::string CFXSplineRays::CLASSNAME("Spline Rays");

class CFXSplineRaysCreator : public CEffectFactory::CObjectCreator
{
	std::string	InstanceClassName() { return CFXSplineRays::CLASSNAME; }
	CEffect*	CreateInstance()    { return new CFXSplineRays;        }
};

CEffectFactory::TRegisterCreator<CFXSplineRaysCreator> theFXSplineRaysRegisterCreator(CEffectFactory::Instance());




CFXSplineRays::CFXSplineRays()
{
	DefineVar("Alpha",                 CVarFloat::CLASSNAME);
	DefineVar("Spline Size",           CVarFloat::CLASSNAME);
	DefineVar("Spline Count",          CVarFloat::CLASSNAME);
	DefineVar("Billboard Count",       CVarInt::CLASSNAME);
	DefineVar("Billboard Size",        CVarFloat::CLASSNAME);
	DefineVar("Seed",                  CVarInt::CLASSNAME);
	DefineVar("Keep Previous Camera",  CVarCombo::CLASSNAME);
	DefineVar("Blend Mode",            CVarCombo::CLASSNAME);
	DefineVar("Close Spline",		   CVarCombo::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Alpha"                 ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Spline Size"           ))->SetConstant(100.0f);
	((CVarInt*)  GetVar("Spline Count"          ))->SetConstant(10);
	((CVarInt*)  GetVar("Spline Count"          ))->LockConstant(true);
	((CVarInt*)  GetVar("Billboard Count"       ))->SetConstant(100);
	((CVarInt*)  GetVar("Billboard Count"       ))->LockConstant(true);
	((CVarFloat*)GetVar("Billboard Size"        ))->SetConstant(10.0f);
	((CVarInt*)  GetVar("Seed"                  ))->SetConstant(0);
	((CVarInt*)  GetVar("Seed"                  ))->LockConstant(true);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Alpha Blend");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Color Mult");

	((CVarCombo*)GetVar("Close Spline"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Close Spline"))->AddOption("No");

	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("No");  // default
	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("Yes");

	GetVar("Billboard Count")->ReloadFXOnChange(this);
	GetVar("Seed"           )->ReloadFXOnChange(this);
	GetVar("Close Spline"   )->ReloadFXOnChange(this);

	DefineAddTransformVars();

	m_pResTexture = NULL;
}

CFXSplineRays::~CFXSplineRays()
{
	Free();
}

bool CFXSplineRays::LoadData(CResourceList* pResourceList)
{
	int i;

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	CVarInt::CValueInt     valueSplineCount;
	CVarInt::CValueInt     valueBillboardCount;
	CVarInt::CValueInt     valueSeed;
	CVarCombo::CValueCombo valueClose;

	EvaluateVar("Spline Count",    0.0f, &valueSplineCount);
	EvaluateVar("Billboard Count", 0.0f, &valueBillboardCount);
	EvaluateVar("Seed",            0.0f, &valueSeed);
	EvaluateVar("Close Spline",    0.0f, &valueClose);

	int nCPoints = valueBillboardCount.GetValue() > 0 ? valueBillboardCount.GetValue() : 1;

	srand(valueSeed.GetValue());

	for(int nSpline = 0; nSpline < valueSplineCount; nSpline++)
	{
		for(i = 0; i < valueSplineControlPoints.GetValue(); i++)
		{
			CSplineCatmullRom::TKey key;
			key.fTime   = nCPoints > 1 ? ((float)i / (nCPoints - 1)) : 0.0f;
			key.v3Value.Set(ComputeRand(-0.5f, 0.5f), ComputeRand(-0.5f, 0.5f), ComputeRand(-0.5f, 0.5f));
			m_spline.AddKey(key);
		}

		if(valueClose.GetValue() == "Yes")
		{
			CVector3 v3Point;
			m_spline.GetKeyValue(0, &v3Point);
			m_spline.SetKeyValue(m_spline.GetNumKeys() - 1, v3Point);
			m_spline.SetKeyTime (m_spline.GetNumKeys() - 1, 1.0f);
		}
	}

	m_pv3PrecalcPoints = new CVector3[PRECALCPOINTS];
	m_pv3BillBoards    = new CVector3[MAXBILLBOARDS];

	for(i = 0; i < PRECALCPOINTS; i++)
	{
		CVector3 v3Point;
		m_spline.Evaluate((float)i / (float)(PRECALCPOINTS - 1), &v3Point);
		m_pv3PrecalcPoints[i] = v3Point;
	}

	return true;
}

bool CFXSplineRays::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	SAFE_DELETE_A(m_pv3PrecalcPoints);
	SAFE_DELETE_A(m_pv3BillBoards);

	m_spline.Delete();

	return true;
}

bool CFXSplineRays::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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

	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valueSplineSize;
	CVarInt::CValueInt     valueBillboardCount;
	CVarFloat::CValueFloat valueBillboardAngle;
	CVarFloat::CValueFloat valueBillboardSize;
	CVarFloat::CValueFloat valueBillboardScroll;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueKeepPreviousCamera;

	EvaluateVar("Alpha",                fEffectTime, &valueAlpha);
	EvaluateVar("Spline Size",          fEffectTime, &valueSplineSize);
	EvaluateVar("Billboard Count",      fEffectTime, &valueBillboardCount);
	EvaluateVar("Billboard Angle",      fEffectTime, &valueBillboardAngle);
	EvaluateVar("Billboard Size",       fEffectTime, &valueBillboardSize);
	EvaluateVar("Billboard Scroll",     fEffectTime, &valueBillboardScroll);
	EvaluateVar("Blend Mode",           fEffectTime, &valueBlendMode);
	EvaluateVar("Keep Previous Camera", fEffectTime, &valueKeepPreviousCamera);

	int nNumBillboards = valueBillboardCount.GetValue() > MAXBILLBOARDS ? MAXBILLBOARDS : valueBillboardCount.GetValue();

	// States

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);

	int nSrcBlend = UtilGL::States::BLEND_SRCALPHA;
	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	if(valueBlendMode.GetValue() == "Add")
	{
		nDstBlend = UtilGL::States::BLEND_ONE;
	}
	else if(valueBlendMode.GetValue() == "Color Mult")
	{
		nSrcBlend = UtilGL::States::BLEND_DSTCOLOR;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}

	UtilGL::States::Set(UtilGL::States::BLENDING,     UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::LIGHTING,     UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D,    UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING,    UtilGL::States::DISABLED);

	UtilGL::States::Set(UtilGL::States::SRCBLEND, nSrcBlend);
	UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);


	UtilGL::States::SetColor(v4Color);

	// Transforming

	CMatrix mtxCam;

	mtxCam.RotateX(GetAddCameraAngles().X());
	mtxCam.RotateY(GetAddCameraAngles().Y());
	mtxCam.RotateZ(GetAddCameraAngles().Z());
	mtxCam.SetPosition(GetAddCameraPos());

	if(valueKeepPreviousCamera.GetValue() == "No")
	{
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW, mtxCam.Inverse());

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
		glMatrixMode(GL_PROJECTION);
		gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);
	}

	CMatrix matrix = GetAddWorldMatrix() * GetAddLocalMatrix();
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, matrix);

	// Render

	float fScroll = valueBillboardScroll.GetValue();

	for(int i = 0; i < nNumBillboards; i++)
	{
		double dT      = ((double)i / (double)nNumBillboards) + (double)fScroll;
		int    nIndex1 = (int)(dT * (double)PRECALCPOINTS) % PRECALCPOINTS;
		int    nIndex2 = (nIndex1 + 1) % PRECALCPOINTS;
		double dTLERP  = (dT * (double)PRECALCPOINTS) - (int)(dT * (double)PRECALCPOINTS);

		float fX = m_pv3PrecalcPoints[nIndex1].X() * dTLERP + m_pv3PrecalcPoints[nIndex2].X() * (1.0 - dTLERP);
		float fY = m_pv3PrecalcPoints[nIndex1].Y() * dTLERP + m_pv3PrecalcPoints[nIndex2].Y() * (1.0 - dTLERP);
		float fZ = m_pv3PrecalcPoints[nIndex1].Z() * dTLERP + m_pv3PrecalcPoints[nIndex2].Z() * (1.0 - dTLERP);

		m_pv3BillBoards[i] = CVector3(fX, fY, fZ) * valueSplineSize.GetValue();
	}

	UtilGL::Rendering::DrawBillboards(nNumBillboards, pTexture, m_pv3BillBoards, NULL, valueBillboardSize.GetValue(), valueBillboardAngle.GetValue());

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING,     UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING,    UtilGL::States::ENABLED);

	return true;
}

  */