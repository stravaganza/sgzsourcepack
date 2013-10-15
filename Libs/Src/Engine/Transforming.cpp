// OpenGL transforming utilities

// ==[ File ]===================================================================================
//
//  -Name     : Transforming.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Texturing namespace implementation.
//
// =============================================================================================

/*

	11/02/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

extern SContextData* _pContextData;

namespace Transforming
{

// ==[ Functions  ]=============================================================================


// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool SetMatrix(EMatrixType eMatrix, const CMatrix& matrix);
//
//  - Purpose   : Sets a matrix (either world, view, projection or texture).
//
// -----------------------------------------------------------------------------
bool SetMatrix(EMatrixType eMatrix, const CMatrix& matrix)
{
	assert(UtilGL::IsReady());

	switch(eMatrix)
	{
	case MATRIX_WORLD:

		_pContextData->_worldMatrix = matrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((_pContextData->_viewMatrix * _pContextData->_worldMatrix).Transposed().Data());
		return true;

	case MATRIX_VIEW:

		_pContextData->_viewMatrix = matrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((_pContextData->_viewMatrix * _pContextData->_worldMatrix).Transposed().Data());
		return true;

	case MATRIX_PROJECTION:

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(matrix.Transposed().Data());
		return true;

	case MATRIX_TEXTURE:

		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(matrix.Transposed().Data());
		return true;

	default: break;
	}

	assert(!"UtilGL::Transforming::SetMatrix() invalid matrix type!");
	return false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : CMatrix GetMatrix(EMatrixType eMatrix)
//
//  - Purpose   : Returns a requested matrix.
//
// -----------------------------------------------------------------------------
CMatrix GetMatrix(EMatrixType eMatrix)
{
	assert(UtilGL::IsReady());

	CMatrix retMatrix;

	switch(eMatrix)
	{
	case MATRIX_WORLD:

		return _pContextData->_worldMatrix;

	case MATRIX_VIEW:

		return _pContextData->_viewMatrix;

	case MATRIX_PROJECTION:

		glGetFloatv(GL_PROJECTION_MATRIX, const_cast<float*>(retMatrix.Data()));
		return retMatrix.Transposed();

	case MATRIX_TEXTURE:

		glGetFloatv(GL_TEXTURE_MATRIX, const_cast<float*>(retMatrix.Data()));
		return retMatrix.Transposed();

	default: break;
	}

	assert(!"UtilGL::Transforming::GetMatrix() invalid matrix type!");
	return CMatrix::IDENTITY;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool PushMatrix(EMatrixType eMatrix)
//
//  - Purpose   : Pushes a matrix into the stack.
//
// -----------------------------------------------------------------------------
bool PushMatrix(EMatrixType eMatrix)
{
	assert(UtilGL::IsReady());

	switch(eMatrix)
	{
	case MATRIX_WORLD:

		_pContextData->_stcMatrices.push(_pContextData->_worldMatrix);
		return true;

	case MATRIX_VIEW:

		_pContextData->_stcMatrices.push(_pContextData->_viewMatrix);
		return true;

	case MATRIX_PROJECTION:

		_pContextData->_stcMatrices.push(GetMatrix(MATRIX_PROJECTION));
		return true;

	case MATRIX_TEXTURE:

		_pContextData->_stcMatrices.push(GetMatrix(MATRIX_TEXTURE));
		return true;

	default: break;
	}

	assert(!"UtilGL::Transforming::PushMatrix() invalid matrix type!");
	return false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool PopMatrix(EMatrixType eMatrix)
//
//  - Purpose   : Pops a matrix from the stack into the given matrix type.
//
// -----------------------------------------------------------------------------
bool PopMatrix(EMatrixType eMatrix)
{
	assert(UtilGL::IsReady());
	assert(!_pContextData->_stcMatrices.empty());

	bool bResult = SetMatrix(eMatrix, _pContextData->_stcMatrices.top());
	_pContextData->_stcMatrices.pop();
	return bResult;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool ClearMatrix(EMatrixType eMatrix)
//
//  - Purpose   : Sets a matrix to the identity.
//
// -----------------------------------------------------------------------------
bool ClearMatrix(EMatrixType eMatrix)
{
	assert(UtilGL::IsReady());

	switch(eMatrix)
	{
	case MATRIX_WORLD:
	case MATRIX_VIEW:

		SetMatrix(eMatrix, CMatrix::IDENTITY);
		return true;

	case MATRIX_PROJECTION:

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		return true;

	case MATRIX_TEXTURE:

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		return true;

	default: break;
	}

	assert(!"UtilGL::Transforming::PushMatrix() invalid matrix type!");
	return false;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool LocalToNormViewport(CVector3* pVertex)
//
//  - Purpose   : Projects a vertex in local coordinates to normalized
//                viewport coordinates upleft(0.0, 0.0) to lowright(1.0, 1.0).
//
// -----------------------------------------------------------------------------
bool LocalToNormViewport(CVector3* pVertex)
{
	assert(UtilGL::IsReady());
	assert(pVertex);

	double dOutX, dOutY, dOutZ;
	double modelviewMatrix[16];
	double projectionMatrix[16];
	int    viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX,  modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Local to window coordinates

	gluProject((double)pVertex->X(), (double)pVertex->Y(), (double)pVertex->Z(),
	           modelviewMatrix, projectionMatrix,
			   viewport,
			   &dOutX, &dOutY, &dOutZ);

	// Window coordinates to viewport coordinates

	assert(CWindowGL::GetActiveWindow());

	int nWidth  = CWindowGL::GetActiveWindow()->GetWidth();
	int nHeight = CWindowGL::GetActiveWindow()->GetHeight();

	dOutY = (double)nHeight - dOutY;

	float fViewpY = (float)(nHeight - (viewport[1] + viewport[3]));

	pVertex->SetX((float)dOutX - (float)viewport[0]);
	pVertex->SetY((float)dOutY - fViewpY);
	pVertex->SetZ((float)dOutZ);

	// Normalize

	pVertex->SetX(pVertex->X() / (float)viewport[2]);
	pVertex->SetY(pVertex->Y() / (float)viewport[3]);

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool WorldToNormViewport(CVector3* pVertex)
//
//  - Purpose   : Projects a vertex in world coordinates to normalized
//                viewport coordinates upleft(0.0, 0.0) to lowright(1.0, 1.0).
//
// -----------------------------------------------------------------------------
bool WorldToNormViewport(CVector3* pVertex)
{
	assert(UtilGL::IsReady());
	assert(pVertex);

	PushMatrix(MATRIX_WORLD);
	ClearMatrix(MATRIX_WORLD);

	bool bResult = LocalToNormViewport(pVertex);

	PopMatrix(MATRIX_WORLD);

	return bResult;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool WindowToNormViewport(CVector3* pVertex)
//
//  - Purpose   : Transforms a vertex in window (pixel) coordinates into normalized
//                viewport coordinates upleft(0.0, 0.0) to lowright(1.0, 1.0).
//
// -----------------------------------------------------------------------------
bool WindowToNormViewport(CVector3* pVertex)
{
	assert(UtilGL::IsReady());
	assert(pVertex);
	assert(CWindowGL::GetActiveWindow());

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float fViewpY = (float)(CWindowGL::GetActiveWindow()->GetHeight() - (viewport[1] + viewport[3]));

	float fX = pVertex->X() - (float)viewport[0];
	float fY = pVertex->Y() - fViewpY;
	float fZ = pVertex->Z();

	// Normalize

	fX /= (float)viewport[2];
	fY /= (float)viewport[3];

	pVertex->Set(fX, fY, fZ);

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool NormViewportToLocal(CVector3* pVertex)
//
//  - Purpose   : Unprojects a vertex in normalized viewport coordinates
//                to local coordinates.
//
// -----------------------------------------------------------------------------
bool NormViewportToLocal(CVector3* pVertex)
{
	assert(UtilGL::IsReady());
	assert(pVertex);
	assert(CWindowGL::GetActiveWindow());

	double dOutX, dOutY, dOutZ;
	double modelviewMatrix[16];
	double projectionMatrix[16];
	int    viewport[4];
	int    nWinHeight = CWindowGL::GetActiveWindow()->GetHeight();

	glGetDoublev(GL_MODELVIEW_MATRIX,  modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Normalized -> viewport pixel

	float fX = pVertex->X();
	float fY = pVertex->Y();
	float fZ = pVertex->Z();

	fX *= (float)(viewport[2]);
	fY *= (float)(viewport[3]);

	// viewport pixel -> window pixel

	fX += (float)viewport[0];
	fY += (float)(nWinHeight - (viewport[1] + viewport[3]));
	fY  = (float)nWinHeight - fY;

	gluUnProject((double)fX, (double)fY, (double)fZ,
	             modelviewMatrix, projectionMatrix,
			     viewport,
			     &dOutX, &dOutY, &dOutZ);

	pVertex->Set((float)dOutX, (float)dOutY, (float)dOutZ);

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool NormViewportToWorld(CVector3* pVertex);
//
//  - Purpose   : Unprojects a vertex in normalized viewport coordinates
//                to world coordinates.
//
// -----------------------------------------------------------------------------
bool NormViewportToWorld(CVector3* pVertex)
{
	assert(UtilGL::IsReady());
	assert(pVertex);

	PushMatrix(MATRIX_WORLD);
	ClearMatrix(MATRIX_WORLD);

	bool bResult = NormViewportToLocal(pVertex);

	PopMatrix(MATRIX_WORLD);

	return bResult;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool NormViewportToWindow(CVector3* pVertex)
//
//  - Purpose   : Transforms a vertex in normalized viewport coordinates to
//                window (pixel) coordinates.
//
// -----------------------------------------------------------------------------
bool NormViewportToWindow(CVector3* pVertex)
{
	assert(UtilGL::IsReady());
	assert(pVertex);
	assert(CWindowGL::GetActiveWindow());

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Denormalize

	float fX = pVertex->X() * (float)viewport[2];
	float fY = pVertex->Y() * (float)viewport[3];
	float fZ = pVertex->Z();

	float fViewpY = (float)(CWindowGL::GetActiveWindow()->GetHeight() - (viewport[1] + viewport[3]));

	fX += (float)viewport[0];
	fY += fViewpY;

	pVertex->Set(fX, fY, fZ);

	return true;
};

} // namespace Transforming

} // namespace UtilGL
