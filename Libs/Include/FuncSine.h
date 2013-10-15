// ==[ File ]===================================================================================
//
//  -Name     : FuncSine.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncSine definition.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


#ifndef __FUNCSINE_H
#define __FUNCSINE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CFuncSine
//
//  - Purpose   : Sine function.
//
// ---------------------------------------------------------------------------
class CFuncSine : public CFunction
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CFuncSine();
	~CFuncSine() { Free(); }

	// From CFunction

	bool		Build			();
	bool		Free			();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII		(CWriterASCII*    pWriter);
	float		Evaluate		(float fTime);

	// CFuncSine

	float		GetMin			()		const  { return m_fMin;         }
	float		GetMax			()		const  { return m_fMax;         }
	float		GetOffset		()		const  { return m_fOffset;      }
	float		GetAssimetry	()		const  { return m_fAssimetry;   }
	float		GetFrequency	()		const  { return m_fFrequency;   }

	void		SetMin			(float fValue) { m_fMin       = fValue; }
	void		SetMax			(float fValue) { m_fMax       = fValue; }
	void		SetOffset		(float fValue) { m_fOffset    = fValue; }
	void		SetAssimetry	(float fValue) { m_fAssimetry = fValue; }
	void		SetFrequency	(float fValue) { m_fFrequency = IS_ZERO(fValue) ? ALMOST_ZERO : fValue; }


private:


	float m_fMin;
	float m_fMax;
	float m_fOffset;
	float m_fFrequency;
	float m_fAssimetry;

};


#endif
