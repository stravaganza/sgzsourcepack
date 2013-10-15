// ==[ File ]===================================================================================
//
//  -Name     : Keyboard.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Keyboard shortcuts.
//
// =============================================================================================

/*

	12/02/2004  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


#define NUM_ACTIONS 1


static ActionDescription pActions[NUM_ACTIONS] =
{
	ID_SHORTCUT_PREVIEW,
	IDS_ACTION_PREVIEW,
	IDS_ACTION_PREVIEW,
	IDS_SGZMAXTOOLS
};

CSGZMaxToolsActionCallback g_SGZActionCallback;


ActionTable* CSGZMaxToolsClassDesc::BuildActionTable()
{
	TSTR name = "SGZ MaxTools Shortcuts";

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_SHORTCUTS));

	ActionTable* pTab = new ActionTable(ID_ACTIONTABLE, ID_ACTIONCONTEXT, name,
										hAccel, NUM_ACTIONS, pActions, hInstance); 

	pTab->SetCallback(&g_SGZActionCallback);
	GetCOREInterface()->GetActionManager()->RegisterActionContext(ID_ACTIONCONTEXT, name.data());

	return pTab;
}


BOOL CSGZMaxToolsActionCallback::ExecuteAction(int id)
{
	switch(id)
	{

	case ID_SHORTCUT_PREVIEW:

		if(!g_SGZMaxTools.PrepareExportPreview(true))
		{
			return TRUE;
		}

		g_SGZMaxTools.Preview();
		g_SGZMaxTools.FinishExportPreview();

		break;

	default:
		
		return FALSE;
	}

	return TRUE;
}