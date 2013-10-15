// ==[ File ]===================================================================================
//
//  -Name     : Transforming.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Transforming namespace. OpenGL transforming utilities.
//
// =============================================================================================

/*

	11/02/2002  -  File created.


*/


#ifndef __TRANSFORMING_H
#define __TRANSFORMING_H


// ==[ Functions  ]=============================================================================


class CWindowGL;


namespace UtilGL
{

namespace Transforming
{

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : EMatrixType
//
//  - Purpose   : Enumerate our OpenGL matrices.
//                Split OpenGL MODELVIEW matrix into MATRIX_WORLD and MATRIX_VIEW.
//
// ---------------------------------------------------------------------------
enum EMatrixType
{
	MATRIX_WORLD,
	MATRIX_VIEW,
	MATRIX_PROJECTION,
	MATRIX_TEXTURE,
};

// Matrices:

bool    SetMatrix            (EMatrixType eMatrix, const CMatrix& matrix);
CMatrix GetMatrix            (EMatrixType eMatrix);
bool    PushMatrix           (EMatrixType eMatrix);
bool    PopMatrix            (EMatrixType eMatrix);
bool    ClearMatrix          (EMatrixType eMatrix);

// Coordinates transform:

bool LocalToNormViewport (CVector3* pVertex);
bool WorldToNormViewport (CVector3* pVertex);
bool WindowToNormViewport(CVector3* pVertex);
bool NormViewportToLocal (CVector3* pVertex);
bool NormViewportToWorld (CVector3* pVertex);
bool NormViewportToWindow(CVector3* pVertex);

} // namespace Transforming

} // namespace UtilGL

#endif

