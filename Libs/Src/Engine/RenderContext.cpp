// OpenGL render context managing

// ==[ File ]===================================================================================
//
//  -Name     : RenderContext.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : OpenGL Render context managing.
//
// =============================================================================================

/*

	25/01/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

extern SContextData* _pContextData;

namespace RenderContext
{


// ==[ Functions  ]=============================================================================


// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool UseDestinationAlpha(int nBits)
//
//  - Purpose   : Some effects may need destination alpha support.
//				  Call this before Create() if you want destination alpha support.
//
// -----------------------------------------------------------------------------
bool UseDestinationAlpha(int nBits)
{
	_pContextData->_nDestAlpha = nBits;

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool Create(int nColorBits, int nDepthBits, int nStencilBits);
//
//  - Purpose   : Function to set the pixel format that will be used to set up
//                the windows created afterwards.
//                All windows share the same render context, so the same pixel
//                format must be used for all of them.
//                Due to this, UtilGL::RenderContext::Create()() can only be used
//                when no existing CWindowGL windows are open. Call method
//                CWindowGL::GetNumWindows() to check for that.
//
// -----------------------------------------------------------------------------
bool Create(int nColorBits, int nDepthBits, int nStencilBits)
{
	if(CWindowGL::GetNumWindows() > 0)
	{
		LOG.Write("\nERROR - UtilGL::RenderContext::Create()() called with %u open windows", CWindowGL::GetNumWindows());
		return false;
	}

	if(_pContextData->_hRC)
	{
		Delete();
	}

	LOG.Write("Creating OpenGL render context.");
	LOG.Write("\n  Properties:");
	LOG.Write("\n    Color depth       : %u bits",       nColorBits);
	LOG.Write("\n    Z-Buffer depth    : %u bits",       nDepthBits);
	LOG.Write("\n    Stencil depth     : %u bits",       nStencilBits);
	LOG.Write("\n    Destination Alpha : %u bits",       _pContextData->_nDestAlpha);
	LOG.Write("\n    Multisampling     : %u sample(s)",  _pContextData->_nNumSamples);

	HWND hWnd;

	// Create a dummy window.

	CWindowGL::RegisterClass();

	if(!(hWnd = CreateWindowEx(	WS_EX_APPWINDOW,
								CWindowGL::GetWndClass().lpszClassName,
								"Stravaganza",
								WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0,
								320, 200,
								NULL,
								NULL,
								CWindowGL::GetWndClass().hInstance,
								NULL)))
	{
		LOG.Write("\n  ERROR - Can't create OpenGL dummy window!, GetLastError returned %u", GetLastError());
		return false; 
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Set pixel format

	HDC hDC = GetDC(hWnd);

	if(hDC == NULL)
	{
		CLogger::ErrorWindow("Error getting device context (GetLastError() = %d).", GetLastError());
		DestroyWindow(hWnd);
		return false;
	}

	LOG.IncIndentation(2);

	_pContextData->_nColorBits   = nColorBits;
	_pContextData->_nDepthBits   = nDepthBits;
	_pContextData->_nStencilBits = nStencilBits;

	if(!SetPixelFormat(hDC))
	{
		LOG.DecIndentation(2);
		DestroyWindow(hWnd);
		return false;
	}

	LOG.DecIndentation(2);

	LOG.Write("\n  Creating render context:");

	// Create render context

	_pContextData->_hRC = wglCreateContext(hDC);

	if(_pContextData->_hRC == NULL)
	{
		CLogger::ErrorWindow("Error creating render context (GetLastError() = %d).", GetLastError());
		ReleaseDC(hWnd, hDC);
		DestroyWindow(hWnd);
		return false;
	}

	if(wglMakeCurrent(hDC, _pContextData->_hRC) == FALSE)
	{
		CLogger::ErrorWindow("Error making render context current: wglMakeCurrent(%u, %u) (GetLastError() = %d).", hDC, _pContextData->_hRC, GetLastError());
		ReleaseDC(hWnd, hDC);
		DestroyWindow(hWnd);
		return false;
	}

	// Dump info

	LOG.Write("\n    HGLRC      : %d", wglGetCurrentContext());
	LOG.Write("\n    Vendor     : %s", glGetString(GL_VENDOR));
	LOG.Write("\n    Renderer   : %s", glGetString(GL_RENDERER));
	LOG.Write("\n    Version    : %s", glGetString(GL_VERSION));
	LOG.Write("\n    Extensions : ");

	const unsigned char* szExtensions = glGetString(GL_EXTENSIONS);
	int i = 0;

	if(szExtensions)
	{
		while(szExtensions[i])
		{
			LOG.Write("%c", szExtensions[i++]);
		}
	}
	else LOG.Write("(null)");

	// Extensions

	LOG.Write("\n  Initializing extensions.");
	UtilGL::Extensions::Init();

	// Multisampling support for this RC

	_pContextData->_mapMultisampleModes.clear();

	bool bMultisamplingSupport = false;

	if(UtilGL::Extensions::GetAvailableExtensions()->wgl.ARB_multisample)
	{
		float pfAttributes[] = {	0.0f, 0.0f };
		int   piAttributes[] = {	WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
									WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
									WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
									WGL_COLOR_BITS_ARB,     _pContextData->_pfd.cColorBits,
									WGL_ALPHA_BITS_ARB,     _pContextData->_pfd.cAlphaBits,
									WGL_DEPTH_BITS_ARB,     _pContextData->_pfd.cDepthBits,
									WGL_STENCIL_BITS_ARB,   _pContextData->_pfd.cStencilBits,
									WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
									WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
									WGL_SAMPLES_ARB,        0,
									0, 0 };

		for(int nSamples = 64; nSamples >= 2; nSamples--)
		{
			int   nPixelFormat;
			int   nValid;
			UINT  nNumFormats;
			piAttributes[19] = nSamples; // Num samples

			nValid = wglChoosePixelFormatARB(hDC, piAttributes, pfAttributes, 1, &nPixelFormat, &nNumFormats);
 
			if(nValid && nNumFormats >= 1)
			{
				_pContextData->_mapMultisampleModes.insert(std::map<int, int>::value_type(nSamples, nPixelFormat));
			}
		}

		std::map<int, int>::iterator it;
		int nCount;

		if(_pContextData->_mapMultisampleModes.size() > 0)
		{
			LOG.Write("\n  Found support for");

			for(nCount = 0, it = _pContextData->_mapMultisampleModes.begin(); it != _pContextData->_mapMultisampleModes.end(); ++it, nCount++)
			{
				LOG.Write(" %u", it->first);
			}

			LOG.Write(" multisampling mode(s).");

			bMultisamplingSupport = true;
		}
	}

	if(!bMultisamplingSupport)
	{
		LOG.Write("\n  No multisampling modes supported");
	}

	// Renderstates

	LOG.Write("\n  Loading renderstate cache.");

	UtilGL::States::InvalidateCache();
	UtilGL::States::SetDefaults();

	// Texturing:

	LOG.Write("\n  Reseting texturing info.");

	UtilGL::Texturing::Reset();

	if(UtilGL::Extensions::GetAvailableExtensions()->ARB_multitexture)
	{
		LOG.Write("\n  Got %u texture units", UtilGL::Texturing::GetNumUnits());
	}
	else
	{
		LOG.Write("\n  No multitexturing present!");
	}

	// Matrices:

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

	// Destroy temp window

	LOG.Write("\n  Releasing dummy window's device context,");
	ReleaseDC(hWnd, hDC);

	LOG.Write("\n  Deleting dummy window.");
	DestroyWindow(hWnd);

	wglMakeCurrent(NULL, NULL);

	LOG.Write("\nOpenGL render context succesfully created.");

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool GetCurrentPixelFormat(PIXELFORMATDESCRIPTOR* pPFDOut)
//
//  - Purpose   : Returns the current videomode pixel format descriptor.
//
// -----------------------------------------------------------------------------
bool GetCurrentPixelFormat(PIXELFORMATDESCRIPTOR* pPFDOut)
{
	assert(pPFDOut);

	if(_pContextData->_nColorBits > 0)
	{
		memcpy(pPFDOut, &_pContextData->_pfd, sizeof(PIXELFORMATDESCRIPTOR));
		return true;
	}

	return false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool Delete();
//
//  - Purpose   : Deletes current render context.
//
// -----------------------------------------------------------------------------
bool Delete()
{
	_pContextData->_nColorBits   = 0;
	_pContextData->_nDepthBits   = 0;
	_pContextData->_nStencilBits = 0;

	if(_pContextData->_hRC == NULL)
	{
		LOG.Write("\nWARNING - CUtilGL::RenderContext::Delete() called over a NULL render context.");
		return true;
	}

	int nErrorCount = 0;

	if(CWindowGL::GetNumWindows() > 0)
	{
		LOG.Write("\nWARNING - CUtilGL::RenderContext::Delete() called with open windows. Continuing.");
	}

	LOG.Write("\n\nDeleting active render context.");

	// Delete render context.

	LOG.Write("\n  Calling wglDeleteContext()");
	if(wglDeleteContext(_pContextData->_hRC) == 0)
	{
		LOG.Write(" Error!");
		nErrorCount++;
	}

	_pContextData->_hRC = NULL;

	CWindowGL::UnregisterClass();

	return nErrorCount == 0 ? true : false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : HGLRC Get();
//
//  - Purpose   : Returns current render context.
//
// -----------------------------------------------------------------------------
HGLRC Get()
{
	return _pContextData->_hRC;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool SetPixelFormat(HDC hDC)
//
//  - Purpose   : Sets the pixel format of a given device context to make it
//                suitable for the OpenGL render context.
//
// -----------------------------------------------------------------------------
bool SetPixelFormat(HDC hDC)
{
	if(!_pContextData->_nColorBits)
	{
		LOG.Write("\nERROR - UtilGL::RenderContext::SetPixelFormat(). No proper pixelformat info.");
		return false;
	}

	// Fill pixel format structure

	PIXELFORMATDESCRIPTOR pfd = \
	{
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |			// |
			PFD_SUPPORT_OPENGL |			// | Flags
			PFD_DOUBLEBUFFER,				// |
			PFD_TYPE_RGBA,					// RGB/indexed
			_pContextData->_nColorBits,		// Color bits per pixel.
			0, 0, 0, 0, 0, 0,				// Number of R/G/B bitplanes and shift counts.
			_pContextData->_nDestAlpha, 0,	// Number of alpha bitplanes and shift counts.
			0, 0, 0, 0, 0,					// Accumulation buffer info.
			_pContextData->_nDepthBits,		// Depth bits per pixel.
			_pContextData->_nStencilBits,	// Stencil bits per pixel.
			0,								// Number of auxiliary buffers.
			PFD_MAIN_PLANE,					// Layer type.
			0,								// Number of overlay/underlay planes.
			0, 0, 0							// Some masks, most of them are no longer used.
	};

	LOG.Write("\nChoosing pixel format.");
	
	// Ask for our pixel format

	int nPixelFormat = -1;

	if(_pContextData->_nNumSamples > 1)
	{
		if(_pContextData->_mapMultisampleModes.find(_pContextData->_nNumSamples) != _pContextData->_mapMultisampleModes.end())
		{
			nPixelFormat = _pContextData->_mapMultisampleModes[_pContextData->_nNumSamples];
		}
		else
		{
			_pContextData->_nNumSamples = 1; // Should not happen if we queried multisampling modes.
		}
	}

	if(nPixelFormat == -1)
	{
		nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	}

	DescribePixelFormat(hDC, nPixelFormat, pfd.nSize, &pfd);

	if(!nPixelFormat)
	{
		LOG.Write(" Error! GetLastError() : %d", GetLastError());
		return false;
	}
	else LOG.Write(" OK, using pixel format index %d.", nPixelFormat);

	LOG.Write("\nComparing requested values with returned ones.");

	// Check if the returned pixel format is the same we asked for.

	if(pfd.cColorBits != _pContextData->_nColorBits)
	{
		LOG.Write("\nWARNING - Device doesn´t support %ubpp color buffer in this mode, using %u.", _pContextData->_nColorBits, pfd.cColorBits);
	}
	
	if(pfd.cDepthBits != _pContextData->_nDepthBits)
	{
		LOG.Write("\nWARNING - Device doesn´t support %ubpp depth buffer in this mode, using %u.", _pContextData->_nDepthBits, pfd.cDepthBits);
	}

	if(pfd.cStencilBits != _pContextData->_nStencilBits)
	{
		LOG.Write("\nWARNING - Device doesn´t support %ubpp stencil buffer in this mode, using %u..", _pContextData->_nStencilBits, pfd.cStencilBits);
	}

	if(pfd.cStencilBits != _pContextData->_nStencilBits)
	{
		LOG.Write("\nWARNING - Device doesn´t support %ubpp stencil buffer in this mode, using %u..", _pContextData->_nStencilBits, pfd.cStencilBits);
	}

	if(pfd.cAlphaBits != _pContextData->_nDestAlpha)
	{
		LOG.Write("\nWARNING - Device doesn´t support %ubpp alpha destination buffer in this mode, using %u..", _pContextData->_nDestAlpha, pfd.cAlphaBits);
	}

	// Set pixel format

	LOG.Write("\nSetting pixel format.");

	SetLastError(0);

	if(::SetPixelFormat(hDC, nPixelFormat, &pfd) == FALSE)
	{
		LOG.Write(" Error setting pixel format! GetLastError() : %d", GetLastError());
		memset(&_pContextData->_pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		return false;
	}
	else LOG.Write(" OK.");

	memcpy(&_pContextData->_pfd, &pfd, sizeof(PIXELFORMATDESCRIPTOR));

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool GetNumMultisampleModes()
//
//  - Purpose   : Gets the number of multisample modes for the current rendering
//				  context mode.
//
// -----------------------------------------------------------------------------
int	GetNumMultisampleModes()
{
	return _pContextData->_mapMultisampleModes.size();
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool GetNumMultisamples(int nMode, int* pnSamples, int* pnPixelFormat)
//
//  - Purpose   : Gets the number of samples and pixel format index of the given
//				  mode. Get mode count using GetNumMultisampleModes().
//
// -----------------------------------------------------------------------------
int	GetNumMultisamples(int nMode, int* pnSamples, int* pnPixelFormat)
{
	std::map<int, int>::iterator it;
	int nCount;

	for(nCount = 0, it = _pContextData->_mapMultisampleModes.begin(); it != _pContextData->_mapMultisampleModes.end(); ++it, nCount++)
	{
		if(nCount == nMode)
		{
			*pnSamples     = it->first;
			*pnPixelFormat = it->second;
			return true;
		}
	}

	return false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : UseMultisampling(int nSamples)
//
//  - Purpose   : Tells to use the given number of samples in multisampling in
//				  the next render context creation calls.
//				  The normal way to do this is to create a rendering context,
//				  then enumerate its valid multisampling modes, check for the
//				  desired one and re-create the rendering context after calling
//				  this function.
//
// -----------------------------------------------------------------------------
bool UseMultisampling(int nSamples)
{
	_pContextData->_nNumSamples = nSamples;
	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : int GetCurrentSamples()
//
//  - Purpose   : Returns the number of samples currently being used.
//				  If it returns a value > 1 then multisampling is active.
//
// -----------------------------------------------------------------------------
int	GetCurrentSamples()
{
	return _pContextData->_nNumSamples;
}


} // namespace RenderContext

} // namespace UtilGL