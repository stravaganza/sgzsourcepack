// OpenGL utilities

// ==[ File ]===================================================================================
//
//  -Name     : UtilGL.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL namespace implementacion.
//
// =============================================================================================

/*

	11/12/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

static SContextData	_contextData;					// The "real" data
SContextData*		_pContextData = &_contextData;	// The one we use to access it, so that we can pass a pointer
													// like this to a dll and share the same data.

// ==[ Functions  ]=============================================================================


// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : bool IsReady()
//
//  - Purpose   : Returns true if our OpenGL Util functions can be called.
//
// ---------------------------------------------------------------------------
bool IsReady()
{
	return RenderContext::Get() != NULL;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : bool CheckError()
//
//  - Purpose   : Returns true if an OpenGL error occured.
//
// ---------------------------------------------------------------------------
bool CheckError()
{
	int nGLErrorValue = glGetError();

	return nGLErrorValue != GL_NO_ERROR ? true : false;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : const char* GetErrorMessage()
//
//  - Purpose   : Returns an error message with the description of last error.
//
// ---------------------------------------------------------------------------
const char* GetErrorMessage()
{
	int nGLErrorValue = glGetError();

	switch(nGLErrorValue)
	{
		case GL_NO_ERROR          : return "OpenGL error: GL_NO_ERROR";
		case GL_INVALID_ENUM      : return "OpenGL error: GL_INVALID_ENUM";
		case GLU_INVALID_ENUM     : return "OpenGL error: GLU_INVALID_ENUM";
		case GL_INVALID_VALUE     : return "OpenGL error: GL_INVALID_VALUE";
		case GLU_INVALID_VALUE    : return "OpenGL error: GLU_INVALID_VALUE";
		case GL_INVALID_OPERATION : return "OpenGL error: GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW    : return "OpenGL error: GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW   : return "OpenGL error: GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY     : return "OpenGL error: GL_OUT_OF_MEMORY";
		case GLU_OUT_OF_MEMORY    : return "OpenGL error: GLU_OUT_OF_MEMORY";

		default: break;
	}

	return "OpenGL error: Unknown Error";
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : int GetMaxLights()
//
//  - Purpose   : Returns the maximum supported number of hardware accelerated lights.
//
// ---------------------------------------------------------------------------
#define MAXLIGHTS 8
int GetMaxLights()
{
	return MAXLIGHTS;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : SContextData
//  - prototype : SetDefaults()
//
//  - Purpose   : Set the context data default values.
//
// -----------------------------------------------------------------------------
bool SContextData::SetDefaults()
{
	_pAvailableExtensions = &extgl_Extensions;

	_nColorBits   = 0;
	_nDepthBits   = 0;
	_nStencilBits = 0;
	_nDestAlpha   = 0;

	_nNumSamples = 1;

	_glSrcBlend  = GL_ONE;
	_glDstBlend  = GL_ZERO;

	_fRedBias    = 0.0f;
	_fGreenBias  = 0.0f;
	_fBlueBias   = 0.0f;
	_fAlphaBias  = 0.0f;
	_fRedScale   = 1.0f;
	_fGreenScale = 1.0f;
	_fBlueScale  = 1.0f;
	_fAlphaScale = 1.0f;

	_nTexActiveUnit = 0;
	_nTexUsedMem    = 0;

	return true;
};

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : bool ShareFrom(SContextData* pContextData)
//
//  - Purpose   : Gets the context data from a shared source. This way we
//				  can communicate static data through exes and dlls.
//
// ---------------------------------------------------------------------------
bool ShareFrom(SContextData* pContextData)
{
	_pContextData = pContextData;

	return _pContextData != NULL;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : SContextData* GetContextData()
//
//  - Purpose   : Returns the render context data structure.
//
// ---------------------------------------------------------------------------
SContextData* GetContextData()
{
	return _pContextData;
}

} // namespace UtilGL