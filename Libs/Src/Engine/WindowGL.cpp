// ==[ File ]===================================================================================
//
//  -Name     : WindowGL.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CWindowGL ìmplementation.
//
// =============================================================================================

/*

	11/12/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


CWindowGL::SWinStaticData  CWindowGL::s_winStaticData;
CWindowGL::SWinStaticData* CWindowGL::s_pWinStaticData = &CWindowGL::s_winStaticData;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : CWindowGL()
//
//  - Purpose   : CWindowGL's default constructor.
//
// -----------------------------------------------------------------------------
CWindowGL::CWindowGL()
{
	m_hWnd = NULL;
	m_hDC  = NULL;

	m_fAspect      = 1.333f;
	m_fViewpX      = 0.0f;
	m_fViewpY      = 0.0f;
	m_fViewpWidth  = 1.0f;
	m_fViewpHeight = 1.0f;

	m_oldProcedure     = NULL;
	m_bCallAlsoOldProc = NULL;

	m_bScissor = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : ~CWindowGL()
//
//  - Purpose   : CWindowGL's destructor.
//
//  - Notes     : Use Destroy() in the WndProc or message loop.
//                Put the delete after the message loop or just before leaving
//                it.
//
// -----------------------------------------------------------------------------
CWindowGL::~CWindowGL()
{
	// Window should have been destroyed

	assert(m_hWnd == NULL);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : static bool ShareFrom(SWinStaticData* pStaticData)
//
//  - Purpose   : Allows to share the static windows data from a source.
//                This is used when we want the data to be shared between
//                exe<->dlls.
//
// -----------------------------------------------------------------------------
bool CWindowGL::ShareFrom(SWinStaticData* pStaticData)
{
	s_pWinStaticData = pStaticData;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : static bool RegisterClass()
//
//  - Purpose   : Registers the window class.
//                Called by UtilGL::RenderContext::Create().
//
// -----------------------------------------------------------------------------
bool CWindowGL::RegisterClass()
{
	if(!s_pWinStaticData->_bWndClassRegistered)
	{
		if(!::RegisterClass(&s_pWinStaticData->_wndClass))
		{
			LOG.Write("\nERROR - CWindowGL::RegisterClass(). Error on ::RegisterClass().");
			return false;
		}

		s_pWinStaticData->_bWndClassRegistered = true;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : static bool UnregisterClass()
//
//  - Purpose   : Unregisters the window class.
//                Called by UtilGL::RenderContext::Delete().
//
// -----------------------------------------------------------------------------
bool CWindowGL::UnregisterClass()
{
	if(!s_pWinStaticData->_bWndClassRegistered)
	{
		LOG.Write("\nWARNING - CWindowGL::UnregisterClass(). Class isn't registered.");
		return true;
	}

	if(!::UnregisterClass(s_pWinStaticData->_wndClass.lpszClassName, GetModuleHandle(NULL)))
	{
		LOG.Write("\nERROR - CWindowGL::UnregisterClass(). Error on ::UnregisterClass().");
		return false;
	}

	s_pWinStaticData->_bWndClassRegistered = false;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : HWND Create(const std::string& strWindowName, HWND hwndParent,
//                            int nWidth, int nHeight, bool bFullscreen, int nHZ)
//
//  - Purpose   : Creates an opengl render window with the specified properties.
//                nHZ is only used in fullscreen modes and, if zero, current
//                refresh rate will be used.
//                If nWidth and/or nHeight are passed as zero, window's
//                width/height will be get from current display size.
//
// -----------------------------------------------------------------------------
HWND CWindowGL::Create(	const std::string& strWindowName, HWND hwndParent,
						int nWidth, int nHeight, bool bFullscreen, int nHZ)
{
	if(UtilGL::RenderContext::Get() == NULL)
	{
		LOG.Write("\nERROR - CWindowGL::Create() called when no render context available.");
		return NULL;
	}

	assert(m_hWnd == NULL);

	m_fViewpX      = 0.0f;
	m_fViewpY      = 0.0f;
	m_fViewpWidth  = 1.0f;
	m_fViewpHeight = 1.0f;

	HWND hWnd;
	int	dwExStyle = 0, dwStyle = 0;

	if(nWidth == 0)
	{
		nWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	}

	if(nHeight == 0)
	{
		nHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	}

	LOG.Write("\n\nCreating new CWindowGL window \"%s\".", strWindowName.data());
	LOG.Write("\n  Properties:");
	LOG.Write("\n    Width        : %u.", nWidth);
	LOG.Write("\n    Height       : %u.", nHeight);
	LOG.Write("\n    Fullscreen?  : %s.", bFullscreen ? "Yes" : "No");
	LOG.Write("\n    Refresh rate : %u.", nHZ);

	// If fullscreen requested, set new display settings

	if(bFullscreen)
	{
		LOG.Write("\n  Switching to fullscreen.");

		DEVMODE dmScreenSettings;

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = nWidth;
		dmScreenSettings.dmPelsHeight = nHeight;
		dmScreenSettings.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;

		if(nHZ != 0)
		{
			dmScreenSettings.dmDisplayFrequency = nHZ;
			dmScreenSettings.dmFields |= DM_DISPLAYFREQUENCY;
		}

		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			CLogger::ErrorWindow("Error setting fullscreen mode.");
			return NULL;
		}

		LOG.Write("\n  Hiding cursor.");
		ShowCursor(FALSE);
	}

	// Set up some extra parameters depending on fullscreen request

	if(bFullscreen)
	{
		if(hwndParent)
		{
			dwStyle = WS_CHILD;
		}
		else
		{
			dwStyle   = WS_POPUP;
			dwExStyle = WS_EX_APPWINDOW;
		}
	}
	else
	{
		if(hwndParent)
		{
			dwStyle = WS_CHILD;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle   = WS_OVERLAPPEDWINDOW;
		}
	}

	// Create window

	CREATESTRUCT cs;

	memset(&cs, 0, sizeof(CREATESTRUCT));

	cs.dwExStyle  = dwExStyle;
	cs.lpszClass  = s_pWinStaticData->_wndClass.lpszClassName;
	cs.lpszName   = strWindowName.data();
	cs.style      = dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	cs.x          = 0;
	cs.y          = 0;
	cs.cx         = nWidth;
	cs.cy         = nHeight;
	cs.hwndParent = hwndParent;
	cs.hMenu      = NULL;
	cs.hInstance  = s_pWinStaticData->_wndClass.hInstance;

	LOG.Write("\n  Creating.");

	if(!(hWnd = CreateWindowEx(	cs.dwExStyle,
								cs.lpszClass,
								cs.lpszName,
								cs.style,
								cs.x,  cs.y,
								cs.cx, cs.cy,
								cs.hwndParent,
								cs.hMenu,
								cs.hInstance,
								NULL)))
	{
		LOG.Write("\n  ERROR - CreateWindowEx() failed.");

		if(bFullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
			ShowCursor(TRUE);
		}

		return NULL;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	LOG.IncIndentation(2);

	// Setting pixel format

	LOG.Write("\n  Setting pixel format.");

	HDC hDC = GetDC(hWnd);

	if(!UtilGL::RenderContext::SetPixelFormat(hDC))
	{
		LOG.Write(" ERROR!");

		if(bFullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
			ShowCursor(TRUE);
		}

		LOG.DecIndentation(2);

		ReleaseDC(hWnd, hDC);
		DestroyWindow(hWnd);
		return NULL;
	}

	LOG.DecIndentation(2);
	LOG.Write("\n  Setting private user data.");

	// Here we are gonna bind this object to the window's private user data.
	// This way, given a window handle (HWND), we can access the object (CWindowGL) it belongs to.
	// Note that messages generated by CreateWindow() won't be processed, so we will 'simulate'
	// a WM_CREATE instead. I'm looking for alternative methods to solve this, get in contact with
	// me if there's something less dirty.

	m_oldProcedure     = NULL;  // Only used in the other Create() method.
	m_bCallAlsoOldProc = false; // "

	SetLastError(0);

	if(SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(this)) == 0)
	{
		if(GetLastError())
		{
			LOG.Write("\n  ERROR - SetWindowLong() failed.");

			if(bFullscreen)
			{
				ChangeDisplaySettings(NULL, 0);
				ShowCursor(TRUE);
			}

			DestroyWindow(hWnd);
			return NULL;
		}
	}

	m_hWnd          = hWnd;
	m_hDC           = hDC;
	m_bFullscreen   = bFullscreen;
	m_strWindowName = strWindowName;
	m_nHZ           = nHZ;
	s_pWinStaticData->_nWindowCount++;

	// Create font

	HDC curDC = wglGetCurrentDC();
	wglMakeCurrent(m_hDC, UtilGL::RenderContext::Get());

	m_nFontListBase      = s_pWinStaticData->_nNextFontListBase;
	s_pWinStaticData->_nNextFontListBase += 255;

	SelectObject(m_hDC, GetStockObject(SYSTEM_FONT));

	LOG.Write("\n  Creating fonts.");

	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));

	if(wglUseFontBitmaps(m_hDC, 0, 255, m_nFontListBase) == TRUE)
	{
		LOG.Write("\n OK.");
	}
	else
	{
		LOG.Write("\n Error!");
	}

	wglMakeCurrent(curDC, UtilGL::RenderContext::Get());

	// Simulate WM_CREATE

	this->WndProc(m_hWnd, WM_CREATE, 0, (LPARAM)(&cs));

	LOG.Write("\nWindow succesfully created.\n");

	return hWnd;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool Create(HWND hWnd, bool bIsFullscreen, bool bCallAlsoOldProc)
//
//  - Purpose   : Setups an already existing window to support opengl render.
//                If the window is in fullscreen mode, bIsFullscreen must be
//                set to true, otherwise false.
//
// -----------------------------------------------------------------------------
bool CWindowGL::Create(HWND hWnd, bool bIsFullscreen, bool bCallAlsoOldProc)
{
	if(UtilGL::RenderContext::Get() == NULL)
	{
		LOG.Write("\nERROR - CWindowGL::Create() called when no render context available.");
		return false;
	}

	HDC hDC = GetDC(hWnd);

	if(!UtilGL::RenderContext::SetPixelFormat(hDC))
	{
		ReleaseDC(hWnd, hDC);
		LOG.Write("\nERROR - CWindowGL::Create(). Error setting pixel format.");
		return false;
	}

	// Like in the previous method, we bind this object to the window.
	// As it is an existing window, it has its own window procedure. We'll save
	// it to call it every time the new procedure is invoked.
	// This way the user can process all the messages without the window losing
	// its message processing.

	LOG.Write("\n\nCreating new CWindowGL from an already existing window.");

	m_oldProcedure     = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	m_bCallAlsoOldProc = bCallAlsoOldProc;

	LOG.Write("\n  Setting private user data.");

	SetLastError(0);

	if(SetWindowLong(hWnd, GWL_WNDPROC, reinterpret_cast<LONG>(this->StaticWndProc)) == 0)
	{
		if(GetLastError())
		{
			LOG.Write("\n  ERROR - SetWindowLong() setting GWL_WNDPROC failed.");
			return false;
		}
	}

	SetLastError(0);

	if(SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(this)) == 0)
	{
		if(GetLastError())
		{
			LOG.Write("\n  ERROR - SetWindowLong() setting GWL_USERDATA failed.");
			return false;
		}
	}

	char* szWindowName = new char[MAX_PATH];
	assert(szWindowName);

	GetWindowText(hWnd, szWindowName, MAX_PATH);

	m_fViewpX      = 0.0f;
	m_fViewpY      = 0.0f;
	m_fViewpWidth  = 1.0f;
	m_fViewpHeight = 1.0f;

	m_hWnd          = hWnd;
	m_hDC           = hDC;
	m_bFullscreen   = bIsFullscreen;
	m_strWindowName = szWindowName;
	s_pWinStaticData->_nWindowCount++;

	SAFE_DELETE_A(szWindowName);

	LOG.Write("\nWindow succesfully created.");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool Destroy()
//
//  - Purpose   : Destroys the window. Should be called on WM_CLOSE or in
//                any event that means destroying the window.
//                Internally, it calls DestroyWindow() generating thus a
//                WM_DESTROY message. In the wndproc, use PostQuitMessage()
//                to handle it.
//
// -----------------------------------------------------------------------------
bool CWindowGL::Destroy()
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::Destroy() called when object not valid.");
		return false;
	}

	int nErrorCount = 0;

	// Get window title.

	LOG.Write("\n\nDestroying window %s. Handle %u.", m_strWindowName.data(), m_hWnd);

	// Exit fullscreen if window is in fullscreen mode.

	if(m_bFullscreen)
	{
		LOG.Write("\n  Exiting fullscreen mode.");

		if(ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
		{
			LOG.Write(" Error!");
			nErrorCount++;
		}

		ShowCursor(TRUE);

		m_bFullscreen = false;
	}

	// Delete font display lists

	glDeleteLists(m_nFontListBase, 255);

	// If it's the current active OpenGL window, unbind render context and
	// make the current active render context NULL.

	if(IsActiveWindow())
	{
		wglMakeCurrent(NULL, NULL);
	}

	// Release device context.

	LOG.Write("\n  Releasing device context.");

	if(ReleaseDC(m_hWnd, m_hDC) == 0)
	{
		LOG.Write(" Error!");
		nErrorCount++;
	}

	LOG.Write("\n  Calling DestroyWindow().");

	if(!DestroyWindow(m_hWnd))
	{
		LOG.Write(" Error!");
		nErrorCount++;
	}

	m_hDC           = NULL;
	m_hWnd          = NULL;
	m_oldProcedure = NULL;

	s_pWinStaticData->_nWindowCount--;

	LOG.Write("\nWindow destroyed.");

	return nErrorCount == 0 ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool IsValid() const
//
//  - Purpose   : Returns true if the window is valid and can be used.
//
// -----------------------------------------------------------------------------
bool CWindowGL::IsValid() const
{
	if(UtilGL::RenderContext::Get() && m_hWnd && m_hDC)
	{
		return true;
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : int GetWidth() const
//
//  - Purpose   : Gets the window's current width in pixels.
//
// -----------------------------------------------------------------------------
int CWindowGL::GetWidth() const
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::GetWidth() called when object not valid.");
		return 0;
	}

	RECT winRect;

	if(GetWindowRect(m_hWnd, &winRect) == 0)
	{
		return 0;
	}

	return winRect.right - winRect.left;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool GetHeight() const
//
//  - Purpose   : Gets the window's current height in pixels.
//
// -----------------------------------------------------------------------------
int CWindowGL::GetHeight() const
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::GetHeight() called when object not valid.");
		return 0;
	}

	RECT winRect;

	if(GetWindowRect(m_hWnd, &winRect) == 0)
	{
		return 0;
	}

	return winRect.bottom - winRect.top;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : float GetAspect() const
//
//  - Purpose   : Gets the window aspect ratio.
//
// -----------------------------------------------------------------------------
float CWindowGL::GetAspect() const
{
	if(m_fAspect < 0.0f)
	{
		return (float)GetWidth() / (float)GetHeight();
	}

	return m_fAspect;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool SetAspect(float fAspect)
//
//  - Purpose   : Sets the window aspect ratio.
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetAspect(float fAspect)
{
	m_fAspect = fAspect;
        if(m_fAspect < 0.01f) m_fAspect = 0.01f;
	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : void SetViewport(float fX, float fY, float fWidth, float fHeight, bool bScissor = true)
//
//  - Purpose   : Specifies a viewport in normalized window coordinates.
//                0 <= fWidth <= 1, 0 <= fHeight <= 1
//                If bScissor = true, scissoring is activated (which affects
//                for example, clearing the framebuffer)
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetViewport(float fX, float fY, float fWidth, float fHeight, bool bScissor)
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::SetViewport() called when object not valid.");
		return 0;
	}

	float fViewpX = GetWidth () * fX;
	float fViewpY = GetHeight() * fY;

	int   nViewpIntX      = MYROUND(fViewpX);
	int   nViewpIntY      = MYROUND(GetHeight() - (fViewpY + (fHeight * GetHeight())));
	int   nViewpIntWidth  = MYROUND(fWidth * GetWidth());
	int   nViewpIntHeight = MYROUND(fHeight * GetHeight());

	glViewport(nViewpIntX, nViewpIntY, nViewpIntWidth, nViewpIntHeight);

	if(bScissor)
	{
		m_bScissor = true;
		glScissor(nViewpIntX, nViewpIntY, nViewpIntWidth, nViewpIntHeight);
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		m_bScissor = false;
		glDisable(GL_SCISSOR_TEST);
	}

	m_fViewpX      = fX;
	m_fViewpY      = fY;
	m_fViewpWidth  = fWidth;
	m_fViewpHeight = fHeight;

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : void SetViewport(int nX, int nY, int nWidth, int nHeight, bool bScissor = false);
//
//  - Purpose   : Specifies a viewport in window pixel coordinates.
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetViewport(int nX, int nY, int nWidth, int nHeight, bool bScissor)
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::SetViewport() called when object not valid.");
		return 0;
	}

	glViewport(nX, GetHeight() - (nY + nHeight), nWidth, nHeight);

	if(bScissor)
	{
		m_bScissor = true;
		glScissor(nX, nY, nWidth, nHeight);
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		m_bScissor = false;
		glDisable(GL_SCISSOR_TEST);
	}

	int nWinWidth  = this->GetWidth();
	int nWinHeight = this->GetHeight();

	m_fViewpX      = (float)nX      / (float)nWinWidth;
	m_fViewpY      = (float)nY      / (float)nWinHeight;
	m_fViewpWidth  = (float)nWidth  / (float)nWinWidth;
	m_fViewpHeight = (float)nHeight / (float)nWinHeight;

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : void SetCenteredViewport(float fWidth, float fHeight, bool bScissor = true
//
//  - Purpose   : Same, but centered on 0.5, 0.5
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetCenteredViewport(float fWidth, float fHeight, bool bScissor)
{
	return this->SetViewport(	0.5f - fWidth  * 0.5f,
								0.5f - fHeight * 0.5f,
								fWidth, fHeight, bScissor);
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool SetAsActiveWindow()
//
//  - Purpose   : Sets the window to be the current target for all OpenGL calls.
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetAsActiveWindow()
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::SetAsActiveWindow() called when object not valid.");
		return false;
	}

	if(this == GetActiveWindow())
	{
		return true;
	}

	if(wglMakeCurrent(m_hDC, UtilGL::RenderContext::Get()) == TRUE)
	{
		s_pWinStaticData->_pActiveWindow = this;
		SetViewport(m_fViewpX, m_fViewpY, m_fViewpWidth, m_fViewpHeight, m_bScissor);
		return true;
	}

	LOG.Write("\nERROR - CWindowGL::SetAsActiveWindow(). Error on wglMakeCurrent().");

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool SetFullscreen(bool bFullscreen)
//
//  - Purpose   : Establishes the window to be either in fullscreen (true)
//                or window mode (false).
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetFullscreen(bool bFullscreen)
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::SetFullscreen() called when object not valid.");
		return false;
	}

	DEVMODE dmScreenSettings;

	if(bFullscreen)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = GetWidth();
		dmScreenSettings.dmPelsHeight = GetHeight();
		dmScreenSettings.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;

		if(m_nHZ != 0)
		{
			dmScreenSettings.dmDisplayFrequency = m_nHZ;
			dmScreenSettings.dmFields |= DM_DISPLAYFREQUENCY;
		}

		SetPos(0, 0);

		SetWindowLong(m_hWnd, GWL_STYLE,   WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			CLogger::ErrorWindow("Error setting fullscreen mode.");
			UpdateWindow(m_hWnd);
			return false;
		}

		m_bFullscreen = true;

		UpdateWindow(m_hWnd);
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(m_hWnd, GWL_STYLE,   WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW     | WS_EX_WINDOWEDGE);

		if(ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
		{
			CLogger::ErrorWindow("Error returning to windowed mode.");
			UpdateWindow(m_hWnd);
			return false;
		}

		m_bFullscreen = false;

		UpdateWindow(m_hWnd);
		ShowCursor(TRUE);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool SetPos(int nPosX,  int nPosY)
//
//  - Purpose   : Sets the window position in screen pixel coordinates.
//                nPosX and nPosY are the upper left window corner's new coordinates.
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetPos(int nPosX, int nPosY)
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::SetPos() called when object not valid.");
		return false;
	}

	return SetWindowPos(m_hWnd, 0, nPosX, nPosY, 0, 0, SWP_NOZORDER | SWP_NOSIZE) ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : bool SetSize(int nWidth, int nHeight)
//
//  - Purpose   : Sets the window size in pixels.
//
// -----------------------------------------------------------------------------
bool CWindowGL::SetSize(int nWidth, int nHeight)
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::SetSize() called when object not valid.");
		return false;
	}

	return SetWindowPos(m_hWnd, 0, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE) ? true : false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool TextOut(float fViewpX, float fViewpY, char *szText, ...)
//
//  - Purpose   : Outputs text to this OpenGL window.
//
// -----------------------------------------------------------------------------
bool CWindowGL::TextOut(float fViewpX, float fViewpY, char *szText, ...)
{
	if(IsValid() == false)
	{
		LOG.Write("\nERROR - CWindowGL::TextOut() called when object not valid.");
		return false;
	}

	CWindowGL *pActiveWin = s_pWinStaticData->_pActiveWindow;

	static char szBuffer[2000];
	va_list arguments;

	// Format string.

	va_start(arguments, szText);
	vsprintf(szBuffer, szText, arguments);
	va_end(arguments);

	// Draw.

	SetAsActiveWindow();

	int      nListBase;
	CVector3 v3RasterPos(fViewpX, fViewpY, 0.1f);

	UtilGL::Transforming::NormViewportToLocal(&v3RasterPos);
	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);

	glGetIntegerv(GL_LIST_BASE, &nListBase);
	glListBase(m_nFontListBase);
	glRasterPos3f(v3RasterPos.X(), v3RasterPos.Y(), v3RasterPos.Z());
	glCallLists(strlen(szBuffer), GL_UNSIGNED_BYTE, szBuffer); 
	glListBase(nListBase);

	s_pWinStaticData->_pActiveWindow = pActiveWin;

	return true;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : static LRESULT WINAPI StaticWndProc(	HWND hWnd, UINT uMessage,
//														WPARAM wParam, LPARAM lParam)
//
//  - Purpose   : This is the WndProc for all windows. It's defined as static
//
//  - Note      : pWindowGL is zero until CreateWindow() has returned, so
//                messages sent before (f.e WM_CREATE) are not processed by
//                CWindowGL::WndProc().
//
// -----------------------------------------------------------------------------
LRESULT WINAPI CWindowGL::StaticWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CWindowGL *pWindowGL = reinterpret_cast<CWindowGL*>(GetWindowLong(hWnd, GWL_USERDATA));

	if(pWindowGL)
	{
		if(pWindowGL->m_oldProcedure && pWindowGL->m_bCallAlsoOldProc)
		{
			// Call to old procedure too
			pWindowGL->m_oldProcedure(hWnd, uMessage, wParam, lParam);
		}

		return pWindowGL->WndProc(hWnd, uMessage, wParam, lParam);
	}
	else if(uMessage == WM_CREATE)
	{
		return 0;
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL::SWinStaticData
//  - prototype : SWinStaticData()
//
//  - Purpose   : SWinStaticData's default constructor.
//
// -----------------------------------------------------------------------------
CWindowGL::SWinStaticData::SWinStaticData()
{
	_pActiveWindow          = NULL;
	_bWndClassRegistered    = false;
	_nWindowCount           = 0;
	_nNextFontListBase      = 1000;

    _wndClass.style         = CS_HREDRAW | CS_VREDRAW;
    _wndClass.lpfnWndProc   = CWindowGL::StaticWndProc;
    _wndClass.cbClsExtra    = 0;
    _wndClass.cbWndExtra    = 0;
    _wndClass.hInstance     = GetModuleHandle(NULL);
    _wndClass.hIcon         = NULL;
    _wndClass.hCursor       = NULL;
    _wndClass.hbrBackground = NULL;
    _wndClass.lpszMenuName  = NULL;
    _wndClass.lpszClassName = "Stravaganza";
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWindowGL
//  - prototype : LRESULT WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
//
//  - Purpose   : Procedure called when a window event occurs.
//                This method can be reimplemented in child classes since it's
//                declared as virtual.
//                This is thus the default implementation.
//
// -----------------------------------------------------------------------------
LRESULT CWindowGL::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage)
	{

	case WM_CREATE:

		return 0;

	case WM_CLOSE:

		Destroy();
		return TRUE;

	case WM_DESTROY:

		PostQuitMessage(0);
		return TRUE;

	case WM_SIZE:

		if(IsActiveWindow())
		{
			this->SetCenteredViewport(1.0f, 1.0f);
			UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
			gluPerspective(60.0f, this->GetAspect(), 1.0f, 3000.0f);
		}

		return TRUE;

	default:

		break;

	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}
