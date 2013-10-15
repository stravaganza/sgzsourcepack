// ==[ File ]===================================================================================
//
//  -Name     : Demo.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CDemo implementation.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : CDemo()
//
//  - Purpose   : CDemo's default constructor.
//
// -----------------------------------------------------------------------------
CDemo::CDemo()
{
	SetDefaultCFG();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : ~CDemo()
//
//  - Purpose   : CDemo's destructor.
//
// -----------------------------------------------------------------------------
CDemo::~CDemo()
{
	FreeScript();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool LoadScript(const std::string& strFile, const std::string& strDatfile)
//
//  - Purpose   : Loads the given demoscript. If strDatfile is not an empty
//                string the script will be searched inside the data file.
//                This method loads only the script, not its associated data
//                (effects, resources...), LoadData is used for that.
//
// -----------------------------------------------------------------------------
bool CDemo::LoadScript(const std::string& strFile, const std::string& strDatfile)
{
	FreeScript();

	// Init parser

	CAdvancedParser parser, auxParser;
	CInputMemory    inputMem;
	CInput* pInput = LoadFileFromDat(strFile, true, strDatfile);

	if(!pInput)
	{
		CLogger::ErrorWindow("Unable to open demoscript %s", strFile.data());
		return false;
	}

	parser.Init(pInput);
	parser.ResetPos();

	// Find demo settings field

	if(-1 == parser.Find("#DemoSettings", CParser::SEARCH_WHOLEDATA))
	{
		CLogger::ErrorWindow("CDemo::LoadScript(): (%s) Error finding DemoSettings", strFile.data());
		parser.Close();
		delete pInput;
		return false;
	}

	// Load string between curly braces

	std::string strDemoSettings;

	if(!parser.ReadStringBetweenChars('{', '}', &strDemoSettings, CParser::SEARCH_FROMCURPOS, true))
	{
		CLogger::ErrorWindow("Parse error (#DemoSettings). Field { }");
		parser.Close();
		delete pInput;
		return false;
	}

	// Start a second parser to parse that string

	inputMem.Open(const_cast<char*>(strDemoSettings.data()), strDemoSettings.length());

	auxParser.Init(&inputMem);
	auxParser.ResetPos();

	SetDefaultCFG();

	CVector4 v4Viewport(m_pfViewport[0], m_pfViewport[1], m_pfViewport[2], m_pfViewport[3]);
	CVector2 v2Window  (m_nWinWidth, m_nWinHeight);
	float fAspect = 1.3333f;

	auxParser.ReadKeyFloat  ("#DemoStart",  '=', &m_fStartTime,     CParser::SEARCH_WHOLEDATA);
	auxParser.ReadKeyFloat  ("#DemoLength", '=', &m_fLength,        CParser::SEARCH_WHOLEDATA);
	auxParser.ReadKeyString ("#MusicFile",  '=', &m_strMusicFile,   CParser::SEARCH_WHOLEDATA);
	auxParser.ReadKeyVector2("#Window",     '=', &v2Window,         CParser::SEARCH_WHOLEDATA);
	auxParser.ReadKeyFloat  ("#Aspect",     '=', &fAspect,          CParser::SEARCH_WHOLEDATA);
	auxParser.ReadKeyBool   ("#Fullscreen", '=', &m_bWinFullscreen, CParser::SEARCH_WHOLEDATA);
	auxParser.ReadKeyVector4("#Viewport",   '=', &v4Viewport,       CParser::SEARCH_WHOLEDATA);

	this->SetAspect(fAspect);

	m_nWinWidth  = v2Window.X();
	m_nWinHeight = v2Window.Y();

	memcpy(m_pfViewport, v4Viewport.Data(), sizeof(float) * 4);

	auxParser.Close();
	inputMem.Close();

	// Find hidden layer list

	if(-1 != parser.Find("#Hidden Layers", CParser::SEARCH_WHOLEDATA))
	{
		// Load string between curly braces

		std::string strHiddenLayers;

		if(!parser.ReadStringBetweenChars('{', '}', &strHiddenLayers, CParser::SEARCH_FROMCURPOS, true))
		{
			CLogger::ErrorWindow("Parse error (#Hidden Layers). Field { }");
			parser.Close();
			delete pInput;
			return false;
		}

		// Start a second parser to parse that string

		inputMem.Open(const_cast<char*>(strHiddenLayers.data()), strHiddenLayers.length());

		auxParser.Init(&inputMem);
		auxParser.ResetPos();

		int nLayer;

		while(auxParser.ReadKeyInt("#Layer", '=', &nLayer, CParser::SEARCH_FROMCURPOS))
		{
			m_vecHiddenLayers.push_back(nLayer);
		}

		auxParser.Close();
		inputMem.Close();
	}

	// Find resource field

	if(-1 == parser.Find("#Resources", CParser::SEARCH_WHOLEDATA))
	{
		CLogger::ErrorWindow("CDemo::LoadScript(): (%s) Error finding resources", strFile.data());
		parser.Close();
		delete pInput;
		return false;
	}

	int nError = 0;

	// Load string between curly braces

	std::string strResources;

	if(!parser.ReadStringBetweenChars('{', '}', &strResources, CParser::SEARCH_FROMCURPOS, true))
	{
		CLogger::ErrorWindow("Parse error (#Resources). Field { }");
		parser.Close();
		delete pInput;
		return false;
	}

	// Start a second parser to parse that string

	inputMem.Open(const_cast<char*>(strResources.data()), strResources.length());

	auxParser.Init(&inputMem);
	auxParser.ResetPos();

	// Parse all resources

	while(-1 != auxParser.Find("#Resource", CParser::SEARCH_FROMCURPOS))
	{
		std::string strResName, strResClass, strResFile, strResAddPath;

		// Name, class and file

		if(!auxParser.ReadKeyString("#Name",  '=', &strResName,  CParser::SEARCH_INCURLINE)) nError++;
		if(!auxParser.ReadKeyString("#Class", '=', &strResClass, CParser::SEARCH_INCURLINE)) nError++;
		if(!auxParser.ReadKeyString("#File",  '=', &strResFile,  CParser::SEARCH_INCURLINE)) nError++;

		if(nError)
		{
			CLogger::ErrorWindow("Parse error on resource %u", m_resourceList.GetResourceCount() + 1);
			break;
		}

		CResource* pResource = (CResource*)CPluginManager::Instance().GetNewPlugin(CResource::GETCLASSTYPE(), strResClass);

		if(!pResource)
		{
			nError++;
			CLogger::ErrorWindow("Parse error on resource %u. Unknown class (%s)", m_resourceList.GetResourceCount() + 1, strResClass.data());
			break;
		}

		// Needs additional path?

		if(pResource->NeedsAdditionalPath())
		{
			if(!auxParser.ReadKeyString("#AdditionalPath", '=', &strResAddPath, CParser::SEARCH_INCURLINE))
			{
				nError++;
				CPluginManager::Instance().DeletePlugin(pResource);
				break;
			}

			pResource->SetAdditionalPath(strResAddPath);
		}

		pResource->SetResourceName(strResName);
		pResource->SetResourceFile(strResFile);
		AddResource(pResource);
	}

	auxParser.Close();
	inputMem.Close();

	if(nError)
	{
		// Error during resource parsing

		m_resourceList.FreeAll();
		m_resourceList.RemoveAll();
		parser.Close();
		delete pInput;
		return false;
	}

	// Parse effects

	while(-1 != parser.Find("#Effect", CParser::SEARCH_FROMCURPOS))
	{
		std::string strFXName, strFXClass, strEffectField;

		if(!parser.ReadKeyString("#Name",  '=', &strFXName,  CParser::SEARCH_INCURLINE)) nError++;
		if(!parser.ReadKeyString("#Class", '=', &strFXClass, CParser::SEARCH_INCURLINE)) nError++;

		if(nError)
		{
			if(strFXName.empty())	CLogger::ErrorWindow("Parse error on effect %d", m_vecEffectList.size() + 1);
			else					CLogger::ErrorWindow("Parse error on effect %s", strFXName.data());

			break;
		}

		if(!parser.ReadStringBetweenChars('{', '}', &strEffectField, CParser::SEARCH_FROMCURPOS, true))
		{
			nError++;
			CLogger::ErrorWindow("Parse error on effect %s. Field { }", strFXName.data());
			break;
		}

		CEffect* pEffect = (CEffect*)CPluginManager::Instance().GetNewPlugin(CEffect::GETCLASSTYPE(), strFXClass);

		if(!pEffect)
		{
			nError++;
			CLogger::ErrorWindow("Parse error on effect %s. Class unknown (%s)", strFXName.data(), strFXClass.data());
			break;
		}

		pEffect->SetFXName(strFXName);

		// Secondary parser for the effect itself

		inputMem.Open(const_cast<char*>(strEffectField.data()), strEffectField.length());

		auxParser.Init(&inputMem);
		auxParser.ResetPos();

		if(!pEffect->Parse(&auxParser))
		{
			nError++;
			CLogger::ErrorWindow("Error on CEffect::Parse(). Effect %s", strFXName.data());
			CPluginManager::Instance().DeletePlugin(pEffect);
		}
		else
		{
			AddEffect(pEffect);
		}
	}

	if(nError)
	{
		// Error during effect parsing

		std::vector<CEffect*>::iterator it;

		for(it = m_vecEffectList.begin(); it != m_vecEffectList.end(); ++it)
		{
			CPluginManager::Instance().DeletePlugin(*it);
		}

		m_vecEffectList.clear();
		m_resourceList.FreeAll();
		m_resourceList.RemoveAll();
		parser.Close();
		delete pInput;
		return false;
	}

	m_fLastTime = 0.0f;

	parser.Close();
	delete pInput;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool SaveScript(const std::string& strFile)
//
//  - Purpose   : Saves all demo effects/variables/resources to a text script.
//                This method is mainly used by the demo editor.
//
// -----------------------------------------------------------------------------
bool CDemo::SaveScript(const std::string& strFile)
{
	CWriterASCII writer;

	// Open file for writing

	if(!writer.InitFile(strFile))
	{
		CLogger::ErrorWindow("Can't open %s", strFile.data());
	}

	// Write demo settings

	writer.Write("#DemoSettings");
	writer.Write("\n{");
	writer.Write("\n  #DemoStart  = %f", m_fStartTime);
	writer.Write("\n  #DemoLength = %f", m_fLength);
	writer.Write("\n  #MusicFile  = \"%s\"", GetRelativePath(GetFileFolder(strFile), m_strMusicFile).data());
	writer.Write("\n  #Window     = (%u, %u)", m_nWinWidth, m_nWinHeight);
	writer.Write("\n  #Aspect     = %f", this->GetAspect());
	writer.Write("\n  #Fullscreen = %s", m_bWinFullscreen ? "True" : "False");
	writer.Write("\n  #Viewport   = (%f, %f, %f, %f)", m_pfViewport[0], m_pfViewport[1], m_pfViewport[2], m_pfViewport[3]);
	writer.Write("\n}\n");

	// Write hidden layer list

	if(m_vecHiddenLayers.size() > 0)
	{
		writer.Write("\n#Hidden Layers");
		writer.Write("\n{");

		std::vector<int>::iterator it;

		for(it = m_vecHiddenLayers.begin(); it != m_vecHiddenLayers.end(); ++it)
		{
			writer.Write("\n  #Layer=%u", *it);
		}

		writer.Write("\n}\n");
	}

	// Write resources

	writer.Write("\n#Resources");
	writer.Write("\n{");

	for(int i = 0; i < m_resourceList.GetResourceCount(); i++)
	{
		CResource* pResource = m_resourceList.GetResource(i);
		assert(pResource);

		writer.Write("\n  #Resource #Name=\"%s\" #Class=\"%s\" #File=\"%s\"",
			pResource->GetResourceName().data(),
			pResource->GetClassName().data(),
			GetRelativePath(GetFileFolder(strFile), pResource->GetResourceFile()).data());

		if(pResource->NeedsAdditionalPath())
		{
			writer.Write(" #AdditionalPath=\"%s\"", GetRelativePath(GetFileFolder(strFile), pResource->GetAdditionalPath()).data());
		}
	}

	writer.Write("\n}");

	// Write effects

	std::vector<CEffect*>::iterator fxIt;

	for(fxIt = m_vecEffectList.begin(); fxIt != m_vecEffectList.end(); ++fxIt)
	{
		assert(*fxIt);

		writer.Write("\n");
		if(!(*fxIt)->WriteASCII(&writer))
		{
			CLogger::ErrorWindow("Error writing effect %s", (*fxIt)->GetFXName().data());
		}
	}

	writer.Close();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool FreeScript()
//
//  - Purpose   : Frees the currently loaded script and all its data.
//
// -----------------------------------------------------------------------------
bool CDemo::FreeScript()
{
	int nError = 0;

	if(!FreeData()) nError++;

	std::vector<CEffect*>::iterator fxIt;

	for(fxIt = m_vecEffectList.begin(); fxIt != m_vecEffectList.end(); ++fxIt)
	{
		if(*fxIt)
		{
			LOG.Write("\nDeleting effect %s.", ((CEffect*)*fxIt)->GetFXName().data());
			CPluginManager::Instance().DeletePlugin(*fxIt);
			LOG.Write(" Done");
		}
	}

	m_strMusicFile.erase();
	m_vecEffectList.clear();
	m_resourceList.RemoveAll();

	SetDefaultCFG();

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool LoadData(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
//
//  - Purpose   : Once a given script is loaded its associated data can be loaded
//                too (effects, resources, internal data...). This method
//                does this task.
//
// -----------------------------------------------------------------------------
bool CDemo::LoadData(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
//	FreeData();

	OnLoadStart();
	OnLoading(0.0f);

	// Count number of elements to load.

	int m_nTotalElements = m_resourceList.GetResourceCount() + m_vecEffectList.size();
	int m_nCurElement  = 0;

	if(!m_strMusicFile.empty())
	{
		m_nTotalElements++;

		LOG.Write("\nSetting up music:");

		// Open music file

		LOG.Write("\n  Loading music file %s", m_strMusicFile.data());

		CInputFile inputMusicFile;

		if(!inputMusicFile.Open(m_strMusicFile, false))
		{
			CLogger::ErrorWindow("Error opening music file %s", m_strMusicFile.data());
			FreeData();
			return false;
		}

		LOG.Write("\n  Initializing sound system");

		// Init sound system

		if(!CSoundSystem::Instance().Init(this->GetHWND()))
		{
			CLogger::ErrorWindow("Error initializing sound system");
			FreeData();
			return false;
		}

		LOG.Write("\n  Setting Volume");

		CSoundSystem::Instance().SetMasterVolume(100.0f);

		// Load music file

		LOG.Write("\n  Loading soundtrack");

		if(!(m_pMusic = CSoundSystem::Instance().LoadSound(&inputMusicFile, GetFileExt(m_strMusicFile))))
		{
			CLogger::ErrorWindow("Error loading sound %s into sound system", m_strMusicFile.data());
			FreeData();
			return false;
		}

		m_nCurElement++;
		OnLoading((m_nCurElement * 100.0f) / m_nTotalElements);
	}

	// Load resources

	LOG.Write("\nLoading resources");

	for(int i = 0; i < m_resourceList.GetResourceCount(); i++)
	{
		CResource* pResource = m_resourceList.GetResource(i);
		assert(pResource);

		if(!pResource->Load(strDatFile, pTexOptions))
		{
			CLogger::ErrorWindow("Error loading resource %s", pResource->GetResourceFile().data());
			FreeData();
			return false;
		}

		m_nCurElement++;
		OnLoading((m_nCurElement * 100.0f) / m_nTotalElements);
	}

	// Load effects

	LOG.Write("\nLoading effects");

	std::vector<CEffect*>::iterator fxIt;

	for(fxIt = m_vecEffectList.begin(); fxIt != m_vecEffectList.end(); ++fxIt)
	{
		assert(*fxIt);

		LOG.Write("\n  Loading effect %s", (*fxIt)->GetFXName().data());

		LOG.IncIndentation(2);

		if(!(*fxIt)->LoadData(&m_resourceList))
		{
			LOG.DecIndentation(2);
			CLogger::ErrorWindow("Error loading effect %s's data", (*fxIt)->GetFXName().data());
			FreeData();
			return false;
		}

		LOG.DecIndentation(2);

		m_nCurElement++;
		OnLoading((m_nCurElement * 100.0f) / m_nTotalElements);
	}

	LOG.Write("\nDone");

	OnLoading(100.0f);
	OnLoadFinish();
	glFinish();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool FreeData()
//
//  - Purpose   : Frees all data (resources, textures...) but not the script.
//                This is useful when some references have been changed to
//                the script and the new data needs to be loaded.
//
// -----------------------------------------------------------------------------
bool CDemo::FreeData()
{
	// Free music and close sound system

	SAFE_DELETE(m_pMusic);
	CSoundSystem::Instance().Close();

	// Free effects

	std::vector<CEffect*>::iterator fxIt;

	for(fxIt = m_vecEffectList.begin(); fxIt != m_vecEffectList.end(); ++fxIt)
	{
		if(*fxIt)
		{
			LOG.Write("\nFreeing effect %s.", ((CEffect*)*fxIt)->GetFXName().data());
			(*fxIt)->Free();
			LOG.Write(" Done");
		}
	}

	LOG.Write("\n");

	// Free resources

	m_resourceList.FreeAll();
	m_vecLastFrameEffectList.clear();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool SetDefaultCFG()
//
//  - Purpose   : Sets the default demo config.
//
// -----------------------------------------------------------------------------
bool CDemo::SetDefaultCFG()
{
	m_pfViewport[0]  = 0.5f;
	m_pfViewport[1]  = 0.5f;
	m_pfViewport[2]  = 1.0f;
	m_pfViewport[3]  = 1.0f;
	m_bScissor       = false;
	m_nWinWidth      = 800;
	m_nWinHeight     = 600;
	m_bWinFullscreen = true;
	m_pMusic         = NULL;
	m_strMusicFile   = "";
	m_fLength        = 0.0f;
	m_bExit          = false;
	m_bLoop          = false;
	m_fLastTime      = 0.0f;
	m_nNumFrame      = 0;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : float GetStart() const
//
//  - Purpose   : Returns the demo start time (seconds).
//
// -----------------------------------------------------------------------------
float CDemo::GetStart() const
{
	return m_fStartTime;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : float GetLength() const
//
//  - Purpose   : Returns the demo length (seconds).
//
// -----------------------------------------------------------------------------
float CDemo::GetLength() const
{
	return m_fLength;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void SetLoop(bool bLoop)
//
//  - Purpose   : Tells if the demo has to be looped.
//
// -----------------------------------------------------------------------------
void CDemo::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool GetLoop() const
//
//  - Purpose   : Returns a boolean telling if the demo has to be looped.
//
// -----------------------------------------------------------------------------
bool CDemo::GetLoop() const
{
	return m_bLoop;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void SetStart(float fSeconds)
//
//  - Purpose   : Sets the demo start time (seconds).
//
// -----------------------------------------------------------------------------
void CDemo::SetStart(float fSeconds)
{
	m_fStartTime = fSeconds;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void SetLength(float fSeconds)
//
//  - Purpose   : Sets the demo length.
//
// -----------------------------------------------------------------------------
void CDemo::SetLength(float fSeconds)
{
	m_fLength = fSeconds;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void SetSoundtrack(const std::string& strFile)
//
//  - Purpose   : Tells which sound file the demo needs to play.
//
// -----------------------------------------------------------------------------
void CDemo::SetSoundtrack(const std::string& strFile)
{
	m_strMusicFile = strFile;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : std::string GetSoundtrack() const
//
//  - Purpose   : Returns which sound file the demo needs to play.
//
// -----------------------------------------------------------------------------
std::string CDemo::GetSoundtrack() const
{
	return m_strMusicFile;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : const CSound* GetSoundIface () const
//
//  - Purpose   : Returns the sound interface being used for the demo.
//
// -----------------------------------------------------------------------------
const CSound* CDemo::GetSoundIface () const
{
	return m_pMusic;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool ReloadSoundtrack()
//
//  - Purpose   : Returns the sound file that the demo needs to play.
//
// -----------------------------------------------------------------------------
bool CDemo::ReloadSoundtrack()
{
	SAFE_DELETE(m_pMusic);
	
	if(m_strMusicFile != "")
	{
		CInputFile inputMusicFile;

		if(!inputMusicFile.Open(m_strMusicFile, false))
		{
			CLogger::ErrorWindow("Error opening music file %s", m_strMusicFile.data());
			return false;
		}

		m_pMusic = CSoundSystem::Instance().LoadSound(&inputMusicFile, GetFileExt(m_strMusicFile));
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void ConfigDemoViewport(const float pfViewport[4], bool bScissor)
//
//  - Purpose   : Tells which viewport the demo has to use (x, y, w, h) and
//                if scissoring has to be used.
//
// -----------------------------------------------------------------------------
void CDemo::ConfigDemoViewport(const float pfViewport[4], bool bScissor)
{
	assert(pfViewport);
	memcpy(m_pfViewport, pfViewport, sizeof(float) * 4);
	m_bScissor = bScissor;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : float* GetDemoViewport()
//
//  - Purpose   : Returns the demo viewport (the one specified by
//                ConfigDemoViewport, not the current one which may have
//                changed by some effect).
//
// -----------------------------------------------------------------------------
float* CDemo::GetDemoViewport()
{
	return m_pfViewport;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void SetDemoViewport()
//
//  - Purpose   : Sets the demo viewport specified by ConfigDemoViewport.
//
// -----------------------------------------------------------------------------
void CDemo::SetDemoViewport()
{
	float fX = m_pfViewport[0] - m_pfViewport[2] * 0.5f;
	float fY = m_pfViewport[1] - m_pfViewport[3] * 0.5f;

	this->SetViewport(fX, fY, m_pfViewport[2], m_pfViewport[3], m_bScissor);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool ScissorActive() const
//
//  - Purpose   : Tells if scissoring is being used.
//
// -----------------------------------------------------------------------------
bool CDemo::ScissorActive() const
{
	return m_bScissor;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void ConfigDemoWindow(int nWidth, int nHeight, bool bFullscreen)
//
//  - Purpose   : Tells the window properties for the demo.
//
// -----------------------------------------------------------------------------
void CDemo::ConfigDemoWindow(int nWidth, int nHeight, bool bFullscreen)
{
	m_nWinWidth      = nWidth;
	m_nWinHeight     = nHeight;
	m_bWinFullscreen = bFullscreen;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : int GetDemoWindowWidth() const
//
//  - Purpose   : Returns the width value specified in ConfigDemoWindow.
//
// -----------------------------------------------------------------------------
int CDemo::GetDemoWindowWidth() const
{
	return m_nWinWidth;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : int GetDemoWindowHeight() const
//
//  - Purpose   : Returns the height value specified in ConfigDemoWindow.
//
// -----------------------------------------------------------------------------
int CDemo::GetDemoWindowHeight() const
{
	return m_nWinHeight;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool IsDemoWindowFS() const
//
//  - Purpose   : Returns the fullscreen value specified in ConfigDemoWindow.
//
// -----------------------------------------------------------------------------
bool CDemo::IsDemoWindowFS() const
{
	return m_bWinFullscreen;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool AddResource(CResource* pResource)
//
//  - Purpose   : Adds a given resource to the demo.
//
// -----------------------------------------------------------------------------
bool CDemo::AddResource(CResource* pResource)
{
	if(!pResource)
	{
		return false;
	}

	return m_resourceList.AddResource(pResource);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : int GetResourceCount() const
//
//  - Purpose   : Returns the number of demo resources added.
//
// -----------------------------------------------------------------------------
int CDemo::GetResourceCount() const
{
	return m_resourceList.GetResourceCount();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : CResource* GetResource(int i)
//
//  - Purpose   : Returns a pointer to the i-th resource.
//
// -----------------------------------------------------------------------------
CResource* CDemo::GetResource(int i)
{
	return m_resourceList.GetResource(i);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : CResource* GetResource(const std::string& strName)
//
//  - Purpose   : Returns a pointer to the resource with the given name.
//
// -----------------------------------------------------------------------------
CResource* CDemo::GetResource(const std::string& strName)
{
	return m_resourceList.GetResource(strName);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool RemoveResource(const std::string& strName, bool bAlsoFree)
//
//  - Purpose   : Removes a resource from the list and also optionally frees it.
//
// -----------------------------------------------------------------------------
bool CDemo::RemoveResource(const std::string& strName, bool bAlsoFree)
{
	return m_resourceList.RemoveResource(strName, bAlsoFree);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : CResourceList* GetResourceList()
//
//  - Purpose   : Returns a pointer to the internal resource list.
//
// -----------------------------------------------------------------------------
CResourceList* CDemo::GetResourceList()
{
	return &m_resourceList;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool AddEffect(CEffect* pEffect)
//
//  - Purpose   : Adds an effect to the demo.
//
// -----------------------------------------------------------------------------
bool CDemo::AddEffect(CEffect* pEffect)
{
	if(!pEffect)
	{
		return false;
	}

	pEffect->SetDemo(this);
	m_vecEffectList.push_back(pEffect);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : int GetEffectCount() const
//
//  - Purpose   : Returns the number of effects added to the demo.
//
// -----------------------------------------------------------------------------
int CDemo::GetEffectCount() const
{
	return m_vecEffectList.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : CEffect* GetEffect(int i)
//
//  - Purpose   : Returns a pointer to the i-th effect.
//
// -----------------------------------------------------------------------------
CEffect* CDemo::GetEffect(int i)
{
	if(i < 0 || i >= m_vecEffectList.size())
	{
		return NULL;
	}

	return m_vecEffectList[i];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : CEffect* GetEffect(const std::string& strName)
//
//  - Purpose   : Returns a pointer to the effect with the given name.
//
// -----------------------------------------------------------------------------
CEffect* CDemo::GetEffect(const std::string& strName)
{
	// I didn't use std::map to speed up the search because effect names can be
	// changed at runtime.

	std::vector<CEffect*>::iterator fxIt;

	for(fxIt = m_vecEffectList.begin(); fxIt != m_vecEffectList.end(); ++fxIt)
	{
		assert(*fxIt);

		if((*fxIt)->GetFXName() == strName)
		{
			return *fxIt;
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool RemoveEffect(const std::string& strName, bool bAlsoDelete)
//
//  - Purpose   : Removes the given effect from the list, and also optionally
//                frees all associated data.
//
// -----------------------------------------------------------------------------
bool CDemo::RemoveEffect(const std::string& strName, bool bAlsoDelete)
{
	// I didn't use std::map to speed up the search because effect names can be
	// changed at runtime.

	std::vector<CEffect*>::iterator fxIt;

	for(fxIt = m_vecEffectList.begin(); fxIt != m_vecEffectList.end(); ++fxIt)
	{
		assert(*fxIt);

		if((*fxIt)->GetFXName() == strName)
		{
			(*fxIt)->SetDemo(NULL);
			m_vecEffectList.erase(fxIt);
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : int GetHiddenLayerCount() const
//
//  - Purpose   : Returns the number of layers hidden (whose effects are not drawn).
//
// -----------------------------------------------------------------------------
int	CDemo::GetHiddenLayerCount() const
{
	return m_vecHiddenLayers.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : int GetHiddenLayer(int i) const
//
//  - Purpose   : Returns the i-th hidden layer.
//
// -----------------------------------------------------------------------------
int	CDemo::GetHiddenLayer(int i) const
{
	return m_vecHiddenLayers[i];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool IsLayerShown(int nLayer) const
//
//  - Purpose   : Returns true if the given layer is hidden.
//
// -----------------------------------------------------------------------------
bool CDemo::IsLayerShown(int nLayer) const
{
	std::vector<int>::const_iterator it;

	for(it = m_vecHiddenLayers.begin(); it != m_vecHiddenLayers.end(); ++it)
	{
		// Found in the hidden layer list?

		if(*it == nLayer)
		{
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void ShowLayer(int nLayer, bool bShow)
//
//  - Purpose   : Shows or hides the given layer.
//
// -----------------------------------------------------------------------------
void CDemo::ShowLayer(int nLayer, bool bShow)
{
	std::vector<int>::iterator it;

	for(it = m_vecHiddenLayers.begin(); it != m_vecHiddenLayers.end(); ++it)
	{
		if(*it == nLayer)
		{
			if(bShow == true)
			{
				m_vecHiddenLayers.erase(it);
				return;
			}
			else
			{
				// Redundant call
				return;
			}
		}
	}

	if(bShow == false)
	{
		m_vecHiddenLayers.push_back(nLayer);
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void ShowAllLayers()
//
//  - Purpose   : Shows all layers that have been hidden.
//
// -----------------------------------------------------------------------------
void CDemo::ShowAllLayers()
{
	m_vecHiddenLayers.clear();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool Prepare()
//
//  - Purpose   : Prepares the demo before being played. Mainly does the
//                layer sorting and some things that may be needed in the future.
//
// -----------------------------------------------------------------------------
bool CDemo::Prepare()
{
	std::sort(m_vecEffectList.begin(), m_vecEffectList.end(), CEffect::SortByTimePredicate);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool Run(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
//
//  - Purpose   : Hi-level method. Runs a whole demo given an optional data
//                file and texture loading options.
//
// -----------------------------------------------------------------------------
bool CDemo::Run(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	if(!UtilGL::RenderContext::Create(32, 24, 8))
	{
		return false;
	}

	if(!CheckRequirements())
	{
		FreeScript();
		UtilGL::RenderContext::Delete();
		return false;
	}

	if(!this->Create("Stravaganza demo", NULL, m_nWinWidth, m_nWinHeight, m_bWinFullscreen, 0))
	{
		UtilGL::RenderContext::Delete();
		return false;
	}

	this->SetAsActiveWindow();
	this->SetDemoViewport();

	// Notify dlls:
	CPluginManager::Instance().NotifyCreateRC();

	if(!this->LoadData(strDatFile, pTexOptions))
	{
		this->Destroy();
		UtilGL::RenderContext::Delete();
		return false;
	}

	this->Prepare();

	float  t;
	CTimer timer;
	MSG    msg;

	this->OnPlayStart();

	do
	{
		if(m_pMusic)
		{
			m_pMusic->SetPlayPos(m_fStartTime);
			m_pMusic->SetVolume(100.0f);
			m_pMusic->Play();
		}

		timer.Start();

		while(((t = timer.Seconds() + m_fStartTime) <= this->GetLength()) && !this->ExitRequested())
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			this->DoFrame(t);
		}

		if(m_pMusic)
		{
			m_pMusic->Stop();
		}

		timer.Stop();

	}while(m_bLoop && !this->ExitRequested());

	this->OnPlayFinish();

	this->FreeScript();
	this->Destroy();

	UtilGL::RenderContext::Delete();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool DoFrame(float fTime)
//
//  - Purpose   : Draws the frame given a time value.
//
// -----------------------------------------------------------------------------
bool CDemo::DoFrame(float fTime)
{
	std::vector<CEffect*>::iterator it, itAux;
	std::vector<CEffect*> vecFrameEffects;

	SetDemoViewport();

	this->OnFrameStart(fTime, m_nNumFrame);

	// Put in a vector all effects that have to be executed this frame

	for(it = m_vecEffectList.begin(); it != m_vecEffectList.end(); ++it)
	{
		if(IsLayerShown((*it)->GetLayer()))
		{
			if(((*it)->GetBegin() <= fTime) && (fTime < (*it)->GetEnd()))
			{
				vecFrameEffects.push_back(*it);
			}
		}
	}

	// Sort this effects by layer

	std::sort(vecFrameEffects.begin(), vecFrameEffects.end(), CEffect::SortByLayerPredicate);

	// Check for commands

	for(it = vecFrameEffects.begin(); it != vecFrameEffects.end(); ++it)
	{
		if(fTime > m_fLastTime)
		{
			// Forward

			for(int i = (*it)->GetCommandCount() - 1; i >= 0; i--)
			{
				float fCmdTime;
				std::string strCommand;
				(*it)->GetCommand(i, &fCmdTime, &strCommand);

				fCmdTime += (*it)->GetBegin();

				if(m_fLastTime <= fCmdTime && fCmdTime < fTime)
				{
					if(!(*it)->ExecuteCommand(strCommand))
					{
						(*it)->FXRuntimeError("Error executing command <%s>", strCommand.data());
					}

					break;
				}
			}
		}
		else
		{
			// Backward

			for(int i = (*it)->GetCommandCount() - 1; i >= 0; i--)
			{
				float fCmdTime;
				std::string strCommand;
				(*it)->GetCommand(i, &fCmdTime, &strCommand);

				fCmdTime += (*it)->GetBegin();

				if(fTime <= fCmdTime && fCmdTime < m_fLastTime)
				{
					// Execute all commands from 0 to the found one

					for(int n = 0; n < i; n++)
					{
						(*it)->GetCommand(n, &fCmdTime, &strCommand);

						if(!(*it)->ExecuteCommand(strCommand))
						{
							(*it)->FXRuntimeError("Error executing command <%s>", strCommand.data());
						}
					}
				}
			}
		}
	}

	// Check need for OnEnter callbacks

	for(it = vecFrameEffects.begin(); it != vecFrameEffects.end(); ++it)
	{
		itAux = std::find(m_vecLastFrameEffectList.begin(), m_vecLastFrameEffectList.end(), *it);

		if(itAux == m_vecLastFrameEffectList.end())
		{
			(*it)->OnEnter(this, fTime - (*it)->GetBegin(), fTime);
		}
	}

	// Check need for OnLeave callbacks

	for(it = m_vecLastFrameEffectList.begin(); it != m_vecLastFrameEffectList.end(); ++it)
	{
		itAux = std::find(vecFrameEffects.begin(), vecFrameEffects.end(), *it);

		if(itAux == vecFrameEffects.end())
		{
			(*it)->OnLeave(this, fTime - (*it)->GetBegin(), fTime);
		}
	}

	// Execute preframe callbacks

	for(it = vecFrameEffects.begin(); it != vecFrameEffects.end(); ++it)
	{
		(*it)->OnDemoFrameBegin(this, fTime - (*it)->GetBegin(), fTime);
	}

	// Execute effect frames

	for(it = vecFrameEffects.begin(); it != vecFrameEffects.end(); ++it)
	{
		OnEffectStart(*it);
		(*it)->DoFrame(this, fTime - (*it)->GetBegin(), fTime);
		OnEffectFinish(*it);
	}

	// Execute post frame callbacks

	for(it = vecFrameEffects.begin(); it != vecFrameEffects.end(); ++it)
	{
		(*it)->OnDemoFrameEnd(this, fTime - (*it)->GetBegin(), fTime);
	}

	this->OnFrameFinish(fTime, m_nNumFrame);

	// Swap Backbuffer -> Frontbuffer

	SwapBuffers(this->GetHDC());

	// Store this frame effects for next frame CEffect::OnEnter/CEffect::OnLeave checks

	m_vecLastFrameEffectList.clear();
	m_vecLastFrameEffectList.assign(vecFrameEffects.begin(), vecFrameEffects.end());

	m_fLastTime = fTime;

	m_nNumFrame++;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : void RequestExit()
//
//  - Purpose   : Allows the demo to be quit from outside.
//
// -----------------------------------------------------------------------------
void CDemo::RequestExit()
{
	m_bExit = true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : bool ExitRequested() const
//
//  - Purpose   : Check if RequestExit() has been called.
//
// -----------------------------------------------------------------------------
bool CDemo::ExitRequested() const
{
	return m_bExit;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CDemo
//  - prototype : LRESULT WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
//
//  - Purpose   : Default demo window procedure.
//
// -----------------------------------------------------------------------------
LRESULT CDemo::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage)
	{

	case WM_CREATE:

		return 0;

	case WM_CLOSE:

		RequestExit();
		return TRUE;

	case WM_DESTROY:

		PostQuitMessage(0);
		return TRUE;

	case WM_SIZE:

		if(IsActiveWindow())
		{
			this->SetDemoViewport();
			UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
			gluPerspective(60.0f, this->GetAspect(), 1.0f, 3000.0f);
		}

		return TRUE;

	case WM_KEYDOWN:

		if(wParam == VK_ESCAPE)
		{
			RequestExit();
			return TRUE;
		}

		break;

	default:

		break;

	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

//////////////////////////////// VIRTUAL METHODS. DEFAULT IMPLEMENTATIONS: /////////////////////////////////

bool CDemo::CheckRequirements()
{
	return true;
}

void CDemo::OnLoadStart()
{

}

void CDemo::OnLoadFinish()
{

}

void CDemo::OnLoading(float fPercentage)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	this->TextOut(0.42f, 0.5f, "Loading %u%%", (int)fPercentage);
	SwapBuffers(this->GetHDC());
}

void CDemo::OnPlayStart()
{

}

void CDemo::OnPlayFinish()
{

}

void CDemo::OnFrameStart(float fDemoTime, int nNumFrame)
{

}

void CDemo::OnFrameFinish(float fDemoTime, int nNumFrame)
{

}

void CDemo::OnEffectStart (CEffect* pEffect)
{

}

void CDemo::OnEffectFinish(CEffect* pEffect)
{

}
