// ==[ File ]===================================================================================
//
//  -Name     : GLWindow.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CWindowGL definition.
//
// =============================================================================================

/*

	11/12/2001  -  File created.


*/


#ifndef __GLWINDOW_H
#define __GLWINDOW_H


// ==[ Headers ]================================================================================


#include <string>

#include <windows.h>


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CWindowGL
//
//  - Purpose   : Provides high level manipulation of OpenGL-support windows.
//
// ---------------------------------------------------------------------------
class CWindowGL
{


public:


	CWindowGL();
	~CWindowGL();

	// Static methods:

	struct SWinStaticData
	{
		SWinStaticData();

		CWindowGL* _pActiveWindow;
		WNDCLASS   _wndClass;
		bool       _bWndClassRegistered;
		int        _nWindowCount;
		int        _nNextFontListBase;
	};

	static int				GetNumWindows   () { return s_pWinStaticData->_nWindowCount;   }
	static CWindowGL*		GetActiveWindow () { return s_pWinStaticData->_pActiveWindow;  }
	static WNDCLASS			GetWndClass     () { return s_pWinStaticData->_wndClass;       }
	static SWinStaticData*	GetStaticData	() { return s_pWinStaticData;                  }
	static bool				ShareFrom		(SWinStaticData* pStaticData);
	static bool				RegisterClass   ();
	static bool				UnregisterClass ();

	// Methods:

	HWND  Create             (const std::string& strWindowName, HWND hwndParent, int nWidth, int nHeight, bool bFullscreen, int nHZ);
	bool  Create             (HWND hWnd, bool bIsFullscreen, bool bCallAlsoOldProc);
	bool  Destroy            ();

	bool  IsValid            () const;
	bool  IsActiveWindow     () const { return this == s_pWinStaticData->_pActiveWindow; }
	std::string GetName      () const { return m_strWindowName;         }

	HWND  GetHWND            () const { return m_hWnd;        }
	HDC   GetHDC             () const { return m_hDC;         }
	bool  GetFullscreen      () const { return m_bFullscreen; }
	int   GetWidth           () const;
	int   GetHeight          () const;
	float GetAspect          () const;

	bool  SetAspect          (float fAspect);
	bool  SetViewport        (float fX, float fY, float fWidth, float fHeight, bool bScissor = false);
	bool  SetViewport        (int   nX, int   nY, int   nWidth, int   nHeight, bool bScissor = false);
	bool  SetCenteredViewport(float fWidth, float fHeight, bool bScissor = false);
	bool  SetAsActiveWindow  ();
	bool  SetFullscreen      (bool bFullscreen);
	bool  SetPos             (int  nPosX,  int nPosY);
	bool  SetSize            (int  nWidth, int nHeight);

	bool  TextOut            (float fViewpX, float fViewpY, char *szText, ...);


	static LRESULT WINAPI StaticWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);


protected:


	static SWinStaticData	s_winStaticData;
	static SWinStaticData*	s_pWinStaticData;

	//

	virtual LRESULT WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	HWND    m_hWnd;
	HDC     m_hDC;
	WNDPROC m_oldProcedure;
	bool    m_bCallAlsoOldProc;

	float m_fAspect;
	float m_fViewpX, m_fViewpY, m_fViewpWidth, m_fViewpHeight;
	bool  m_bScissor;

	std::string m_strWindowName;
	bool m_bFullscreen;
	int  m_nHZ;

	int  m_nFontListBase;

};


#endif
