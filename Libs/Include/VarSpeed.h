// ==[ File ]===================================================================================
//
//  -Name     : VarSpeed.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVarSpeed definition.
//
// =============================================================================================

/*

	09/05/2003  -  File created.


*/


#ifndef __VARSPEED_H
#define __VARSPEED_H


// ==[ Headers ]================================================================================


#include "VarFloat.h"


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVarSpeed
//
//  - Purpose   : Speed variable. Like float but instead of representing
//                a value it represents its speed. When we call Evaluate we
//                get a result that can be integrated to get the value we are
//                interested in, instead of the speed.
//
// ---------------------------------------------------------------------------
class CVarSpeed : public CVarFloat
{


public:


	static const std::string CLASSNAME;


	class CValueSpeed : public CVar::CValue
	{

	public:

		CValueSpeed() : m_pKeys(NULL), m_pModifier(NULL),
						m_fInternalTime(-FLOAT_MAX), m_fValue(0.0f),
						m_fIntegratedValue(0.0f) { }

		~CValueSpeed() { }

		std::string GetClassName() const { return CVarSpeed::CLASSNAME; }

		// CValueSpeed

		float		GetValue()				              const { return m_fValue;         }
		void		SetValue(float fValue)		                { m_fValue = fValue;       }

		void		SetKeys    (CCurve* pKeys)			{ m_pKeys = pKeys;         }
		void		SetModifier(CFunction*  pModifier)	{ m_pModifier = pModifier; }
		float       Integrate  (float fBegin, float fTime);

		float       ModifiedValue(float fTime, float fValue);

	private:

		float m_fValue, m_fIntegratedValue, m_fInternalTime;
		CCurve*		m_pKeys;
		CFunction*	m_pModifier;
	};

	//

	CVarSpeed();
	~CVarSpeed();

	std::string GetClassName() const { return CVarSpeed::CLASSNAME; }

	// From CVar

	bool		Evaluate        (float fTime, CValue *pValueOut);
	bool		EvaluateAsString(float fTime, std::string* pStrOut);

};


#endif
