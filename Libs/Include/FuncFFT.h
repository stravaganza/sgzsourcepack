// ==[ File ]===================================================================================
//
//  -Name     : FuncFFT.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncFFT definition.
//
// =============================================================================================

/*

	08/05/2004  -  File created.


*/


#ifndef __FUNCFFT_H
#define __FUNCFFT_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CFuncFFT
//
//  - Purpose   : FFT function.
//
// ---------------------------------------------------------------------------
class CFuncFFT : public CFunction
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CFuncFFT();
	~CFuncFFT() { Free(); }

	// From CFunction

	bool		Build			() { return true; }
	bool		Free			();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII		(CWriterASCII*    pWriter);
	float		Evaluate		(float fTime);

	// CFuncFFT

	float		GetScale		() const { return m_fScale;            }
	float		GetBias			() const { return m_fBias;             }
	int			GetFFTPos		() const { return m_nFFTPos;           }
	int			GetSmoothSamples() const { return m_nNumSmoothSamples; }

	void		SetScale		(float fScale)   { m_fScale            = fScale;   }
	void		SetBias			(float fBias)    { m_fBias             = fBias;    }
	void		SetFFTPos		(int   nFFTPos)  { m_nFFTPos           = nFFTPos;  }
	void		SetSmoothSamples(int   nSamples);


private:


	float  m_fScale;
	float  m_fBias;

	int    m_nFFTPos;
	int    m_nNumSmoothSamples;
	float* m_pfSmoothSamples;

	float* m_pFFT512;
};


#endif
