// ==[ File ]===================================================================================
//
//  -Name     : Rendering.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Rendering namespace. OpenGL rendering utilities.
//
// =============================================================================================

/*

	28/01/2002  -  File created.


*/


#ifndef __RENDERING_H
#define __RENDERING_H


// ==[ Functions  ]=============================================================================


namespace UtilGL
{

namespace Rendering
{

bool DrawRect(	UtilGL::Texturing::CTexture2D *pTexture,
				float fPosX,  float fPosY,  float fSizeX, float fSizeY,
				float fAngle, const CVector4& v4Color, int nSrcBlend, int nDstBlend);

bool DrawCenteredQuad(	UtilGL::Texturing::CTexture2D *pTexture,
						float fCenterX, float fCenterY, float fWidth, float fHeight,
						float fAngle, const CVector4& v4Color, int nSrcBlend, int nDstBlend);

bool DrawBillboards(int nNumBillboards,
					UtilGL::Texturing::CTexture2D *pTexture,
					const CVector3* pPositions,
					const CVector4* pColors,
					float fSize,
					float fAngle = 0.0f);

bool DrawBillboards(int nNumBillboards,
					UtilGL::Texturing::CTexture2D *pTexture,
					const CVector3* pPositions,
					const CVector4* pColors,
					const float*    pfSizes);

bool ComputeBoxBlur(int	nRadiusPwr2, UtilGL::Texturing::CTexture2D* pTexture);

}

}

#endif

