#include "Plugins.h"
#include "PluginFactory.h"
#include "VarString.h"


const std::string CVarString::CLASSNAME = "String";

REGISTER_PLUGIN(CPluginFactory, CVarString)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : CVarString()
//
//  - Purpose   : CVarString's default constructor.
//
// -----------------------------------------------------------------------------
CVarString::CVarString()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : ~CVarString()
//
//  - Purpose   : CVarString's destructor.
//
// -----------------------------------------------------------------------------
CVarString::~CVarString()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : bool Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CVarString::Free()
{
	m_str.erase();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the string data. The read position is at the same
//                line where the key and value are.
//
// -----------------------------------------------------------------------------
bool CVarString::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strValue;
	pParser->ReadKeyString("#Value", '=', &strValue, CParser::SEARCH_INCURLINE);
	assert(strValue.length() > 0);

	m_str = strValue;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the txt script data.
//
// -----------------------------------------------------------------------------
bool CVarString::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	// Prevent empty field that would cause parsing errors.

	std::string strValue = m_str.empty() ? std::string("empty") : m_str;

	pWriter->Write("\n#Var #Name=\"%s\" #Class=\"%s\" #Value=\"%s\"", this->GetVarName().data(), this->GetClassName().data(), strValue.data());
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : bool Evaluate(float fTime, CValue *pValueOut)
//
//  - Purpose   : Evaluates the string given a time.
//
// -----------------------------------------------------------------------------
bool CVarString::Evaluate(float fTime, CValue *pValueOut)
{
	assert(pValueOut);
	assert(pValueOut->GetClassName() == this->GetClassName());

	((CValueString*)pValueOut)->SetString(m_str);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : bool EvaluateAsString(float fTime, std::string* pStrOut)
//
//  - Purpose   : Evaluates the var as string. In this case it already
//                is a string.
//
// -----------------------------------------------------------------------------
bool CVarString::EvaluateAsString(float fTime, std::string* pStrOut)
{
	CValueString value;

	if(!Evaluate(fTime, &value))
	{
		return false;
	}

	*pStrOut = value.GetString();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : std::string GetString() const
//
//  - Purpose   : Returns the constant string.
//
// -----------------------------------------------------------------------------
std::string CVarString::GetString() const
{
	return m_str;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarString
//  - prototype : void SetString(const std::string& str)
//
//  - Purpose   : Sets the constant string.
//
// -----------------------------------------------------------------------------
void CVarString::SetString(const std::string& str)
{
	m_str = str;

	NotifyChanged();
}
