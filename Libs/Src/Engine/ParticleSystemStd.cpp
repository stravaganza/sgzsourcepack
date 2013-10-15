// ==[ File ]===================================================================================
//
//  -Name     : ParticleSystemStd.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CParticleSystemStd implementation
//
// =============================================================================================

/*

	02/03/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementations ]==================================================================


using namespace UtilGL::Texturing;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : CParticleSystemStd()
//
//  - Purpose   : CParticleSystemStd's default constructor.
//
// -----------------------------------------------------------------------------
CParticleSystemStd::CParticleSystemStd()
{
	m_bInit              = false;
	m_bTexLoadedFromFile = false;

	SetDefaults();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : ~CParticleSystemStd()
//
//  - Purpose   : CParticleSystemStd's destructor
//
// -----------------------------------------------------------------------------
CParticleSystemStd::~CParticleSystemStd()
{
	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : void SetDefaults()
//
//  - Purpose   : Sets the default parameter values.
//
// -----------------------------------------------------------------------------
void CParticleSystemStd::SetDefaults()
{
	m_v4PartColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	m_nNumParticles             = 0;
	m_bNumRate                  = false;
	m_nSeed                     = 0;
	m_bAffectLighting           = false;
	m_strTexture                = "";
	m_bBlendMult                = false;
	m_fEmitStart                = 0.0f;
	m_fEmitStop                 = 0.0f;
	m_fEmitRadius               = 0.0f;
	m_fEmitOffAxisAngle         = 0.0f;
	m_fEmitOffAxisSpreadAngle   = 0.0f;
	m_fEmitOffPlaneAngle        = 0.0f;
	m_fEmitOffPlaneSpreadAngle  = 0.0f;
	m_fPartLife                 = 0.0f;
	m_fPartLifeVariation        = 0.0f;
	m_fPartSpeed                = 0.0f;
	m_fPartSpeedVariation       = 0.0f;
	m_fPartSize                 = 0.0f;
	m_fPartSizeVariation        = 0.0f;
	m_fPartFadeStart            = 0.0f;
	m_bFadeSize                 = false;
	m_fPartGrowEnd              = 0.0f;
	m_fBubbleAmplitude          = 0.0f;
	m_fBubbleAmplitudeVariation = 0.0f;
	m_fBubblePeriod             = FLOAT_MAX;
	m_fBubblePeriodVariation    = 0.0f;
	m_fBubblePhase              = 0.0f;
	m_fBubblePhaseVariation     = 0.0f;

	m_fCurTime                  = 0.0f;
	m_fLastGenParticleTime      = 0.0f;
	m_fPartGenPeriod            = 0.0f;
	m_nTotParts                 = 0;
	m_nToBeDrawn                = 0;
	m_bBubbleMotion             = false;

	m_pParticles                = NULL;
	m_pv3DrawPartPositions      = NULL;
	m_pv4DrawPartColors         = NULL;
	m_pfDrawPartSizes           = NULL;
	m_pTexture                  = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : void SetNumParticles(int nNum, bool bRate)
//
//  - Purpose   : If bRate is true, nNum specifies the number of particles
//                emitted per second. If bRate is false, nNum specifies the
//                total number of particles formed over the life of the system.
//
// -----------------------------------------------------------------------------
void CParticleSystemStd::SetNumParticles(int nNum, bool bRate)
{
	m_nNumParticles = nNum;
	m_bNumRate      = bRate;

	ReInit();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool InitSystem()
//
//  - Purpose   : Initializes the particle system.
//
//  - Note      : To avoid wasting processing time, call this method after
//                having set all system's properties (Set methods).
//                Set methods can be called at runtime, but require the system
//                to be reinitialized and, what's more expensive, reevaluating it.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::InitSystem()
{
	// Compute total number of particle slots needed

	if(m_nNumParticles < 1)
	{
		return false;
	}

	if(m_bNumRate)
	{
		m_fPartGenPeriod = 1.0f / m_nNumParticles;
	}
	else
	{
		m_fPartGenPeriod = (m_fEmitStop - m_fEmitStart) / m_nNumParticles;
	}

	m_nTotParts = (1.0f / m_fPartGenPeriod) * (m_fPartLife + (m_fPartLife * m_fPartLifeVariation));

	if(m_nTotParts < 1)
	{
		m_nTotParts = 1;
	}

	SAFE_DELETE_A(m_pParticles);
	SAFE_DELETE_A(m_pv3DrawPartPositions);
	SAFE_DELETE_A(m_pv4DrawPartColors);
	SAFE_DELETE_A(m_pfDrawPartSizes);

	m_pParticles           = new SParticle[m_nTotParts];
	m_pv3DrawPartPositions = new CVector3 [m_nTotParts];
	m_pv4DrawPartColors    = new CVector4 [m_nTotParts];
	m_pfDrawPartSizes      = new float    [m_nTotParts];

	assert(m_pParticles);
	assert(m_pv3DrawPartPositions);
	assert(m_pv4DrawPartColors);
	assert(m_pfDrawPartSizes);

	for(int i = 0; i < m_nTotParts; i++)
	{
		m_pParticles[i].bAlive = false;
	}

	// Set the initial state of the system

	if(m_fEmitStart < 0.0f)
	{
		m_fCurTime             = m_fEmitStart;
		m_fLastGenParticleTime = m_fEmitStart;
	}
	else
	{
		// We normally will do the first call of SetEvalTime with t = 0.0f
		// so this will be more effective:

		m_fCurTime             = 0.0f;
		m_fLastGenParticleTime = 0.0f;
	}

	m_bInit = true;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : int GetNumParticles() const
//
//  - Purpose   : Returns the number of particles currently being computed.
//
// -----------------------------------------------------------------------------
int CParticleSystemStd::GetNumParticles() const
{
	return m_nToBeDrawn;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool HasAliveParticles() const
//
//  - Purpose   : Returns true if the particle system has alive particles, so
//                nothing has to be computed drawn.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::HasAliveParticles() const
{
	if(!m_bInit)
	{
		return false;
	}

	if(m_fCurTime > (m_fEmitStop + (m_fPartLife + (m_fPartLife * m_fPartLifeVariation))))
	{
		return false;
	}

	if(m_fCurTime < m_fEmitStart)
	{
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool SetEvalTime(float fTime)
//
//  - Purpose   : Updates the particle system's internal state to the given
//                time value.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::SetEvalTime(float fTime)
{
	assert(m_bInit);

	float fDT = 0.1f; // dt when integrating, we assure 1/dt minimum updates per second

	if(fTime < m_fCurTime)
	{
		fDT = -fDT;
	}

	// Integrate until finished

	srand(m_nSeed + (fTime * 1000.0f));

	bool bFinished = false;

	while(!bFinished)
	{
		// Check if this is the last integration step

		if(MYFABSF(fTime - m_fCurTime) < MYFABSF(fDT))
		{
			fDT = fTime - m_fCurTime;
			bFinished = true;
		}

		m_fCurTime += fDT;

		// Compute how many new particles to generate

		int nNewParticles = 0; // Will store the number of particles to be generated this step.

		if(m_fCurTime >= m_fEmitStart && m_fCurTime <= m_fEmitStop)
		{
			while(MYFABSF(m_fLastGenParticleTime - m_fCurTime) > m_fPartGenPeriod)
			{
				if(m_fLastGenParticleTime < m_fCurTime)
				{
					m_fLastGenParticleTime += m_fPartGenPeriod;
				}
				else
				{
					m_fLastGenParticleTime -= m_fPartGenPeriod;
				}

				// Don't create particles if we go backwards and m_fCurTime < m_fEmitStart + m_fPartLife

				if(!(fDT < 0.0f && m_fCurTime < m_fEmitStart + m_fPartLife))
				{
					nNewParticles++;
				}
			}
		}

		// Update particle system: Integration step with fDT being the dt:

		m_nToBeDrawn = 0;

		for(int i = 0; i < m_nTotParts; i++)
		{
			// Check if the particle dies

			SParticle* pParticle = &m_pParticles[i];

			if(pParticle->bAlive)
			{
				pParticle->fAge += fDT;

				if(pParticle->fAge > pParticle->fLife || pParticle->fAge < 0.0f)
				{
					pParticle->bAlive = false;
				}
			}

			// If this is a dead particle (it died in the previous check or it
			// was already dead) use it to generate a new one if there are particles
			// remaining to be generated.

			if(!pParticle->bAlive && nNewParticles > 0)
			{
				nNewParticles--;
				float fAge = MYFABSF(m_fLastGenParticleTime - m_fCurTime) + (nNewParticles * m_fPartGenPeriod);

				if(fDT < 0.0f)
				{
					fAge = m_fPartLife - fAge;
				}

				GenParticle(pParticle, fAge);
			}

			// If alive, update data and add it to the list of to-be-drawn particles:

			if(pParticle->bAlive)
			{
				// Update position:

				CVector3 v3HalfAccelTSquare(0.0f, 0.0f, 0.0f); // 0.5f * a * t^2

				if(m_bAcceleration)
				{
					v3HalfAccelTSquare = m_v3Acceleration * (0.5f * pParticle->fAge * pParticle->fAge);
				}

				// pos = startpos + speed * t + 0.5 * accel^2

				pParticle->v3Pos = pParticle->v3StartPos + (pParticle->v3Speed * pParticle->fAge) + v3HalfAccelTSquare;

				if(m_bBubbleMotion)
				{
					float fOffset    = pParticle->fBubbleAmp * sinf(DEG_TO_RAD(pParticle->fBubblePhase) + (pParticle->fAge * pParticle->fBubbleFreq));
					pParticle->v3Pos = pParticle->v3Pos + pParticle->v3BubbleDir * fOffset;
				}

				// Update size (growing from 0 to maxsize):

				pParticle->fSize = pParticle->fMaxSize;

				if(pParticle->fAge < pParticle->fGrowEnd && !IS_ZERO(pParticle->fGrowEnd))
				{
					pParticle->fSize = (pParticle->fAge / pParticle->fGrowEnd) * pParticle->fMaxSize;
					if(pParticle->fSize < 0.0f) pParticle->fSize = 0.0f;
				}

				// Update fade (reduce transparency and optionally size):

				CVector4 v4Color = m_v4PartColor;

				if(pParticle->fAge > pParticle->fFadeStart && !ARE_EQUAL(pParticle->fLife, pParticle->fFadeStart))
				{
					float fFadeLength  = pParticle->fLife - pParticle->fFadeStart;
					float fReduce      = (pParticle->fAge - pParticle->fFadeStart) / fFadeLength;

					if(m_bFadeSize)
					{
						pParticle->fSize = pParticle->fMaxSize - (fReduce * pParticle->fMaxSize * 0.7);
					}

					v4Color.SetW(m_v4PartColor.W() - (fReduce * m_v4PartColor.W()));
				}

				m_pv3DrawPartPositions[m_nToBeDrawn] = pParticle->v3Pos;
				m_pv4DrawPartColors   [m_nToBeDrawn] = v4Color;
				m_pfDrawPartSizes     [m_nToBeDrawn] = pParticle->fSize;

				m_nToBeDrawn++;
			}
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool Render()
//
//  - Purpose   : Draws the particle system.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::Render()
{
	assert(m_bInit);
	unsigned uError = 0;

	if(m_nToBeDrawn < 1) return true;

	// States

	int nLightingState, nBlendState = UtilGL::States::Get(UtilGL::States::BLENDING);

	if(!m_bAffectLighting)
	{
		nLightingState = UtilGL::States::Get(UtilGL::States::LIGHTING);
		UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	}

	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,     UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::ALPHATEST,    UtilGL::States::ENABLED);
	glAlphaFunc(GL_GREATER, 0.0f);

	int nDstBlend = m_bBlendMult ? UtilGL::States::BLEND_INVSRCALPHA : UtilGL::States::BLEND_ONE;

	UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);

	// Render

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);

	if(IS_ZERO(m_fPartSizeVariation) && ((m_fPartFadeStart >= m_fPartLife) || !m_bFadeSize))
	{
		// Optimize draw if there isn't per particle size

		if(!UtilGL::Rendering::DrawBillboards(m_nToBeDrawn, m_pTexture, m_pv3DrawPartPositions, m_pv4DrawPartColors, m_fPartSize))
		{
			uError++;
		}
	}
	else
	{
		// Draw using per particle size

		if(!UtilGL::Rendering::DrawBillboards(m_nToBeDrawn, m_pTexture, m_pv3DrawPartPositions, m_pv4DrawPartColors, m_pfDrawPartSizes))
		{
			uError++;
		}
	}

	// States

	if(!m_bAffectLighting)
	{
		UtilGL::States::Set(UtilGL::States::LIGHTING, nLightingState);
	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);
	}

	UtilGL::States::Set(UtilGL::States::BLENDING,     nBlendState);
	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::ALPHATEST,    UtilGL::States::DISABLED);

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool LoadResources(	const std::string& strDatfile   = "",
//										const std::string& strTexFolder = "",
//										UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL)
//
//  - Purpose   : Loads the particle system resources. In this case only
//                the particle texture.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::LoadResources(	const std::string& strDatfile,
										const std::string& strTexFolder,
										UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	assert(strDatfile.data());
	assert(strTexFolder.data());

	if(m_strTexture == "")
	{
		return true;
	}

	// Build full texture path

	std::string strTexPath = "";

	if(strTexFolder[strTexFolder.size() - 1] != '\\' && strTexFolder != "")
	{
		strTexPath = strTexFolder + "\\";
	}
	else
	{
		strTexPath = strTexFolder;
	}

	std::string strTexFile = strTexPath + m_strTexture;

	// Load texture

	CInput* pInput = LoadFileFromDat(strTexFile, false, strDatfile);

	if(pInput == NULL)
	{
		CLogger::ErrorWindow("Unable to open %s", strTexFile.data());
		SAFE_DELETE(pInput);
		return false;
	}

	CTexture2D* pTexture = new CTexture2D;
	assert(pTexture);

	if(!pTexture->Load(	pInput,
						strTexFile,
						GetFileExt(strTexFile),
						false,
						pTexOptions))
	{
		CLogger::ErrorWindow("Error loading texture %s", strTexFile.data());
		delete pTexture;
		delete pInput;
		return false;
	}

	m_pTexture = pTexture;

	delete pInput;

	m_bTexLoadedFromFile = true;

	return m_pTexture != NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : void ReInit()
//
//  - Purpose   : Some parameters, when modified, require the system to be
//                reinitialized in order for the changes to take place.
//                Most of the Set methods call ReInit() at the end.
//
// -----------------------------------------------------------------------------
void CParticleSystemStd::ReInit()
{
	float fTime = m_fCurTime;

	if(m_bInit)
	{
		InitSystem();
		SetEvalTime(fTime);
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : void GenParticle(SParticle* pParticle, float fAge)
//
//  - Purpose   : Generates the parameters of a new particle.
//
// -----------------------------------------------------------------------------
void CParticleSystemStd::GenParticle(SParticle* pParticle, float fAge)
{
	assert(pParticle);

	pParticle->bAlive = true;
	pParticle->fLife  = ComputeRandWithVariation(m_fPartLife, m_fPartLifeVariation);

	float fLifeRatio = pParticle->fLife / m_fPartLife;

	pParticle->fAge         = fAge * fLifeRatio;
	pParticle->fMaxSize     = ComputeRandWithVariation(m_fPartSize, m_fPartSizeVariation);
	pParticle->fSize        = pParticle->fMaxSize;
	pParticle->fGrowEnd     = m_fPartGrowEnd   * fLifeRatio;
	pParticle->fFadeStart   = m_fPartFadeStart * fLifeRatio;
	pParticle->fBubbleAmp   = ComputeRandWithVariation(m_fBubbleAmplitude, m_fBubbleAmplitudeVariation);
	pParticle->fBubbleFreq  = ComputeRandWithVariation(m_fBubblePeriod,    m_fBubblePeriodVariation);
	pParticle->fBubblePhase = ComputeRandWithVariation(m_fBubblePhase,     m_fBubblePhaseVariation);

	pParticle->fBubbleFreq  = 1.0f / pParticle->fBubbleFreq; // Period -> Frecuency
	pParticle->fBubbleFreq *= TWOPI;                         // Precalc radian transform

	// Set the particle start pos at a random distance from the
	// emitter in its XY plane. The distance depends on the emitter radius.

	pParticle->v3StartPos.Set(0.0f, 0.0f, 0.0f);
	CMatrix mtxRandRotZ;

	if(!IS_ZERO(m_fEmitRadius))
	{
		CMatrix mtxLocalEmitAxes;
		mtxLocalEmitAxes.BuildRotationY(m_fEmitOffAxisAngle);
		mtxLocalEmitAxes.RotateZ(m_fEmitOffPlaneAngle);

		mtxRandRotZ.BuildRotationZ(ComputeRand(0.0f, 360.0f));

		CVector3 v3Vector = mtxRandRotZ * mtxLocalEmitAxes.XAxis();
		v3Vector.Normalize();
		pParticle->v3StartPos = v3Vector * ComputeRand(0.0f, m_fEmitRadius);
	}

	// Assign a velocity to the particle. The direction depends on two
	// parameters that control the aperture of emission, and modify
	// the default Z Axis which is the one that emits.

	pParticle->v3Speed.Set(0.0f, 0.0f, 1.0f);

	CMatrix mtxRandEmit;

	mtxRandEmit.BuildRotationY(m_fEmitOffAxisAngle);
	mtxRandEmit.RotateY(ComputeRand(-m_fEmitOffAxisSpreadAngle, m_fEmitOffAxisSpreadAngle));
	mtxRandEmit.RotateZ(m_fEmitOffPlaneAngle);
	mtxRandEmit.RotateZ(ComputeRand(-m_fEmitOffPlaneSpreadAngle, m_fEmitOffPlaneSpreadAngle));

	pParticle->v3Speed = mtxRandEmit * pParticle->v3Speed;
	pParticle->v3Speed.Normalize();
	pParticle->v3Speed = pParticle->v3Speed * ComputeRandWithVariation(m_fPartSpeed, m_fPartSpeedVariation);

	// Assign a random bubble direction (perpendicular to the particle's direction)
	// Note: We start taking a perpendicular vector to the Z Axis (emitter).

	pParticle->v3BubbleDir.Set(1.0f, 0.0f, 0.0f);
	mtxRandRotZ.BuildRotationZ(ComputeRand(0.0f, 360.0f));

	pParticle->v3BubbleDir = (mtxRandRotZ * mtxRandEmit) * pParticle->v3BubbleDir;
	pParticle->v3BubbleDir.Normalize();

	// Transform vectors to world space
	// Compute accurrately the worldmatrix for the birthtime

	CMatrix mtxWorld;

	if(!ComputeAbsoluteTM(m_fCurTime - fAge, &mtxWorld))
	{
		// Well, actually should only happen if you update the system
		// for a time outside the animation range.

		mtxWorld = this->GetAbsoluteTM();
	}

	mtxWorld.Transform(&pParticle->v3StartPos,  false);
	mtxWorld.Transform(&pParticle->v3Speed,     true);
	mtxWorld.Transform(&pParticle->v3BubbleDir, true);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the memory used by the particle system.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::FreeNodeData()
{
	SAFE_DELETE_A(m_pParticles);
	SAFE_DELETE_A(m_pv3DrawPartPositions);
	SAFE_DELETE_A(m_pv4DrawPartColors);
	SAFE_DELETE_A(m_pfDrawPartSizes);

	if(m_bTexLoadedFromFile)
	{
		SAFE_DELETE(m_pTexture);
	}
	else
	{
		m_pTexture = NULL;
	}

	SetDefaults();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool ReadNodeData(CInput* pInput)
//
//  - Purpose   : Reads the properties from a data source.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;
	int      nAux;

	if(!pInput->ReadInt(&m_nNumParticles))               uError++; // 2
	if(!pInput->ReadInt(&nAux))                          uError++; // 3

	m_bNumRate = nAux == 1;
	SetNumParticles(m_nNumParticles, m_bNumRate);

	if(!pInput->ReadInt(&m_nSeed))                       uError++; // 4
	if(!m_v4PartColor.Read(pInput))                      uError++; // 5
	if(!pInput->ReadStringZero(&m_strTexture))           uError++; // 6
	if(!pInput->ReadInt(&nAux))                          uError++; // 7

	m_bAffectLighting = (nAux & 1) ? true : false;
	m_bBlendMult      = (nAux & 2) ? true : false;

	if(!pInput->ReadFloat(&m_fEmitStart))                uError++; // 8
	if(!pInput->ReadFloat(&m_fEmitStop))                 uError++; // 9
	if(!pInput->ReadFloat(&m_fEmitRadius))               uError++; // 10
	if(!pInput->ReadFloat(&m_fEmitOffAxisAngle))         uError++; // 11
	if(!pInput->ReadFloat(&m_fEmitOffAxisSpreadAngle))   uError++; // 12
	if(!pInput->ReadFloat(&m_fEmitOffPlaneAngle))        uError++; // 13
	if(!pInput->ReadFloat(&m_fEmitOffPlaneSpreadAngle))  uError++; // 14
	if(!pInput->ReadFloat(&m_fPartLife))                 uError++; // 15
	if(!pInput->ReadFloat(&m_fPartLifeVariation))        uError++; // 16
	if(!pInput->ReadFloat(&m_fPartSpeed))                uError++; // 17
	if(!pInput->ReadFloat(&m_fPartSpeedVariation))       uError++; // 18
	if(!pInput->ReadFloat(&m_fPartSize))                 uError++; // 19
	if(!pInput->ReadFloat(&m_fPartSizeVariation))        uError++; // 20
	if(!pInput->ReadFloat(&m_fPartFadeStart))            uError++; // 21
	if(!pInput->ReadInt  (&nAux))                        uError++; // 22
	m_bFadeSize = nAux == 1;
	if(!pInput->ReadFloat(&m_fPartGrowEnd))              uError++; // 23
	if(!m_v3Acceleration.Read(pInput))                   uError++; // 24
	if(!pInput->ReadInt  (&nAux))                        uError++; // 25
	m_bAcceleration = nAux == 1;
	if(!pInput->ReadFloat(&m_fBubbleAmplitude))          uError++; // 26
	if(!pInput->ReadFloat(&m_fBubbleAmplitudeVariation)) uError++; // 27
	if(!pInput->ReadFloat(&m_fBubblePeriod))             uError++; // 28
	if(!pInput->ReadFloat(&m_fBubblePeriodVariation))    uError++; // 29
	if(!pInput->ReadFloat(&m_fBubblePhase))              uError++; // 30
	if(!pInput->ReadFloat(&m_fBubblePhaseVariation))     uError++; // 31
	if(!pInput->ReadInt  (&nAux))                        uError++; // 32

	if(!ReadAnimation(pInput)) uError++;

	m_bBubbleMotion = nAux == 1;

	m_pParticles           = NULL;
	m_pv3DrawPartPositions = NULL;
	m_pv4DrawPartColors    = NULL;
	m_pfDrawPartSizes      = NULL;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Outputs the properties.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	int nFlags = 0;

	if(m_bAffectLighting) nFlags |= 1;
	if(m_bBlendMult)      nFlags |= 2;

	if(!pOutput->WriteInt  (ID_PARTICLESYSTEM_STANDARD))  uError++; // 1
	if(!pOutput->WriteInt  (m_nNumParticles))             uError++; // 2
	if(!pOutput->WriteInt  (m_bNumRate ? 1 : 0))          uError++; // 3
	if(!pOutput->WriteInt  (m_nSeed))                     uError++; // 4
	if(!m_v4PartColor.Write(pOutput))                     uError++; // 5
	if(!pOutput->WriteStringZero(m_strTexture))           uError++; // 6
	if(!pOutput->WriteInt  (nFlags))					  uError++; // 7
	if(!pOutput->WriteFloat(m_fEmitStart))                uError++; // 8
	if(!pOutput->WriteFloat(m_fEmitStop))                 uError++; // 9
	if(!pOutput->WriteFloat(m_fEmitRadius))               uError++; // 0
	if(!pOutput->WriteFloat(m_fEmitOffAxisAngle))         uError++; // 11
	if(!pOutput->WriteFloat(m_fEmitOffAxisSpreadAngle))   uError++; // 12
	if(!pOutput->WriteFloat(m_fEmitOffPlaneAngle))        uError++; // 13
	if(!pOutput->WriteFloat(m_fEmitOffPlaneSpreadAngle))  uError++; // 14
	if(!pOutput->WriteFloat(m_fPartLife))                 uError++; // 15
	if(!pOutput->WriteFloat(m_fPartLifeVariation))        uError++; // 16
	if(!pOutput->WriteFloat(m_fPartSpeed))                uError++; // 17
	if(!pOutput->WriteFloat(m_fPartSpeedVariation))       uError++; // 18
	if(!pOutput->WriteFloat(m_fPartSize))                 uError++; // 19
	if(!pOutput->WriteFloat(m_fPartSizeVariation))        uError++; // 20
	if(!pOutput->WriteFloat(m_fPartFadeStart))            uError++; // 21
	if(!pOutput->WriteInt  (m_bFadeSize ? 1 : 0))         uError++; // 22
	if(!pOutput->WriteFloat(m_fPartGrowEnd))              uError++; // 23
	if(!m_v3Acceleration.Write(pOutput))                  uError++; // 24
	if(!pOutput->WriteInt  (m_bAcceleration ? 1 : 0))     uError++; // 25
	if(!pOutput->WriteFloat(m_fBubbleAmplitude))          uError++; // 26
	if(!pOutput->WriteFloat(m_fBubbleAmplitudeVariation)) uError++; // 27
	if(!pOutput->WriteFloat(m_fBubblePeriod))             uError++; // 28
	if(!pOutput->WriteFloat(m_fBubblePeriodVariation))    uError++; // 29
	if(!pOutput->WriteFloat(m_fBubblePhase))              uError++; // 30
	if(!pOutput->WriteFloat(m_fBubblePhaseVariation))     uError++; // 31
	if(!pOutput->WriteInt  (m_bBubbleMotion ? 1 : 0))     uError++; // 32

	if(!WriteAnimation(pOutput)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParticleSystemStd
//  - Prototype : bool IsOpaque() const
//
//  - Purpose   : Returns true if particles all opaque, false if blending has
//                to be used in order to draw them.
//
// -----------------------------------------------------------------------------
bool CParticleSystemStd::IsOpaque() const
{
	bool bTexAlpha = m_pTexture ? m_pTexture->HasAlpha() : false;

	return !bTexAlpha && (m_v4PartColor.W() > (1.0f - ALMOST_ZERO)) && (m_fPartFadeStart >= m_fPartLife);
}