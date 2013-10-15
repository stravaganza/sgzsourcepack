// ==[ File ]===================================================================================
//
//  -Name     : BuildAnimation.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Animation related.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


// ==[ Implementation ]=========================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool IsAnimated(INode* pMaxNode, bool *pbPosOut, bool* pbSclOut, bool* pbRotOut)
//
//  - Purpose   : Checks if a given node has animation. If it indeed has
//                animation, it returns true, otherwise false.
//                Additional information can be retreived trough parameters,
//                telling wheter they have position/scaling/rotation animation.
//                This can be useful to export only neccesary information.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::IsAnimated(INode* pMaxNode, bool *pbPosOut, bool* pbSclOut, bool* pbRotOut)
{
	if(pMaxNode == NULL)
	{
		return false;
	}

	AffineParts ap;
	Matrix3 tm;
	Point3  startPos;
	Point3  startScl;
	Quat    startRot;

	if(pbPosOut) *pbPosOut = false;
	if(pbSclOut) *pbSclOut = false;
	if(pbRotOut) *pbRotOut = false;

	bool bResult = false;

	// Speedup:

	bool bPosChecked = false;
	bool bSclChecked = false;
	bool bRotChecked = false;

	int  tStart = SECONDS_TO_TICKS(m_fStartTime);
	int  tEnd   = SECONDS_TO_TICKS(m_fEndTime);
	int  tInc   = GetTicksPerFrame() * m_nAnimCheckStep;

	for(int t = tStart; t <= tEnd; t += tInc)
	{
		tm = GetMaxTM(pMaxNode, TICKS_TO_SECONDS(t), false, IsBoneInList(m_vecBones, pMaxNode));

		decomp_affine(tm, &ap); // Get individual transformation values

		// If t == start, we get the initial transformation values we'll use
		// later (t > start) to check if they change.

		if(t != tStart)
		{
			// Position check:

			if(!bPosChecked && pbPosOut)
			{
				if(!Point3Equal(ap.t, startPos))
				{
					*pbPosOut   = true;
					bResult     = true;
					bPosChecked = true;
				}
			}

			// Scale check:

			if(!bSclChecked && pbSclOut)
			{
				if(!Point3Equal(ap.k, startScl))
				{
					*pbSclOut   = true;
					bResult     = true;
					bSclChecked = true;
				}
			}

			// Rotation check:

			if(!bRotChecked && pbRotOut)
			{
				if(!QuatEqual(ap.q, startRot))
				{
					*pbRotOut   = true;
					bResult     = true;
					bRotChecked = true;
				}
			}

			// Stop if we've checked them all

			if(bPosChecked && bSclChecked && bRotChecked)
			{
				break;
			}
		}
		else
		{
			// Initialize start values:

			startPos = ap.t;
			startScl = ap.k;
			startRot = ap.q;
		}
	}

	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool HasAnimCameraSettings(INode* pMaxCameraNode)
//
//  - Purpose   : Checks if a given camera has animated settings (FOV...).
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::HasAnimCameraSettings(INode* pMaxCameraNode)
{
	int   nCamSettings = (int)(m_fDuration * m_nNumCamSettingsPerSec);
	float fIncSeconds  = TICKS_TO_SECONDS(GetTicksPerFrame() * m_nAnimCheckStep);
	
	ObjectState   os         = pMaxCameraNode->EvalWorldState(m_fStartTime); // Evaluate object pipeline result at t = 0
	CameraObject* pMaxCamera = (CameraObject *)os.obj;

	assert(pMaxCamera);

	CameraState maxCameraSettings;
	Interval    validRange = m_pMaxInterface->GetAnimRange();

	// Get camera settings at time 0
	pMaxCamera->EvalCameraState(m_fStartTime, validRange, &maxCameraSettings);

	bool bAnimNear, bAnimFar, bAnimFOV;
	bAnimNear = bAnimFar = bAnimFOV = false;

	float fStartNear = maxCameraSettings.nearRange;
	float fStartFar  = maxCameraSettings.farRange;
	float fStartFOV  = maxCameraSettings.fov;
	
	float fTime;
	
	for(fTime = m_fStartTime; fTime < m_fEndTime; fTime += fIncSeconds)
	{
		// Get the camera settings at time fTime and check if they are different from start settings
		pMaxCamera->EvalCameraState((int)(SECONDS_TO_TICKS(fTime)), validRange, &maxCameraSettings);

		if(!IS_ZERO(maxCameraSettings.nearRange - fStartNear)) bAnimNear = true;
		if(!IS_ZERO(maxCameraSettings.farRange  - fStartFar))  bAnimFar  = true;
		if(!IS_ZERO(maxCameraSettings.fov       - fStartFOV))  bAnimFOV  = true;

		if(bAnimNear || bAnimFar || bAnimFOV) break;
	}

	return bAnimNear || bAnimFar || bAnimFOV;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool HasAnimLightSettings(INode* pMaxLightNode)
//
//  - Purpose   : Checks if a given light has animated settings (color...).
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::HasAnimLightSettings(INode* pMaxLightNode)
{
	int   nLightSettings = (int)(m_fDuration * m_nNumLightSettingsPerSec);
	float fIncSeconds  = TICKS_TO_SECONDS(GetTicksPerFrame() * m_nAnimCheckStep);
	
	ObjectState os        = pMaxLightNode->EvalWorldState(m_fStartTime); // Evaluate object pipeline result at t = 0
	GenLight*   pMaxLight = (GenLight *)os.obj;

	assert(pMaxLight);

	struct LightState maxLightSettings;
	Interval validRange = m_pMaxInterface->GetAnimRange();

	// Get light settings at time 0
	pMaxLight->EvalLightState(m_fStartTime, validRange, &maxLightSettings);

	bool bColorAnim, bIntensAnim, bFallOffAnim;
	bColorAnim = bIntensAnim = bFallOffAnim = false;

	Color col = maxLightSettings.color;
	Point3 startCol(col.r, col.g, col.b);
	float  fStartIntens  = maxLightSettings.intens;
	float  fStartFallOff = maxLightSettings.fallsize;
	
	float fTime;
	
	for(fTime = m_fStartTime; fTime < m_fEndTime; fTime += fIncSeconds)
	{
		// Get light settings at time fTime and check if they are different from start settings
		pMaxLight->EvalLightState((int)(SECONDS_TO_TICKS(fTime)), validRange, &maxLightSettings);

		Point3 sampleCol(maxLightSettings.color.r, maxLightSettings.color.g, maxLightSettings.color.b);

		if(!Point3Equal(startCol, sampleCol))                   bColorAnim   = true;
		if(!IS_ZERO(maxLightSettings.intens   - fStartIntens )) bIntensAnim  = true;
		if(!IS_ZERO(maxLightSettings.fallsize - fStartFallOff))	bFallOffAnim = true;

		if(bColorAnim || bIntensAnim || bFallOffAnim) break;
	}

	return bColorAnim || bIntensAnim || bFallOffAnim;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool IsKnownController(Control *pController)
//
//  - Purpose   : Returns true if we know how to export this controller,
//                otherwise false.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::IsKnownController(Control *pController)
{
	if(!pController)
	{
		return false;
	}

	int nPartA, nPartB;

	nPartA = pController->ClassID().PartA();
	nPartB = pController->ClassID().PartB();

	if(nPartB != 0x00)
	{
		return false;
	}

	switch(nPartA)
	{
		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_ROTATION_CLASS_ID:
		case TCBINTERP_SCALE_CLASS_ID:
		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_ROTATION_CLASS_ID:
		case HYBRIDINTERP_SCALE_CLASS_ID:
		case LININTERP_POSITION_CLASS_ID:
		case LININTERP_ROTATION_CLASS_ID:
		case LININTERP_SCALE_CLASS_ID:

			return true;

		default:

			break;
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool BuildNodeAnimation(INode* pMaxNode,
//                                        CNode* pAnimNode)
//
//  - Purpose   : Builds the animation of a given node.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::BuildNodeAnimation(INode* pMaxNode, CNode* pAnimNode)
{
	bool bHasPosAnim, bHasSclAnim, bHasRotAnim;

	pAnimNode->SetRelativeTM(GetTM(pMaxNode, m_fStartTime, false, IsBoneInList(m_vecBones, pMaxNode)));
	pAnimNode->SetAbsoluteTM(GetTM(pMaxNode, m_fStartTime, true,  IsBoneInList(m_vecBones, pMaxNode)));

	int nStartTicks = SECONDS_TO_TICKS(m_fStartTime);

	AffineParts ap;
	Matrix3 tm = GetMaxTM(pMaxNode, m_fStartTime, false, IsBoneInList(m_vecBones, pMaxNode));
	decomp_affine(tm, &ap);

	if(TMNegParity(tm))
	{
		ap.k = -ap.k;
	}

	pAnimNode->SetInitialPos(Point3ToVector3(ap.t));
	pAnimNode->SetInitialRot(QuatToQuaternion(ap.q));
	pAnimNode->SetInitialScl(QuatToQuaternion(ap.u), Point3ToVector3(ap.k));

	if(IsAnimated(pMaxNode, &bHasPosAnim, &bHasSclAnim, &bHasRotAnim) == false)
	{
		return true;
	}

	// Build

	LOG.Write("\nAnimation");
	LOG.IncIndentation(2);

	if(bHasPosAnim) pAnimNode->SetPosAnimation(BuildAnimPos(pMaxNode));
	if(bHasSclAnim) pAnimNode->SetSclAnimation(BuildAnimScl(pMaxNode));
	if(bHasRotAnim) pAnimNode->SetRotAnimation(BuildAnimRot(pMaxNode));

	LOG.DecIndentation(2);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CAnimPos* BuildAnimPos(INode* pMaxNode)
//
//  - Purpose   : Builds a CAnimPos from a node, representing its position anim.
//
// -----------------------------------------------------------------------------
CAnimPos* CStravaganzaMaxTools::BuildAnimPos(INode* pMaxNode)
{
	int nCount;

	// If export max keys has been selected (instead of keys sampling), we have to
	// check first if we know how to export them. If the position animation
	// controller is unknown or invalid, we force sampling for this one.

	bool bForceSampling  = true; /* WE DON'T SUPPORT MAX KEYFRAMING YET!! */

	Control* pController = pMaxNode->GetTMController()->GetPositionController();

	if(m_eAnimType == ANIMATION_MAXKEYS)
	{
		if(IsKnownController(pController) == false)
		{
			bForceSampling = true;
		}
	}

	if(m_eAnimType == ANIMATION_SAMPLING || bForceSampling)
	{
		LOG.Write("\nSampled translation (%u samples)", m_nNumPosSamples);

		// Sample translation keys:

		CAnimPosSampled* pPosAnimSampled   = new CAnimPosSampled;
		CAnimPosSampled::TSample* pSamples = new CAnimPosSampled::TSample[m_nNumPosSamples];

		Matrix3     tm;
		AffineParts ap;
		float       fT, fIncSeconds = m_fDuration / (float)(m_nNumPosSamples - 1);
		int         nSample;
	
		for(fT = m_fStartTime, nSample = 0; nSample < m_nNumPosSamples; fT += fIncSeconds, nSample++)
		{
			tm = GetMaxTM(pMaxNode, fT, false, IsBoneInList(m_vecBones, pMaxNode));
			decomp_affine(tm, &ap);

			pSamples[nSample].pos = Point3ToVector3(ap.t);
		}

		pPosAnimSampled->SetData(pSamples, m_nNumPosSamples, m_nNumPosSamplesPerSec, 0.0f);
		return pPosAnimSampled;
	}
	else if(m_eAnimType == ANIMATION_MAXKEYS)
	{
		IKeyControl *pKCI = GetKeyControlInterface(pController);

		// TCB
		if(pKCI && pController->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nTranslation TCB (%u keys)", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				ITCBPoint3Key key;
				pKCI->GetKey(nCount, &key);

				float    fVal;
				CVector3 v3Pos;

				// Building not implemented, insert build code here.
				// Anyways, show what we have.

				fVal  = key.time;
				v3Pos = Point3ToVector3(key.val);

				fVal  = key.tens;
				fVal  = key.cont;
				fVal  = key.bias;
				fVal  = key.easeIn;
				fVal  = key.easeOut;
			}
		}
		// Bezier
		else if(pKCI && pController->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nTranslation Bezier (%u keys)", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				IBezPoint3Key key;
				pKCI->GetKey(nCount, &key);

				float    fVal;
				int      nFlags;
				CVector3 v3Pos, v3InTan, v3OutTan;

				// Building not implemented, insert build code here.
				// Anyways, show what we have.

				fVal  = key.time;
				v3Pos = Point3ToVector3(key.val);

				v3InTan  = Point3ToVector3(key.intan);
				v3OutTan = Point3ToVector3(key.outtan);
				nFlags   = key.flags;
			}
		}

		// Linear
		else if (pKCI && pController->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nTranslation linear keys", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				ILinPoint3Key key;
				pKCI->GetKey(nCount, &key);

				float    fVal;
				CVector3 v3Pos;

				fVal  = key.time;
				v3Pos = Point3ToVector3(key.val);
			}
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CAnimScl* BuildAnimScl(INode* pMaxNode)
//
//  - Purpose   : Builds a CAnimScl from a node, representing its scale animation.
//
// -----------------------------------------------------------------------------
CAnimScl* CStravaganzaMaxTools::BuildAnimScl(INode* pMaxNode)
{
	int nCount;

	// If export max keys has been indicated (instead of keys sampling), we have to
	// check first if we know how to export them. If the scale animation
	// controller is unknown or invalid, we force sampling for this one.

	bool bForceSampling  = true; /* MAX KEYFRAMING NOT SUPPORTED YET */
	Control* pController = pMaxNode->GetTMController()->GetScaleController();

	if(m_eAnimType == ANIMATION_MAXKEYS)
	{
		if(IsKnownController(pController) == false)
		{
			bForceSampling = true;
		}
	}

	if(m_eAnimType == ANIMATION_SAMPLING || bForceSampling)
	{
		// Sample scale keys:

		LOG.Write("\nSampled scaling (%u samples)", m_nNumSclSamples);

		CAnimSclSampled* pSclAnimSampled   = new CAnimSclSampled;
		CAnimSclSampled::TSample* pSamples = new CAnimSclSampled::TSample[m_nNumSclSamples];

		Matrix3     tm;
		AffineParts ap;
		float       fT, fIncSeconds = m_fDuration / (float)(m_nNumSclSamples - 1);
		int         nSample;
	
		for(fT = m_fStartTime, nSample = 0; nSample < m_nNumSclSamples; fT += fIncSeconds, nSample++)
		{
			tm = GetMaxTM(pMaxNode, fT, false, IsBoneInList(m_vecBones, pMaxNode));
			decomp_affine(tm, &ap);

			if(TMNegParity(tm))
			{
				ap.k = -ap.k;
			}

			pSamples[nSample].axes  = QuatToQuaternion(ap.u); // Sample scale axis value
			pSamples[nSample].scale = Point3ToVector3 (ap.k); // Sample scale value
		}

		pSclAnimSampled->SetData(pSamples, m_nNumSclSamples, m_nNumSclSamplesPerSec, 0.0f);
		return pSclAnimSampled;
	}
	else if(m_eAnimType == ANIMATION_MAXKEYS)
	{
		IKeyControl *pKCI = GetKeyControlInterface(pController);

		// TCB
		if(pKCI && pController->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nScaling TCB (%u keys)", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				ITCBScaleKey key;
				pKCI->GetKey(nCount, &key);

				float       fVal;
				CVector3    v3Scl;
				CQuaternion quatAxes;

				// Building not implemented, insert build code here.
				// Anyways, show what we have.

				fVal     = key.time;
				v3Scl    = Point3ToVector3 (key.val.s);
				quatAxes = QuatToQuaternion(key.val.q);

				fVal  = key.tens;
				fVal  = key.cont;
				fVal  = key.bias;
				fVal  = key.easeIn;
				fVal  = key.easeOut;
			}
		}

		// Bezier
		else if(pKCI && pController->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nScaling Bezier (%u keys)", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				IBezScaleKey key;
				pKCI->GetKey(nCount, &key);

				int         nAux;
				float       fVal;
				CVector3    v3Scl, v3Aux;
				CQuaternion quatAxes;

				fVal     = key.time;
				v3Scl    = Point3ToVector3 (key.val.s);
				quatAxes = QuatToQuaternion(key.val.q);

				v3Aux = Point3ToVector3(key.intan);
				v3Aux = Point3ToVector3(key.outtan);
				nAux  = key.flags;
			}
		}

		// Linear
		else if (pKCI && pController->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nScaling linear (%u keys)", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				ILinScaleKey key;
				pKCI->GetKey(nCount, &key);

				CVector3    v3Scl;
				CQuaternion quatAxes;

				v3Scl    = Point3ToVector3 (key.val.s);
				quatAxes = QuatToQuaternion(key.val.q);
			}
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CAnimRot* BuildAnimRot(INode* pMaxNode)
//
//  - Purpose   : Builds a CAnimRot from a node, representing its rotation anim.
//
// -----------------------------------------------------------------------------
CAnimRot* CStravaganzaMaxTools::BuildAnimRot(INode* pMaxNode)
{
	int nCount;

	// If export max keys has been indicated (instead of keys sampling), we have to
	// check first if we know how to export them. If the rotation animation
	// controller is unknown or invalid, we force sampling for this one.

	bool bForceSampling  = true; /* MAX KEYFRAMING NOT SUPPORTED YET */

	Control* pController = pMaxNode->GetTMController()->GetRotationController();

	if(m_eAnimType == ANIMATION_MAXKEYS)
	{
		if(IsKnownController(pController) == false)
		{
			bForceSampling = true;
		}
	}

	if(m_eAnimType == ANIMATION_SAMPLING || bForceSampling)
	{
		// Sample rotation keys:

		LOG.Write("\nSampled rotation (%u samples)", m_nNumRotSamples);

		CAnimRotSampled* pRotAnimSampled   = new CAnimRotSampled;
		CAnimRotSampled::TSample* pSamples = new CAnimRotSampled::TSample[m_nNumRotSamples];
	
		Matrix3     tm;
		AffineParts ap;
		float       fT, fIncSeconds = m_fDuration / (float)(m_nNumRotSamples - 1);
		int         nSample;
	
		for(fT = m_fStartTime, nSample = 0; nSample < m_nNumRotSamples; fT += fIncSeconds, nSample++)
		{
			tm = GetMaxTM(pMaxNode, fT, false, IsBoneInList(m_vecBones, pMaxNode));
			decomp_affine(tm, &ap);

			pSamples[nSample].quat = QuatToQuaternion(ap.q);
		}

		pRotAnimSampled->SetData(pSamples, m_nNumRotSamples, m_nNumRotSamplesPerSec, 0.0f);
		return pRotAnimSampled;
	}
	else if(m_eAnimType == ANIMATION_MAXKEYS)
	{
		IKeyControl *pKCI = GetKeyControlInterface(pController);

		// TCB
		if(pKCI && pController->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nRotation TCB (%u keys)", nNumKeys);

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				ITCBRotKey key;
				pKCI->GetKey(nCount, &key);

				float       fVal;
				Quat        maxQuat(key.val);
				CQuaternion quat;

				// Building not implemented, insert build code here.
				// Anyways, show what we have.

				fVal = key.time;
				quat = QuatToQuaternion(maxQuat);

				fVal  = key.tens;
				fVal  = key.cont;
				fVal  = key.bias;
				fVal  = key.easeIn;
				fVal  = key.easeOut;
			}
		}

		// Bezier
		else if(pKCI && pController->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nRotation Bezier (%u keys)", nNumKeys);

			float       fVal;
			CQuaternion quat;

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				IBezQuatKey key;
				pKCI->GetKey(nCount, &key);

				// Only Quat. No intan/outtan/flags

				fVal = key.time;
				quat = QuatToQuaternion(key.val); // Quat
			}
		}

		// Linear
		else if (pKCI && pController->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0))
		{
			int nNumKeys = pKCI->GetNumKeys();

			LOG.Write("\nRotation linear (%u keys)", nNumKeys);

			float       fVal;
			CQuaternion quat;

			for(nCount = 0; nCount < nNumKeys; nCount++)
			{
				ILinRotKey key;
				pKCI->GetKey(nCount, &key);
					
				fVal = key.time;
				quat = QuatToQuaternion(key.val);
			}
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool BuildCameraSettings(INode* pMaxCameraNode, CCamera* pCamera)
//
//  - Purpose   : Builds the animated camera settings of a given camera.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::BuildCameraSettings(INode* pMaxCameraNode, CCamera* pCamera)
{
	assert(pMaxCameraNode);
	assert(pCamera);

	// Sample camera settings

	int   nCamSettings = (int)(m_fDuration * m_nNumCamSettingsPerSec);
	float fIncSeconds  = m_fDuration / (float)(nCamSettings - 1);

	ObjectState   os         = pMaxCameraNode->EvalWorldState(m_fStartTime); // Evaluate object pipeline result at t = 0
	CameraObject* pMaxCam    = (CameraObject *)os.obj;
	Interval      validRange = m_pMaxInterface->GetAnimRange();
	CameraState   maxCamSettings;

	assert(pMaxCam);
	pMaxCam->EvalCameraState((int)(SECONDS_TO_TICKS(m_fStartTime)), validRange, &maxCamSettings);

	float fNear = maxCamSettings.nearRange;

	if(fNear < 1.0f)
	{
		fNear = 1.0f;
	}

	pCamera->SetFOV (maxCamSettings.fov * RAD_TO_DEG);
	pCamera->SetNear(fNear);
	pCamera->SetFar (maxCamSettings.farRange);

	if(!HasAnimCameraSettings(pMaxCameraNode))
	{
		return true;
	}

	LOG.Write("\nSampled camera settings (%u)", nCamSettings);

	float fT;
	int   nSample = 0;

	CCamera::TSettings* pSamples = new CCamera::TSettings[nCamSettings];

	for(fT = m_fStartTime; nSample < nCamSettings; fT += fIncSeconds, nSample++)
	{
		// Get the camera settings at time fT

		pMaxCam->EvalCameraState((int)(SECONDS_TO_TICKS(fT)), validRange, &maxCamSettings);

		pSamples[nSample].fFOV  = RAD_TO_DEG * maxCamSettings.fov;
		pSamples[nSample].fNear = maxCamSettings.nearRange;
		pSamples[nSample].fFar  = maxCamSettings.farRange;

		if(pSamples[nSample].fNear < 1.0f)
		{
			pSamples[nSample].fNear = 1.0f;
		}
	}

	return pCamera->SetAnimCamSettingsData(pSamples, nCamSettings, m_nNumCamSettingsPerSec, 0.0f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool BuildLightSettings(INode* pMaxLightNode, CLight* pLight)
//
//  - Purpose   : Builds the animated settings of a given light.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::BuildLightSettings(INode* pMaxLightNode, CLight* pLight)
{
	assert(pMaxLightNode);
	assert(pLight);

	// Sample light settings

	int   nLightSettings = (int)(m_fDuration * m_nNumLightSettingsPerSec);
	float fIncSeconds    = m_fDuration / (float)(nLightSettings - 1);

	ObjectState os         = pMaxLightNode->EvalWorldState(m_fStartTime); // Evaluate object pipeline result at t = 0
	GenLight*   pMaxLight  = (GenLight *)os.obj;
	Interval    validRange = m_pMaxInterface->GetAnimRange();
	struct LightState maxLightSettings;

	assert(pMaxLight);
	pMaxLight->EvalLightState((int)(SECONDS_TO_TICKS(m_fStartTime)), validRange, &maxLightSettings);

	pLight->SetDefaultSettings();

	switch(maxLightSettings.type)
	{

	case OMNI_LGT:

		pLight->SetLightType(ID_LIGHT_OMNI);
        break;

	case SPOT_LGT:

		pLight->SetLightType   (ID_LIGHT_SPOT);
		pLight->SetSpotCutoff  (maxLightSettings.fallsize * 0.5f);
		pLight->SetSpotExponent(0.0f);
        break;

	case DIRECT_LGT:

		pLight->SetLightType(ID_LIGHT_DIRECTIONAL);
		break;

	default:

		// CLogger::NotifyWindow("%s light node: undefined type.", pLight->GetNodeName().data());
		return false;
	}

	Color col = maxLightSettings.color;
	CVector4 v4Color;
	
	v4Color = CVector3(col.r, col.g, col.b) * maxLightSettings.intens;
	
	if(v4Color.X() > 1.0f) v4Color.SetX(1.0f); if(v4Color.X() < 0.0f) v4Color.SetX(0.0f);
	if(v4Color.Y() > 1.0f) v4Color.SetY(1.0f); if(v4Color.Y() < 0.0f) v4Color.SetY(0.0f);
	if(v4Color.Z() > 1.0f) v4Color.SetZ(1.0f); if(v4Color.Z() < 0.0f) v4Color.SetZ(0.0f);

	pLight->SetAmbient (v4Color);
	pLight->SetDiffuse (v4Color);
	pLight->SetSpecular(v4Color);

	if(!HasAnimLightSettings(pMaxLightNode))
	{
		return true;
	}

	LOG.Write("\nSampled light settings (%u)", nLightSettings);

	float fT;
	int   nSample = 0;

	CLight::TSettings* pSamples = new CLight::TSettings[nLightSettings];

	for(fT = m_fStartTime; nSample < nLightSettings; fT += fIncSeconds, nSample++)
	{
		// Get light settings at time fT

		pMaxLight->EvalLightState((int)(SECONDS_TO_TICKS(fT)), validRange, &maxLightSettings);

		col     = maxLightSettings.color;
		v4Color = CVector3(col.r, col.g, col.b) * maxLightSettings.intens;

		if(v4Color.X() > 1.0f) v4Color.SetX(1.0f); if(v4Color.X() < 0.0f) v4Color.SetX(0.0f);
		if(v4Color.Y() > 1.0f) v4Color.SetY(1.0f); if(v4Color.Y() < 0.0f) v4Color.SetY(0.0f);
		if(v4Color.Z() > 1.0f) v4Color.SetZ(1.0f); if(v4Color.Z() < 0.0f) v4Color.SetZ(0.0f);

		pSamples[nSample].v4Ambient       = v4Color;
		pSamples[nSample].v4Diffuse       = v4Color;
		pSamples[nSample].v4Specular      = v4Color;
		pSamples[nSample].fAttenConstant  = 1.0f;
		pSamples[nSample].fAttenLinear    = 0.0f;
		pSamples[nSample].fAttenQuadratic = 0.0f;
		pSamples[nSample].fSpotCutoff     = maxLightSettings.fallsize * 0.5f;
		pSamples[nSample].fSpotExponent   = 0.0f;
	}

	return pLight->SetAnimLightSettingsData(pSamples, nLightSettings, m_nNumLightSettingsPerSec, 0.0f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CMatrix GetTM(INode* pMaxNode, float fTime,
//								bool bIncludeParent, bool bRemoveNonUniform)
//
//  - Purpose   : Returns the transformation matrix of a given MAX node.
//				  (Our matrix format)
//
//  - Note      : If bIncludeParent = false, Transformation matrix won't
//                include parent's transform, so it's relative to it.
//
// -----------------------------------------------------------------------------
CMatrix CStravaganzaMaxTools::GetTM(INode* pMaxNode, float fTime, bool bIncludeParent, bool bRemoveNonUniform)
{
	CMatrix matrix;
	Matrix3 maxTM;

	maxTM = GetMaxTM(pMaxNode, fTime, bIncludeParent, bRemoveNonUniform);

	Point3 row0, row1, row2, row3;

	row0 = maxTM.GetRow(0);
	row1 = maxTM.GetRow(1);
	row2 = maxTM.GetRow(2);
	row3 = maxTM.GetRow(3);

	// Transpose matrix to this form:
	//
	//    | R  R  R  T |
	//    | R  R  R  T |
	//    | R  R  R  T |
	//    | 0  0  0  1 |
	//

	matrix.SetRow0(row0.x, row1.x, row2.x, row3.x);
	matrix.SetRow1(row0.y, row1.y, row2.y, row3.y);
	matrix.SetRow2(row0.z, row1.z, row2.z, row3.z);
	matrix.SetRow3(0.0f,   0.0f,   0.0f,   1.0f  );

	return matrix;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CMatrix GetMaxTM(	INode* pMaxNode, float fTime,
//									bool bIncludeParent, bool bRemoveNonUniform)
//
//  - Purpose   : Returns the transformation matrix of a given MAX node.
//				  (Max matrix format)
//
//  - Note      : If bIncludeParent = false, Transformation matrix won't
//                include parent's transform, so it's relative to it.
//				  bRemoveNonUniform tells if non uniform scaling has to be
//				  removed from the transformation.
//
// -----------------------------------------------------------------------------
Matrix3 CStravaganzaMaxTools::GetMaxTM(INode* pMaxNode, float fTime, bool bIncludeParent, bool bRemoveNonUniform)
{
	CMatrix matrix;
	Matrix3 maxTM;

	maxTM = pMaxNode->GetNodeTM(SECONDS_TO_TICKS(fTime));

	if(bRemoveNonUniform)
	{
		maxTM = UniformMatrix(maxTM);
	}
		
	if(!bIncludeParent && pMaxNode->GetParentNode())
	{
		Matrix3 maxParentTM = pMaxNode->GetParentNode()->GetNodeTM(SECONDS_TO_TICKS(fTime));

		if(bRemoveNonUniform)
		{
			maxParentTM = UniformMatrix(maxParentTM);
		}

		maxTM = maxTM * Inverse(maxParentTM);
	}

	return maxTM;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : Matrix3 UniformMatrix(Matrix3 maxMatrix)
//
//  - Purpose   : Removes the non uniform scaling from a matrix.
//
// -----------------------------------------------------------------------------
Matrix3	CStravaganzaMaxTools::UniformMatrix(Matrix3 maxMatrix)
{
	AffineParts ap;
	Matrix3     mtx;

	decomp_affine(maxMatrix, &ap);

	ap.q.MakeMatrix(mtx);
	mtx.SetRow(3, ap.t);

	return mtx;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool Point3Equal(const Point3& p1, const Point3& p2)
//
//  - Purpose   : Checks if 2 Point3 can be considered as equal.
//                Returns true if they are, false if they don't.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::Point3Equal(const Point3& p1, const Point3& p2)
{
	if(!ARE_EQUAL(p1.x, p2.x)) return false;
	if(!ARE_EQUAL(p1.y, p2.y)) return false;
	if(!ARE_EQUAL(p1.z, p2.z)) return false;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool QuatEqual(const Quat& q1, const Quat& q2)
//
//  - Purpose   : Checks if 2 quaternions (Quat) can be considered as equal.
//                Returns true if they are, false if they don't.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::QuatEqual(const Quat& q1, const Quat& q2)
{
	Point3 axis1, axis2;
	float  fAngle1, fAngle2;

	AngAxisFromQ(q1, &fAngle1, axis1);
	AngAxisFromQ(q2, &fAngle2, axis2);

	if(ARE_EQUAL(fAngle1, fAngle2))
	{
		if(Point3Equal(axis1, axis2))
		{
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool TMNegParity(const Matrix3 &mat)
//
//  - Purpose   : Checks if a given matrix has negative scaling..
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::TMNegParity(const Matrix3 &mat)
{
	return ((DotProd(CrossProd(mat.GetRow(0), mat.GetRow(1)), mat.GetRow(2)) < 0.0) ? true : false);
}