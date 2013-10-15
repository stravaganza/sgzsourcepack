// ==[ File ]===================================================================================
//
//  -Name     : FuncConstant.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFuncConstant definition.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


#ifndef __FUNCONSTANT_H
#define __FUNCONSTANT_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CFuncConstant
//
//  - Purpose   : Constant function.
//
// ---------------------------------------------------------------------------
class CFuncConstant : public CFunction
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CFuncConstant();
	~CFuncConstant() { Free(); }

	// From CFunction

	bool		Build			() { return true; }
	bool		Free			() { return true; }
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII		(CWriterASCII*    pWriter);
	float		Evaluate		(float fTime);

	// CFuncConstant

	float		GetValue		()		const  { return m_fValue;	}

	void		SetValue		(float fValue) { m_fValue = fValue;	}


private:


	float m_fValue;

};


#endif
