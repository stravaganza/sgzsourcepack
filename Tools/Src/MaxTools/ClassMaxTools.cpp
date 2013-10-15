// ==[ File ]===================================================================================
//
//  -Name     : ClassMaxTools.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Constructor / destructor and some basic methods.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


extern CSGZMaxToolsActionCallback g_SGZActionCallback;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CStravaganzaMaxTools()
//
//  - Purpose   : CStravaganzaMaxTools's default constructor.
//
// -----------------------------------------------------------------------------
CStravaganzaMaxTools::CStravaganzaMaxTools()
{
	m_bWinEnabled   = true;

	m_pMaxInterface = NULL;	
	m_pMaxUtil      = NULL;
	m_hPanel        = NULL;

	m_fStartTime    = 0.0f;
	m_fEndTime      = 0.0f;

	m_nPreviewResX  = 800;
	m_nPreviewResY  = 600;

	// Build config file name

	char szCfgFile[MAX_PATH];

	// Get user temp path

	int nChars = GetTempPath(MAX_PATH, szCfgFile);

	// Add the config file name to the path

	if(szCfgFile[nChars - 1] != '\\')
	{
		strcat(szCfgFile, "\\");
	}

	strcat(szCfgFile, CFGFILE);
	m_strCfgFile = szCfgFile;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : void BeginEditParams(Interface *ip, IUtil *iu)
//
//  - Purpose   : (taken from the MAXSDK doc)
//                This method is called when the utility plug-in may be used in
//                the Utility branch of the command panel. The plug-in may add
//                rollup pages to the command panel in this method for example.
//
// -----------------------------------------------------------------------------
void CStravaganzaMaxTools::BeginEditParams(Interface *ip, IUtil *iu) 
{
	m_pMaxInterface = ip;
	m_pMaxUtil      = iu;

	// Add our panel:

	m_hPanel = ip->AddRollupPage(	hInstance,
									MAKEINTRESOURCE(IDD_TOOLDIALOG),
									CStravaganzaMaxTools::ToolDlgProc,
									NULL,
									0);

	ip->GetActionManager()->ActivateActionTable(&g_SGZActionCallback, ID_ACTIONTABLE);

	EnableWindow(m_hPanel, m_bWinEnabled ? TRUE : FALSE);
}
	
// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : void EndEditParams(Interface *ip, IUtil *iu) 
//
//  - Purpose   : (taken from the MAXSDK doc)
//                This method is called when the utility plug-in is done being
//                used in the Utility branch of the command panel (for example
//                if the user changes to the Create branch).
//
// -----------------------------------------------------------------------------
void CStravaganzaMaxTools::EndEditParams(Interface *ip, IUtil *iu) 
{
	//m_pMaxInterface = NULL;
	//m_pMaxUtil      = NULL;

	// Delete our panel

	SaveConfig();
	ip->DeleteRollupPage(m_hPanel);
	m_hPanel = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CVector3 Point3ToVector3(Point3& p)
//
//  - Purpose   : Transforms a Point3 into our CVector type.
//
// -----------------------------------------------------------------------------
CVector3 CStravaganzaMaxTools::Point3ToVector3(Point3& p)
{
	return CVector3(p.x, p.y, p.z);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CQuaternion QuatToQuaternion(Quat& q);
//
//  - Purpose   : Transforms a Quat from MAX into our own CQuaternion type.
//
// -----------------------------------------------------------------------------
CQuaternion CStravaganzaMaxTools::QuatToQuaternion(Quat& q)
{
	return CQuaternion(q.w, q.x, q.y, q.z);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CVector4 ColorToVector4(Color& col, float fAlpha = 1.0f)
//
//  - Purpose   : Transforms a Color from MAX into our own CVector4 type.
//
// -----------------------------------------------------------------------------
CVector4 CStravaganzaMaxTools::ColorToVector4(Color& col, float fAlpha)
{
	return CVector4(col.r, col.g, col.b, fAlpha);
}