// ==[ File ]===================================================================================
//
//  -Name     : ClassDesc.h
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSGZMaxToolsClassDesc definition
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


#ifndef CLASSDESC_H
#define CLASSDESC_H


#include <iparamb2.h>
#include <actiontable.h>


#define ID_ACTIONTABLE     0x362440e9
#define ID_ACTIONCONTEXT   0x4dfd44e8


// ==[ Classes ]================================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSGZMaxToolsClassDesc
//
//  - Purpose   : Our class descriptor. Must be child of MAXSDK's ClassDesc2.
//                Returns information to MAX about our plugin.
//
// ---------------------------------------------------------------------------
class CSGZMaxToolsClassDesc : public ClassDesc2
{


public:


	void* Create(BOOL loading = FALSE) { return &g_SGZMaxTools; }

	int          IsPublic     () { return 1;                           }
	const TCHAR* ClassName    () { return _T("Stravaganza MAX Tools"); }
	const TCHAR* Category     () { return _T("Stravaganza");           }
	SClass_ID    SuperClassID () { return UTILITY_CLASS_ID;            }
	Class_ID     ClassID      () { return SGZMAXTOOLS_CLASS_ID;        }

	const TCHAR* InternalName () { return _T("Stravaganza MAX Tools"); }
	HINSTANCE    HInstance    () { return hInstance;                   }

	int			 NumActionTables()		{ return 1;                    }
	ActionTable* GetActionTable (int i) { return BuildActionTable();   }


private:


	ActionTable* BuildActionTable();

};


class CSGZMaxToolsActionCallback : public ActionCallback
{


public:


	BOOL ExecuteAction(int id);

};


#endif