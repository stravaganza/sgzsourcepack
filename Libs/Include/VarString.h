// ==[ File ]===================================================================================
//
//  -Name     : VarString.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVarString definition.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __VARSTRING_H
#define __VARSTRING_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVarString
//
//  - Purpose   : String variable.
//
// ---------------------------------------------------------------------------
class CVarString : public CVar
{


public:


	static const std::string CLASSNAME;


	class CValueString : public CVar::CValue
	{

	public:

		CValueString()  { }
		~CValueString() { }

		std::string GetClassName() const { return CVarString::CLASSNAME; }

		// CValueString

		std::string GetString()						const { return m_str;	}
		void		SetString(const std::string& str)	  { m_str = str;	}

	private:

		std::string m_str;
	};

	//

	CVarString();
	~CVarString();

	std::string GetClassName () const { return CVarString::CLASSNAME; }

	// From CVar

	bool        Free            ();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII      (CWriterASCII*    pWriter);
	bool		Evaluate        (float fTime, CValue *pValueOut);
	bool		EvaluateAsString(float fTime, std::string* pStrOut);
	bool		IsConstant		()            const { return true; }
	bool		ScaleTime		(float fScale)      { return true; }

	// CVarString

	std::string GetString		() const;
	void		SetString		(const std::string& str);


private:


	std::string m_str;

};


#endif
