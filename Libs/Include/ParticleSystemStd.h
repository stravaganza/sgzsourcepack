// ==[ File ]===================================================================================
//
//  -Name     : ParticleSystemStd.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Standard particle system class definition.
//
// =============================================================================================

/*

	02/03/2003  -  File created.

*/


#ifndef __PARTICLESYSTEMSTD_H
#define __PARTICLESYSTEMSTD_H


class UtilGL::Texturing::CTexture;


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CParticleSystemStd
//
//  - Purpose   : Standard particle system
//
// ---------------------------------------------------------------------------
class CParticleSystemStd : public CParticleSystem
{


public:


	CParticleSystemStd();
	~CParticleSystemStd();

	void SetDefaults      ();
	void SetNumParticles  (int   nNum, bool bRate);
	void SetSeed          (int   nSeed)                            { m_nSeed              = nSeed;             }
	void SetPartColor     (const CVector4& v4Color)                { m_v4PartColor        = v4Color;           }
	void SetPartTexture   (const std::string& strTexture)          { m_strTexture         = strTexture;        }
	void SetAffectLighting(bool  bAffectLighting)                  { m_bAffectLighting    = bAffectLighting;   }
	void SetBlendMult     (bool  bTrue)                            { m_bBlendMult         = bTrue;             }
	void SetEmitStart     (float fTime)                            { m_fEmitStart         = fTime;   ReInit(); }
	void SetEmitStop      (float fTime)                            { m_fEmitStop          = fTime;   ReInit(); }
	void SetEmitRadius    (float fRadius)                          { m_fEmitRadius        = fRadius; /* ReInit(); */ }
	void SetEmitOffAxis   (float fAngle,  float fSpreadAngle)      { m_fEmitOffAxisAngle  = fAngle;  m_fEmitOffAxisSpreadAngle   = fSpreadAngle; /* ReInit(); */ }
	void SetEmitOffPlane  (float fAngle,  float fSpreadAngle)      { m_fEmitOffPlaneAngle = fAngle;  m_fEmitOffPlaneSpreadAngle  = fSpreadAngle; /* ReInit(); */ }
	void SetPartLife      (float fLife,   float fVariation = 0.0f) { m_fPartLife          = fLife;   m_fPartLifeVariation        = fVariation;                   }
	void SetPartSpeed     (float fSpeed,  float fVariation = 0.0f) { m_fPartSpeed         = fSpeed;  m_fPartSpeedVariation       = fVariation;   /* ReInit(); */ }
	void SetPartSize      (float fSize,   float fVariation = 0.0f) { m_fPartSize          = fSize;   m_fPartSizeVariation        = fVariation;   /* ReInit(); */ }
	void SetPartGrowEnd   (float fGrowEnd)                         { m_fPartGrowEnd       = fGrowEnd;   /* ReInit(); */ }
	void SetPartFadeStart (float fFadeStart)                       { m_fPartFadeStart     = fFadeStart; /* ReInit(); */ }
	void SetPartFadeSize  (bool  bFadeSize)                        { m_bFadeSize          = bFadeSize;  /* ReInit(); */ }
	void SetPartWorldAccel(const CVector3 v3Accel)                 { m_v3Acceleration     = v3Accel; m_bAcceleration = v3Accel == CVector3::NULLVECTOR ? false : true; /* ReInit(); */ }
	void SetBubbleAmp     (float fAmp,    float fVariation = 0.0f) { m_fBubbleAmplitude   = fAmp;    m_fBubbleAmplitudeVariation = fVariation; m_bBubbleMotion = true; /* ReInit(); */ }
	void SetBubblePeriod  (float fPeriod, float fVariation = 0.0f) { m_fBubblePeriod      = fPeriod; m_fBubblePeriodVariation    = fVariation; m_bBubbleMotion = true; /* ReInit(); */ }
	void SetBubblePhase   (float fPhase,  float fVariation = 0.0f) { m_fBubblePhase       = fPhase;  m_fBubblePhaseVariation     = fVariation; m_bBubbleMotion = true; /* ReInit(); */ }

	void SetTexture       (UtilGL::Texturing::CTexture2D* pTexture){ m_pTexture = pTexture; }

	// From CParticleSystem:

	bool InitSystem();
	int  GetNumParticles()   const;
	bool HasAliveParticles() const;
	bool SetEvalTime(float fTime);
	bool Render();

	bool LoadResources(	const std::string& strDatfile   = "",
						const std::string& strTexFolder = "",
						UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL);

	// From CNode:

	int  GetNodeType      ()                           const { return ID_NODE_PARTICLESYSTEM;                  }
	int  GetNodeSubType   ()                           const { return ID_PARTICLESYSTEM_STANDARD;              }
	bool AcceptVisitor    (CNodeVisitor* pNodeVisitor)       { return pNodeVisitor->VisitParticleSystem(this); }
	bool FreeNodeData     ();
	bool ReadNodeData     (CInput*  pInput);
	bool WriteNodeData    (COutput* pOutput) const;

	// From CRenderable:

	bool           IsPostRender() const { return false; }
	bool           IsOpaque()     const;
	const CShader* GetShader()    const { return NULL;  } // No shader use


private:


	struct SParticle
	{
		bool     bAlive;
		float    fLife;
		float    fAge;
		float    fMaxSize;
		float    fSize;
		float    fGrowEnd;
		float    fFadeStart;
		float    fBubbleAmp;
		float    fBubbleFreq;
		float    fBubblePhase;
		CVector3 v3StartPos;
		CVector3 v3Pos;
		CVector3 v3Speed;
		CVector3 v3BubbleDir;
	};

	void ReInit();
	void GenParticle(SParticle* pParticle, float fLife);

	// Properties

	CVector4    m_v4PartColor;
	std::string m_strTexture;

	int         m_nNumParticles;
	int         m_nSeed;
	bool        m_bNumRate;
	bool        m_bAffectLighting;
	bool        m_bBlendMult;
	float       m_fEmitStart;
	float       m_fEmitStop;
	float       m_fEmitRadius;
	float       m_fEmitOffAxisAngle,  m_fEmitOffAxisSpreadAngle;
	float       m_fEmitOffPlaneAngle, m_fEmitOffPlaneSpreadAngle;
	float       m_fPartLife,          m_fPartLifeVariation;
	float       m_fPartSpeed,         m_fPartSpeedVariation;
	float       m_fPartSize,          m_fPartSizeVariation;
	float       m_fPartFadeStart;
	bool        m_bFadeSize;
	float       m_fPartGrowEnd;
	CVector3    m_v3Acceleration;
	float       m_fBubbleAmplitude,   m_fBubbleAmplitudeVariation;
	float       m_fBubblePeriod,      m_fBubblePeriodVariation;
	float       m_fBubblePhase,       m_fBubblePhaseVariation;

	// Internal use

	bool        m_bInit;
	float       m_fCurTime;
	float       m_fLastGenParticleTime;
	float       m_fPartGenPeriod;
	int         m_nTotParts;
	int         m_nToBeDrawn;
	bool        m_bAcceleration;
	bool        m_bBubbleMotion;

	SParticle*  m_pParticles;
	CVector3*   m_pv3DrawPartPositions;
	CVector4*   m_pv4DrawPartColors;
	float*      m_pfDrawPartSizes;

	UtilGL::Texturing::CTexture2D* m_pTexture;

	bool        m_bTexLoadedFromFile;

};


#endif