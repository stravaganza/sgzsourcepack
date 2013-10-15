#include "Plugins.h"
#include "FXSplineBillboards.h"


const std::string CFXSplineBillboards::CLASSNAME("Spline Billboards");

REGISTER_PLUGIN(CPluginFactory, CFXSplineBillboards)


#define MAXBILLBOARDS 30000
#define PRECALCPOINTS 1000


CFXSplineBillboards::CFXSplineBillboards()
{
	DefineVar("Alpha",                 CVarFloat::CLASSNAME);
	DefineVar("Spline Control Points", CVarInt::CLASSNAME);
	DefineVar("Spline Size",           CVarFloat::CLASSNAME);
	DefineVar("Billboard Count",       CVarInt::CLASSNAME);
	DefineVar("Billboard Angle",       CVarFloat::CLASSNAME);
	DefineVar("Billboard Size",        CVarFloat::CLASSNAME);
	DefineVar("Billboard Scroll",      CVarFloat::CLASSNAME);
	DefineVar("Seed Spline",           CVarInt::CLASSNAME);
	DefineVar("Keep Previous Camera",  CVarCombo::CLASSNAME);
	DefineVar("Blend Mode",            CVarCombo::CLASSNAME);
	DefineVar("Close Spline",		   CVarCombo::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Alpha"                 ))->SetConstant(1.0f);
	((CVarInt*)  GetVar("Spline Control Points" ))->SetConstant(8);
	((CVarInt*)  GetVar("Spline Control Points" ))->LockConstant(true);
	((CVarFloat*)GetVar("Spline Size"           ))->SetConstant(100.0f);
	((CVarInt*)  GetVar("Billboard Count"       ))->SetConstant(500);
	((CVarFloat*)GetVar("Billboard Angle"       ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Billboard Size"        ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Billboard Scroll"      ))->SetConstant(0.0f);
	((CVarInt*)  GetVar("Seed Spline"           ))->SetConstant(0);
	((CVarInt*)  GetVar("Seed Spline"           ))->LockConstant(true);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Alpha Blend");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Color Mult");

	((CVarCombo*)GetVar("Close Spline"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Close Spline"))->AddOption("No");

	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("No");  // default
	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("Yes");

	GetVar("Spline Control Points")->ReloadFXOnChange(this);
	GetVar("Seed Spline"          )->ReloadFXOnChange(this);
	GetVar("Close Spline"         )->ReloadFXOnChange(this);

	DefineAddTransformVars(this);

	m_pResTexture      = NULL;
	m_pv3PrecalcPoints = NULL;
	m_pv3BillBoards    = NULL;
}

CFXSplineBillboards::~CFXSplineBillboards()
{
	Free();
}

bool CFXSplineBillboards::LoadData(CResourceList* pResourceList)
{
	int i;

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	CVarInt::CValueInt     valueSplineControlPoints;
	CVarInt::CValueInt     valueSplineSeed;
	CVarCombo::CValueCombo valueClose;

	EvaluateVar("Spline Control Points", 0.0f, &valueSplineControlPoints);
	EvaluateVar("Seed Spline",           0.0f, &valueSplineSeed);
	EvaluateVar("Close Spline",          0.0f, &valueClose);

	int nCPoints = valueSplineControlPoints.GetValue() > 0 ? valueSplineControlPoints.GetValue() : 1;

	srand(valueSplineSeed.GetValue());

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

bool CFXSplineBillboards::Free()
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

bool CFXSplineBillboards::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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
	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	mtxCam.RotateX(v3AddCamAngles.X());
	mtxCam.RotateY(v3AddCamAngles.Y());
	mtxCam.RotateZ(v3AddCamAngles.Z());
	mtxCam.SetPosition(v3AddCamPos);

	if(valueKeepPreviousCamera.GetValue() == "No")
	{
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW, mtxCam.Inverse());

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
		glMatrixMode(GL_PROJECTION);
		gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);
	}

	CMatrix matrix = mtxAddWorld * mtxAddLocal;
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
