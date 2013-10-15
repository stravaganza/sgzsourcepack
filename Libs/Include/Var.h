// ==[ File ]===================================================================================
//
//  -Name     : Var.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVar definition.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __VAR_H
#define __VAR_H


// ==[ Class definitions ]======================================================================


class CDemo;
class CEffect;


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVar
//
//  - Purpose   : Base class for effect variables.
//
// ---------------------------------------------------------------------------
class CVar : public CPlugin
{


public:

	// Base class for values

	class CValue
	{

	public:

		CValue()          { }
		virtual ~CValue() { }

		// Returns the same class name as the CVar it belongs to, allowing
		// type checks on CEffect::EvaluateVar() and CVar::Evaluate().
		virtual std::string GetClassName() const = 0;
	};

	CVar();
	virtual ~CVar() { }

	static std::string	GETCLASSTYPE	();
	std::string			GetClassType	() const { return GETCLASSTYPE(); }

	// Implement in childs:

	virtual bool		Parse			(CAdvancedParser* pParser)          = 0;
	virtual bool		WriteASCII      (CWriterASCII*    pWriter)          = 0;
	virtual bool        Free            ()                                  = 0;
	virtual bool		Evaluate        (float fTime, CValue *pValueOut)    = 0;
	virtual bool		EvaluateAsString(float fTime, std::string* pStrOut) = 0;
	virtual bool        IsConstant      () const                            = 0;
	virtual bool		ScaleTime		(float fScale)                      = 0;

	// CVar

	std::string			GetVarName		() const;
	void				SetVarName		(const std::string& strName);

	std::string			GetVarDescription() const;
	void				SetVarDescription(const std::string& strDescription);

	void				ReloadFXOnChange(CEffect *pEffect);


protected:


	void				NotifyChanged	();


private:


	std::string m_strName;
	std::string m_strDescription;
	CEffect*	m_pEffectToBeReloaded;

};


#endif