// ==[ File ]===================================================================================
//
//  -Name     : VarColor.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVarColor definition.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __VARCOLOR_H
#define __VARCOLOR_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVarColor
//
//  - Purpose   : Color variable.
//
// ---------------------------------------------------------------------------
class CVarColor : public CVar
{


public:


	static const std::string CLASSNAME;


	class CValueColor : public CVar::CValue
	{

	public:

		CValueColor()  { }
		~CValueColor() { }

		std::string GetClassName() const { return CVarColor::CLASSNAME; }

		// CValueColor

		CVector4	GetValue()                        const { return m_v4Value;    }
		void		SetValue(const CVector4& v4Value)       { m_v4Value = v4Value; }

	private:

		CVector4 m_v4Value;
	};

	//

	CVarColor();
	~CVarColor();

	std::string GetClassName () const { return CVarColor::CLASSNAME; }

	// From CVar

	bool        Free            ();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII      (CWriterASCII*    pWriter);
	bool		Evaluate        (float fTime, CValue *pValueOut);
	bool		EvaluateAsString(float fTime, std::string* pStrOut);
	bool		IsConstant		() const;
	bool		ScaleTime		(float fScale);

	// CVarColor

	CVector3		GetConstant	() const;
	void			SetConstant	(CVector3 v3Col);
	const CCurve*	GetKeys		() const;
	void			SetKeys		(CCurve* pKeys);


private:


	bool	 m_bConstant;
	CVector3 m_v3ConstantColor;
	CCurve*	 m_pKeys;

};


#endif
