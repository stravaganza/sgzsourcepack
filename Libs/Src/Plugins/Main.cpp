// ==[ File ]===================================================================================
//
//  -Name     : Main.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Dll Entry point
//
// =============================================================================================

/*

	04/12/2004  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"


// ==[ Constants ]==============================================================================


#define DLL_EXPORT __declspec(dllexport)


// ==[ Functions ]==============================================================================


#ifdef __cplusplus
	extern "C" {  // Otherwise our function names won't be exported properly
#endif


// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
//
//  - Purpose   : Entry point to the dll. Called when loaded / unloaded.
//
// ---------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		
	case DLL_PROCESS_ATTACH:

		break;

	case DLL_PROCESS_DETACH:

		break;

	default:

		break;

	}

	return TRUE;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT bool InitSGZDll(SCommonData* pCommonData);
//
//  - Purpose   : Should be called in first place.
//                Shares common data between the main program instance and
//				  this dll, and allows to initialize all necessary data.
//
// ---------------------------------------------------------------------------
DLL_EXPORT bool InitSGZDll(SCommonData* pCommonData)
{
	CMemTracer::ShareFrom		(*pCommonData->pMemTracer);
	CLogger::ShareFrom			(*pCommonData->pLogger);
	CPluginManager::ShareFrom	(*pCommonData->pPluginManager);
	UtilGL::ShareFrom			(pCommonData->pRenderContextData);
	CWindowGL::ShareFrom		(pCommonData->pWinStaticData);
	CSoundSystem::ShareFrom		(*pCommonData->pSoundSystem);

	return true;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT void CloseSGZDll(void)
//
//  - Purpose   : Removes all links between the exe and this dll, and
//                frees all allocated data.
//
// ---------------------------------------------------------------------------
DLL_EXPORT void CloseSGZDll(void)
{
	CPluginFactory::Instance().UnregisterAll();
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT int GetPluginClassTypeCount(void)
//
//  - Purpose   : Returns the number of available plugin class types.
//
// ---------------------------------------------------------------------------
DLL_EXPORT int GetPluginClassTypeCount(void)
{
	return CPluginFactory::Instance().GetClassTypeCount();
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT int GetPluginClassType(int i, char* pszDest)
//
//  - Purpose   : Writes in the second parameter the i-th plugin class type
//				  and the string length.
//				  Passing NULL can be used to determine the necessary length.
//
// ---------------------------------------------------------------------------
DLL_EXPORT int GetPluginClassType(int i, char* pszDest)
{
	std::string strClassType = CPluginFactory::Instance().GetClassType(i);

	if(pszDest)
	{
		strcpy(pszDest, strClassType.c_str());
	}

	return strClassType.length() + 1;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT int GetPluginClassCount(const char* szType)
//
//  - Purpose   : Returns the number of available plugins given a plugin type.
//
// ---------------------------------------------------------------------------
DLL_EXPORT int GetPluginClassCount(const char* szType)
{
	return CPluginFactory::Instance().GetClassCount(szType);
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT int GetPluginClassName(const char* szType, int i, char* pszDest)
//
//  - Purpose   : Given a plugin type, writes in the third parameter the i-th
//				  plugin class name. Returns the string length.
//				  Passing NULL can be used to determine the necessary length.
//
// ---------------------------------------------------------------------------
DLL_EXPORT int GetPluginClassName(const char* szType, int i, char* pszDest)
{
	std::string strClassName = CPluginFactory::Instance().GetClassName(szType, i);

	if(pszDest)
	{
		strcpy(pszDest, strClassName.c_str());
	}

	return strClassName.length() + 1;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT CPlugin* NewPlugin(const char* szType, const char* szName)
//
//  - Purpose   : Gets a new instance of a plugin, given his type and name.
//				  Returns NULL if strType is not a valid type of plugin or if
//				  strName is not a valid name of plugin.
//
// ---------------------------------------------------------------------------
DLL_EXPORT CPlugin* NewPlugin(const char* szType, const char* szName)
{
	return CPluginFactory::Instance().GetNew(szType, szName);
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT void DeletePlugin(CPlugin* pPlugin)
//
//  - Purpose   : Performs a delete over a given plugin.
//
// ---------------------------------------------------------------------------
DLL_EXPORT void DeletePlugin(CPlugin* pPlugin)
{
	if(pPlugin)
	{
		delete pPlugin;
	}
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT void NotifyCreateRC(void)
//
//  - Purpose   : Called once the opengl render context has been created.
//
// ---------------------------------------------------------------------------
DLL_EXPORT void NotifyCreateRC(void)
{
	UtilGL::Extensions::Init();
}


#ifdef __cplusplus
	}
#endif
