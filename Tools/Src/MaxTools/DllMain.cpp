// ==[ File ]===================================================================================
//
//  -Name     : DllMain.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : DllMain() and dll exported functions.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


// ==[ Variables ]==============================================================================


// Global instances

HINSTANCE                  hInstance;
CStravaganzaMaxTools       g_SGZMaxTools;
CSGZMaxToolsClassDesc      g_MaxToolsClassDesc;


// ==[ Functions ]==============================================================================


TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if(hInstance)
	{
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	}

	return NULL;
}


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
	static bool bControlsInitialized = false;

	hInstance = hinstDLL;

	setlocale(LC_ALL, "English");

	if(!bControlsInitialized)
	{
		bControlsInitialized = true;
		InitCommonControls();
		InitCustomControls(hInstance); // Max custom controls

		LOG.Start("log.txt");
	}

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
//  - Prototype : DLL_EXPORT const TCHAR *LibDescription()
//
//  - Purpose   : Exported function to report Max our plugin description.
//
// ---------------------------------------------------------------------------
DLL_EXPORT const TCHAR *LibDescription()
{
	return _T("Stravaganza 3DStudioMAX Tools");
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT int LibNumberClasses()
//
//  - Purpose   : Exported function to report Max the number of classes we have.
//
// ---------------------------------------------------------------------------
DLL_EXPORT int LibNumberClasses()
{
	return 1;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT CSGZMaxToolsClassDesc* LibClassDesc(int nIndex)
//
//  - Purpose   : Exported function to send Max the descriptor of our nth class.
//
// ---------------------------------------------------------------------------
DLL_EXPORT CSGZMaxToolsClassDesc* LibClassDesc(int nIndex)
{
	switch(nIndex)
	{
	case 0:	 return &g_MaxToolsClassDesc;
	default: break;
	}
	
	return NULL;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : DLL_EXPORT ULONG LibVersion()
//
//  - Purpose   : Exported function to report Max the version of the SDK we
//                used. This way each version of 3DStudioMax can know if
//                the plugin is compatible or not.
//
// ---------------------------------------------------------------------------
DLL_EXPORT ULONG LibVersion()
{
	return VERSION_3DSMAX;
}


#ifdef __cplusplus
}
#endif
