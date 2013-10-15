#include "Plugins.h"
#include "PluginFactory.h"
#include "VarInt.h"


const std::string CVarInt::CLASSNAME = "Int";

REGISTER_PLUGIN(CPluginFactory, CVarInt)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarInt
//  - prototype : CVarInt()
//
//  - Purpose   : CVarInt's default constructor.
//
// -----------------------------------------------------------------------------
CVarInt::CVarInt()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarInt
//  - prototype : ~CVarInt()
//
//  - Purpose   : CVarInt's destructor.
//
// -----------------------------------------------------------------------------
CVarInt::~CVarInt()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarInt
//  - prototype : bool Evaluate(float fTime, CValue *pValueOut)
//
//  - Purpose   : Evaluates the int given a time value.
//
// -----------------------------------------------------------------------------
bool CVarInt::Evaluate(float fTime, CValue *pValueOut)
{
	assert(pValueOut);
	assert(pValueOut->GetClassName() == this->GetClassName());

	CVector3 v3Value;

	if(m_bConstant)
	{
		((CValueInt*)pValueOut)->SetValue(MYROUND(m_fConstantFloat));
		return true;
	}
	else
	{
		if(!m_pKeys) return false;

		if(m_pKeys->Evaluate(fTime, &v3Value))
		{
			if(m_pModifier)
			{
				float fModValue = m_pModifier->Evaluate(fTime);

				switch(m_pModifier->GetOperation())
				{
				case CFunction::OP_ADD:  v3Value.SetX(v3Value.X() + fModValue); break;
				case CFunction::OP_SUB:  v3Value.SetX(v3Value.X() - fModValue); break;
				case CFunction::OP_MULT: v3Value.SetX(v3Value.X() * fModValue); break;
				default: break;
				}
			}
			((CValueInt*)pValueOut)->SetValue(MYROUND(v3Value.X()));
			return true;
		}
	}

	((CValueInt*)pValueOut)->SetValue(0);

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarInt
//  - prototype : bool EvaluateAsString(float fTime, std::string* pStrOut)
//
//  - Purpose   : Evaluates the int given a time value. Text format version.
//
// -----------------------------------------------------------------------------
bool CVarInt::EvaluateAsString(float fTime, std::string* pStrOut)
{
	CValueInt value;

	if(!Evaluate(fTime, &value))
	{
		return false;
	}

	char buff[20];
	sprintf(buff, "%d", value.GetValue());
	*pStrOut = buff;

	return true;
}
