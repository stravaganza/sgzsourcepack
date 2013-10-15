#include "Plugins.h"
#include "PluginFactory.h"
#include "VarCombo.h"


const std::string CVarCombo::CLASSNAME = "Combo";

REGISTER_PLUGIN(CPluginFactory, CVarCombo)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : CVarCombo()
//
//  - Purpose   : CVarCombo's default constructor.
//
// -----------------------------------------------------------------------------
CVarCombo::CVarCombo()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : ~CVarCombo()
//
//  - Purpose   : CVarCombo's destructor.
//
// -----------------------------------------------------------------------------
CVarCombo::~CVarCombo()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : bool Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CVarCombo::Free()
{
	m_vecOptions.clear();
	m_strActiveOpt.erase();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the variable data from a script file.
//
// -----------------------------------------------------------------------------
bool CVarCombo::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	std::string strValue;
	pParser->ReadKeyString("#Value", '=', &strValue, CParser::SEARCH_INCURLINE);
	assert(strValue.length() > 0);

	if(!SetOption(strValue))
	{
		LOG.Write("\nERROR - CVarCombo::Parse(): \"%s\" is an invalid value.", strValue.data());
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the variable data to the script.
//
// -----------------------------------------------------------------------------
bool CVarCombo::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	// Prevent empty field that would cause parsing errors.

	std::string strValue = m_strActiveOpt.empty() ? std::string("empty") : m_strActiveOpt;

	pWriter->Write("\n#Var #Name=\"%s\" #Class=\"%s\" #Value=\"%s\"", this->GetVarName().data(), this->GetClassName().data(), strValue.data());
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : bool Evaluate(float fTime, CValue *pValueOut)
//
//  - Purpose   : Evaluates the variable given a specific time.
//
// -----------------------------------------------------------------------------
bool CVarCombo::Evaluate(float fTime, CValue *pValueOut)
{
	assert(pValueOut);
	assert(pValueOut->GetClassName() == this->GetClassName());

	((CValueCombo*)pValueOut)->SetValue(m_strActiveOpt);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : EvaluateAsString(float fTime, std::string* pStrOut)
//
//  - Purpose   : Evaluates the variable given a specific time. Text version.
//
// -----------------------------------------------------------------------------
bool CVarCombo::EvaluateAsString(float fTime, std::string* pStrOut)
{
	CValueCombo value;

	if(!Evaluate(fTime, &value))
	{
		return false;
	}

	*pStrOut = value.GetValue();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : void AddOption(const std::string& strOption)
//
//  - Purpose   : Adds a new option to the var's possible choices.
//
// -----------------------------------------------------------------------------
void CVarCombo::AddOption(const std::string& strOption)
{
	if(m_vecOptions.empty())
	{
		m_strActiveOpt = strOption;
	}

	m_vecOptions.push_back(strOption);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : int GetOptionCount() const
//
//  - Purpose   : Returns the current number of options.
//
// -----------------------------------------------------------------------------
int CVarCombo::GetOptionCount() const
{
	return m_vecOptions.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : bool GetOption(int i, std::string* pstrOut) const
//
//  - Purpose   : Returns the i-th option.
//
// -----------------------------------------------------------------------------
bool CVarCombo::GetOption(int i, std::string* pstrOut) const
{
	if(i < 0 || i >= m_vecOptions.size())
	{
		return false;
	}

	*pstrOut = m_vecOptions[i];
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarCombo
//  - prototype : bool SetOption(const std::string& strOption)
//
//  - Purpose   : Sets the currently active option.
//
// -----------------------------------------------------------------------------
bool CVarCombo::SetOption(const std::string& strOption)
{
	bool bFound = false;
	std::vector<std::string>::iterator it;

	for(it = m_vecOptions.begin(); it != m_vecOptions.end(); ++it)
	{
		if(!stricmp(it->data(), strOption.data()))
		{
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		return false;
	}

	m_strActiveOpt = strOption;
	NotifyChanged();
	return true;
}