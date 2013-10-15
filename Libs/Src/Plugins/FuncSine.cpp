// ==[ File ]===================================================================================
//
//  -Name     : FuncSine.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncSine implementation
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "FuncSine.h"


const std::string CFuncSine::CLASSNAME = "Sine";

REGISTER_PLUGIN(CPluginFactory, CFuncSine)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncSine
//  - Prototype : CFuncSine()
//
//  - Purpose   : CFuncSine's default constructor.
//
// -----------------------------------------------------------------------------
CFuncSine::CFuncSine()
{
	m_fMin       = 0.0f;
	m_fMax       = 1.0f;
	m_fOffset    = 0.0f;
	m_fFrequency = 1.0f;
	m_fAssimetry = 0.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncSine
//  - Prototype : bool Build()
//
//  - Purpose   : Builds the internal curve data. Does nothing here.
//
// -----------------------------------------------------------------------------
bool CFuncSine::Build()
{
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncSine
//  - Prototype : bool Free()
//
//  - Purpose   : Frees all internal data. Does nothing here.
//
// -----------------------------------------------------------------------------
bool CFuncSine::Free()
{
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncSine
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the function parameters from a script.
//
// -----------------------------------------------------------------------------
bool CFuncSine::Parse(CAdvancedParser* pParser)
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
//  - Class     : CFuncSine
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the function parameters in a text script format.
//
// -----------------------------------------------------------------------------
bool CFuncSine::WriteASCII(CWriterASCII* pWriter)
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
//  - Class     : CFuncSine
//  - Prototype : float Evaluate(float fTime)
//
//  - Purpose   : Returns a float value as a result of evaluating the function
//                given a time value.
//
// -----------------------------------------------------------------------------
float CFuncSine::Evaluate(float fTime)
{
	float fPeriod       = 1.0f / m_fFrequency;
	float fInvAssimetry = 1.0f - m_fAssimetry;
	float fValue;

	fTime = fTime + (m_fOffset * fPeriod);

	ComputeLoop(0.0f, fPeriod, &fTime);
	CVector3 v3Val;

	if(fTime > fInvAssimetry * fPeriod)
	{
		fValue = m_fMin;
	}
	else
	{
		fTime /= (fInvAssimetry * fPeriod);

		fValue = sinf(-HALFPI + (fTime * TWOPI));
		fValue = (fValue + 1.0f) * 0.5f;              // range 0-1
		fValue = ComputeLERP(m_fMin, m_fMax, fValue); // range min-max
	}

	return fValue;
}
