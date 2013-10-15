#include "Plugins.h"
#include "PluginFactory.h"
#include "VarColor.h"


const std::string CVarColor::CLASSNAME = "Color";

REGISTER_PLUGIN(CPluginFactory, CVarColor)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : CVarColor()
//
//  - Purpose   : CVarColor's default constructor.
//
// -----------------------------------------------------------------------------
CVarColor::CVarColor()
{
	m_bConstant = true;
	m_pKeys     = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : ~CVarColor()
//
//  - Purpose   : CVarColor's destructor.
//
// -----------------------------------------------------------------------------
CVarColor::~CVarColor()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CVarColor::Free()
{
	SAFE_DELETE(m_pKeys);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the variable's data from a text script.
//
// -----------------------------------------------------------------------------
bool CVarColor::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strValue;
	pParser->ReadKeyRaw("#Value", '=', &strValue, CParser::SEARCH_FROMCURPOSTOLINEEND);
	assert(strValue.length() > 0);

	if(strValue[0] == '(')
	{
		m_bConstant = true;
		pParser->ReadKeyVector3("#Value", '=', &m_v3ConstantColor, CParser::SEARCH_INCURLINE);
	}
	else
	{
		assert(strValue == "curve");

		std::string strClass;
		pParser->ReadKeyString("#Curve", '=', &strClass, CParser::SEARCH_FROMCURPOS);
		m_pKeys = (CCurve*)CPluginFactory::Instance().GetNew(CCurve::GETCLASSTYPE(), strClass);

		if(!m_pKeys)
		{
			return false;
		}

		std::string strKeySetInfo;
		if(!pParser->ReadStringBetweenChars('{', '}', &strKeySetInfo, CParser::SEARCH_FROMCURPOS, true))
		{
			LOG.Write("\nERROR - CVarColor::Parse(): Error parsing keys field.");
			return false;
		}

		CInputMemory inputMem;
		inputMem.Open(const_cast<char*>(strKeySetInfo.data()), strKeySetInfo.length());
		CAdvancedParser keysParser;
		keysParser.Init(&inputMem);

		if(!m_pKeys->Parse(&keysParser))
		{
			LOG.Write("\nERROR - CVarColor::Parse(): Error parsing keys field (CEvaluatableKeySet::Parse() failed)");
			return false;
		}

		m_bConstant = false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the variable's data to a text script.
//
// -----------------------------------------------------------------------------
bool CVarColor::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	pWriter->Write("\n#Var #Name=\"%s\" #Class=\"%s\" #Value=", this->GetVarName().data(), this->GetClassName().data());

	if(m_bConstant)
	{
		pWriter->Write("(%f, %f, %f)", m_v3ConstantColor.X(), m_v3ConstantColor.X(), m_v3ConstantColor.Z());
	}
	else
	{
		pWriter->Write("curve");
		m_pKeys->WriteASCII(pWriter);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool Evaluate(float fTime, CValue *pValueOut)
//
//  - Purpose   : Evaluates the color given a specific time.
//
// -----------------------------------------------------------------------------
bool CVarColor::Evaluate(float fTime, CValue *pValueOut)
{
	assert(pValueOut);
	assert(pValueOut->GetClassName() == this->GetClassName());

	CVector3 v3Value;

	if(m_bConstant)
	{
		CVector4 v4Color;
		v4Color = m_v3ConstantColor;
		((CValueColor*)pValueOut)->SetValue(v4Color);
		return true;
	}
	else
	{
		if(!m_pKeys) return false;

		if(m_pKeys->Evaluate(fTime, &v3Value))
		{
			CVector4 v4Color;
			v4Color = v3Value;
			((CValueColor*)pValueOut)->SetValue(v4Color);
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool EvaluateAsString(float fTime, std::string* pStrOut)
//
//  - Purpose   : Evaluates the color given a specific time. Text version.
//
// -----------------------------------------------------------------------------
bool CVarColor::EvaluateAsString(float fTime, std::string* pStrOut)
{
	CValueColor value;

	if(!Evaluate(fTime, &value))
	{
		return false;
	}

	CVector4 rgb = value.GetValue();

	char buff[40];
	sprintf(buff, "(%d, %d, %d)", MYROUND(rgb.X() * 255), MYROUND(rgb.Y() * 255), MYROUND(rgb.Z() * 255));
	*pStrOut = buff;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool IsConstant() const
//
//  - Purpose   : Tells if it's a constant color (can't be animated).
//
// -----------------------------------------------------------------------------
bool CVarColor::IsConstant() const
{
	return m_bConstant;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : bool ScaleTime(float fScale)
//
//  - Purpose   : Scales the time space of the variable. Internally this means
//                scaling all animation keys.
//
// -----------------------------------------------------------------------------
bool CVarColor::ScaleTime(float fScale)
{
	if(m_pKeys)
	{
		for(int i = 0; i < m_pKeys->GetNumKeys(); i++)
		{
			float fTime;
			m_pKeys->GetKeyTime(i, &fTime);
			fTime *= fScale;
			m_pKeys->SetKeyTime(i, fTime);
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : CVector3 GetConstant() const
//
//  - Purpose   : Returns the constant color value.
//
// -----------------------------------------------------------------------------
CVector3 CVarColor::GetConstant() const
{
	return m_v3ConstantColor;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : void SetConstant(CVector3 v3Col)
//
//  - Purpose   : Sets the constant color value.
//
// -----------------------------------------------------------------------------
void CVarColor::SetConstant(CVector3 v3Col)
{
	m_bConstant       = true;
	m_v3ConstantColor = v3Col;

	NotifyChanged();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : const CCurve* GetKeys() const
//
//  - Purpose   : Gets the animated color keys.
//
// -----------------------------------------------------------------------------
const CCurve* CVarColor::GetKeys() const
{
	return m_pKeys;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarColor
//  - prototype : void SetKeys(CCurve* pKeys)
//
//  - Purpose   : Sets the animated color keys.
//
// -----------------------------------------------------------------------------
void CVarColor::SetKeys(CCurve* pKeys)
{
	if(pKeys != m_pKeys)
	{
		SAFE_DELETE(m_pKeys);
		m_pKeys     = pKeys;
	}

	m_bConstant = false;

	NotifyChanged();
}

