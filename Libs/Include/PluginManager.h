// ==[ File ]===================================================================================
//
//  -Name     : PluginManager.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPluginManager definition.
//
// =============================================================================================

/*

	05/12/2004  -  File created.


*/


#ifndef __PLUGINMANAGER_H
#define __PLUGINMANAGER_H


class CPluginManager;
class CSoundSystem;


struct SCommonData
{
	CLogger*					pLogger;
	CMemTracer*					pMemTracer;
	CPluginManager*				pPluginManager;
	UtilGL::SContextData*		pRenderContextData;
	CWindowGL::SWinStaticData*	pWinStaticData;
	CSoundSystem*				pSoundSystem;
};


typedef bool		PROCINITSGZDLL				(SCommonData*);
typedef void        PROCCLOSESGZDLL				(void);
typedef int			PROCGETPLUGINCLASSTYPECOUNT	(void);
typedef int			PROCGETPLUGINCLASSTYPE		(int,  char*);
typedef int			PROCGETPLUGINCLASSCOUNT		(const char*);
typedef int			PROCGETPLUGINCLASSNAME		(const char*, int, char*);
typedef CPlugin*	PROCNEWPLUGIN				(const char*, const char*);
typedef void		PROCDELETEPLUGIN			(CPlugin*);
typedef void		PROCNOTIFYCREATERC			(void);


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CPluginManager
//
//  - Purpose   : Manager to access plugin functionality.
//
// ---------------------------------------------------------------------------
class CPluginManager
{

private:


	CPluginManager();
	CPluginManager(const CPluginManager& pluginManager)                   { }
	CPluginManager& operator = (const CPluginManager& pluginManager)      { }

	bool ProcessDll(const std::string& strDll);

	struct SDllInfo
	{
		HMODULE hDll;

		PROCINITSGZDLL*					procInitSGZDll;
		PROCCLOSESGZDLL*				procCloseSGZDll;
		PROCGETPLUGINCLASSTYPECOUNT*	procGetPluginClassTypeCount;
		PROCGETPLUGINCLASSTYPE*			procGetPluginClassType;
		PROCGETPLUGINCLASSCOUNT*		procGetPluginClassCount;
		PROCGETPLUGINCLASSNAME*			procGetPluginClassName;
		PROCNEWPLUGIN*					procNewPlugin;
		PROCDELETEPLUGIN*				procDeletePlugin;
		PROCNOTIFYCREATERC*				procNotifyCreateRC;
	};

	typedef std::map<std::string, SDllInfo>   MAPDLL2DLLINFO;
	typedef std::map<std::string, SDllInfo*>  MAPCLASS2PDLLINFO;
	typedef std::vector<std::string>          VECPLUGINS;
	typedef std::map<std::string, VECPLUGINS> MAPTYPE2PLUGINS;

	MAPDLL2DLLINFO    m_mapDll2DllInfo;
	MAPCLASS2PDLLINFO m_mapClass2PDllInfo;
	MAPTYPE2PLUGINS   m_mapType2Plugins;

	static CPluginManager* s_pSharedInstance;


public:


	~CPluginManager();

	// Access to global interface

	static CPluginManager& Instance	();
	static void ShareFrom			(CPluginManager& pluginManager);

	// Methods

	bool		Init				(const std::string& strPluginPath);
	bool		Close				();

	int			GetNumPluginTypes	();
	std::string	GetPluginClassType	(int i);

	int			GetNumPlugins		(const std::string& strClassType);
	std::string	GetPluginClassName	(const std::string& strClassType, int i);

	CPlugin*	GetNewPlugin		(const std::string& strClassType, const std::string& strClassName);
	bool		DeletePlugin		(CPlugin* pPlugin);

	void		NotifyCreateRC		();

};


#endif