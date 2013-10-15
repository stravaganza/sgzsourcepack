// ==[ File ]===================================================================================
//
//  -Name     : FuncFFT.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncFFT implementation
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "FuncFFT.h"


const std::string CFuncFFT::CLASSNAME = "FFT";

REGISTER_PLUGIN(CPluginFactory, CFuncFFT)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncFFT
//  - Prototype : CFuncFFT()
//
//  - Purpose   : CFuncFFT's default constructor.
//
// -----------------------------------------------------------------------------
CFuncFFT::CFuncFFT()
{
	m_nFFTPos = 0;
	m_fScale  = 1.0f;
	m_fBias   = 0.0f;

	m_nNumSmoothSamples = 0;
	m_pfSmoothSamples   = NULL;

	m_pFFT512 = new float[512];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncFFT
//  - Prototype : bool Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CFuncFFT::Free()
{
	SAFE_DELETE_A(m_pFFT512);
	SAFE_DELETE_A(m_pfSmoothSamples);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncFFT
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the function parameters from a script.
//
// -----------------------------------------------------------------------------
bool CFuncFFT::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strOperation;

	pParser->ReadKeyRaw  ("#Operation",     '=', &strOperation,        CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyInt  ("#FFTPos",        '=', &m_nFFTPos,           CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Scale",         '=', &m_fScale,            CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyFloat("#Bias",          '=', &m_fBias,             CParser::SEARCH_WHOLEDATA);
	pParser->ReadKeyInt  ("#SmoothSamples", '=', &m_nNumSmoothSamples, CParser::SEARCH_WHOLEDATA);

	if(m_nNumSmoothSamples > 0)
	{
		m_pfSmoothSamples = new float[m_nNumSmoothSamples];

		for(int i = 0; i < m_nNumSmoothSamples; i++)
		{
			m_pfSmoothSamples[i] = -1.0f;
		}
	}

	SetOperation(StringToOperation(strOperation));
	m_pFFT512 = new float[512];

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncFFT
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the function parameters in a text script format.
//
// -----------------------------------------------------------------------------
bool CFuncFFT::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	pWriter->Write("\n#Function=\"%s\"", GetClassName().c_str());
	pWriter->Write("\n{");
	pWriter->Write("\n  #Operation     = %s", OperationToString(GetOperation()).c_str());
	pWriter->Write("\n  #FFTPos        = %d", m_nFFTPos);
	pWriter->Write("\n  #Scale         = %f", m_fScale);
	pWriter->Write("\n  #Bias          = %f", m_fBias);
	pWriter->Write("\n  #SmoothSamples = %d", m_nNumSmoothSamples);
	pWriter->Write("\n}");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncFFT
//  - Prototype : float Evaluate(float fTime)
//
//  - Purpose   : Returns a float value as a result of evaluating the function
//                given a time value.
//
// -----------------------------------------------------------------------------
float CFuncFFT::Evaluate(float fTime)
{
	if(CSoundSystem::Instance().GetNumSounds() < 1)
	{
		return 0.0f;
	}

	CSound* pSound = CSoundSystem::Instance().GetSound(0); // There should be only 1 sound

	int i = m_nFFTPos;

	if(i < 0)   i = 0;
	if(i > 511) i = 511;

	pSound->GetFFT512(m_pFFT512);
	float fValue = (m_pFFT512[i] * m_fScale) + m_fBias;

	// Smooth

	if(m_nNumSmoothSamples > 0 && m_pfSmoothSamples)
	{
		int n;

		for(n = 0; n < m_nNumSmoothSamples - 1; n++)
		{
			m_pfSmoothSamples[n] = m_pfSmoothSamples[n + 1];
		}

		m_pfSmoothSamples[m_nNumSmoothSamples - 1] = fValue;

		int   nValidSamples = 0;
		float fAccumSample  = 0.0f;

		for(n = 0; n < m_nNumSmoothSamples; n++)
		{
			if(!ARE_EQUAL(m_pfSmoothSamples[n], -1.0f))
			{
				fAccumSample += m_pfSmoothSamples[n];
				nValidSamples++;
			}
		}

		if(nValidSamples > 0)
		{
			fValue = fAccumSample / (float)nValidSamples;
		}
	}

	return fValue;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CFuncFFT
//  - Prototype : void SetSmoothSamples(int nSamples)
//
//  - Purpose   : Tells how many samples to use for averaging.
//
// -----------------------------------------------------------------------------
void CFuncFFT::SetSmoothSamples(int nSamples)
{
	m_nNumSmoothSamples = nSamples;
	SAFE_DELETE_A(m_pfSmoothSamples);

	if(nSamples > 0)
	{
		m_pfSmoothSamples = new float[m_nNumSmoothSamples];
	}
	else
	{
		m_nNumSmoothSamples = 0;
	}
}
