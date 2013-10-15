// ==[ File ]===================================================================================
//
//  -Name     : RenderContext.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::RenderContext namespace. OpenGL render context managing.
//
// =============================================================================================

/*

	28/01/2002  -  File created.


*/


#ifndef __RENDERCONTEXT_H
#define __RENDERCONTEXT_H


// ==[ Functions  ]=============================================================================


namespace UtilGL
{

namespace RenderContext
{

bool  UseDestinationAlpha(int nBits);
bool  Create(int nColorBits, int nDepthBits, int nStencilBits);
bool  GetCurrentPixelFormat(PIXELFORMATDESCRIPTOR* pPFDOut);
bool  Delete();
HGLRC Get();

bool  SetPixelFormat(HDC hDC);

int	  GetNumMultisampleModes();
int	  GetNumMultisamples    (int nMode, int* pnSamples, int* pnPixelFormat);
bool  UseMultisampling      (int nSamples);
int	  GetCurrentSamples		();


}

}

#endif
