#include "Plugins.h"
#include "PluginFactory.h"
#include "VarFloat.h"


const std::string CVarFloat::CLASSNAME = "Float";

REGISTER_PLUGIN(CPluginFactory, CVarFloat)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CValueFloat
//  - prototype : std::string GetClassName() const
//
//  - Purpose   : Returns the CVarFloat::CValueFloat class name.
//
// -----------------------------------------------------------------------------
std::string CVarFloat::CValueFloat::GetClassName()	const
{
	return CVarFloat::CLASSNAME;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : CVarFloat()
//
//  - Purpose   : CVarFloat's default constructor.
//
// -----------------------------------------------------------------------------
CVarFloat::CVarFloat()
{
	m_bConstantLocked = false;
	m_bConstant       = true;
	m_fConstantFloat  = 0.0f;
	m_pKeys           = NULL;
	m_pModifier		  = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : ~CVarFloat()
//
//  - Purpose   : CVarFloat's destructor.
//
// -----------------------------------------------------------------------------
CVarFloat::~CVarFloat()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : std::string GetClassName() const
//
//  - Purpose   : Returns the CVarFloat class name.
//
// -----------------------------------------------------------------------------
std::string CVarFloat::GetClassName() const
{
	return CVarFloat::CLASSNAME;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : Free()
//
//  - Purpose   : Frees all internal data.
//
// -----------------------------------------------------------------------------
bool CVarFloat::Free()
{
	SAFE_DELETE(m_pKeys);
	SAFE_DELETE(m_pModifier);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses the variable from the text script.
//
// -----------------------------------------------------------------------------
bool CVarFloat::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Free();

	std::string strValue;
	pParser->ReadKeyRaw("#Value", '=', &strValue, CParser::SEARCH_FROMCURPOSTOLINEEND);
	assert(strValue.length() > 0);

	if(isdigit(strValue[0]) || strValue[0] == '-')
	{
		m_bConstant = true;
		pParser->ReadKeyFloat("#Value", '=', &m_fConstantFloat, CParser::SEARCH_INCURLINE);
	}
	else
	{
		if(m_bConstantLocked)
		{
			LOG.Write("\nERROR - CVarFloat::Parse(): Value must be a constant.");
			return false;
		}

		assert(strValue == "curve");

		CInputMemory    inputField,  inputSubField;
		CAdvancedParser parserField, parserSubField;
		std::string     stringField, stringSubField;
		std::string     strAux;

		// Old versions don't have big field with curve+modifier inside, only the curve and no { } field

		int nPos = pParser->GetInputInterface()->GetPosition();

		pParser->JumpSpacing();
		pParser->ReadStringUntilSpacing(&strAux);
		pParser->GetInputInterface()->SetPosition(nPos, CInput::SEEKFROM_START);

		if(strAux[0] != '{')
		{
			// Old version:

			// Read curve class

			if(!pParser->ReadKeyString("#Curve", '=', &strAux, CParser::SEARCH_FROMCURPOS))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): No curve found.");
				return false;
			}

			// Instance depending on curve class

			m_pKeys = (CCurve*)CPluginFactory::Instance().GetNew(CCurve::GETCLASSTYPE(), strAux);

			// Read curve parameters field

			if(!pParser->ReadStringBetweenChars('{', '}', &stringField, CParser::SEARCH_FROMCURPOS, true))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): Error finding curve field.");
				return false;
			}

			inputField.Open(const_cast<char*>(stringField.data()), stringField.length());
			parserField.Init(&inputField);

			if(!m_pKeys->Parse(&parserField))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): Error parsing keys field (CEvaluatableKeySet::Parse() failed)");
				return false;
			}
		}
		else
		{
			// New version:

			// Read big field { }, which includes both curve and modifier

			if(!pParser->ReadStringBetweenChars('{', '}', &stringField, CParser::SEARCH_FROMCURPOS, true))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): Error finding curve+modifier field.");
				return false;
			}

			// Search for the curve ////////////////

			inputField.Open(const_cast<char*>(stringField.data()), stringField.length());
			parserField.Init(&inputField);

			if(!parserField.ReadKeyString("#Curve", '=', &strAux, CParser::SEARCH_WHOLEDATA))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): No curve specified.");
				return false;
			}

			// Instance depending on curve class

			m_pKeys = (CCurve*)CPluginFactory::Instance().GetNew(CCurve::GETCLASSTYPE(), strAux);

			if(!m_pKeys) return false;

			// Read curve subfield

			if(!parserField.ReadStringBetweenChars('{', '}', &stringSubField, CParser::SEARCH_FROMCURPOS, true))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): Error finding curve subfield.");
				return false;
			}

			inputSubField.Open(const_cast<char*>(stringSubField.data()), stringSubField.length());
			parserSubField.Init(&inputSubField);

			if(!m_pKeys->Parse(&parserSubField))
			{
				LOG.Write("\nERROR - CVarFloat::Parse(): Error parsing keys field (CEvaluatableKeySet::Parse() failed)");
				return false;
			}

			// Search for the modifier ////////////////

			if(parserField.ReadKeyString("#Function", '=', &strAux, CParser::SEARCH_WHOLEDATA))
			{
				// Instance depending on function class

				m_pModifier = (CFunction*)CPluginFactory::Instance().GetNew(CFunction::GETCLASSTYPE(), strAux);

				if(!m_pModifier) return false;

				// Read modifier subfield

				if(!parserField.ReadStringBetweenChars('{', '}', &stringSubField, CParser::SEARCH_FROMCURPOS, true))
				{
					LOG.Write("\nERROR - CVarFloat::Parse(): Error finding modifier subfield.");
					return false;
				}

				inputSubField.Close();
				parserSubField.Close();

				inputSubField.Open(const_cast<char*>(stringSubField.data()), stringSubField.length());
				parserSubField.Init(&inputSubField);

				if(!m_pModifier->Parse(&parserSubField))
				{
					LOG.Write("\nERROR - CVarFloat::Parse(): Error parsing modifier field (CFunction::Parse() failed)");
					return false;
				}

				if(!m_pModifier->Build())
				{
					LOG.Write("\nERROR - CVarFloat::Parse(): Error building modifier CFunction::Build() failed)");
					return false;
				}
			}
		}

		m_bConstant = false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the variable data to a script file.
//
// -----------------------------------------------------------------------------
bool CVarFloat::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	pWriter->Write("\n#Var #Name=\"%s\" #Class=\"%s\" #Value=", this->GetVarName().data(), this->GetClassName().data());

	if(m_bConstant)
	{
		pWriter->Write("%f", m_fConstantFloat);
	}
	else
	{
		pWriter->Write("curve");
		pWriter->Write("\n{");
		pWriter->IncIndentation(2);

		m_pKeys->WriteASCII(pWriter);
		if(m_pModifier)
		{
			m_pModifier->WriteASCII(pWriter);
		}

		pWriter->DecIndentation(2);
		pWriter->Write("\n}");
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool Evaluate(float fTime, CValue *pValueOut)
//
//  - Purpose   : Evaluates the variable given a specific time.
//
// -----------------------------------------------------------------------------
bool CVarFloat::Evaluate(float fTime, CValue *pValueOut)
{
	assert(pValueOut);
	assert(pValueOut->GetClassName() == this->GetClassName());

	CVector3 v3Value;

	if(m_bConstant)
	{
		((CValueFloat*)pValueOut)->SetValue(m_fConstantFloat);
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
			((CValueFloat*)pValueOut)->SetValue(v3Value.X());
			return true;
		}
	}

	float fEndTime;
	m_pKeys->GetEndTime(&fEndTime);

	if(fTime > fEndTime)
	{
		m_pKeys->GetKeyValue(m_pKeys->GetNumKeys() - 1, &v3Value);
	}
	else
	{
		m_pKeys->GetKeyValue(0, &v3Value);
	}

	((CValueFloat*)pValueOut)->SetValue(v3Value.X());

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool EvaluateAsString(float fTime, std::string* pStrOut)
//
//  - Purpose   : Evaluates the variable given a specific time. Text version.
//
// -----------------------------------------------------------------------------
bool CVarFloat::EvaluateAsString(float fTime, std::string* pStrOut)
{
	CValueFloat value;

	if(!Evaluate(fTime, &value))
	{
		return false;
	}

	char buff[20];
	sprintf(buff, "%.4f", value.GetValue());
	*pStrOut = buff;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool LockConstant(bool bLock)
//
//  - Purpose   : Prevents the variable from being animated.
//
// -----------------------------------------------------------------------------
bool CVarFloat::LockConstant(bool bLock)
{
	m_bConstantLocked = bLock;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool IsConstant() const
//
//  - Purpose   : Tells if the variable is constant (can't be animated).
//
// -----------------------------------------------------------------------------
bool CVarFloat::IsConstant() const
{
	return m_bConstant;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : bool ScaleTime(float fScale)
//
//  - Purpose   : Scales the time space of the variable. Internally this means
//                scaling all key times.
//                This method is called when an effect time segment needs to
//                be dynamically changed (i.e. from the editor).
//
// -----------------------------------------------------------------------------
bool CVarFloat::ScaleTime(float fScale)
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
//  - Class     : CVarFloat
//  - prototype : float GetConstant() const
//
//  - Purpose   : Gets the variable's constant value.
//
// -----------------------------------------------------------------------------
float CVarFloat::GetConstant() const
{
	return m_fConstantFloat;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : void SetConstant(float fValue)
//
//  - Purpose   : Sets the variable's constant value.
//
// -----------------------------------------------------------------------------
void CVarFloat::SetConstant(float fValue)
{
	m_bConstant      = true;
	m_fConstantFloat = fValue;

	NotifyChanged();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : const CCurve* GetKeys() const
//
//  - Purpose   : Gets the variable animation keys.
//
// -----------------------------------------------------------------------------
const CCurve* CVarFloat::GetKeys() const
{
	return m_pKeys;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : void SetKeys(CCurve* pKeys)
//
//  - Purpose   : Sets the variable animation keys.
//
// -----------------------------------------------------------------------------
void CVarFloat::SetKeys(CCurve* pKeys)
{
	if(m_bConstantLocked)
	{
		return;
	}

	if(pKeys != m_pKeys)
	{
		SAFE_DELETE(m_pKeys);
		m_pKeys     = pKeys;
	}

	m_bConstant = false;

	NotifyChanged();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : const CFunction* GetModifier() const
//
//  - Purpose   : Returns the variable modifier interface (curve modifier).
//
// -----------------------------------------------------------------------------
const CFunction* CVarFloat::GetModifier() const
{
	return m_pModifier;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVarFloat
//  - prototype : void SetModifier(CFunction* pModifier)
//
//  - Purpose   : Sets the variable's curve modifier.
//
// -----------------------------------------------------------------------------
void CVarFloat::SetModifier(CFunction* pModifier)
{
	if(m_bConstantLocked)
	{
		return;
	}

	if(pModifier != m_pModifier)
	{
		SAFE_DELETE(m_pModifier);
		m_pModifier = pModifier;
	}

	m_bConstant = false;

	NotifyChanged();
}

