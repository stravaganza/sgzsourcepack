// ==[ File ]===================================================================================
//
//  -Name     : Function.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFunction implementation.
//
// =============================================================================================

/*

	21/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


std::string CFunction::GETCLASSTYPE()
{
	static std::string CLASSTYPE = "Function";
	return CLASSTYPE;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFunction
//  - prototype : std::string OperationToString(EOperation operation) const
//
//  - Purpose   : Returns a string given an operation.
//
// -----------------------------------------------------------------------------
std::string CFunction::OperationToString(EOperation operation) const
{
	switch(operation)
	{
	case OP_ADD:  return std::string("Add");
	case OP_SUB:  return std::string("Sub");
	case OP_MULT: return std::string("Mult");
	default:
		
		break;
	}

	return std::string("None");
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFunction
//  - prototype : CFunction::EOperation StringToOperation(const std::string& strOperation) const
//
//  - Purpose   : Returns an operation given its string.
//
// -----------------------------------------------------------------------------
CFunction::EOperation CFunction::StringToOperation(const std::string& strOperation) const
{
	     if(!stricmp(strOperation.c_str(), "Add" )) return OP_ADD;
	else if(!stricmp(strOperation.c_str(), "Sub" )) return OP_SUB;
	else if(!stricmp(strOperation.c_str(), "Mult")) return OP_MULT;

	return OP_MULT;
}
