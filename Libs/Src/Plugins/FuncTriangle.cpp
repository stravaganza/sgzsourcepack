// ==[ File ]===================================================================================
//
//  -Name     : FuncTriangle.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncTriangle implementation
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "FuncTriangle.h"


const std::string CFuncTriangle::CLASSNAME = "Triangle";

REGISTER_PLUGIN(CPluginFactory, CFuncTriangle)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncTriangle
//  - Prototype : CFuncTriangle()
//
//  - Purpose   : CFuncTriangle's default constructor.
//
// -----------------------------------------------------------------------------
CFuncTriangle::CFuncTriangle()
{
	m_fMin       = 0.0f;
	m_fMax       = 1.0f;
	m_fOffset    = 0.0f;
	m_fFrequency = 1.0f;
	m_fAssimetry = 0.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncTriangle
//  - Prototype : bool Build()
//
//  - Purpose   : Builds the internal curve data.
//
// -----------------------------------------------------------------------------
bool CFuncTriangle::Build()
{
	Free();

	CSegmentSetLinear::TKey key;

	key.fTime = 0.0f;
	key.v3Value.SetX(m_fMin);

	m_crvFunction.AddKey(key);

	key.fTime = 0.5f / m_fFrequency;
	key.v3Value.SetX(m_fMax);

	m_crvFunction.AddKey(key);

	key.fTime = 1.0f / m_fFrequency;
	key.v3Value.SetX(m_fMin);

	m_crvFunction.AddKey(key);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncTriangle
//  - Prototype : bool Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CFuncTriangle::Free()
{
	return m_crvFunction.Delete();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncTriangle
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the function parameters from a script.
//
// -----------------------------------------------------------------------------
bool CFuncTriangle::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strOperation;

	pParser->ReadKeyRaw  ("#Operation", '=', &strOperation, CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Min",       '=', &m_fMin,       CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Max",       '=', &m_fMax,       CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Offset",    '=', &m_fOffset,    CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Frequency", '=', &m_fFrequency, CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Assimetry", '=', &m_fAssimetry, CParser::SEARCH_WHOLEDATA);

	SetOperation(StringToOperation(strOperation));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncTriangle
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the function parameters in a text script format.
//
// -----------------------------------------------------------------------------
bool CFuncTriangle::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	pWriter->Write("\n#Function=\"%s\"", GetClassName().c_str());
	pWriter->Write("\n{");
	pWriter->Write("\n  #Operation = %s", OperationToString(GetOperation()).c_str());
	pWriter->Write("\n  #Min       = %f", m_fMin);
	pWriter->Write("\n  #Max       = %f", m_fMax);
	pWriter->Write("\n  #Offset    = %f", m_fOffset);
	pWriter->Write("\n  #Frequency = %f", m_fFrequency);
	pWriter->Write("\n  #Assimetry = %f", m_fAssimetry);
	pWriter->Write("\n}");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncTriangle
//  - Prototype : float Evaluate(float fTime)
//
//  - Purpose   : Returns a float value as a result of evaluating the function
//                given a time value.
//
// -----------------------------------------------------------------------------
float CFuncTriangle::Evaluate(float fTime)
{
	float fPeriod       = 1.0f / m_fFrequency;
	float fInvAssimetry = 1.0f - m_fAssimetry;

	fTime = fTime + (m_fOffset * fPeriod);

	ComputeLoop(0.0f, fPeriod, &fTime);
	CVector3 v3Val;

	if(fTime > fInvAssimetry * fPeriod)
	{
		v3Val.SetX(m_fMin);
	}
	else
	{
		m_crvFunction.Evaluate(fTime / fInvAssimetry, &v3Val);
	}

	return v3Val.X();
}
