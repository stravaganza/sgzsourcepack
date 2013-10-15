// ==[ File ]===================================================================================
//
//  -Name     : VarInt.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVarInt definition.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __VARINT_H
#define __VARINT_H


// ==[ Headers ]================================================================================


#include "VarFloat.h"


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVarInt
//
//  - Purpose   : Int variable. Like floatvar but we cast to int.
//
// ---------------------------------------------------------------------------
class CVarInt : public CVarFloat
{


public:


	static const std::string CLASSNAME;


	class CValueInt : public CVar::CValue
	{

	public:

		CValueInt()  { m_nValue = 0; }
		~CValueInt() {               }

		std::string GetClassName() const { return CVarInt::CLASSNAME; }

		// CValueInt

		int			GetValue()				const { return m_nValue;   }
		void		SetValue(int nValue)		  { m_nValue = nValue; }

	private:

		int m_nValue;
	};

	//

	CVarInt();
	~CVarInt();

	std::string GetClassName () const { return CVarInt::CLASSNAME; }

	// From CVar

	bool		Evaluate        (float fTime, CValue *pValueOut);
	bool		EvaluateAsString(float fTime, std::string* pStrOut);
};


#endif
