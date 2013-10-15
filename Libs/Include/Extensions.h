// ==[ File ]===================================================================================
//
//  -Name     : Extensions.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Extensions namespace. OpenGL extensions managing.
//
// =============================================================================================

/*

	28/01/2002  -  File created.


*/


#ifndef __EXTENSIONS_H
#define __EXTENSIONS_H


// ==[ Functions  ]=============================================================================


namespace UtilGL
{

namespace Extensions
{
	// Extensions:

	bool Init();
	const ExtensionTypes* GetAvailableExtensions();
}

}

#endif