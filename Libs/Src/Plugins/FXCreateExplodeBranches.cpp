#include "Plugins.h"
#include "FXCreateExplodeBranches.h"


const std::string CFXCreateExplodeBranches::CLASSNAME("Create+Explode Branches");

REGISTER_PLUGIN(CPluginFactory, CFXCreateExplodeBranches)


CFXCreateExplodeBranches::CFXCreateExplodeBranches()
{
	DefineVar("Time Value (0-2)",       CVarFloat::CLASSNAME);
	DefineVar("Num Branches",           CVarInt::CLASSNAME);
	DefineVar("Alpha",					CVarFloat::CLASSNAME);
	DefineVar("Origin X",               CVarFloat::CLASSNAME);
	DefineVar("Origin Y",               CVarFloat::CLASSNAME);
	DefineVar("Origin Z",               CVarFloat::CLASSNAME);
	DefineVar("Origin Dispertion",      CVarFloat::CLASSNAME);
	DefineVar("Dir X",                  CVarFloat::CLASSNAME);
	DefineVar("Dir Y",                  CVarFloat::CLASSNAME);
	DefineVar("Dir Z",                  CVarFloat::CLASSNAME);
	DefineVar("Dir Aperture Angle 1",   CVarFloat::CLASSNAME);
	DefineVar("Dir Aperture Angle 2",   CVarFloat::CLASSNAME);
	DefineVar("Branch Radius 1",        CVarFloat::CLASSNAME);
	DefineVar("Branch Radius 2",        CVarFloat::CLASSNAME);
	DefineVar("Branch Length",          CVarFloat::CLASSNAME);
	DefineVar("Branch Sections",        CVarInt::CLASSNAME);
	DefineVar("Branch Keypoints",       CVarInt::CLASSNAME);
	DefineVar("Randomness (0-1)",       CVarFloat::CLASSNAME);
	DefineVar("Seed",                   CVarInt::CLASSNAME);
	DefineVar("Section Vertices",       CVarInt::CLASSNAME);
	DefineVar("Section Max Torsion",	CVarFloat::CLASSNAME);
	DefineVar("Accumulative Torsion",	CVarCombo::CLASSNAME);
	DefineVar("Keep Previous Camera",   CVarCombo::CLASSNAME);
	DefineVar("Keep Previous Lighting",	CVarCombo::CLASSNAME);
	DefineVar("Wireframe",			    CVarCombo::CLASSNAME);
	DefineVar("Explosion Intensity",	CVarFloat::CLASSNAME);
	DefineVar("Explosion Gravity",		CVarFloat::CLASSNAME);
	DefineVar("Shininess",		        CVarFloat::CLASSNAME);
	DefineVar("Specular Strength",		CVarFloat::CLASSNAME);
	DefineVar("Precalced Sections",		CVarInt::CLASSNAME);
	DefineVar("Wireframe Color",		CVarColor::CLASSNAME);

	DefineAddTransformVars(this);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Time Value (0-2)"    ))->SetConstant(1.0f);
	((CVarInt*  )GetVar("Num Branches"        ))->SetConstant(5);
	((CVarInt*  )GetVar("Num Branches"        ))->LockConstant(true);
	((CVarFloat*)GetVar("Alpha"				  ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Origin X"            ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Origin Y"            ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Origin Z"            ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Origin X"            ))->LockConstant(true);
	((CVarFloat*)GetVar("Origin Y"            ))->LockConstant(true);
	((CVarFloat*)GetVar("Origin Z"            ))->LockConstant(true);
	((CVarFloat*)GetVar("Origin Dispertion"   ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Origin Dispertion"   ))->LockConstant(true);
	((CVarFloat*)GetVar("Dir X"               ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Dir Y"               ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Dir Z"               ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Dir X"               ))->LockConstant(true);
	((CVarFloat*)GetVar("Dir Y"               ))->LockConstant(true);
	((CVarFloat*)GetVar("Dir Z"               ))->LockConstant(true);
	((CVarFloat*)GetVar("Dir Aperture Angle 1"))->SetConstant(20.0f);
	((CVarFloat*)GetVar("Dir Aperture Angle 2"))->SetConstant(20.0f);
	((CVarFloat*)GetVar("Dir Aperture Angle 1"))->LockConstant(true);
	((CVarFloat*)GetVar("Dir Aperture Angle 2"))->LockConstant(true);
	((CVarFloat*)GetVar("Branch Radius 1"     ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Branch Radius 2"     ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Branch Length"       ))->SetConstant(50.0f);
	((CVarFloat*)GetVar("Branch Length"       ))->LockConstant(true);
	((CVarInt*)  GetVar("Branch Sections"     ))->SetConstant(20);
	((CVarInt*)  GetVar("Branch Sections"     ))->LockConstant(true);
	((CVarInt*)  GetVar("Branch Keypoints"    ))->SetConstant(7);
	((CVarInt*)  GetVar("Branch Keypoints"    ))->LockConstant(true);
	((CVarFloat*)GetVar("Randomness (0-1)"    ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Randomness (0-1)"    ))->LockConstant(true);
	((CVarInt*)  GetVar("Seed"                ))->SetConstant(0);
	((CVarInt*)  GetVar("Seed"                ))->LockConstant(true);
	((CVarInt*)  GetVar("Section Vertices"    ))->SetConstant(5);
	((CVarInt*)  GetVar("Section Vertices"    ))->LockConstant(true);
	((CVarFloat*)GetVar("Section Max Torsion" ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Section Max Torsion" ))->LockConstant(true);
	((CVarFloat*)GetVar("Explosion Intensity" ))->SetConstant(20.0f);
	((CVarFloat*)GetVar("Explosion Gravity"   ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Shininess"           ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Specular Strength"   ))->SetConstant(0.5f);
	((CVarInt  *)GetVar("Precalced Sections"  ))->SetConstant(150);
	((CVarInt  *)GetVar("Precalced Sections"  ))->LockConstant(true);

	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("No");
	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("Yes");
	((CVarCombo*)GetVar("Keep Previous Lighting"))->AddOption("No");
	((CVarCombo*)GetVar("Keep Previous Lighting"))->AddOption("Yes");
	((CVarCombo*)GetVar("Wireframe"))->AddOption("No");
	((CVarCombo*)GetVar("Wireframe"))->AddOption("Yes");
	((CVarCombo*)GetVar("Accumulative Torsion"))->AddOption("Yes");
	((CVarCombo*)GetVar("Accumulative Torsion"))->AddOption("No");

	GetVar("Num Branches"        )->ReloadFXOnChange(this);
	GetVar("Origin X"            )->ReloadFXOnChange(this);
	GetVar("Origin Y"            )->ReloadFXOnChange(this);
	GetVar("Origin Z"            )->ReloadFXOnChange(this);
	GetVar("Origin Dispertion"   )->ReloadFXOnChange(this);
	GetVar("Dir X"               )->ReloadFXOnChange(this);
	GetVar("Dir Y"               )->ReloadFXOnChange(this);
	GetVar("Dir Z"               )->ReloadFXOnChange(this);
	GetVar("Dir Aperture Angle 1")->ReloadFXOnChange(this);
	GetVar("Dir Aperture Angle 2")->ReloadFXOnChange(this);
	GetVar("Branch Length"       )->ReloadFXOnChange(this);
	GetVar("Branch Sections"     )->ReloadFXOnChange(this);
	GetVar("Branch Keypoints"    )->ReloadFXOnChange(this);
	GetVar("Randomness (0-1)"    )->ReloadFXOnChange(this);
	GetVar("Seed"                )->ReloadFXOnChange(this);
	GetVar("Section Vertices"    )->ReloadFXOnChange(this);
	GetVar("Section Max Torsion" )->ReloadFXOnChange(this);
	GetVar("Accumulative Torsion")->ReloadFXOnChange(this);
	GetVar("Precalced Sections"  )->ReloadFXOnChange(this);

	m_pResTexture = NULL;
}

CFXCreateExplodeBranches::~CFXCreateExplodeBranches()
{
	Free();
}

bool CFXCreateExplodeBranches::LoadData(CResourceList* pResourceList)
{
	int i, j, k;

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	CVarInt::CValueInt		valueNumBranches;
	CVarFloat::CValueFloat	valueOriginX;
	CVarFloat::CValueFloat	valueOriginY;
	CVarFloat::CValueFloat	valueOriginZ;
	CVarFloat::CValueFloat	valueOriginDispertion;
	CVarFloat::CValueFloat	valueDirX;
	CVarFloat::CValueFloat	valueDirY;
	CVarFloat::CValueFloat	valueDirZ;
	CVarFloat::CValueFloat	valueApertureAngle1;
	CVarFloat::CValueFloat	valueApertureAngle2;
	CVarFloat::CValueFloat	valueBranchRadius1;
	CVarFloat::CValueFloat	valueBranchRadius2;
	CVarFloat::CValueFloat	valueBranchLength;
	CVarInt::CValueInt		valueBranchSections;
	CVarInt::CValueInt		valueBranchKeypoints;
	CVarFloat::CValueFloat	valueRandomness;
	CVarInt::CValueInt		valueSeed;
	CVarInt::CValueInt		valueSectionVertices;
	CVarFloat::CValueFloat	valueSectionMaxTorsion;
	CVarCombo::CValueCombo	valueAccumulativeTorsion;
	CVarInt::CValueInt		valuePrecalcedSections;

	EvaluateVar("Num Branches"        , 0.0f, &valueNumBranches);
	EvaluateVar("Origin X"            , 0.0f, &valueOriginX);
	EvaluateVar("Origin Y"            , 0.0f, &valueOriginY);
	EvaluateVar("Origin Z"            , 0.0f, &valueOriginZ);
	EvaluateVar("Origin Dispertion"   , 0.0f, &valueOriginDispertion);
	EvaluateVar("Dir X"               , 0.0f, &valueDirX);
	EvaluateVar("Dir Y"               , 0.0f, &valueDirY);
	EvaluateVar("Dir Z"               , 0.0f, &valueDirZ);
	EvaluateVar("Dir Aperture Angle 1", 0.0f, &valueApertureAngle1);
	EvaluateVar("Dir Aperture Angle 2", 0.0f, &valueApertureAngle2);
	EvaluateVar("Branch Radius 1"     , 0.0f, &valueBranchRadius1);
	EvaluateVar("Branch Radius 2"     , 0.0f, &valueBranchRadius2);
	EvaluateVar("Branch Length"       , 0.0f, &valueBranchLength);
	EvaluateVar("Branch Sections"     , 0.0f, &valueBranchSections);
	EvaluateVar("Branch Keypoints"    , 0.0f, &valueBranchKeypoints);
	EvaluateVar("Randomness (0-1)"    , 0.0f, &valueRandomness);
	EvaluateVar("Seed"                , 0.0f, &valueSeed);
	EvaluateVar("Section Vertices"    , 0.0f, &valueSectionVertices);
	EvaluateVar("Section Max Torsion" , 0.0f, &valueSectionMaxTorsion);
	EvaluateVar("Accumulative Torsion", 0.0f, &valueAccumulativeTorsion);
	EvaluateVar("Precalced Sections"  , 0.0f, &valuePrecalcedSections);

	int nPrecalcedSections = valuePrecalcedSections.GetValue() < 2 ? 2 : valuePrecalcedSections.GetValue();

	srand(valueSeed.GetValue());

	if(valueBranchSections.GetValue() < 2)
	{
		valueBranchSections.SetValue(2);
	}

	float fMaxTorsion          = valueSectionMaxTorsion.GetValue();
	bool  bAccumulativeTorsion = valueAccumulativeTorsion.GetValue() == "Yes";

	if(fMaxTorsion > +170.0f) fMaxTorsion = +170.0f;
	if(fMaxTorsion < -170.0f) fMaxTorsion = -170.0f;

	int nKeyPoints = valueBranchKeypoints.GetValue() < 2 ? 2 : valueBranchKeypoints.GetValue();

	CQuaternion* pSectionQuats    = new CQuaternion[nKeyPoints];
	float*		 pfSegmentLengths = NULL;

	if(bAccumulativeTorsion == false)
	{
		pfSegmentLengths = new float[nKeyPoints];

		for(int j = 0; j < nKeyPoints; j++)
		{
			float fT1 = j / (float)(nKeyPoints - 1), fT2 = (j + 1) / (float)(nKeyPoints - 1);

			if(j == 0)
			{
				pfSegmentLengths[0] = ComputeRand(0.0f, fT2);
			}
			else
			{
				pfSegmentLengths[j] = ComputeRand(fT1, fT2);
			}
		}
	}

	CVector3 v3Origin(valueOriginX.GetValue(), valueOriginY.GetValue(), valueOriginZ.GetValue());
	CVector3 v3Dir   (valueDirX.GetValue(),    valueDirY.GetValue(),    valueDirZ.GetValue());
	v3Dir.Normalize();

	for(i = 0; i < valueNumBranches.GetValue(); i++)
	{
		// Allocate mem

		SBranch*           pBranch  = new SBranch;
		CSplineCatmullRom* pSpline  = new CSplineCatmullRom;

		pBranch->nPrecTotalSections = nPrecalcedSections;

		pBranch->pSpline            = pSpline;
		pBranch->ppv3PrecVertices   = new CVector3*[pBranch->nPrecTotalSections];
		pBranch->ppv3PrecNormals    = new CVector3*[pBranch->nPrecTotalSections];
		pBranch->ppv2PrecMapping    = new CVector2*[pBranch->nPrecTotalSections];
		pBranch->pv3PrecSectionDirs = new CVector3 [pBranch->nPrecTotalSections];
		pBranch->pv3PrecSections    = new CVector3 [pBranch->nPrecTotalSections];

		pBranch->nSections          = valueBranchSections.GetValue();
		pBranch->nVertsPerSection   = valueSectionVertices.GetValue() + 1; // +1 Because the additional section close vertex (needs different mapping coord).
		int nMaxVerts               = pBranch->nSections * pBranch->nVertsPerSection;
		int nFaces					= (pBranch->nSections - 1) * (pBranch->nVertsPerSection - 1);

		for(j = 0; j < pBranch->nPrecTotalSections; j++)
		{
			pBranch->ppv3PrecVertices[j] = new CVector3[pBranch->nVertsPerSection];
			pBranch->ppv3PrecNormals [j] = new CVector3[pBranch->nVertsPerSection];
			pBranch->ppv2PrecMapping [j] = new CVector2[pBranch->nVertsPerSection];
		}

		pBranch->bComplete        = false;
		pBranch->pv3Vertices      = new CVector3[nMaxVerts];
		pBranch->pv3Normals       = new CVector3[nMaxVerts];
		pBranch->pv2Mapping       = new CVector2[nMaxVerts];
		pBranch->pnIndices        = new int[nFaces * 4];

		pBranch->nExplFaces           = nFaces;
		pBranch->bExploded            = false;
		pBranch->pv3ExplFaceCenters   = new CVector3[nFaces];
		pBranch->pv3ExplDirection     = new CVector3[nFaces];
		pBranch->pv3ExplWorldVertices = new CVector3[nFaces * 4];
		pBranch->pv3ExplLocalVertices = new CVector3[nFaces * 4];
		pBranch->pv3ExplNormals       = new CVector3[nFaces * 4];
		pBranch->pv2ExplMapping       = new CVector2[nFaces * 4];

		float fCreationDuration = 1.0f - ComputeRand(0.0f, valueRandomness.GetValue());
		if(IS_ZERO(fCreationDuration)) fCreationDuration = 0.001f;
		pBranch->fTStart  = ComputeRand(0.0f, 1.0f - fCreationDuration);
		pBranch->fTEnd    = pBranch->fTStart + fCreationDuration;
		pBranch->fRadius1 = valueBranchRadius1.GetValue();
		pBranch->fRadius2 = valueBranchRadius2.GetValue();

		// Precalc some stuff:

		// Compute the spline

		for(j = 0; j < nKeyPoints; j++)
		{
			float fSegmentLength = valueBranchLength.GetValue() / (float)(nKeyPoints - 1);
			CSplineCatmullRom::TKey key;

			key.fTime = (float)j / (float)(nKeyPoints - 1);

			if(j == 0) // Start point
			{
				// Compute origin plus a random start given the dispertion radius

				CVector3 v3Perp(-v3Dir.Y(), v3Dir.X(), 0.0f);

				if(v3Perp == CVector3::NULLVECTOR)
				{
					v3Perp.Set(0.0f, -v3Dir.Z(), 0.0f);
				}

				v3Perp.Normalize();

				CMatrix mtxRot;
				CMatrix mtxSection;
				mtxSection.SetYAxis(v3Dir);
				mtxSection.SetZAxis(v3Perp);
				mtxSection.SetXAxis(v3Dir ^ v3Perp);
				pSectionQuats[j] = mtxSection.Quaternion();

				mtxRot.BuildRotation(ComputeRand(0.0f, 360.0f), v3Dir.X(), v3Dir.Y(), v3Dir.Z());
				mtxRot.Transform(&v3Perp, true);

				key.v3Value = v3Origin + (v3Perp * ComputeRand(0.0f, valueOriginDispertion.GetValue()));
			}
			else
			{
				float    fAngle1, fAngle2;
				CVector3 v3Previous;
				CVector3 v3Perp1, v3Perp2;

				CQuaternion quatPrevious;

				if(j == 1) // Second point (needs to be computed with the aperture angles
				{
					fAngle1 = valueApertureAngle1.GetValue();
					fAngle2 = valueApertureAngle2.GetValue();
				}
				else       // Rest
				{
					fAngle1 = fAngle2 = fMaxTorsion;
				}

				bool bDoAccumulative = bAccumulativeTorsion || j == 1;

				if(bDoAccumulative)
				{
					v3Perp1 = pSectionQuats[j - 1].Matrix().XAxis();
					v3Perp2 = pSectionQuats[j - 1].Matrix().ZAxis();

					pSpline->GetKeyValue(pSpline->GetNumKeys() - 1, &v3Previous);
					quatPrevious = pSectionQuats[j - 1];
				}
				else
				{
					v3Perp1 = pSectionQuats[1].Matrix().XAxis();
					v3Perp2 = pSectionQuats[1].Matrix().ZAxis();

					pSpline->GetKeyValue(0, &v3Previous);
					quatPrevious = pSectionQuats[1];
				}

				CMatrix mtxRot1, mtxRot2;
				mtxRot1.BuildRotation(ComputeRand(-fAngle1 * 0.5f, fAngle1 * 0.5f), v3Perp1.X(), v3Perp1.Y(), v3Perp1.Z());
				mtxRot2.BuildRotation(ComputeRand(-fAngle2 * 0.5f, fAngle2 * 0.5f), v3Perp2.X(), v3Perp2.Y(), v3Perp2.Z());

				pSectionQuats[j] = (mtxRot2.Quaternion() * mtxRot1.Quaternion()) * quatPrevious;
				CVector3 v3NewDir = pSectionQuats[j].Matrix().YAxis();

				if(bAccumulativeTorsion)
				{
					key.v3Value = v3Previous + (v3NewDir * fSegmentLength);
				}
				else
				{
					key.v3Value = v3Previous + (v3NewDir * (pfSegmentLengths[j] * valueBranchLength.GetValue()));
				}
			}

			pSpline->AddKey(key);
		}

		// Reorder directions

		for(j = 0; j < nKeyPoints - 1; j++)
		{
			pSectionQuats[j] = pSectionQuats[j + 1];
		}

		// Tesselate the spline

		for(j = 0; j < pBranch->nPrecTotalSections; j++)
		{
			float fT = (float)j / (float)(pBranch->nPrecTotalSections - 1);

			CVector3 v3Section, v3SectionDir, v3Perp;
			pSpline->Evaluate(fT, &v3Section);

			int nQuatA = fT * nKeyPoints;
			int nQuatB = nQuatA + 1;

			if(nQuatA >= nKeyPoints) nQuatA = nKeyPoints - 1;
			if(nQuatB >= nKeyPoints) nQuatB = nKeyPoints - 1;

			float fKeyPointLength = 1.0f / ((float)nKeyPoints - 1.0f);
			float fSLERP          = fmod(fT, fKeyPointLength) / fKeyPointLength;

			CQuaternion quat = pSectionQuats[nQuatA].SLERP(pSectionQuats[nQuatB], fSLERP);
			v3SectionDir = quat.Matrix().YAxis();
			v3Perp       = quat.Matrix().ZAxis();

			pBranch->pv3PrecSections   [j] = v3Section;
			pBranch->pv3PrecSectionDirs[j] = v3SectionDir;

			for(k = 0; k < pBranch->nVertsPerSection; k++)
			{
				float fTSection = (float)k / (float)(pBranch->nVertsPerSection - 1);

				CMatrix mtxRot;

				mtxRot.BuildRotation(	-360.0f * fTSection,
										v3SectionDir.X(), v3SectionDir.Y(), v3SectionDir.Z());

				CVector3 v3Vertex = v3Perp;
				mtxRot.Transform(&v3Vertex, true);

				pBranch->ppv3PrecVertices[j][k] = v3Vertex;
				pBranch->ppv3PrecNormals [j][k] = v3Vertex;
				pBranch->ppv2PrecMapping [j][k].Set(fTSection, fT);
			}
		}

		// Create Face List

		int nCount = 0;

		for(j = 0; j < pBranch->nSections - 1; j++)
		{
			for(k = 0; k < pBranch->nVertsPerSection - 1; k++)
			{
				int nBaseIndex = j * pBranch->nVertsPerSection + k;

				pBranch->pnIndices[nCount + 0] = nBaseIndex + pBranch->nVertsPerSection;
				pBranch->pnIndices[nCount + 1] = nBaseIndex;
				pBranch->pnIndices[nCount + 2] = nBaseIndex + 1;
				pBranch->pnIndices[nCount + 3] = nBaseIndex + pBranch->nVertsPerSection + 1;

				nCount += 4;
			}
		}

		// Compute exploding mesh

		GenerateBranch(pBranch, 1.0f);

		for(j = 0; j < nFaces; j++)
		{
			CVector3 v3Center(0.0f, 0.0f, 0.0f), v3Direction(0.0f, 0.0f, 0.0f);
			int nIndex = j * 4;

			for(k = 0; k < 4; k++)
			{
				pBranch->pv3ExplLocalVertices[nIndex + k] = pBranch->pv3Vertices[pBranch->pnIndices[nIndex + k]];
				pBranch->pv3ExplNormals      [nIndex + k] = pBranch->pv3Normals [pBranch->pnIndices[nIndex + k]];
				pBranch->pv2ExplMapping      [nIndex + k] = pBranch->pv2Mapping [pBranch->pnIndices[nIndex + k]];

				v3Center    = v3Center    + pBranch->pv3ExplLocalVertices[nIndex + k];
				v3Direction = v3Direction + pBranch->pv3ExplNormals      [nIndex + k];
			}

			v3Center = v3Center * 0.25f;

			if(v3Direction != CVector3::NULLVECTOR)
			{
				v3Direction.Normalize();
			}

			v3Direction = v3Direction + CVector3(ComputeRand(-1.0f, 1.0f), ComputeRand(-1.0f, 1.0f), ComputeRand(-1.0f, 1.0f));

			for(k = 0; k < 4; k++)
			{
				// To face local
				pBranch->pv3ExplLocalVertices[nIndex + k] = pBranch->pv3ExplLocalVertices[nIndex + k] - v3Center;
			}

			pBranch->pv3ExplFaceCenters[j] = v3Center;
			pBranch->pv3ExplDirection  [j] = v3Direction;
		}

		m_vecBranches.push_back(pBranch);
	}

	SAFE_DELETE_A(pSectionQuats);
	SAFE_DELETE_A(pfSegmentLengths);

	m_pResTexture->NotifyDependency(this);
	return true;
}

bool CFXCreateExplodeBranches::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	VECBRANCHES::iterator it;

	for(it = m_vecBranches.begin(); it != m_vecBranches.end(); ++it)
	{
		SBranch* pBranch = *it;
		assert(pBranch);

		for(int j = 0; j < pBranch->nPrecTotalSections; j++)
		{
			SAFE_DELETE_A(pBranch->ppv3PrecVertices[j]);
			SAFE_DELETE_A(pBranch->ppv3PrecNormals[j]);
			SAFE_DELETE_A(pBranch->ppv2PrecMapping[j]);
		}

		SAFE_DELETE  (pBranch->pSpline);
		SAFE_DELETE_A(pBranch->ppv3PrecVertices);
		SAFE_DELETE_A(pBranch->ppv3PrecNormals);
		SAFE_DELETE_A(pBranch->ppv2PrecMapping);
		SAFE_DELETE_A(pBranch->pv3PrecSectionDirs);
		SAFE_DELETE_A(pBranch->pv3PrecSections);

		SAFE_DELETE_A(pBranch->pv3Vertices);
		SAFE_DELETE_A(pBranch->pv3Normals);
		SAFE_DELETE_A(pBranch->pv2Mapping);
		SAFE_DELETE_A(pBranch->pnIndices);

		SAFE_DELETE_A(pBranch->pv3ExplFaceCenters);
		SAFE_DELETE_A(pBranch->pv3ExplDirection);
		SAFE_DELETE_A(pBranch->pv3ExplWorldVertices);
		SAFE_DELETE_A(pBranch->pv3ExplLocalVertices);
		SAFE_DELETE_A(pBranch->pv3ExplNormals);
		SAFE_DELETE_A(pBranch->pv2ExplMapping);

		SAFE_DELETE(pBranch);
	}

	m_vecBranches.clear();

	return true;
}

bool CFXCreateExplodeBranches::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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
		FXRuntimeError("WARNING: Texture not available, effect will draw without texturing");
	}
	else
	{
		pTexture->SetActive();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	CVarFloat::CValueFloat	valueTime;
	CVarFloat::CValueFloat	valueAlpha;
	CVarCombo::CValueCombo  valueKeepPreviousCamera;
	CVarCombo::CValueCombo  valueKeepPreviousLighting;
	CVarCombo::CValueCombo  valueWireframe;
	CVarFloat::CValueFloat	valueBranchRadius1;
	CVarFloat::CValueFloat	valueBranchRadius2;
	CVarFloat::CValueFloat	valueExplosionIntensity;
	CVarFloat::CValueFloat	valueExplosionGravity;
	CVarFloat::CValueFloat	valueShininess;
	CVarColor::CValueColor	valueWireColor;
	CVarFloat::CValueFloat	valueSpecularStrength;

	EvaluateVar("Time Value (0-2)"      , fEffectTime, &valueTime);
	EvaluateVar("Alpha"					, fEffectTime, &valueAlpha);
	EvaluateVar("Keep Previous Camera"  , fEffectTime, &valueKeepPreviousCamera);
	EvaluateVar("Keep Previous Lighting", fEffectTime, &valueKeepPreviousLighting);
	EvaluateVar("Wireframe"             , fEffectTime, &valueWireframe);
	EvaluateVar("Branch Radius 1"       , fEffectTime, &valueBranchRadius1);
	EvaluateVar("Branch Radius 2"       , fEffectTime, &valueBranchRadius2);
	EvaluateVar("Explosion Intensity"   , fEffectTime, &valueExplosionIntensity);
	EvaluateVar("Explosion Gravity"     , fEffectTime, &valueExplosionGravity);
	EvaluateVar("Shininess"             , fEffectTime, &valueShininess);
	EvaluateVar("Wireframe Color"       , fEffectTime, &valueWireColor);
	EvaluateVar("Specular Strength"     , fEffectTime, &valueSpecularStrength);

	float fAlpha = valueAlpha.GetValue();
	bool  bAlpha = true;

	float fSpecular = valueSpecularStrength.GetValue();

	if(fAlpha < 0.0f) return true;
	if(fAlpha > 1.0f) fAlpha = 1.0f;

	if(ARE_EQUAL(fAlpha, 1.0f)) bAlpha = false;

	// States

	if(valueKeepPreviousLighting.GetValue() == "Yes")
	{
		if(UtilGL::States::Get(UtilGL::States::LIGHTING) == UtilGL::States::ENABLED)
		{
			UtilGL::States::SetMaterialAmbient (GL_FRONT_AND_BACK, CVector4(0.2f, 0.2f, 0.2f, 0.0f));
			UtilGL::States::SetMaterialDiffuse (GL_FRONT_AND_BACK, CVector4(0.8f, 0.8f, 0.8f, fAlpha));
			UtilGL::States::SetMaterialSpecular(GL_FRONT_AND_BACK, CVector4(fSpecular, fSpecular, fSpecular, 1.0f));
			glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, (int)(valueShininess.GetValue() * 128.0f));
		}
		else
		{
			CVector4 v4Color = valueWireColor.GetValue();
			v4Color.SetW(fAlpha);
			UtilGL::States::SetColor(v4Color);
		}
	}
	else
	{
		// TODO: Use previous light states instead of manually setting up lighing

		UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::ENABLED);

		for(int i = 0; i < UtilGL::GetMaxLights(); i++)
		{
			UtilGL::States::Set(UtilGL::States::GetLight(i), UtilGL::States::DISABLED);
		}

		CMatrix mtxWorld = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_WORLD);
		CMatrix mtxView  = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW);

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);
		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);

		CVector4 v4White(1.0f, 1.0f, 1.0f, 1.0f);
		CVector4 v4Pos1 (0.0f, 0.0f,  +700.0f, 1.0f);
		CVector4 v4Pos2 (-700.0f, +700.0f,  -700.0f, 1.0f);

		UtilGL::States::Set(UtilGL::States::GetLight(0), UtilGL::States::ENABLED);
		glLighti(GL_LIGHT0,  GL_SPOT_CUTOFF, 180);
		glLightfv(GL_LIGHT0, GL_AMBIENT,  v4White.Data());
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  v4White.Data());
		glLightfv(GL_LIGHT0, GL_SPECULAR, v4White.Data());
		glLightfv(GL_LIGHT0, GL_POSITION, v4Pos1.Data());

		UtilGL::States::Set(UtilGL::States::GetLight(1), UtilGL::States::ENABLED);
		glLighti(GL_LIGHT1,  GL_SPOT_CUTOFF, 180);
		glLightfv(GL_LIGHT1, GL_AMBIENT,  v4White.Data());
		glLightfv(GL_LIGHT1, GL_DIFFUSE,  v4White.Data());
		glLightfv(GL_LIGHT1, GL_SPECULAR, v4White.Data());
		glLightfv(GL_LIGHT1, GL_POSITION, v4Pos2.Data());

		UtilGL::States::SetMaterialAmbient (GL_FRONT_AND_BACK, CVector4(0.2f, 0.2f, 0.2f, 0.0f));
		UtilGL::States::SetMaterialDiffuse (GL_FRONT_AND_BACK, CVector4(0.8f, 0.8f, 0.8f, fAlpha));
		UtilGL::States::SetMaterialSpecular(GL_FRONT_AND_BACK, CVector4(fSpecular, fSpecular, fSpecular, 1.0f));
		glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, (int)(valueShininess.GetValue() * 128.0f));

		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, mtxWorld);
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW,  mtxView);
	}

	if(valueWireframe.GetValue() == "Yes")
	{
		UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_LINE);
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	}

	if(bAlpha)
	{
		UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
		UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_INVSRCALPHA);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	}

	CMatrix  mtxCam;
	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	mtxCam.RotateX(v3AddCamAngles.X());
	mtxCam.RotateY(v3AddCamAngles.Y());
	mtxCam.RotateZ(v3AddCamAngles.Z());
	mtxCam.SetPosition(v3AddCamPos);

	CMatrix matCombined = mtxAddWorld * mtxAddLocal;
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, matCombined);

	if(valueKeepPreviousCamera.GetValue() != "Yes")
	{
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW, mtxCam.Inverse());

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
		glMatrixMode(GL_PROJECTION);
		gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);
	}
	else mtxCam = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW).Inverse();

	// Render

	VECBRANCHES::iterator it;

	for(it = m_vecBranches.begin(); it != m_vecBranches.end(); ++it)
	{
		SBranch* pBranch = *it;
		assert(pBranch);

		bool bComplete = false;

		float fT = valueTime.GetValue();

		if(valueTime.GetValue() <= 1.0f)
		{
			fT = (valueTime.GetValue() - pBranch->fTStart) / (pBranch->fTEnd - pBranch->fTStart);

			if(fT < 0.0f) continue;
			if(fT > 1.0f)
			{
				bComplete = true; fT = 1.0f;
			}
			else
			{
				pBranch->bComplete = false;
			}

			pBranch->bExploded = false;
		}
		else
		{
			UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::DISABLED);
			pBranch->bExploded = true;
		}

		pBranch->fRadius1            = valueBranchRadius1.GetValue();
		pBranch->fRadius2            = valueBranchRadius2.GetValue();
		pBranch->fExplosionIntensity = valueExplosionIntensity.GetValue();
		pBranch->fExplosionGravity   = valueExplosionGravity.GetValue();

		GenerateBranch(pBranch, fT);
		RenderBranch  (pBranch, fT);

		pBranch->bComplete = bComplete; // if next t >= 1.0, avoid redundant tesselation
	}

//	UtilGL::States::Set(UtilGL::States::LIGHTING,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING,   UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_FILL);

	return true;
}

// t < 0 = invisible, 0 < t < 1 = creating, t > 1 = exploding

bool CFXCreateExplodeBranches::GenerateBranch(SBranch* pBranch, float fT)
{
	assert(pBranch);

	int i, j;

	if(pBranch->bExploded == false)
	{
		// Creation

		int nSections = (fT * pBranch->nSections) + 1;

		if(nSections < 2)                  nSections = 2;
		if(nSections > pBranch->nSections) nSections = pBranch->nSections;

		pBranch->nRenderSections = nSections;

		float fPrecSectionTLength = 1.0f / ((float)pBranch->nPrecTotalSections - 1.0f);

		float fRadius1 = ComputeLERP(0.0f, pBranch->fRadius1, fT);
		float fRadius2 = ComputeLERP(0.0f, pBranch->fRadius2, fT);

		if(pBranch->bComplete == false)
		{
			// Create Mesh

			for(i = 0; i < nSections; i++)
			{
				float fTSection = i == nSections - 1 ? fT : (float)i / (float)(pBranch->nSections - 1);
				float fRadius   = ComputeLERP(fRadius1, fRadius2, (float)i / (float)(nSections - 1));

				// Compute which two precalculated sections this lays between.

				int nSectionA = (int)(fTSection * (float)pBranch->nPrecTotalSections);
				int nSectionB = nSectionA + 1;

				if(nSectionA > pBranch->nPrecTotalSections - 1) nSectionA = pBranch->nPrecTotalSections - 1;
				if(nSectionB > pBranch->nPrecTotalSections - 1) nSectionB = pBranch->nPrecTotalSections - 1;

				// Compute intermediate LERP value

				float fLERP    = fmod(fTSection, fPrecSectionTLength) / fPrecSectionTLength;
				int nBaseIndex = i * pBranch->nVertsPerSection;

				// Compute LERP

				CVector3 v3Section = pBranch->pv3PrecSections[nSectionA].LERP(pBranch->pv3PrecSections[nSectionB], fLERP);

				for(j = 0; j < pBranch->nVertsPerSection; j++)
				{
					CVector3 v3Vertex = pBranch->ppv3PrecVertices[nSectionA][j].LERP(pBranch->ppv3PrecVertices[nSectionB][j], fLERP);
					v3Vertex = v3Section + (v3Vertex * fRadius);
					pBranch->pv3Vertices[nBaseIndex + j] = v3Vertex;
					pBranch->pv3Normals [nBaseIndex + j] = pBranch->ppv3PrecNormals [nSectionA][j];
					pBranch->pv2Mapping [nBaseIndex + j].Set(ComputeLERP(pBranch->ppv2PrecMapping[nSectionA][j].X(), pBranch->ppv2PrecMapping[nSectionB][j].X(), fLERP),
															 ComputeLERP(pBranch->ppv2PrecMapping[nSectionA][j].Y(), pBranch->ppv2PrecMapping[nSectionB][j].Y(), fLERP));
				}
			}
		}
	}
	else
	{
		// Explosion

		fT -= 1.0f;
		float fExplT = fT;

		pBranch->nRenderSections = pBranch->nSections;

		for(i = 0; i < pBranch->nExplFaces; i++)
		{
			CVector3 v3Pos;
			CVector3 v3Dir = pBranch->pv3ExplDirection[i] * pBranch->fExplosionIntensity;

			// s = v0*t + 0.5(a*t^2)

			v3Pos = pBranch->pv3ExplFaceCenters[i] +
					(v3Dir * fExplT) + (CVector3(0.0f, -pBranch->fExplosionGravity, 0.0f) * fExplT * fExplT * 0.5f);

			int nIndex = i * 4;

			for(j = 0; j < 4; j++)
			{
				pBranch->pv3ExplWorldVertices[nIndex + j] = pBranch->pv3ExplLocalVertices[nIndex + j] + v3Pos;
			}
		}
	}

	return true;
}

bool CFXCreateExplodeBranches::RenderBranch(SBranch* pBranch, float fT)
{
	// Render Mesh

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	int nElements = (pBranch->nRenderSections - 1) * (pBranch->nVertsPerSection - 1) * 4;

	if(pBranch->bExploded == false)
	{
		glVertexPointer  (3, GL_FLOAT, 0, pBranch->pv3Vertices);
		glNormalPointer  (   GL_FLOAT, 0, pBranch->pv3Normals);
		glTexCoordPointer(2, GL_FLOAT, 0, pBranch->pv2Mapping);

		int nLastIndex = (pBranch->nRenderSections * pBranch->nVertsPerSection) - 1;

		glDrawRangeElements(GL_QUADS, 0, nLastIndex, nElements, GL_UNSIGNED_INT, pBranch->pnIndices);
	}
	else
	{
		glVertexPointer  (3, GL_FLOAT, 0, pBranch->pv3ExplWorldVertices);
		glNormalPointer  (   GL_FLOAT, 0, pBranch->pv3ExplNormals);
		glTexCoordPointer(2, GL_FLOAT, 0, pBranch->pv2ExplMapping);

		glDrawArrays(GL_QUADS, 0, nElements);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(pBranch->bExploded == false)
	{
		// Bottom

		glBegin(GL_POLYGON);

		glNormal3fv((-pBranch->pv3PrecSectionDirs[0]).Data());

		for(int i = 0; i < pBranch->nVertsPerSection; i++)
		{
			glTexCoord2fv(pBranch->pv2Mapping [pBranch->nVertsPerSection - i - 1].Data());	
			glVertex3fv  (pBranch->pv3Vertices[pBranch->nVertsPerSection - i - 1].Data());
		}

		glEnd();

		// Top

		int nBaseIndex = (pBranch->nRenderSections - 1) * pBranch->nVertsPerSection;

		glBegin(GL_POLYGON);

		glNormal3fv(pBranch->pv3PrecSectionDirs[pBranch->nRenderSections - 1].Data());

		for(i = 0; i < pBranch->nVertsPerSection; i++)
		{
			glTexCoord2fv(pBranch->pv2Mapping [nBaseIndex + i].Data());	
			glVertex3fv  (pBranch->pv3Vertices[nBaseIndex + i].Data());
		}

		glEnd();
	}

	return true;
}
