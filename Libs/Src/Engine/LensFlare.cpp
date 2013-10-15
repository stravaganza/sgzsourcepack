// ==[ File ]===================================================================================
//
//  -Name     : LensFlare.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLensFlare implementation
//
// =============================================================================================

/*

	04/04/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementations ]==================================================================


using namespace UtilGL::Texturing;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : CLensFlare()
//
//  - Purpose   : CLensFlare's default constructor.
//
// -----------------------------------------------------------------------------
CLensFlare::CLensFlare()
{
	m_fIntensity    = 1.0f;
	m_bZTestFade    = true;
	m_fFadeDuration = 0.1f;

	m_fLastInsideIntensity = 1.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : ~CLensFlare()
//
//  - Purpose   : CLensFlare's destructor
//
// -----------------------------------------------------------------------------
CLensFlare::~CLensFlare()
{
	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : void AddComponent(const std::string& strTexture, float fPos, float fSize)
//
//  - Purpose   : Adds a flare component. fPos goes from 0 to 1 and means the
//                position in the 'invisible' lensflare segment that crosses
//                the screen, from 0 (origin) to (1 segment end). fSize is
//                the size of the flare in normalized viewport space.
//
// -----------------------------------------------------------------------------
void CLensFlare::AddComponent(const std::string& strTexture, float fPos, float fSize)
{
	SComponent component;

	component.fPos       = fPos;
	component.fSize      = fSize;
	component.strTexture = strTexture;

	m_vecComponents.push_back(component);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : bool LoadResources(	const std::string& strDatfile   = "",
//										const std::string& strTexFolder = "",
//										UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL)
//
//  - Purpose   : Load lensflare resources (textures).
//
// -----------------------------------------------------------------------------
bool CLensFlare::LoadResources(	const std::string& strDatfile,
								const std::string& strTexFolder,
								UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	std::vector<SComponent>::iterator it;
	unsigned uError = 0;

	for(it = m_vecComponents.begin(); it != m_vecComponents.end(); ++it)
	{
		CTexture2D* pTexture = new CTexture2D;
		std::string strTexPath = "";

		// Get full texture path

		if(strTexFolder[strTexFolder.size() - 1] != '\\' && strTexFolder != "")
		{
			strTexPath = strTexFolder + "\\";
		}
		else
		{
			strTexPath = strTexFolder;
		}

		strTexPath = strTexPath + it->strTexture;

		// Load texture file

		CInput* pTexFile = LoadFileFromDat(strTexPath, false, strDatfile);

		if(!pTexFile)
		{
			SAFE_DELETE(pTexture);
			FreeNodeData();
			return false;
		}

		// Load texture

		assert(pTexture);

		if(!pTexture->Load(pTexFile, strTexPath, GetFileExt(strTexPath), false, pTexOptions))
		{
			SAFE_DELETE(pTexture);
			delete pTexFile;
			CLogger::ErrorWindow("ERROR - Error loading texture %s", strTexPath.data());
			FreeNodeData();
			return false;
		}

		delete pTexFile;

		m_vecTextures.push_back(pTexture);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : bool SetEvalTime(float fTime)
//
//  - Purpose   : Transforms the lensflare to the given time.
//
// -----------------------------------------------------------------------------
bool CLensFlare::SetEvalTime(float fTime)
{
	// We don't do the transforming in the Render() method for 2 reasons:
	// 1 - Scene speed has nothing to do with light speed, so we use an internal
	//     timer to transform the lensflare (visible/nonvisible transitions).
	//     It will hide or show up at the same speed regardless of the scene
	//     speed playback.
	// 2 - We need to read from the zbuffer to get the visibility state, so
	//     doing it just before the render (which is done after all other draws)
	//     instead of here, we assure a correct zbuffer state.
	//
	// On the negative side, if the same lensflare is rendered more than once
	// for the same frame, the transitions will be incorrect.
	// This can happen f.e when doing a crossfade between the same scene but
	// 2 different cams.

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : bool Render()
//
//  - Purpose   : Renders the lensflare. Important!! it should be rendered after
//                all other entities in the scene since it's a postrender effect.
//                The CRenderable sorting algorithm takes this into account.
//
// -----------------------------------------------------------------------------
bool CLensFlare::Render()
{
	bool bFirstFrame = false;

	if(m_intensTimer.State() == CTimer::STATE_STOPPED)
	{
		// First call to render: activate timer.
		m_intensTimer.Start();
		bFirstFrame = true;
	}

	float fIncSeconds = m_intensTimer.Seconds();

	m_intensTimer.Stop();
	m_intensTimer.Start();

	// Transform

	CVector3 v3LensViewportPos = GetAbsoluteTM().Position();

	UtilGL::Transforming::WorldToNormViewport(&v3LensViewportPos);

	if(m_bZTestFade)
	{
		bool bOnScreen =(v3LensViewportPos.X() >= 0.0f && v3LensViewportPos.X() <= 1.0f) &&
						(v3LensViewportPos.Y() >= 0.0f && v3LensViewportPos.Y() <= 1.0f);
		if(bOnScreen)
		{
			assert(CWindowGL::GetActiveWindow());

			const float DELTA = 0.0f;
			float fZValue;
			int   nWindowHeight = CWindowGL::GetActiveWindow()->GetHeight();

			CVector3 v3LensScreenPos(v3LensViewportPos);

			UtilGL::Transforming::NormViewportToWindow(&v3LensScreenPos);
			glReadPixels(v3LensScreenPos.X(), nWindowHeight - v3LensScreenPos.Y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fZValue);

			if(v3LensViewportPos.Z() - DELTA >= fZValue && !IS_ZERO(m_fFadeDuration))
			{
				m_fIntensity = bFirstFrame ? 0.0f : m_fIntensity - (fIncSeconds / m_fFadeDuration);
			}
			else if(!IS_ZERO(m_fFadeDuration))
			{
				m_fIntensity = bFirstFrame ? 1.0f : m_fIntensity + (fIncSeconds / m_fFadeDuration);
			}

			m_fLastInsideIntensity = m_fIntensity;

			if(m_fLastInsideIntensity > 1.0f) m_fLastInsideIntensity = 1.0f;
			if(m_fLastInsideIntensity < 0.0f) m_fLastInsideIntensity = 0.0f;
		}
		else
		{
			CVector3 v3Center(0.5f, 0.5f, v3LensViewportPos.Z()), v3Corner(0.0f, 0.0f, v3LensViewportPos.Z());

			float fDistCornerFromCenter = v3Corner.Distance(v3Center);
			float fDistLFlareFromCenter = v3LensViewportPos.Distance(v3Center);

			if(fDistLFlareFromCenter > fDistCornerFromCenter)
			{
				m_fIntensity = m_fLastInsideIntensity - ((fDistLFlareFromCenter / fDistCornerFromCenter) - 1.0f);
			}
			else
			{
				m_fIntensity = m_fLastInsideIntensity;
			}
		}
	}

	if(m_fIntensity > 1.0f) m_fIntensity = 1.0f;
	if(m_fIntensity < 0.0f) m_fIntensity = 0.0f;

	// Render

	if(v3LensViewportPos.Z() < 0.0f)
	{
		m_fIntensity = 0.0f;
		return true;
	}

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);

	float    fSizeMultiplier = 0.750f + (m_fIntensity / 4.0f); // Relation intensity->size
	float    fAuxLensSize;
	CVector3 v3AuxLensViewportPos;
	CVector3 v3LensTrayectory;

	// Compute the viewport space ray where the lensflare quads lay on.

	v3LensViewportPos.SetZ(0.001f);
	v3LensTrayectory.Build(v3LensViewportPos, CVector3(0.5f, 0.5f, 0.001f));
	v3LensTrayectory = v3LensTrayectory * 2.0f;

	// Render flares

	CVector4 v4Color(1.0f, 1.0f, 1.0f, m_fIntensity);

	std::vector<SComponent>::iterator it;

	int i;

	for(i = 0, it = m_vecComponents.begin(); it != m_vecComponents.end(); ++it, i++)
	{
		fAuxLensSize = it->fSize * fSizeMultiplier;
		v3AuxLensViewportPos = v3LensViewportPos + (v3LensTrayectory * it->fPos);

		m_vecTextures[i]->SetActive();

		UtilGL::Rendering::DrawCenteredQuad(m_vecTextures[i],
											v3AuxLensViewportPos.X(), v3AuxLensViewportPos.Y(),
											fAuxLensSize, fAuxLensSize, ALMOST_ZERO * 2.0f, v4Color,
											UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_ONE);
	}

	UtilGL::States::Set(UtilGL::States::ZBUFFER, UtilGL::States::ENABLED);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the node data.
//
// -----------------------------------------------------------------------------
bool CLensFlare::FreeNodeData()
{
	std::vector<CTexture2D*>::iterator itTex;

	for(itTex = m_vecTextures.begin(); itTex != m_vecTextures.end(); ++itTex)
	{
		SAFE_DELETE(*itTex);
	}

	m_vecTextures.clear();
	m_vecComponents.clear();

	return true;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : bool FreeNodeData()
//
//  - Purpose   : Reads the lensflare parameters from a data source.
//
// -----------------------------------------------------------------------------
bool CLensFlare::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;
	int		 nAux, nZTest;

	if(!pInput->ReadInt(&nAux))              uError++;
	if(!pInput->ReadInt(&nZTest))            uError++;
	if(!pInput->ReadFloat(&m_fFadeDuration)) uError++;

	m_bZTestFade = nZTest != 0;

	for(int i = 0; i < nAux; i++)
	{
		SComponent component;

		if(!pInput->ReadFloat     (&component.fPos))       uError++;
		if(!pInput->ReadFloat     (&component.fSize))      uError++;
		if(!pInput->ReadStringZero(&component.strTexture)) uError++;

		m_vecComponents.push_back(component);
	}

	// Animation

	if(!ReadAnimation(pInput)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLensFlare
//  - Prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Writes the lensflare parameters to an output.
//
// -----------------------------------------------------------------------------
bool CLensFlare::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(!pOutput->WriteInt  (m_vecComponents.size())) uError++;
	if(!pOutput->WriteInt  (m_bZTestFade ? 1: 0))    uError++;
	if(!pOutput->WriteFloat(m_fFadeDuration))        uError++;

	for(int i = 0; i < m_vecComponents.size(); i++)
	{
		SComponent component = m_vecComponents[i];

		if(!pOutput->WriteFloat     (component.fPos))       uError++;
		if(!pOutput->WriteFloat     (component.fSize))      uError++;
		if(!pOutput->WriteStringZero(component.strTexture)) uError++;
	}

	// Animation

	if(!WriteAnimation(pOutput)) uError++;

	return uError == 0;
}
