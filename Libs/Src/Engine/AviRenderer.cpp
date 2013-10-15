// ==[ File ]===================================================================================
//
//  -Name     : AviRenderer.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAviRenderer ìmplementation.
//
//  -Note     : Code ripped from NVidia's great OpenGL SDK with some modifications by myself.
//
// =============================================================================================

/*

	22/02/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Constants ]==============================================================================


#define AVIIF_KEYFRAME	0x00000010L


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : CAviRenderer()
//
//  - Purpose   : CAviRenderer's default constructor.
//
// -----------------------------------------------------------------------------
CAviRenderer::CAviRenderer()
{
	m_nFrameCount = 0;
	m_bIsWorking  = false;

  	m_pAviFile             = NULL;
	m_pAviStream           = NULL;
	m_pAviStreamCompressed = NULL;
	m_pAviText             = NULL;
	m_pAviOptions[0]       = &m_aviOptions;

	m_pWindowGL = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : ~CAviRenderer()
//
//  - Purpose   : CAviRenderer's destructor.
//
// -----------------------------------------------------------------------------
CAviRenderer::~CAviRenderer()
{
	StopRender();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : bool StartRender(CWindowGL *pWindowGL, std::string strAviFile, int nFramerate)
//
//  - Purpose   : Prepares the object for capturing the opengl window to an AVI.
//
// -----------------------------------------------------------------------------
bool CAviRenderer::StartRender(CWindowGL *pWindowGL, std::string strAviFile, int nFramerate)
{
	assert(pWindowGL);

	m_pWindowGL  = NULL;
	m_bIsWorking = false;

	// Check VFW version.

	WORD wVer = HIWORD(VideoForWindowsVersion());

	if(wVer < 0x010A)
	{
		CLogger::ErrorWindow("Video For Windows outdated version");
		return false;
	}

	// Init library

	AVIFileInit();

    // Get an image and stuff it into a bitmap.

	HRESULT hr;
    HBITMAP bmp;

	if((bmp = LoadBMPFromFB(pWindowGL)) == NULL)
	{
		return false;
	}

	LPBITMAPINFOHEADER lpInfoHeader = (LPBITMAPINFOHEADER)GlobalLock(MakeDib(bmp, 32));
    DeleteObject(bmp);

	if(lpInfoHeader == NULL)
	{
		LOG.Write("\nERROR - CAviRenderer::StartRender(): GlobalLock() failed.");
		return false;
	}

	m_nWidth  = (int)lpInfoHeader->biWidth;
	m_nHeight = (int)lpInfoHeader->biHeight;

	// Open an avi file for writing

	hr = AVIFileOpen(	&m_pAviFile,		    // returned file pointer
						strAviFile.data(),		// file name
						OF_WRITE | OF_CREATE,	// mode to open file with
						NULL);					// use handler determined
												// from file extension....
	if (hr != AVIERR_OK)
	{
		LOG.Write("\nERROR - CAviRenderer::StartRender(): AVIFileOpen() failed.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	// Configure the stream

	_fmemset(&m_aviStreamInfo, 0, sizeof(m_aviStreamInfo));

	m_aviStreamInfo.fccType                = streamtypeVIDEO; // stream type
	m_aviStreamInfo.fccHandler             = 0;
	m_aviStreamInfo.dwScale                = 1;
	m_aviStreamInfo.dwRate                 = nFramerate;
	m_aviStreamInfo.dwSuggestedBufferSize  = lpInfoHeader->biSizeImage;

	SetRect(&m_aviStreamInfo.rcFrame, 0, 0,	// rectangle for stream
			(int)lpInfoHeader->biWidth,
			(int)lpInfoHeader->biHeight);

	// And create the stream

	hr = AVIFileCreateStream(m_pAviFile,		// file pointer
						     &m_pAviStream,		// returned stream pointer
							 &m_aviStreamInfo);	// stream header
	if(hr != AVIERR_OK)
	{
		LOG.Write("\nERROR - CAviRenderer::StartRender(): AVIFileCreateStream() failed.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	// Get save options (prompt dialog)

	_fmemset(&m_aviOptions, 0, sizeof(m_aviOptions));

	if(!AVISaveOptions(NULL, 0, 1, &m_pAviStream, (LPAVICOMPRESSOPTIONS FAR *) &m_pAviOptions))
	{
        LOG.Write("\nERROR - CAviRenderer::StartRender(): AVISaveOptions() failed.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	// Create compressed stream

	hr = AVIMakeCompressedStream(&m_pAviStreamCompressed, m_pAviStream, &m_aviOptions, NULL);

	if(hr != AVIERR_OK)
	{
        LOG.Write("\nERROR - CAviRenderer::StartRender(): AVIMakeCompressedStream() failed.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	// Set it's format

	hr = AVIStreamSetFormat(m_pAviStreamCompressed, 0,
							lpInfoHeader,	         // stream format
							lpInfoHeader->biSize +   // format size
							lpInfoHeader->biClrUsed * sizeof(RGBQUAD));

	if(hr != AVIERR_OK)
	{
		LOG.Write("\nERROR - CAviRenderer::StartRender(): AVIStreamSetFormat() failed.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	m_pWindowGL  = pWindowGL;
	m_bIsWorking = true;

	UpdateWindow(pWindowGL->GetHWND());

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : bool StopRender()
//
//  - Purpose   : Stops capturing and closes the file.
//
// -----------------------------------------------------------------------------
bool CAviRenderer::StopRender()
{
	// Close/free stuff

	if(m_pAviStream)
	{
		AVIStreamClose(m_pAviStream);
		m_pAviStream = NULL;
	}

	if(m_pAviStreamCompressed)
	{
		AVIStreamClose(m_pAviStreamCompressed);
		m_pAviStreamCompressed = NULL;
	}

	if(m_pAviText)
	{
		AVIStreamClose(m_pAviText);
		m_pAviText = NULL;
	}

	if(m_pAviFile)
	{
		AVIFileClose(m_pAviFile);
		m_pAviFile = NULL;
	}

	WORD wVer = HIWORD(VideoForWindowsVersion());

	if(wVer >= 0x010A)
	{
		AVIFileExit();
	}

	m_bIsWorking = false;
	m_pWindowGL  = NULL;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : bool CAviRenderer::SaveFrame()
//
//  - Purpose   : Captures a frame and saves it into the avi file.
//
// -----------------------------------------------------------------------------
bool CAviRenderer::SaveFrame()
{
	if(!IsValid())
	{
		return false;
	}

    // Get an image and stuff it into a bitmap.

	HRESULT hr;
    HBITMAP bmp;

    if((bmp = LoadBMPFromFB(m_pWindowGL)) == NULL)
	{
		return false;
	}

	LPBITMAPINFOHEADER lpInfoHeader = (LPBITMAPINFOHEADER)GlobalLock(MakeDib(bmp, 32));
    DeleteObject(bmp);

	if(lpInfoHeader == NULL)
	{
		LOG.Write("\nERROR - CAviRenderer::SaveFrame(). GlobalLock() failed.");
		return false;
	}

	if(m_nWidth != lpInfoHeader->biWidth)
	{
		LOG.Write("\nERROR - CAviRenderer::SaveFrame(): mismatched window width / info header width.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	if(m_nHeight != lpInfoHeader->biHeight)
	{
		LOG.Write("\nERROR - CAviRenderer::SaveFrame(): mismatched window height / info header height.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	// Save frame

	hr = AVIStreamWrite(m_pAviStreamCompressed,			// stream pointer
						m_nFrameCount,					// frame
						1,								// number of frames to write
						(LPBYTE) lpInfoHeader +			// pointer to data
						lpInfoHeader->biSize +
						lpInfoHeader->biClrUsed * sizeof(RGBQUAD),
						lpInfoHeader->biSizeImage,		// size of this frame
						AVIIF_KEYFRAME,					// flags....
						NULL,
						NULL);

	if(hr != AVIERR_OK)
	{
		CLogger::ErrorWindow("CAviRenderer::SaveFrame(). AVIStreamWrite() failed.");
		GlobalFreePtr(lpInfoHeader);
		return false;
	}

	GlobalFreePtr(lpInfoHeader);
	m_nFrameCount++;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : HANDLE MakeDib(HBITMAP hBitmap, UINT uBits)
//
//  - Purpose   : Makes a dib from a bitmap. If succesfull, it returns the
//                HANDLE, otherwise it returns NULL.
//
// -----------------------------------------------------------------------------
HANDLE CAviRenderer::MakeDib(HBITMAP hBitmap, UINT uBits)
{
	HANDLE              hdib;
	HDC                 hdc;
	BITMAP              bitmap;
	UINT                wLineLen;
	DWORD               dwSize;
	DWORD               wColSize;
	LPBITMAPINFOHEADER  lpbi;
	LPBYTE              lpBits;

	GetObject(hBitmap, sizeof(BITMAP), &bitmap) ;

	// DWORD align the width of the DIB.
	// Figure out the size of the colour table
	// Calculate the size of the DIB


	wLineLen = (bitmap.bmWidth * uBits + 31) / 32 * 4;
	wColSize = sizeof(RGBQUAD) * ((uBits <= 8) ? 1 << uBits : 0);
	dwSize   = sizeof(BITMAPINFOHEADER) + wColSize + (DWORD)(UINT)wLineLen * (DWORD)(UINT)bitmap.bmHeight;

	// Allocate room for a DIB and set the LPBI fields

	hdib = GlobalAlloc(GHND, dwSize);

	if(!hdib)
	{
		return hdib;
	}

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib) ;

	lpbi->biSize          = sizeof(BITMAPINFOHEADER) ;
	lpbi->biWidth         = bitmap.bmWidth ;
	lpbi->biHeight        = bitmap.bmHeight ;
	lpbi->biPlanes        = 1 ;
	lpbi->biBitCount      = (WORD) uBits ;
	lpbi->biCompression   = BI_RGB ;
	lpbi->biSizeImage     = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
	lpbi->biXPelsPerMeter = 0 ;
	lpbi->biYPelsPerMeter = 0 ;
	lpbi->biClrUsed       = (uBits <= 8) ? 1 << uBits : 0;
	lpbi->biClrImportant  = 0 ;

	// Get the bits from the bitmap and stuff them after the LPBI

	lpBits = (LPBYTE)(lpbi + 1) + wColSize ;

	hdc = CreateCompatibleDC(NULL) ;

	GetDIBits(hdc, hBitmap, 0, bitmap. bmHeight, lpBits, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	// Fix this if GetDIBits messed it up...

	lpbi->biClrUsed = (uBits <= 8) ? 1<<uBits : 0;

	DeleteDC(hdc) ;
	GlobalUnlock(hdib);

	return hdib ;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAviRenderer
//  - prototype : HBITMAP LoadBMPFromFB(CWindowGL* pWindowGL)
//
//  - Purpose   : Creates an HBITMAP from the window framebuffer.
//                Returns true if failed, otherwise the HBITMAP.
//
// -----------------------------------------------------------------------------
HBITMAP CAviRenderer::LoadBMPFromFB(CWindowGL* pWindowGL)
{
	assert(pWindowGL);

    // Create a normal DC and a memory DC for the entire screen. The 
    // normal DC provides a "snapshot" of the screen contents. The 
    // memory DC keeps a copy of this "snapshot" in the associated 
    // bitmap. 
 
    HDC hdcScreen     = pWindowGL->GetHDC();
    HDC hdcCompatible = CreateCompatibleDC(hdcScreen); 
 
    // Create a compatible bitmap for hdcScreen. 

    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, pWindowGL->GetWidth(), pWindowGL->GetHeight());

    if(hbmScreen == 0)
	{
		LOG.Write("\nERROR - CAviRenderer::LoadBMPFromFB(). hbmScreen == NULL.");
		return 0;
	}
 
    // Select the bitmaps into the compatible DC. 
 
    if(!SelectObject(hdcCompatible, hbmScreen))
	{
        LOG.Write("\nERROR - CAviRenderer::LoadBMPFromFB(). SelectObject() failed.");
        return 0;
	}
 
    if(!BitBlt(	hdcCompatible,
				0,0, pWindowGL->GetWidth(), pWindowGL->GetHeight(),
				hdcScreen,
				0, 0,
				SRCCOPY))
	{
		LOG.Write("\nERROR - CAviRenderer::LoadBMPFromFB(). BitBlt() failed.");
		return 0;
	}
 
   //ShowWindow(hwnd, SW_SHOW); 

    DeleteDC(hdcCompatible);
    return(hbmScreen);
}


