// ==[ File ]===================================================================================
//
//  -Name     : FuncRandom.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncRandom implementation
//
// =============================================================================================

/*

	21/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "FuncRandom.h"


const std::string CFuncRandom::CLASSNAME = "Random";

REGISTER_PLUGIN(CPluginFactory, CFuncRandom)


// ==[ Class implementation ]===================================================================


#define LOOPLENGTH 20.0f


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncRandom
//  - Prototype : CFuncRandom()
//
//  - Purpose   : CFuncRandom's default constructor.
//
// -----------------------------------------------------------------------------
CFuncRandom::CFuncRandom()
{
	m_fMin       = 0.0f;
	m_fMax       = 1.0f;
	m_fFrequency = 1.0f;
	m_nSeed      = 0;
	m_bSmooth    = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncRandom
//  - Prototype : bool Build()
//
//  - Purpose   : Builds the internal curve data.
//
// -----------------------------------------------------------------------------
bool CFuncRandom::Build()
{
	Free();

	srand(m_nSeed);

	// Create 20 seconds loop, that would be enough

	int nNumKeys = m_fFrequency * LOOPLENGTH;

	CSplineCatmullRom::TKey keySmooth;
	CSegmentSetLinear::TKey keyLinear;

	for(int i = 0; i < nNumKeys; i++)
	{
		keySmooth.fTime = i * (1.0f / m_fFrequency);
		keySmooth.v3Value.Set(ComputeRand(m_fMin, m_fMax), 0.0f, 0.0f);

		m_crvFunctionSmooth.AddKey(keySmooth);

		keyLinear.fTime   = keySmooth.fTime;
		keyLinear.v3Value = keySmooth.v3Value;

		m_crvFunctionLinear.AddKey(keyLinear);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncRandom
//  - Prototype : bool Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CFuncRandom::Free()
{
	m_crvFunctionLinear.Delete();
	m_crvFunctionSmooth.Delete();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncRandom
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the function parameters from a script.
//
// -----------------------------------------------------------------------------
bool CFuncRandom::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strOperation;

	pParser->ReadKeyRaw  ("#Operation", '=', &strOperation, CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Min",       '=', &m_fMin,       CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Max",       '=', &m_fMax,       CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Frequency", '=', &m_fFrequency, CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyInt  ("#Seed",      '=', &m_nSeed,      CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyBool ("#Smooth",    '=', &m_bSmooth,    CParser::SEARCH_WHOLEDATA);

	SetOperation(StringToOperation(strOperation));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncRandom
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the function parameters in a text script format.
//
// -----------------------------------------------------------------------------
bool CFuncRandom::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	pWriter->Write("\n#Function=\"%s\"", GetClassName().c_str());
	pWriter->Write("\n{");
	pWriter->Write("\n  #Operation = %s", OperationToString(GetOperation()).c_str());
	pWriter->Write("\n  #Min       = %f", m_fMin);
	pWriter->Write("\n  #Max       = %f", m_fMax);
	pWriter->Write("\n  #Frequency = %f", m_fFrequency);
	pWriter->Write("\n  #Seed      = %d", m_nSeed);
	pWriter->Write("\n  #Smooth    = %s", m_bSmooth ? "True" : "False");
	pWriter->Write("\n}");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncRandom
//  - Prototype : float Evaluate(float fTime)
//
//  - Purpose   : Returns a float value as a result of evaluating the function
//                given a time value.
//
// -----------------------------------------------------------------------------
float CFuncRandom::Evaluate(float fTime)
{
	ComputeLoop(0.0f, LOOPLENGTH - (1.0f / m_fFrequency), &fTime);
	CVector3 v3Val;

	CCurve* pCurve;
	
	if(m_bSmooth) pCurve = &m_crvFunctionSmooth; else pCurve = &m_crvFunctionLinear;
	pCurve->Evaluate(fTime, &v3Val);

	return v3Val.X();
}
