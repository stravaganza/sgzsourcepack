// ==[ File ]===================================================================================
//
//  -Name     : PluginFactory.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : PluginFactory implementation.
//
// =============================================================================================

/*

	04/12/2004  -  File created.


*/


#include "Plugins.h"


// ==[ Class Implementation ]=====================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPluginFactory
//  - Prototype : static CPluginFactory& Instance();
//
//  - Purpose   : Returns the global CPluginFactory instance.
//
// -----------------------------------------------------------------------------
CPluginFactory& CPluginFactory::Instance()
{
	static CPluginFactory s_instance;
	return s_instance;
}