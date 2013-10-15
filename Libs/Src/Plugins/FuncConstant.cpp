// ==[ File ]===================================================================================
//
//  -Name     : FuncConstant.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncConstant implementation
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "FuncConstant.h"


const std::string CFuncConstant::CLASSNAME = "Constant";

REGISTER_PLUGIN(CPluginFactory, CFuncConstant)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncConstant
//  - Prototype : CFuncConstant()
//
//  - Purpose   : CFuncConstant's default constructor.
//
// -----------------------------------------------------------------------------
CFuncConstant::CFuncConstant()
{
	m_fValue = 0.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncConstant
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the function parameters from a script.
//
// -----------------------------------------------------------------------------
bool CFuncConstant::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strOperation;

	pParser->ReadKeyRaw  ("#Operation", '=', &strOperation, CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Value", '=', &m_fValue, CParser::SEARCH_WHOLEDATA);
	SetOperation(StringToOperation(strOperation));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncConstant
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the function parameters in a text script format.
//
// -----------------------------------------------------------------------------
bool CFuncConstant::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	pWriter->Write("\n#Function=\"%s\"", GetClassName().c_str());
	pWriter->Write("\n{");
	pWriter->Write("\n  #Operation = %s", OperationToString(GetOperation()).c_str());
	pWriter->Write("\n  #Value     = %f", m_fValue);
	pWriter->Write("\n}");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncConstant
//  - Prototype : float Evaluate(float fTime)
//
//  - Purpose   : Returns a float value as a result of evaluating the function
//                given a time value.
//
// -----------------------------------------------------------------------------
float CFuncConstant::Evaluate(float fTime)
{
	return m_fValue;
}
