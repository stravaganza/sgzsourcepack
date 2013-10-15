// OpenGL extensions managing

// ==[ File ]===================================================================================
//
//  -Name     : Extension.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Extensions namespace implementacion.
//              Uses Lev Povalahev's extension library 
//
// =============================================================================================

/*

	11/12/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

extern SContextData* _pContextData;

namespace Extensions
{


// ==[ Functions  ]=============================================================================


// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : bool InitExtensions()
//
//  - Purpose   : Inits all available extensions.
//
// ---------------------------------------------------------------------------
bool Init()
{
	assert(UtilGL::IsReady());

	return extgl_Initialize() == 0;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : bool IsSupported(const std::string& strExtension);
//
//  - Purpose   : Checks for a specific OpenGL extension, and returns true
//                if exists.
//
// ---------------------------------------------------------------------------
bool IsSupported(const std::string& strExtension)
{
	assert(UtilGL::IsReady());

	const char *szExtensions = (const char*)glGetString(GL_EXTENSIONS);

	return strstr(szExtensions, strExtension.data()) != NULL;
}

// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : const ExtensionTypes* GetAvailableExtensions()
//
//  - Purpose   : Returns the available extensions information.
//
// ---------------------------------------------------------------------------
const ExtensionTypes* GetAvailableExtensions()
{
	return _pContextData->_pAvailableExtensions;
}


} // namespace Extensions

} // namespace UtilGL