// ==[ File ]===================================================================================
//
//  -Name     : VarFloat.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVarFloat definition.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __VARFLOAT_H
#define __VARFLOAT_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVarFloat
//
//  - Purpose   : Float variable.
//
// ---------------------------------------------------------------------------
class CVarFloat : public CVar
{


public:


	static const std::string CLASSNAME;


	class CValueFloat : public CVar::CValue
	{

	public:

		CValueFloat()  { m_fValue = 0.0f; }
		~CValueFloat() {                  }

		std::string GetClassName() const;

		// CValueFloat

		float		GetValue()				const { return m_fValue;   }
		void		SetValue(float fValue)		  { m_fValue = fValue; }

	private:

		float m_fValue;
	};

	//

	CVarFloat();
	~CVarFloat();

	std::string GetClassName() const;

	// From CVar

	bool        Free            ();
	bool		Parse			(CAdvancedParser* pParser);
	bool		WriteASCII      (CWriterASCII*    pWriter);
	bool		Evaluate        (float fTime,     CValue *pValueOut);
	bool		EvaluateAsString(float fTime,     std::string* pStrOut);
	bool        IsConstant		() const;
	bool		ScaleTime		(float fScale);

	// CVarFloat

	bool						LockConstant	(bool bLock);
	bool						ConstantLocked	() const { return m_bConstantLocked; }

	float						GetConstant		() const;
	void						SetConstant		(float fValue);

	const CCurve*				GetKeys			() const;
	void						SetKeys			(CCurve* pKeys);

	const CFunction*			GetModifier		() const;
	void						SetModifier		(CFunction* pFunction);

protected:


	bool		m_bConstantLocked;
	bool		m_bConstant;
	float		m_fConstantFloat;
	CCurve*		m_pKeys;
	CFunction*	m_pModifier;

};


#endif
