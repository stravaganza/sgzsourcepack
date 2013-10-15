// ==[ File ]===================================================================================
//
//  -Name     : FuncRandom.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncRandom definition.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


#ifndef __FUNCRANDOM_H
#define __FUNCRANDOM_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CFuncRandom
//
//  - Purpose   : Random function.
//
// ---------------------------------------------------------------------------
class CFuncRandom : public CFunction
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CFuncRandom();
	~CFuncRandom() { Free(); }

	// From CFunction

	bool		Build			();
	bool		Free			();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII		(CWriterASCII*    pWriter);
	float		Evaluate		(float fTime);

	// CFuncRandom

	float		GetMin			()		const  { return m_fMin;         }
	float		GetMax			()		const  { return m_fMax;         }
	int         GetSeed         ()      const  { return m_nSeed;        }
	bool        GetSmooth		()		const  { return m_bSmooth;      }
	float		GetFrequency	()		const  { return m_fFrequency;   }

	void		SetMin			(float fValue) { m_fMin       = fValue; }
	void		SetMax			(float fValue) { m_fMax       = fValue; }
	void		SetSeed         (int   nValue) { m_nSeed      = nValue; }
	void        SetSmooth		(bool  bValue) { m_bSmooth    = bValue; }
	void		SetFrequency	(float fValue) { m_fFrequency = IS_ZERO(fValue) ? ALMOST_ZERO : fValue; }

private:


	float m_fMin;
	float m_fMax;
	float m_fFrequency;
	int   m_nSeed;
	bool  m_bSmooth;

	CSegmentSetLinear m_crvFunctionLinear;
	CSplineCatmullRom m_crvFunctionSmooth;
};


#endif
