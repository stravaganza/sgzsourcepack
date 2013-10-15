#include "Plugins.h"
#include "FXRings.h"


const std::string CFXRings::CLASSNAME("Cylinder Rings");

REGISTER_PLUGIN(CPluginFactory, CFXRings)


CFXRings::CFXRings()
{
	DefineVar("Alpha",                  CVarFloat::CLASSNAME);
	DefineVar("Horizontal Ring Count",  CVarInt::CLASSNAME);
	DefineVar("Horizontal Ring Radius", CVarFloat::CLASSNAME);
	DefineVar("Horizontal Ring Width",  CVarFloat::CLASSNAME);
	DefineVar("Horizontal Variation",   CVarFloat::CLASSNAME);
	DefineVar("Vertical Ring Count",    CVarInt::CLASSNAME);
	DefineVar("Vertical Ring Radius",   CVarFloat::CLASSNAME);
	DefineVar("Vertical Ring Width",    CVarFloat::CLASSNAME);
	DefineVar("Vertical Variation",     CVarFloat::CLASSNAME);
	DefineVar("Oscillation Amplitude",  CVarFloat::CLASSNAME);
	DefineVar("Oscillation Frequency",  CVarFloat::CLASSNAME);
	DefineVar("Ring Subdivisions",      CVarInt::CLASSNAME);
	DefineVar("Cylinder Height",        CVarFloat::CLASSNAME);
	DefineVar("Rotation Speed",         CVarFloat::CLASSNAME);
	DefineVar("Tile U",                 CVarFloat::CLASSNAME);
	DefineVar("Tile V",                 CVarFloat::CLASSNAME);
	DefineVar("Seed",                   CVarInt::CLASSNAME);
	DefineVar("Blend Mode",             CVarCombo::CLASSNAME);
	DefineVar("Keep Previous Camera",   CVarCombo::CLASSNAME);

	DefineResourceReference("Horizontal Texture", CResourceTexture2D::CLASSNAME);
	DefineResourceReference("Vertical Texture",   CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Alpha"                 ))->SetConstant(1.0f);
	((CVarInt*)  GetVar("Horizontal Ring Count" ))->SetConstant(8);
	((CVarInt*)  GetVar("Horizontal Ring Count" ))->LockConstant(true);
	((CVarFloat*)GetVar("Horizontal Ring Radius"))->SetConstant(100.0f);
	((CVarFloat*)GetVar("Horizontal Ring Width" ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Horizontal Variation"  ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Horizontal Variation"  ))->LockConstant(true);
	((CVarInt*)  GetVar("Vertical Ring Count"   ))->SetConstant(8);
	((CVarInt*)  GetVar("Vertical Ring Count"   ))->LockConstant(true);
	((CVarFloat*)GetVar("Vertical Ring Radius"  ))->SetConstant(100.0f);
	((CVarFloat*)GetVar("Vertical Ring Width"   ))->SetConstant(5.0f);
	((CVarFloat*)GetVar("Vertical Variation"    ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Vertical Variation"    ))->LockConstant(true);

	((CVarInt*)  GetVar("Ring Subdivisions"     ))->SetConstant(100);
	((CVarInt*)  GetVar("Ring Subdivisions"     ))->LockConstant(true);
	((CVarFloat*)GetVar("Oscillation Amplitude" ))->SetConstant(0);
	((CVarFloat*)GetVar("Oscillation Frequency" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Cylinder Height"       ))->SetConstant(200.0f);
	((CVarFloat*)GetVar("Rotation Speed"        ))->SetConstant(0.05f);
	((CVarFloat*)GetVar("Tile U"                ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Tile V"                ))->SetConstant(1.0f);
	((CVarInt*)  GetVar("Seed"                  ))->SetConstant(0);
	((CVarInt*)  GetVar("Seed"                  ))->LockConstant(true);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Alpha Blend");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Color Mult");

	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("No");  // default
	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("Yes");

	GetVar("Ring Subdivisions"    )->ReloadFXOnChange(this);
	GetVar("Horizontal Ring Count")->ReloadFXOnChange(this);
	GetVar("Horizontal Variation" )->ReloadFXOnChange(this);
	GetVar("Vertical Ring Count"  )->ReloadFXOnChange(this);
	GetVar("Vertical Variation"   )->ReloadFXOnChange(this);
	GetVar("Seed"                 )->ReloadFXOnChange(this);

	DefineAddTransformVars(this);

	m_pResHorTexture  = NULL;
	m_pResVertTexture = NULL;

	m_pv3Circle     = NULL;
	m_nSubdivisions = 0;

	m_pv3MeshVertices = NULL;
	m_pv2MeshMapping  = NULL;
	m_pnMeshFaceList  = NULL;
}

CFXRings::~CFXRings()
{
	Free();
}

bool CFXRings::LoadData(CResourceList* pResourceList)
{
	int i;

	if(!(m_pResHorTexture = this->FindResource(pResourceList, "Horizontal Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find horizontal texture resource");
		return false;
	}

	m_pResHorTexture->NotifyDependency(this);

	if(!(m_pResVertTexture = this->FindResource(pResourceList, "Vertical Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find vertical texture resource");
		return false;
	}

	m_pResVertTexture->NotifyDependency(this);

	// Build mesh and ring properties

	CVarInt::CValueInt     valueHorRingCount;
	CVarFloat::CValueFloat valueHorVariation;
	CVarInt::CValueInt     valueVertRingCount;
	CVarFloat::CValueFloat valueVertVariation;
	CVarInt::CValueInt     valueRingSubdivisions;
	CVarInt::CValueInt     valueSeed;

	EvaluateVar("Horizontal Ring Count",  0.0f, &valueHorRingCount);
	EvaluateVar("Horizontal Variation",   0.0f, &valueHorVariation);
	EvaluateVar("Vertical Ring Count",    0.0f, &valueVertRingCount);
	EvaluateVar("Vertical Variation",     0.0f, &valueVertVariation);
	EvaluateVar("Ring Subdivisions",      0.0f, &valueRingSubdivisions);
	EvaluateVar("Seed",                   0.0f, &valueSeed);

	srand(valueSeed.GetValue());

	m_nSubdivisions = valueRingSubdivisions.GetValue();
	if(m_nSubdivisions < 2) m_nSubdivisions = 2;

	m_pv3Circle       = new CVector3[m_nSubdivisions + 1];

	m_pv3MeshVertices = new CVector3[(m_nSubdivisions + 1) * 2];
	m_pv2MeshMapping  = new CVector2[(m_nSubdivisions + 1) * 2];
	m_pnMeshFaceList  = new int     [m_nSubdivisions * 4];

	for(i = 0; i < m_nSubdivisions + 1; i++)
	{
		// Precomputed normalized circle

		CMatrix rotMatrix;
		rotMatrix.BuildRotationY(((float)i / (float)m_nSubdivisions) * 360.0f);
		m_pv3Circle[i].Set(1.0f, 0.0f, 0.0f);
		rotMatrix.Transform(&m_pv3Circle[i]);

		// Precomputed mapping

		m_pv2MeshMapping[i                      ].Set((float)i / (float)m_nSubdivisions, 0.0f);
		m_pv2MeshMapping[i + m_nSubdivisions + 1].Set((float)i / (float)m_nSubdivisions, 1.0f);

		// Precomputed face list 

		if(i < m_nSubdivisions)
		{
			m_pnMeshFaceList[i * 4 + 0] = i;
			m_pnMeshFaceList[i * 4 + 1] = i + m_nSubdivisions + 1;
			m_pnMeshFaceList[i * 4 + 2] = i + m_nSubdivisions + 2;
			m_pnMeshFaceList[i * 4 + 3] = i + 1;
		}
	}

	for(i = 0; i < valueHorRingCount.GetValue(); i++)
	{
		SRing ring;

		ring.eType                 = SRing::HORIZONTAL;
		ring.fRadius               = ComputeRandWithVariation(1.0f, valueHorVariation.GetValue());
		ring.fWidth                = ComputeRandWithVariation(1.0f, valueHorVariation.GetValue());
		ring.fAlpha                = ComputeRandWithVariation(1.0f, valueHorVariation.GetValue());
		ring.fDistToCenter         = ComputeLERP(-0.5f, 0.5f, (float)i / (float)valueHorRingCount.GetValue()) + ComputeRandWithVariation((float)i / (float)valueHorRingCount.GetValue(), valueHorVariation.GetValue());
		ring.fRotSpeed             = ComputeRandWithVariation(1.0f, valueHorVariation.GetValue());
		ring.fRotAngleStart        = ComputeRand(0.0f, 360.0f);
		ring.fOscillationAmplitude = ComputeRandWithVariation(1.0f, valueHorVariation.GetValue());
		ring.fOscillationFrequency = ComputeRandWithVariation(1.0f, valueHorVariation.GetValue());;
		ring.fOscillationOffset    = ComputeRand(0.0f, 1.0f);

		if(rand() % 2)
		{
			ring.fRotSpeed = -ring.fRotSpeed;
		}

		m_vecRings.push_back(ring);
	}

	for(i = 0; i < valueVertRingCount.GetValue(); i++)
	{
		SRing ring;

		ring.eType                 = SRing::VERTICAL;
		ring.fRadius               = ComputeRandWithVariation(1.0f, valueVertVariation.GetValue());
		ring.fWidth                = ComputeRandWithVariation(1.0f, valueVertVariation.GetValue());
		ring.fAlpha                = ComputeRandWithVariation(1.0f, valueVertVariation.GetValue());
		ring.fDistToCenter         = ComputeLERP(-0.5f, 0.5f, (float)i / (float)valueVertRingCount.GetValue()) + ComputeRandWithVariation((float)i / (float)valueVertRingCount.GetValue(), valueVertVariation.GetValue());
		ring.fRotSpeed             = ComputeRandWithVariation(1.0f, valueVertVariation.GetValue());
		ring.fRotAngleStart        = ComputeRand(0.0f, 360.0f);
		ring.fOscillationAmplitude = ComputeRandWithVariation(1.0f, valueVertVariation.GetValue());
		ring.fOscillationFrequency = ComputeRandWithVariation(1.0f, valueVertVariation.GetValue());;
		ring.fOscillationOffset    = ComputeRand(0.0f, 1.0f);

		if(rand() % 2)
		{
			ring.fRotSpeed = -ring.fRotSpeed;
		}

		m_vecRings.push_back(ring);
	}

	return true;
}

bool CFXRings::Free()
{
	if(m_pResHorTexture)
	{
		m_pResHorTexture->RemoveDependency(this);
		m_pResHorTexture = NULL;
	}

	if(m_pResVertTexture)
	{
		m_pResVertTexture->RemoveDependency(this);
		m_pResVertTexture = NULL;
	}

	SAFE_DELETE_A(m_pv3Circle);
	SAFE_DELETE_A(m_pv3MeshVertices);
	SAFE_DELETE_A(m_pv2MeshMapping);
	SAFE_DELETE_A(m_pnMeshFaceList);

	m_vecRings.clear();

	return true;
}

bool CFXRings::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResHorTexture)
	{
		FXRuntimeError("Horizontal texture resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pHorTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResHorTexture)->GetTexture2D());

	if(!pHorTexture)
	{
		FXRuntimeError("WARNING: Horizontal texture not available");
		return false;
	}

	if(!m_pResVertTexture)
	{
		FXRuntimeError("Vertical texture resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pVertTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResVertTexture)->GetTexture2D());

	if(!pVertTexture)
	{
		FXRuntimeError("WARNING: Texture not available");
		return false;
	}

	// Vars

	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valueHorRingRadius;
	CVarFloat::CValueFloat valueHorRingWidth;
	CVarFloat::CValueFloat valueVertRingRadius;
	CVarFloat::CValueFloat valueVertRingWidth;
	CVarFloat::CValueFloat valueCylinderHeight;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueKeepPreviousCamera;
	CVarFloat::CValueFloat valueOscillationAmplitude;
	CVarFloat::CValueFloat valueOscillationFrequency;
	CVarFloat::CValueFloat valueRotationSpeed;
	CVarFloat::CValueFloat valueTileU;
	CVarFloat::CValueFloat valueTileV;

	EvaluateVar("Alpha",                  fEffectTime, &valueAlpha);
	EvaluateVar("Horizontal Ring Radius", fEffectTime, &valueHorRingRadius);
	EvaluateVar("Horizontal Ring Width",  fEffectTime, &valueHorRingWidth);
	EvaluateVar("Vertical Ring Radius",   fEffectTime, &valueVertRingRadius);
	EvaluateVar("Vertical Ring Width",    fEffectTime, &valueVertRingWidth);
	EvaluateVar("Cylinder Height",        fEffectTime, &valueCylinderHeight);
	EvaluateVar("Blend Mode",             fEffectTime, &valueBlendMode);
	EvaluateVar("Keep Previous Camera",   fEffectTime, &valueKeepPreviousCamera);
	EvaluateVar("Oscillation Amplitude",  fEffectTime, &valueOscillationAmplitude);
	EvaluateVar("Oscillation Frequency",  fEffectTime, &valueOscillationFrequency);
	EvaluateVar("Rotation Speed",         fEffectTime, &valueRotationSpeed);
	EvaluateVar("Tile U",                 fEffectTime, &valueTileU);
	EvaluateVar("Tile V",                 fEffectTime, &valueTileV);

	// States

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
	UtilGL::States::Set(UtilGL::States::BFCULLING,    UtilGL::States::DISABLED);

	UtilGL::States::Set(UtilGL::States::SRCBLEND, nSrcBlend);
	UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);

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

	CMatrix mtxTexturing;
	mtxTexturing.BuildScaling(valueTileU.GetValue(), valueTileV.GetValue(), 1.0f);

	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD,   mtxAddWorld);
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_TEXTURE, mtxTexturing);

	// Render

	glEnableClientState(GL_VERTEX_ARRAY);

	VECRINGS::iterator it;

	for(it = m_vecRings.begin(); it != m_vecRings.end(); ++it)
	{
		float fDistToCenter         = it->fDistToCenter         * valueCylinderHeight.GetValue();
		float fRotSpeed             = it->fRotSpeed             * valueRotationSpeed.GetValue();
		float fOscillationAmplitude = it->fOscillationAmplitude * valueOscillationAmplitude.GetValue();
		float fOscillationFrequency = it->fOscillationFrequency * valueOscillationFrequency.GetValue();
		float fHorRingWidth         = it->fWidth                * valueHorRingWidth.GetValue();
		float fHorRingRadius        = it->fRadius               * valueHorRingRadius.GetValue();
		float fVertRingWidth        = it->fWidth                * valueVertRingWidth.GetValue();
		float fVertRingRadius       = it->fRadius               * valueVertRingRadius.GetValue();

		if(!IS_ZERO(fOscillationAmplitude))
		{
			fDistToCenter += sinf(((it->fOscillationOffset + fEffectTime) * PI) * fOscillationFrequency) * fOscillationAmplitude;
		}

		CMatrix localMatrix = mtxAddLocal;
		localMatrix.PreRotateY(it->fRotAngleStart + (fRotSpeed * 360.0f * fEffectTime));
		localMatrix.Translate (0.0f, fDistToCenter, 0.0f);

		// Compute Mesh

		for(int i = 0; i < m_nSubdivisions + 1; i++)
		{
			float fRadius = it->eType == SRing::HORIZONTAL ? fHorRingRadius : fVertRingRadius;
			float fWidth  = it->eType == SRing::HORIZONTAL ? fHorRingWidth  : fVertRingWidth;

			m_pv3MeshVertices[i] = m_pv3Circle[i] * fRadius;
			m_pv3MeshVertices[i + m_nSubdivisions + 1] = m_pv3MeshVertices[i];

			if(it->eType == SRing::HORIZONTAL)
			{
				if(IS_ZERO(fRadius)) fRadius = 0.001f;
				float fWidthRatio = fWidth / fRadius;
				m_pv3MeshVertices[i + m_nSubdivisions + 1] = m_pv3MeshVertices[i + m_nSubdivisions + 1] * (1.0f - fWidthRatio);
			}

			if(it->eType == SRing::VERTICAL)
			{
				m_pv3MeshVertices[i + m_nSubdivisions + 1].SetY(m_pv3MeshVertices[i + m_nSubdivisions + 1].Y() + fWidth);
			}

			localMatrix.Transform(&m_pv3MeshVertices[i]);
			localMatrix.Transform(&m_pv3MeshVertices[i + m_nSubdivisions + 1]);
		}

		// Draw

		CVector4 v4Color(1.0f, 1.0f, 1.0f, it->fAlpha * valueAlpha.GetValue());
		UtilGL::States::SetColor(v4Color);

		glVertexPointer(3, GL_FLOAT, 0, m_pv3MeshVertices);

		UtilGL::Texturing::CTexture2D* pTexture = it->eType == SRing::HORIZONTAL ? pHorTexture : pVertTexture;

		if(pTexture)
		{
			UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, m_pv2MeshMapping);
			pTexture->SetActive();
			pTexture->SetTiling(true);
		}
		else
		{
			UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glDrawRangeElements(GL_QUADS, 0, (m_nSubdivisions * 2) + 1, m_nSubdivisions * 4, GL_UNSIGNED_INT, m_pnMeshFaceList);
	}

 	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING,     UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING,    UtilGL::States::ENABLED);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);

	return true;
}
