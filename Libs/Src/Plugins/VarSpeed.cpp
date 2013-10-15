#include "Plugins.h"
#include "PluginFactory.h"
#include "VarSpeed.h"


const std::string CVarSpeed::CLASSNAME = "Speed";

REGISTER_PLUGIN(CPluginFactory, CVarSpeed)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CValueSpeed
//  - prototype : float Integrate(float fBegin, float fTime)
//
//  - Purpose   : Integrates the speed using fixed time steps.
//
// -----------------------------------------------------------------------------
float CVarSpeed::CValueSpeed::Integrate(float fBegin, float fTime)
{
	if(!m_pKeys)
	{
		return (fTime - fBegin) * ModifiedValue(fTime, m_fValue);
	}

	if(MYFABSF(fBegin - fTime) < MYFABSF(m_fInternalTime - fTime))
	{
		m_fInternalTime    = fBegin;
		m_fIntegratedValue = 0.0f;
	}

	float fDT = 0.02f; // dt when integrating, we assure 1/dt minimum updates per second

	if(fTime < m_fInternalTime)
	{
		fDT = -fDT;
	}

	bool bFinished = false;

	while(!bFinished)
	{
		// Check if this is the last integration step

		if(MYFABSF(fTime - m_fInternalTime) < MYFABSF(fDT))
		{
			fDT = fTime - m_fInternalTime;
			bFinished = true;
		}

		m_fInternalTime += fDT;

		CVector3 v3Value;

		if(m_pKeys->Evaluate(m_fInternalTime, &v3Value))
		{
			m_fIntegratedValue += ModifiedValue(m_fInternalTime, v3Value.X()) * MYFABSF(fDT);
		}
	}

	return m_fIntegratedValue;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CValueSpeed
//  - prototype : float ModifiedValue(float fTime, float fValue)
//
//  - Purpose   : Returns the value applying the modifier.
//
// -----------------------------------------------------------------------------
float CVarSpeed::CValueSpeed::ModifiedValue(float fTime, float fValue)
{
	if(m_pModifier)
	{
		float fModValue = m_pModifier->Evaluate(fTime);

		switch(m_pModifier->GetOperation())
		{
		case CFunction::OP_ADD:  return fValue + fModValue; break;
		case CFunction::OP_SUB:  return fValue - fModValue; break;
		case CFunction::OP_MULT: return fValue * fModValue; break;
		default: break;
		}
	}

	return fValue;
}

//////////////////////////////////////////////////////////////////////////////////////////

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarSpeed
//  - prototype : CVarSpeed()
//
//  - Purpose   : CVarSpeed default constructor.
//
// -----------------------------------------------------------------------------
CVarSpeed::CVarSpeed()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarSpeed
//  - prototype : ~CVarSpeed()
//
//  - Purpose   : CVarSpeed destructor.
//
// -----------------------------------------------------------------------------
CVarSpeed::~CVarSpeed()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarSpeed
//  - prototype : bool Evaluate(float fTime, CVar::CValue *pValueOut)
//
//  - Purpose   : Evaluates the speed value given a time.
//
// -----------------------------------------------------------------------------
bool CVarSpeed::Evaluate(float fTime, CVar::CValue *pValueOut)
{
	assert(pValueOut);
	assert(pValueOut->GetClassName() == this->GetClassName());

	CVector3 v3Value;

	if(m_bConstant)
	{
		((CValueSpeed*)pValueOut)->SetValue(m_fConstantFloat);
		((CValueSpeed*)pValueOut)->SetKeys (NULL);
		return true;
	}
	else
	{
		if(!m_pKeys) return false;

		if(m_pKeys->Evaluate(fTime, &v3Value))
		{
			((CValueSpeed*)pValueOut)->SetValue   (v3Value.X());
			((CValueSpeed*)pValueOut)->SetKeys    (m_pKeys);
			((CValueSpeed*)pValueOut)->SetModifier(m_pModifier);
			return true;
		}
	}

	((CValueSpeed*)pValueOut)->SetValue(0.0f);

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarSpeed
//  - prototype : EvaluateAsString(float fTime, std::string* pStrOut)
//
//  - Purpose   : Evaluates the speed value given a time but in text format.
//
// -----------------------------------------------------------------------------
bool CVarSpeed::EvaluateAsString(float fTime, std::string* pStrOut)
{
	CValueSpeed value;

	if(!Evaluate(fTime, &value))
	{
		return false;
	}

	char buff[20];
	sprintf(buff, "%.4f", value.GetValue());
	*pStrOut = buff;

	return true;
}
