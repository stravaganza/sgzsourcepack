// ==[ File ]===================================================================================
//
//  -Name     : Plugin.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPlugin definition.
//
// =============================================================================================

/*

	04/12/2004  -  File created.


*/


#ifndef __PLUGIN_H
#define __PLUGIN_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CPlugin
//
//  - Purpose   : Base class for plugins.
//
// ---------------------------------------------------------------------------
class CPlugin
{


public:

	// Methods

	CPlugin()          { }
	virtual ~CPlugin() { }

	virtual std::string GetClassType() const = 0;
	virtual std::string GetClassName() const = 0;
};


#endif