// ==[ File ]===================================================================================
//
//  -Name     : PluginManager.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPluginManager implementation.
//
// =============================================================================================

/*

	05/12/2004  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


//#define CPLUGINMANAGER_VERBOSE


CPluginManager* CPluginManager::s_pSharedInstance = NULL;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : CPluginManager()
//
//  - Purpose   : CPluginManager's default constructor.
//
// -----------------------------------------------------------------------------
CPluginManager::CPluginManager()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : ~CPluginManager()
//
//  - Purpose   : CPluginManager's destructor. If this is the global instance,
//                close it.
//
// -----------------------------------------------------------------------------
CPluginManager::~CPluginManager()
{
	if(!s_pSharedInstance)
	{
		Close();
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : CPluginManager& Instance()
//
//  - Purpose   : Returns the global instance using the singleton pattern.
//
// -----------------------------------------------------------------------------
CPluginManager& CPluginManager::Instance()
{
	static CPluginManager pluginInterface;

	if(s_pSharedInstance)
	{
		return *s_pSharedInstance;
	}

	return pluginInterface;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : void ShareFrom(CPluginManager& pluginManager)
//
//  - Purpose   : Shares the plugin manager so that it can be used from
//                other modules -like a dll- where the singleton pattern
//                won't work (each one has its own global variables).
//
// -----------------------------------------------------------------------------
void CPluginManager::ShareFrom(CPluginManager& pluginManager)
{
	s_pSharedInstance = &pluginManager;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : bool Init(const std::string& strPluginPath)
//
//  - Purpose   : Initializes the plugin system. Loads all available plugins
//                searching in the given path ("" = current directory).
//
// -----------------------------------------------------------------------------
bool CPluginManager::Init(const std::string& strPluginPath)
{
	WIN32_FIND_DATA  fileFoundData;   // File found info.
	HANDLE           fileFoundHandle; // File found handle.

	std::string strSearch = strPluginPath;

	// As we need to append *.dll to the path, check if we need to append a backslash first

	if(strSearch.length() > 0)
	{
		if(strSearch.c_str()[strSearch.length() - 1] != '\\')
		{
			strSearch += "\\";
		}
	}

	strSearch += "*.dll";

	LOG.Write("\nInitializing plugin manager. Performing search of plugins (path=\"%s\"):\n", strSearch.c_str());
	LOG.IncIndentation(2);

	// FindFirst

	fileFoundHandle = FindFirstFile(strSearch.c_str(), &fileFoundData);

	if(fileFoundHandle == INVALID_HANDLE_VALUE)
	{
		CLogger::ErrorWindow("No plugins found or invalid path: \"%s\"", strPluginPath.c_str());
		return true;
	}

	if(!(fileFoundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		ProcessDll(fileFoundData.cFileName);
	}

	// FindNext

	while(FindNextFile(fileFoundHandle, &fileFoundData))
	{
		if(!(fileFoundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			ProcessDll(fileFoundData.cFileName);
		}
	}

	FindClose(fileFoundHandle);

	LOG.DecIndentation(2);
	LOG.Write("\nPlugin manager ready.\n");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : bool Close()
//
//  - Purpose   : Closes the plugin manager. Frees all dlls.
//
// -----------------------------------------------------------------------------
bool CPluginManager::Close()
{
	LOG.Write("\nShutting down plugin manager: ");

	MAPDLL2DLLINFO::iterator it;

	for(it = m_mapDll2DllInfo.begin(); it != m_mapDll2DllInfo.end(); ++it)
	{
		it->second.procCloseSGZDll();
		FreeLibrary(it->second.hDll);
	}

	m_mapDll2DllInfo.clear();
	m_mapClass2PDllInfo.clear();
	m_mapType2Plugins.clear();

	LOG.Write("OK");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : bool ProcessDll(const std::string& strDll)
//
//  - Purpose   : Processes a valid stravaganza dll. Iterates through all
//                available plugins storing its information.
//
// -----------------------------------------------------------------------------
bool CPluginManager::ProcessDll(const std::string& strDll)
{
	// Load DLL

	HMODULE hDll = LoadLibrary(strDll.c_str());

	if(!hDll)
	{
		return false;
	}

	// Try to get our function adresses, remember it may not be a stravaganza dll.

	SDllInfo dllInfo;

	dllInfo.procInitSGZDll				= (PROCINITSGZDLL*)				GetProcAddress(hDll, "InitSGZDll");
	dllInfo.procCloseSGZDll				= (PROCCLOSESGZDLL*)			GetProcAddress(hDll, "CloseSGZDll");
	dllInfo.procGetPluginClassTypeCount	= (PROCGETPLUGINCLASSTYPECOUNT*)GetProcAddress(hDll, "GetPluginClassTypeCount");
	dllInfo.procGetPluginClassType		= (PROCGETPLUGINCLASSTYPE*)		GetProcAddress(hDll, "GetPluginClassType");
	dllInfo.procGetPluginClassCount		= (PROCGETPLUGINCLASSCOUNT*)	GetProcAddress(hDll, "GetPluginClassCount");
	dllInfo.procGetPluginClassName		= (PROCGETPLUGINCLASSNAME*)		GetProcAddress(hDll, "GetPluginClassName");
	dllInfo.procNewPlugin				= (PROCNEWPLUGIN*)				GetProcAddress(hDll, "NewPlugin");
	dllInfo.procDeletePlugin			= (PROCDELETEPLUGIN*)			GetProcAddress(hDll, "DeletePlugin");
	dllInfo.procNotifyCreateRC			= (PROCNOTIFYCREATERC*)			GetProcAddress(hDll, "NotifyCreateRC");

	if(	!dllInfo.procInitSGZDll              ||
		!dllInfo.procCloseSGZDll             ||
		!dllInfo.procGetPluginClassTypeCount ||
		!dllInfo.procGetPluginClassType      ||
		!dllInfo.procGetPluginClassCount     ||
		!dllInfo.procGetPluginClassName      ||
		!dllInfo.procNewPlugin               ||
		!dllInfo.procDeletePlugin            ||
		!dllInfo.procNotifyCreateRC)
	{
		FreeLibrary(hDll);
		return false;
	}

	m_mapDll2DllInfo.insert(MAPDLL2DLLINFO::value_type(strDll, dllInfo));

	// Initialize dll

	SCommonData commonData;
	commonData.pLogger				= &CLogger::Instance();
	commonData.pMemTracer			= &CMemTracer::Instance();
	commonData.pPluginManager		= &CPluginManager::Instance();
	commonData.pRenderContextData	= UtilGL::GetContextData();
	commonData.pWinStaticData		= CWindowGL::GetStaticData();
	commonData.pSoundSystem			= &CSoundSystem::Instance();

	dllInfo.procInitSGZDll(&commonData);

	// Iterate through all plugins

	LOG.Write("\n%s:", strDll.c_str());

	for(int nType = 0; nType < dllInfo.procGetPluginClassTypeCount(); nType++)
	{
		char* szType = new char[dllInfo.procGetPluginClassType(nType, NULL)];
		dllInfo.procGetPluginClassType(nType, szType);
		std::string strClassType = szType;
		delete[] szType;

		LOG.Write("\n    Plugin Type \"%s\"", strClassType.c_str());

		for(int nPlugin = 0; nPlugin < dllInfo.procGetPluginClassCount(strClassType.c_str()); nPlugin++)
		{
			char* szName = new char[dllInfo.procGetPluginClassName(strClassType.c_str(), nPlugin, NULL)];
			dllInfo.procGetPluginClassName(strClassType.c_str(), nPlugin, szName);
			std::string strClassName = szName;
			delete[] szName;

			LOG.Write("\n        Class \"%s\"", strClassName.c_str());

			// Add to map: dll -> pointer to dllinfo

			m_mapClass2PDllInfo.insert(MAPCLASS2PDLLINFO::value_type(strClassName, &m_mapDll2DllInfo[strDll]));

			// Add to map: type -> vector to plugins of that type

			MAPTYPE2PLUGINS::iterator itAux;
			itAux = m_mapType2Plugins.find(strClassType);

			if(itAux == m_mapType2Plugins.end())
			{
				VECPLUGINS vecPlugins;
				vecPlugins.push_back(strClassName);
				m_mapType2Plugins.insert(MAPTYPE2PLUGINS::value_type(strClassType, vecPlugins));
			}
			else
			{
				itAux->second.push_back(strClassName);
			}
		}
	}

	LOG.Write("\n");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : int GetNumPluginTypes()
//
//  - Purpose   : Returns the number of registered plugin class types.
//
// -----------------------------------------------------------------------------
int	CPluginManager::GetNumPluginTypes()
{
	return m_mapType2Plugins.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : std::string GetPluginClassType(int i)
//
//  - Purpose   : Returns the i-th classtype.
//
// -----------------------------------------------------------------------------
std::string CPluginManager::GetPluginClassType(int i)
{
	MAPTYPE2PLUGINS::iterator it;

	int n = 0;

	for(it = m_mapType2Plugins.begin(); it != m_mapType2Plugins.end(); ++it, n++)
	{
		if(n == i)
		{
			return it->first;
		}
	}

	return "";
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : int GetNumPlugins(const std::string& strClassType)
//
//  - Purpose   : Gets the number of available plugins of a given type.
//
// -----------------------------------------------------------------------------
int	CPluginManager::GetNumPlugins(const std::string& strClassType)
{
	MAPTYPE2PLUGINS::iterator it = m_mapType2Plugins.find(strClassType);

	if(it != m_mapType2Plugins.end())
	{
		return it->second.size();
	}

	return 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : std::string GetPluginClassName(const std::string& strClassType, int i)
//
//  - Purpose   : Returns the i-th plugin class name, of a given type.
//
// -----------------------------------------------------------------------------
std::string CPluginManager::GetPluginClassName(const std::string& strClassType, int i)
{
	MAPTYPE2PLUGINS::iterator it = m_mapType2Plugins.find(strClassType);

	if(it != m_mapType2Plugins.end())
	{
		if(i < it->second.size())
		{
			return it->second[i];
		}
	}

	return "";
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : CPlugin GetNewPlugin(const std::string& strClassType, const std::string& strClassName)
//
//  - Purpose   : Gets a new plugin. The type and class need to be specified.
//
// -----------------------------------------------------------------------------
CPlugin* CPluginManager::GetNewPlugin(const std::string& strClassType, const std::string& strClassName)
{
	CPlugin* pPlugin = NULL;

#ifdef CPLUGINMANAGER_VERBOSE
	LOG.Write("\nCPluginManager::GetNewPlugin(\"%s\", \"%s\"): ", strClassType.c_str(), strClassName.c_str());
#endif

	MAPCLASS2PDLLINFO::iterator it = m_mapClass2PDllInfo.find(strClassName);

	if(it != m_mapClass2PDllInfo.end())
	{
		pPlugin = it->second->procNewPlugin(strClassType.c_str(), strClassName.c_str());
	}

	if(pPlugin == NULL)
	{
		CLogger::ErrorWindow("Error instancing plugin.\nType=\"%s\" Class=\"%s\"", strClassType.c_str(), strClassName.c_str());
	}
	else
	{
#ifdef CPLUGINMANAGER_VERBOSE
		LOG.Write("OK");
#endif
	}

	return pPlugin;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : bool DeletePlugin(CPlugin* pPlugin)
//
//  - Purpose   : Deletes a given plugin (obtained with GetNewPlugin).
//
// -----------------------------------------------------------------------------
bool CPluginManager::DeletePlugin(CPlugin* pPlugin)
{
	if(pPlugin == NULL)
	{
		return false;
	}

#ifdef CPLUGINMANAGER_VERBOSE
	LOG.Write("\nCPluginManager::DeletePlugin(\"%s\", \"%s\"): ", pPlugin->GetClassType().c_str(), pPlugin->GetClassName().c_str());
#endif

	MAPCLASS2PDLLINFO::iterator it = m_mapClass2PDllInfo.find(pPlugin->GetClassName());

	if(it != m_mapClass2PDllInfo.end())
	{
		it->second->procDeletePlugin(pPlugin);
	}
	else
	{
		LOG.Write("\nWARNING - CPluginManager::DeletePlugin() : Couldn't find dll info for \"%s\"", pPlugin->GetClassName().c_str());
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginManager
//  - prototype : void NotifyCreateRC()
//
//  - Purpose   : Should be called when the opengl render context is created
//                to allow the dlls to load the opengl extensions.
//
// -----------------------------------------------------------------------------
void CPluginManager::NotifyCreateRC()
{
	MAPDLL2DLLINFO::iterator it;

	for(it = m_mapDll2DllInfo.begin(); it != m_mapDll2DllInfo.end(); ++it)
	{
		it->second.procNotifyCreateRC();
	}
}