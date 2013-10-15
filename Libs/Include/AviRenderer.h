// ==[ File ]===================================================================================
//
//  -Name     : AviRenderer.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAviRenderer definition.
//
//  -Note     : Code ripped from NVidia's great OpenGL SDK, slightly modified.
//
// =============================================================================================

/*

	22/02/2002  -  File created.


*/


#ifndef _AVIRENDERER_H
#define _AVIRENDERER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAviRenderer
//
//  - Purpose   : Render to .avi file.
//
// ---------------------------------------------------------------------------
class CAviRenderer
{


public:

    
	CAviRenderer();
    ~CAviRenderer();

    bool StartRender(CWindowGL *pWindowGL, std::string strAviFile, int nFramerate);
    bool StopRender();
    bool SaveFrame();

	bool IsValid() const { return m_bIsWorking && m_pWindowGL; }


private:


	HANDLE  MakeDib(HBITMAP hBitmap, UINT uBits);
	HBITMAP LoadBMPFromFB(CWindowGL* pWindowGL);

	CWindowGL*    m_pWindowGL;
	std::string   m_strAviFile;

	AVISTREAMINFO m_aviStreamInfo;
	PAVIFILE      m_pAviFile;
	PAVISTREAM    m_pAviStream;
	PAVISTREAM    m_pAviStreamCompressed;
	PAVISTREAM    m_pAviText;

	AVICOMPRESSOPTIONS      m_aviOptions;
	AVICOMPRESSOPTIONS FAR* m_pAviOptions[1];

	int  m_nWidth, m_nHeight;
	int  m_nFrameCount;
	bool m_bIsWorking;
};


#endif
