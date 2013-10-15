// ==[ File ]===================================================================================
//
//  -Name     : Camera.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CCamera definition.
//
// =============================================================================================

/*

	29/11/2002  -  File created.
	03/04/2003  -  removed m_fAspect member.


*/


#ifndef __CAMERA_H
#define __CAMERA_H


#pragma pack(1)


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CCamera
//
//  - Purpose   : Camera node class.
//
// ---------------------------------------------------------------------------
class CCamera : public CNode
{


public:


	struct TSettings
	{
		float fFOV;
		float fNear;
		float fFar;
	};

	enum ECamePlane
	{
		FRUSTUM_PLANE_NEAR = 0,
		FRUSTUM_PLANE_FAR,
		FRUSTUM_PLANE_LEFT,
		FRUSTUM_PLANE_RIGHT,
		FRUSTUM_PLANE_TOP,
		FRUSTUM_PLANE_BOTTOM,
	};

	CCamera();
	~CCamera();

	// Get

	static float GetVerticalFOV	(float fHorizontalFOV, float fAspect);

	inline int   GetNodeType    ()  const { return ID_NODE_CAMERA;            }
	inline int   GetNodeSubType ()  const { return ID_CAMERA_STANDARD;        }

	inline float Near  ()           const { return m_CSCurSettings.fNear;     }
	inline float Far   ()           const { return m_CSCurSettings.fFar;      }
	inline float FOV   ()           const { return m_CSCurSettings.fFOV;      }

	inline bool  GetCSLoop()        const { return m_bCSLoop;                 }

	inline CPlane FrustumPlane(unsigned uPlane) const
	{
		assert(uPlane < 6);
		return m_frustumPlanes[uPlane];
	}

	// Set

	inline void SetNear  (float fNear)   { m_CSCurSettings.fNear   = fNear;   }
	inline void SetFar   (float fFar)    { m_CSCurSettings.fFar    = fFar;    }
	inline void SetFOV   (float fFOV)    { m_CSCurSettings.fFOV    = fFOV;    }

	inline void SetCSLoop(bool  bLoop)   { m_bCSLoop               = bLoop;   }

	inline void SetFrustumPlane(unsigned uPlane, const CPlane& plane)
	{
		assert(uPlane < 6);
		m_frustumPlanes[uPlane] = plane;
	}

	// Camera routines

	void Place(const CVector3& pos, const CQuaternion& quat);
	void Place(const CVector3& pos, const CVector3& targetPos, const CVector3& upVec);
	void ComputeClippingPlanes(float fCurrentAspect);
	bool TestAgainstFrustum(const CVector3& v3Vertex) const;

	void SetDefaultSettings();

	// Animated camera settings

	bool SetAnimCamSettingsData    (TSettings* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime);
	bool FreeAnimCamSettingsData   ();
	bool HasAnimCamSettingsData    ()            const { return m_pCSSamples != NULL; }
	bool ValidAnimCamSettingsTime  (float fTime);
	bool SetAnimCamSettingsEvalTime(float fTime);

	// Visitors

	bool AcceptVisitor(CNodeVisitor* pNodeVisitor) { return pNodeVisitor->VisitCamera(this); }


protected:


	bool ReadSettings  (CInput*  pInput);
	bool WriteSettings (COutput* pOutput) const;

	bool FreeNodeData  ();
	bool ReadNodeData  (CInput*  pInput);
	bool WriteNodeData (COutput* pOutput) const;

	TSettings  m_CSCurSettings;

	TSettings* m_pCSSamples;
	unsigned   m_uCSNumSamples;
	unsigned   m_uCSFrecuency;
	float      m_fCSEndTime;
	float      m_fCSStartTime;
	bool       m_bCSLoop;

	CPlane m_frustumPlanes[6];
};


#pragma pack()


#endif