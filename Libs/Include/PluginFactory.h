// ==[ File ]===================================================================================
//
//  -Name     : PluginFactory.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPluginFactory definition.
//
// =============================================================================================

/*

	04/12/2004  -  File created.


*/


#ifndef __PLUGINFACTORY_H
#define __PLUGINFACTORY_H


// ==[ Class definitions ]=======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CPluginFactory
//
//  - Purpose   : Plugin factory class.
//
// ---------------------------------------------------------------------------
class CPluginFactory : public TFactory<CPlugin>
{


public:


	static CPluginFactory& Instance();


private:


	CPluginFactory				()                              { }
	CPluginFactory				(const CPluginFactory& factory) { }
	CPluginFactory& operator =	(const CPluginFactory& factory) { }

};


// ==[ Macros ]==================================================================================


#define REGISTER_PLUGIN(PluginFactory, PluginClass)											\
																							\
	class PluginClass##Creator : public PluginFactory::CObjectCreator						\
	{																						\
		std::string	InstanceClassType() { return PluginClass::GETCLASSTYPE();	}			\
		std::string	InstanceClassName() { return PluginClass::CLASSNAME;		}			\
		CPlugin*	CreateInstance()    { return new PluginClass;				}			\
	};																						\
																							\
	PluginFactory::TRegisterCreator<PluginClass##Creator> the##PluginClass##RegisterCreator(PluginFactory::Instance());


#endif
