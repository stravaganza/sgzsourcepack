// ==[ File ]===================================================================================
//
//  -Name     : Function.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CFunction definition.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


#ifndef __FUNCTION_H
#define __FUNCTION_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CFunction
//
//  - Purpose   : Base function class. Mainly used to serve as animated
//                effect var modifiers.
//
// ---------------------------------------------------------------------------
class CFunction : public CPlugin
{


public:


	enum EOperation
	{
		OP_ADD  = 0,
		OP_SUB  = 1,
		OP_MULT = 2
	};

	CFunction() :        m_eOperation(OP_MULT) { }
	virtual ~CFunction()                       { }

	static std::string	GETCLASSTYPE();
	std::string			GetClassType() const { return GETCLASSTYPE(); }

	EOperation		GetOperation    ()              const { return m_eOperation; }
	void			SetOperation	(EOperation op)       { m_eOperation = op;   }

	// Implement in childs:

	virtual bool	Build			()  = 0;
	virtual bool	Free			()  = 0;
	virtual bool	Parse			(CAdvancedParser* pParser) = 0;
	virtual bool	WriteASCII		(CWriterASCII*    pWriter) = 0;
	virtual float	Evaluate		(float fTime)              = 0;


protected:


	std::string OperationToString(EOperation operation)            const;
	EOperation	StringToOperation(const std::string& strOperation) const;


private:


	EOperation m_eOperation;

};


#endif
