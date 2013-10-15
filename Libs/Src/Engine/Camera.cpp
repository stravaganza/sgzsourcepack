// ==[ File ]===================================================================================
//
//  -Name     : Camera.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CCamera ìmplementation.
//
// =============================================================================================

/*

	03/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : CCamera()
//
//  - Purpose   : CCamera's default constructor.
//
// -----------------------------------------------------------------------------
CCamera::CCamera()
{
	m_fCSEndTime   = FLOAT_MIN;
	m_fCSStartTime = FLOAT_MAX;

	SetDefaultSettings();

	m_uCSNumSamples = 0;
	m_pCSSamples    = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : ~CCamera()
//
//  - Purpose   : CCamera's destructor.
//
// -----------------------------------------------------------------------------
CCamera::~CCamera()
{
	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : static float GetVerticalFOV(float fHorizontalFOV, float fAspect)
//
//  - Purpose   : Returns the vertical FOV given the horizontal one and the
//				  window aspect ratio.
//
// -----------------------------------------------------------------------------
float CCamera::GetVerticalFOV(float fHorizontalFOV, float fAspect)
{
	return RAD_TO_DEG(2.0 * atanf(tanf(DEG_TO_RAD(fHorizontalFOV / 2.0f)) / fAspect));	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : void Place(const CVector3& pos, const CQuaternion& quat)
//
//  - Purpose   : Places the camera given a 3D position and an orientation
//                represented as a quaternion.
//
// -----------------------------------------------------------------------------
void CCamera::Place(const CVector3& pos, const CQuaternion& quat)
{
	CMatrix matrix = quat.Matrix();
	matrix.SetPosition(pos);

	SetAbsoluteTM(matrix);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : void Place(const CVector3& pos,
//                           const CVector3& targetPos,
//                           const CVector3& upVec)
//
//  - Purpose   : Places the camera given a 3D position, a target position where
//                the camera points to, and the up vector of the camera (the y
//                axis).
//
// -----------------------------------------------------------------------------
void CCamera::Place(const CVector3& pos, const CVector3& targetPos, const CVector3& upVec)
{
	CMatrix matrix;
	CVector3 view(pos, targetPos);

	matrix.SetXAxis(view ^ upVec);
	matrix.SetYAxis(upVec);
	matrix.SetZAxis(view);
	matrix.SetPosition(pos);

	SetAbsoluteTM(matrix);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : void ComputeClippingPlanes(float fCurrentAspect)
//
//  - Purpose   : Computes the clipping planes for the actual camera's
//                position and orientation. We use its transformation matrix
//                for this.
//                fCurrentAspect is the current aspect ratio used for rendering,
//                we need it in order to get the vertical camera FOV.
//                Planes are stored in world space.
//
// -----------------------------------------------------------------------------
void CCamera::ComputeClippingPlanes(float fCurrentAspect)
{
	CMatrix matrix;
	CMatrix mtxCam = GetAbsoluteTM();

	// Possibly not the fastest algorithm, but it's IMHO very clear

	// Compute frustum plane normals in local space.
	// Left, right, top and bottom must be slightly modified
	// (rotated) in order to be adjusted to the frustum.

	CVector3 v3NPlaneNear   =  CVector3( 0.0f,  0.0f, -1.0f);
	CVector3 v3NPlaneFar    =  CVector3( 0.0f,  0.0f,  1.0f);
	CVector3 v3NPlaneLeft   =  CVector3( 1.0f,  0.0f,  0.0f);
	CVector3 v3NPlaneRight  =  CVector3(-1.0f,  0.0f,  0.0f);
	CVector3 v3NPlaneTop    =  CVector3( 0.0f, -1.0f,  0.0f);
	CVector3 v3NPlaneBottom =  CVector3( 0.0f,  1.0f,  0.0f);

	// Transform to world space

	float fVerticalFOV = GetVerticalFOV(m_CSCurSettings.fFOV, fCurrentAspect);

	mtxCam.Transform(&v3NPlaneNear, true);
	mtxCam.Transform(&v3NPlaneFar,  true);

	matrix.BuildRotationY( m_CSCurSettings.fFOV * 0.5f);
	(mtxCam * matrix).Transform(&v3NPlaneLeft,   true);

	matrix.BuildRotationY(-m_CSCurSettings.fFOV * 0.5f);
	(mtxCam * matrix).Transform(&v3NPlaneRight,  true);

	matrix.BuildRotationX( fVerticalFOV * 0.5f);
	(mtxCam * matrix).Transform(&v3NPlaneTop,    true);

	matrix.BuildRotationX(-fVerticalFOV * 0.5f);
	(mtxCam * matrix).Transform(&v3NPlaneBottom, true);

	// Build planes (plane = BuildPlane(vertex, normal))

	CVector3 v3Pos  =  mtxCam.Position();
	CVector3 v3Near = v3Pos + v3NPlaneNear * m_CSCurSettings.fNear;
	CVector3 v3Far  = v3Pos + v3NPlaneNear * m_CSCurSettings.fFar;

	m_frustumPlanes[FRUSTUM_PLANE_NEAR]   = CPlane(v3Near, v3NPlaneNear);
	m_frustumPlanes[FRUSTUM_PLANE_FAR]    = CPlane(v3Far,  v3NPlaneFar);
	m_frustumPlanes[FRUSTUM_PLANE_LEFT]   = CPlane(v3Pos,  v3NPlaneLeft);
	m_frustumPlanes[FRUSTUM_PLANE_RIGHT]  = CPlane(v3Pos,  v3NPlaneRight);
	m_frustumPlanes[FRUSTUM_PLANE_TOP]    = CPlane(v3Pos,  v3NPlaneTop);
	m_frustumPlanes[FRUSTUM_PLANE_BOTTOM] = CPlane(v3Pos,  v3NPlaneBottom);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool TestAgainstFrustum(const CVector3& v3Vertex) const
//
//  - Purpose   : Returns true if the given vertex lays inside camera's frustum,
//                false if not.
//
// -----------------------------------------------------------------------------
bool CCamera::TestAgainstFrustum(const CVector3& v3Vertex) const
{
	int n;

	for(n = 0; n < 6; n++)
	{
		if(!v3Vertex.IsOnPositiveSide(m_frustumPlanes[n]))
		{
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : void SetDefaultSettings()
//
//  - Purpose   : Sets the default camera settings values.
//
// -----------------------------------------------------------------------------
void CCamera::SetDefaultSettings()
{
	m_CSCurSettings.fFOV  = 60.0f;
	m_CSCurSettings.fNear = 1.0f;
	m_CSCurSettings.fFar  = 1000.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool SetAnimCamSettingsData(TSettings* pSamples,
//                                            unsigned   uNumSamples,
//                                            unsigned   uFrecuency,
//                                            float      fStartTime)
//
//  - Purpose   : Set's the animated camera settings sample data.
//
// -----------------------------------------------------------------------------
bool CCamera::SetAnimCamSettingsData(TSettings* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime)
{
	SAFE_DELETE_A(m_pCSSamples);

	if(pSamples && uNumSamples > 0)
	{
		m_pCSSamples    = pSamples;
		m_uCSNumSamples = uNumSamples;
		m_uCSFrecuency  = uFrecuency;
		m_fCSStartTime  = fStartTime;
		m_fCSEndTime    = fStartTime + (uNumSamples * (1.0f / uFrecuency));
	}
	else
	{
		m_pCSSamples    = NULL;
		m_uCSNumSamples = 0;
		m_uCSFrecuency  = 0;
		m_fCSStartTime  = FLOAT_MAX;
		m_fCSEndTime    = FLOAT_MIN;
		m_bCSLoop       = false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool FreeAnimCamSettingsData()
//
//  - Purpose   : Frees the animated camera settings data.
//
// -----------------------------------------------------------------------------
bool CCamera::FreeAnimCamSettingsData()
{
	SAFE_DELETE_A(m_pCSSamples);

	m_uCSNumSamples = 0;

	m_fCSEndTime    = FLOAT_MIN;
	m_fCSStartTime  = FLOAT_MAX;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool ValidAnimCamSettingsTime(float fTime)
//
//  - Purpose   : Checks if the time provided is in the valid animated camera
//                settings range.
//                Returns true if it's valid, false if it isn't.
//
// -----------------------------------------------------------------------------
bool CCamera::ValidAnimCamSettingsTime(float fTime)
{
	if(!m_bCSLoop)
	{
		if(fTime > m_fCSEndTime)   return false;
		if(fTime < m_fCSStartTime) return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool SetAnimCamSettingsEvalTime(float fTime)
//
//  - Purpose   : Evaluates the animation sample data to set the camera values
//                according to the time value given.
//
// -----------------------------------------------------------------------------
bool CCamera::SetAnimCamSettingsEvalTime(float fTime)
{
	if(!m_pCSSamples)
	{
		return true;
	}

	if(!m_bCSLoop)
	{
		if(!ValidAnimCamSettingsTime(fTime))
		{
			return false;
		}
	}
	else
	{
		ComputeLoop(m_fCSStartTime, m_fCSEndTime, &fTime);
	}
	
	unsigned uSample = m_uCSFrecuency * (fTime - m_fCSStartTime);
	float    fPeriod = 1.0f / m_uCSFrecuency;
	float    fT      = (fTime - (m_fCSStartTime + (uSample * fPeriod))) / fPeriod;

	if(uSample < 0 || uSample >= (m_uCSNumSamples - 1))
	{
		return false;
	}

	m_CSCurSettings.fNear = ComputeLERP(m_pCSSamples[uSample].fNear, m_pCSSamples[uSample + 1].fNear, fT);
	m_CSCurSettings.fFar  = ComputeLERP(m_pCSSamples[uSample].fFar,  m_pCSSamples[uSample + 1].fFar,  fT);
	m_CSCurSettings.fFOV  = ComputeLERP(m_pCSSamples[uSample].fFOV,  m_pCSSamples[uSample + 1].fFOV,  fT);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the camera data.
//
// -----------------------------------------------------------------------------
bool CCamera::FreeNodeData()
{
	unsigned uError = 0;

	if(!FreeAnimCamSettingsData()) uError++;

	SetDefaultSettings();

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool ReadSettings(CInput* pInput)
//
//  - Purpose   : Reads both the static and animated camera settings from a
//                data source.
//
// -----------------------------------------------------------------------------
bool CCamera::ReadSettings(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	int      nAux;
	unsigned uError = 0;

	// Check

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux != ID_CAMERA_SETTINGSCHUNKSTART)
	{
		LOG.Write("\nERROR - CCamera::ReadSettings() of %s: Expected camera settings chunk start.");
		return false;
	}

	// Initial settings

	if(pInput->ReadRawData(sizeof(TSettings), 1, &m_CSCurSettings) != 1)
	{
		LOG.Write("\nERROR - CCamera::ReadSettings() of %s: Error reading static settings.");
	}

	// Animated settings

	if(!pInput->ReadInt  ((int*)&m_uCSNumSamples)) uError++;
	if(!pInput->ReadInt  ((int*)&m_uCSFrecuency))  uError++;
	if(!pInput->ReadFloat(&m_fCSStartTime))        uError++;
	if(!pInput->ReadFloat(&m_fCSEndTime))          uError++;
	if(!pInput->ReadInt  (&nAux))                  uError++;

	m_bCSLoop = nAux ? true : false;

	SAFE_DELETE_A(m_pCSSamples);

	if(m_uCSNumSamples > 0)
	{
		LOG.Write("\n  Animated camera settings: %u samples.", m_uCSNumSamples);

		m_pCSSamples = new TSettings[m_uCSNumSamples];
		assert(m_pCSSamples);

		if(pInput->ReadRawData(sizeof(TSettings) * m_uCSNumSamples, 1, m_pCSSamples) != 1)
		{
			LOG.Write("\nERROR - CCamera::ReadSettings() of %s: Error reading animated setting samples.");
			uError++;
		}
	}

	// Check

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux != ID_CAMERA_SETTINGSCHUNKEND)
	{
		LOG.Write("\nERROR - CCamera::ReadSettings() of %s: Expected camera settings chunk start.");
		return false;
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool WriteSettings(COutput* pOutput) const
//
//  - Purpose   : Writes both the static and animated camera settings to
//                the output passed.
//
// -----------------------------------------------------------------------------
bool CCamera::WriteSettings(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(!pOutput->WriteInt(ID_CAMERA_SETTINGSCHUNKSTART)) uError++;

	// Initial settings

	if(pOutput->WriteRawData(sizeof(TSettings), 1, &m_CSCurSettings) != 1)
	{
		LOG.Write("\nERROR - CCamera::WriteSettings() of %s: Error writing static settings.");
	}

	// Animated settings

	if(!pOutput->WriteInt  (m_uCSNumSamples))   uError++;
	if(!pOutput->WriteInt  (m_uCSFrecuency))    uError++;
	if(!pOutput->WriteFloat(m_fCSStartTime))    uError++;
	if(!pOutput->WriteFloat(m_fCSEndTime))      uError++;
	if(!pOutput->WriteInt  (m_bCSLoop ? 1 : 0)) uError++;

	if(m_uCSNumSamples > 0)
	{
		LOG.Write("\n  Animated camera settings: %u samples.", m_uCSNumSamples);

		if(pOutput->WriteRawData(sizeof(TSettings) * m_uCSNumSamples, 1, m_pCSSamples) != 1)
		{
			LOG.Write("\nERROR - CCamera::WriteSettings() of %s: Error writing animated setting samples.");
			uError++;
		}
	}

	if(!pOutput->WriteInt(ID_CAMERA_SETTINGSCHUNKEND)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool ReadNodeData(CInput* pInput)
//
//  - Purpose   : Reads the camera data from a source.
//
// -----------------------------------------------------------------------------
bool CCamera::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;

	if(!ReadSettings (pInput)) uError++;
	if(!ReadAnimation(pInput)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CCamera
//  - prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Writes the camera data in a given destiny.
//
// -----------------------------------------------------------------------------
bool CCamera::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(!WriteSettings (pOutput)) uError++;
	if(!WriteAnimation(pOutput)) uError++;

	return uError == 0;
}