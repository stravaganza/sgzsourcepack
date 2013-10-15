// ==[ File ]===================================================================================
//
//  -Name     : Light.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLight definition.
//
// =============================================================================================

/*

	29/11/2002  -  File created.


*/


#ifndef __LIGHT_H
#define __LIGHT_H


#pragma pack(1)


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CLight
//
//  - Purpose   : Light node class.
//
// ---------------------------------------------------------------------------
class CLight : public CNode
{


public:


	CLight();
	~CLight();


	struct TSettings
	{
		CVector4 v4Ambient;
		CVector4 v4Diffuse;
		CVector4 v4Specular;
		float    fAttenConstant;
		float    fAttenLinear;
		float    fAttenQuadratic;
		float    fSpotCutoff;
		float    fSpotExponent;
	};


	// Get

	inline int      GetNodeType       () const { return ID_NODE_LIGHT;    }
	inline int      GetNodeSubType    () const { return m_nLightType;     }

	// Get

	inline CVector4 GetAmbient        () const { return m_LSCurSettings.v4Ambient;         }
	inline CVector4 GetDiffuse        () const { return m_LSCurSettings.v4Diffuse;         }
	inline CVector4 GetSpecular       () const { return m_LSCurSettings.v4Specular;        }

	inline float    GetAttenConstant  () const { return m_LSCurSettings.fAttenConstant;    }
	inline float    GetAttenLinear    () const { return m_LSCurSettings.fAttenLinear;      }
	inline float    GetAttenQuadratic () const { return m_LSCurSettings.fAttenQuadratic;   }

	inline float    GetSpotCutoff     () const { return m_LSCurSettings.fSpotCutoff;       }
	inline float    GetSpotExponent   () const { return m_LSCurSettings.fSpotExponent;     }

	inline bool     GetLSLoop         () const { return m_bLSLoop;                         }

	// Set

	void            SetLightType      (int   nType);

	inline void     SetAmbient        (const CVector4& v4Ambient)  { m_LSCurSettings.v4Ambient       = v4Ambient;  }
	inline void     SetDiffuse        (const CVector4& v4Diffuse)  { m_LSCurSettings.v4Diffuse       = v4Diffuse;  }
	inline void     SetSpecular       (const CVector4& v4Specular) { m_LSCurSettings.v4Specular      = v4Specular; }

	inline void     SetAttenConstant  (float fValue)               { m_LSCurSettings.fAttenConstant  = fValue;     }
	inline void     SetAttenLinear    (float fValue)               { m_LSCurSettings.fAttenLinear    = fValue;     }
	inline void     SetAttenQuadratic (float fValue)               { m_LSCurSettings.fAttenQuadratic = fValue;     }

	inline void     SetSpotCutoff     (float fValue)               { m_LSCurSettings.fSpotCutoff     = fValue;     }
	inline void     SetSpotExponent   (float fValue)               { m_LSCurSettings.fSpotExponent   = fValue;     }

	inline void     SetLSLoop         (bool  bLoop)                { m_bLSLoop                       = bLoop;      }

	void     SetDefaultSettings();

	// Animated light settings

	bool     SetAnimLightSettingsData    (TSettings* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime);
	bool     FreeAnimLightSettingsData   ();
	bool     HasAnimLightSettingsData    ()            const { return m_pLSSamples != NULL; }
	bool     ValidAnimLightSettingsTime  (float fTime);
	bool     SetAnimLightSettingsEvalTime(float fTime);
	
	// Visitors

	bool     AcceptVisitor(CNodeVisitor* pNodeVisitor) { return pNodeVisitor->VisitLight(this); }


protected:


	bool       ReadSettings  (CInput*  pInput);
	bool       WriteSettings (COutput* pOutput) const;

	bool       FreeNodeData  ();
	bool       ReadNodeData  (CInput*  pInput);
	bool       WriteNodeData (COutput* pOutput) const;

	int        m_nLightType;

	TSettings  m_LSCurSettings;

	TSettings* m_pLSSamples;
	unsigned   m_uLSNumSamples;
	unsigned   m_uLSFrecuency;
	float      m_fLSEndTime;
	float      m_fLSStartTime;
	bool       m_bLSLoop;
};


#pragma pack()


#endif