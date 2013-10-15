// ==[ File ]===================================================================================
//
//  -Name     : VarCombo.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVarCombo definition.
//
// =============================================================================================

/*

	15/05/2003  -  File created.


*/


#ifndef __VARCOMBO_H
#define __VARCOMBO_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVarCombo
//
//  - Purpose   : Combo variable. Has different options (strings) and only
//                one can be active.
//
// ---------------------------------------------------------------------------
class CVarCombo : public CVar
{


public:


	static const std::string CLASSNAME;


	class CValueCombo : public CVar::CValue
	{

	public:

		CValueCombo()  { }
		~CValueCombo() { }

		std::string GetClassName() const { return CVarCombo::CLASSNAME; }

		// CValueCombo

		std::string GetValue()                           const { return m_strValue;     }
		void		SetValue(const std::string& strValue)      { m_strValue = strValue; }

	private:

		std::string m_strValue;
	};

	//

	CVarCombo();
	~CVarCombo();

	std::string GetClassName () const { return CVarCombo::CLASSNAME; }

	// From CVar

	bool        Free            ();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII      (CWriterASCII*    pWriter);
	bool		Evaluate        (float fTime, CValue *pValueOut);
	bool		EvaluateAsString(float fTime, std::string* pStrOut);
	bool		IsConstant		()       const { return true; }
	bool		ScaleTime		(float fScale) { return true; }

	// CVarCombo

	std::string GetActiveOption () const { return m_strActiveOpt; }

	void		AddOption		(const std::string& strOption);
	int         GetOptionCount  () const;
	bool		GetOption       (int i, std::string* pstrOut) const;
	bool        SetOption       (const std::string& strOption);


private:


	std::vector<std::string> m_vecOptions;
	std::string              m_strActiveOpt;

};


#endif
