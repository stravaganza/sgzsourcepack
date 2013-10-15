// ==[ File ]===================================================================================
//
//  -Name     : WinPreview.h
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Preview window class definition.
//
// =============================================================================================

/*

	15/03/2003  -  File created.


*/


#ifndef __WINPREVIEW_H
#define __WINPREVIEW_H


// ==[ Classes ]================================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CWinPreview
//
//  - Purpose   : The preview window class, derived from CWindowGL.
//
// ---------------------------------------------------------------------------
class CWinPreview : public CWindowGL
{


public:


	CWinPreview()  { }
	~CWinPreview() { }

	// From CWindowGL

	LRESULT WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};


#endif