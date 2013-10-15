// ==[ File ]===================================================================================
//
//  -Name     : Effect.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CEffect implementation.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"
#include "VarFloat.h"


// ==[ Class implementation ]===================================================================


std::string CEffect::GETCLASSTYPE()
{
	static std::string CLASSTYPE = "Effect";
	return CLASSTYPE;
}


bool CEffect::s_ShowErrors = true;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : CEffect()
//
//  - Purpose   : CEffect's default constructor.
//
// -----------------------------------------------------------------------------
CEffect::CEffect()
{
	m_pDemo  = NULL;

	m_fBegin = m_fEnd = 0.0f;
	m_nLayer = 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : CEffect()
//
//  - Purpose   : CEffect's destructor
//
// -----------------------------------------------------------------------------
CEffect::~CEffect()
{
	MAPVARS::iterator itVar;

	for(itVar = m_mapVars.begin(); itVar != m_mapVars.end(); ++itVar)
	{
		CPluginManager::Instance().DeletePlugin(itVar->second);
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : std::string GetFXName() const
//
//  - Purpose   : Returns the effect's name.
//
// -----------------------------------------------------------------------------
std::string CEffect::GetFXName() const
{
	return m_strName;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : void SetFXName(const std::string& strName)
//
//  - Purpose   : Sets the name of the effect.
//
// -----------------------------------------------------------------------------
void CEffect::SetFXName(const std::string& strName)
{
	m_strName = strName;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool LeftScale(float fNewBegin)
//
//  - Purpose   : Sets a new begin time for the effect, without changing
//                the end.
//
// -----------------------------------------------------------------------------
bool CEffect::LeftScale(float fNewBegin)
{
	if(fNewBegin > m_fEnd)
	{
		return false;
	}

	float fScale = (m_fEnd - fNewBegin) / (m_fEnd - m_fBegin);

	// Scale all variables

	for(int i = 0; i < GetVarCount(); i++)
	{
		CVar* pVar = GetVar(i);
		pVar->ScaleTime(fScale);
	}

	m_fBegin = fNewBegin;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool RightScale(float fNewEnd)
//
//  - Purpose   : Sets a new end time for the effect, without changing
//                the end.
//
// -----------------------------------------------------------------------------
bool CEffect::RightScale(float fNewEnd)
{
	if(fNewEnd < m_fBegin)
	{
		return false;
	}

	float fScale = (fNewEnd - m_fBegin) / (m_fEnd - m_fBegin);

	// Scale all variables

	for(int i = 0; i < GetVarCount(); i++)
	{
		CVar* pVar = GetVar(i);
		pVar->ScaleTime(fScale);
	}

	m_fEnd = fNewEnd;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Parses an effect. The parser has only the text data of
//                the effect, not of the whole script.
//
// -----------------------------------------------------------------------------
bool CEffect::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	// General info

	pParser->ResetPos();

	if(!pParser->ReadKeyFloat("#Begin", '=', &m_fBegin, CParser::SEARCH_WHOLEDATA)) return false;
	if(!pParser->ReadKeyFloat("#End",   '=', &m_fEnd,   CParser::SEARCH_WHOLEDATA)) return false;
	if(!pParser->ReadKeyInt  ("#Layer", '=', &m_nLayer, CParser::SEARCH_WHOLEDATA)) return false;

	// Resources

	int nError = 0;

	pParser->ResetPos();

	while(-1 != pParser->Find("#Resource", CParser::SEARCH_FROMCURPOS))
	{
		std::string strClass, strName, strValue;

		if(!pParser->ReadKeyString("#Name",  '=', &strName,  CParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyString("#Class", '=', &strClass, CParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyString("#Value", '=', &strValue, CParser::SEARCH_INCURLINE)) nError++;

		if(nError)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing resource.");
			return false;
		}

		int i = GetResourceIndex(strName);

		if(i == -1)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Resource \"%s\" doesn't belong to this effect.", strName.data());
			return false;
		}

		std::string strClassAux;
		GetResourceClassName(i, &strClassAux);

		if(strClassAux != strClass)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing resource \"%s\". Class=%s (expected %s)", strName.data(), strClass.data(), strClassAux.data());
			return false;
		}

		if(!SetResourceValue(i, strValue))
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error setting resource \"%s\" value.", strName.data());
			return false;
		}
	}

	// Variables

	pParser->ResetPos();

	while(-1 != pParser->Find("#Var", CParser::SEARCH_FROMCURPOS))
	{
		std::string strClass, strName;

		if(!pParser->ReadKeyString("#Name",  '=', &strName,  CParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyString("#Class", '=', &strClass, CParser::SEARCH_INCURLINE)) nError++;
		
		if(nError)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing var.");
			return false;
		}

		CVar* pVar = GetVar(strName);

		if(!pVar)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing var. \"%s\" doesn't belong to this effect", strName.data());
			return false;
		}

		if(pVar->GetClassName() != strClass)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing var \"%s\". Class=%s (expected %s)", strName.data(), strClass.data(), pVar->GetClassName().data());
			return false;
		}

		if(!pVar->Parse(pParser))
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing var \"%s\" (CVar::Parse() failed).", strName.data());
			return false;
		}
	}

	// Commands

	pParser->ResetPos();

	while(-1 != pParser->Find("#Command", CParser::SEARCH_FROMCURPOS))
	{
		std::string strCommand;
		float       fTime;

		if(!pParser->ReadStringBetweenChars('<', '>', &strCommand, CParser::SEARCH_INCURLINE, true)) nError++;
		if(!pParser->ReadKeyFloat ("#Time", '=', &fTime, CParser::SEARCH_INCURLINE)) nError++;

		if(nError)
		{
			LOG.Write("\nERROR - CEffect::Parse(): Error parsing command.");
			return false;
		}

		AddCommand(fTime, strCommand);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the script data of the effect.
//
// -----------------------------------------------------------------------------
bool CEffect::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	// Description

	pWriter->Write("\n#Effect #Name=\"%s\" #Class=\"%s\"", GetFXName().data(), GetClassName().data());
	pWriter->Write("\n{");
	pWriter->Write("\n  #Begin=%f #End=%f #Layer=%u", GetBegin(), GetEnd(), GetLayer());

	// Resources

	MAPRESOURCES::iterator itRes;

	for(itRes = m_mapResources.begin(); itRes != m_mapResources.end(); ++itRes)
	{
		pWriter->Write("\n  #Resource #Name=\"%s\" #Class=\"%s\" #Value=\"%s\"",
						itRes->first.data(), itRes->second.strClass.data(), itRes->second.strValue.data());
	}
	
	// Variables

	MAPVARS::iterator itVar;

	pWriter->IncIndentation(2);
	pWriter->Write("\n");

	for(itVar = m_mapVars.begin(); itVar != m_mapVars.end(); ++itVar)
	{
		itVar->second->WriteASCII(pWriter);
	}

	pWriter->DecIndentation(2);

	// Commands

	pWriter->Write("\n");

	VECCOMMANDS::iterator itCmds;

	for(itCmds = m_vecCommands.begin(); itCmds != m_vecCommands.end(); ++itCmds)
	{
		pWriter->Write("\n  #Command #Time=%f #Send=<%s>", itCmds->fTime, itCmds->strCommand.data());
	}

	pWriter->Write("\n}");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool DefineVar(const std::string& strName, const std::string& strClassName, const std::string& strDescription)
//
//  - Purpose   : Defines a new variable for the effect.
//                strName        : Variable name.
//                strClassName   : Effect class name. This will be used to find the proper plugin.
//                strDescription : Optional description. Mainly for future editors.
//
// -----------------------------------------------------------------------------
bool CEffect::DefineVar(const std::string& strName, const std::string& strClassName, const std::string& strDescription)
{
	CVar* pNewVar = (CVar*)CPluginManager::Instance().GetNewPlugin(CVar::GETCLASSTYPE(), strClassName);

	if(!pNewVar)
	{
		return false;
	}

	pNewVar->SetVarName(strName);
	pNewVar->SetVarDescription(strDescription);
	m_mapVars.insert(MAPVARS::value_type(strName, pNewVar));
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool EvaluateVar(const std::string& strName, float fTime, CVar::CValue* pValueOut)
//
//  - Purpose   : Evaluates a variable for a given effect relative time (seconds).
//
// -----------------------------------------------------------------------------
bool CEffect::EvaluateVar(const std::string& strName, float fTime, CVar::CValue* pValueOut)
{
	assert(pValueOut);

	MAPVARS::iterator it = m_mapVars.find(strName);

	if(it != m_mapVars.end())
	{
		return it->second->Evaluate(fTime, pValueOut);
	}

	FXRuntimeError("Error evaluating var %s", strName.data());

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : CVar* GetVar(const std::string& strName)
//
//  - Purpose   : Returns the interface of a given variable.
//
// -----------------------------------------------------------------------------
CVar* CEffect::GetVar(const std::string& strName)
{
	MAPVARS::iterator it = m_mapVars.find(strName);

	if(it != m_mapVars.end())
	{
		return it->second;
	}

	//CLogger::ErrorWindow("CEffect::GetVar(%s)", strName.c_str());
	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : CVar* GetVar(int i)
//
//  - Purpose   : Returns the interface of the i-th variable.
//
// -----------------------------------------------------------------------------
CVar* CEffect::GetVar(int i)
{
	// TODO: improve search

	MAPVARS::iterator it;

	int n;

	for(it = m_mapVars.begin(), n = 0; it != m_mapVars.end(); ++it, n++)
	{
		if(n == i)
		{
			return it->second;
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : int GetVarCount() const
//
//  - Purpose   : Returns the variable count.
//
// -----------------------------------------------------------------------------
int CEffect::GetVarCount() const
{
	return m_mapVars.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool DefineResourceReference(const std::string& strName, const std::string& strClassName)
//
//  - Purpose   : Defines a new resource to be needed for this effect.
//                strName      : Name of the resource
//                strClassName : Resource class name, used to find the proper plugin.
//
// -----------------------------------------------------------------------------
bool CEffect::DefineResourceReference(const std::string& strName, const std::string& strClassName)
{
	SResourceRef newRef;

	newRef.strClass = strClassName;
	newRef.strValue = "empty";
	m_mapResources.insert(MAPRESOURCES::value_type(strName, newRef));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : int GetResourceReferenceCount() const
//
//  - Purpose   : Returns the number of resources needed by the effect.
//
// -----------------------------------------------------------------------------
int CEffect::GetResourceReferenceCount() const
{
	return m_mapResources.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : int GetResourceIndex(const std::string& strName)
//
//  - Purpose   : Returns the resource index given its name.
//
// -----------------------------------------------------------------------------
int CEffect::GetResourceIndex(const std::string& strName)
{
	// TODO: improve search

	MAPRESOURCES::iterator it;

	int n;

	for(it = m_mapResources.begin(), n = 0; it != m_mapResources.end(); ++it, n++)
	{
		if(it->first == strName)
		{
			return n;
		}
	}

	return -1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool GetResourceName(int i, std::string* pstrName)
//
//  - Purpose   : Returns the resource name given its index.
//
// -----------------------------------------------------------------------------
bool CEffect::GetResourceName(int i, std::string* pstrName)
{
	assert(pstrName);

	// TODO: improve search

	MAPRESOURCES::iterator it;

	int n;

	for(it = m_mapResources.begin(), n = 0; it != m_mapResources.end(); ++it, n++)
	{
		if(n == i)
		{
			*pstrName = it->first;
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool GetResourceClassName(int i, std::string* pstrClassName)
//
//  - Purpose   : Returns the resource class name given its index.
//
// -----------------------------------------------------------------------------
bool CEffect::GetResourceClassName(int i, std::string* pstrClassName)
{
	assert(pstrClassName);

	// TODO: improve search

	MAPRESOURCES::iterator it;

	int n;

	for(it = m_mapResources.begin(), n = 0; it != m_mapResources.end(); ++it, n++)
	{
		if(n == i)
		{
			*pstrClassName = it->second.strClass;
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool GetResourceValue(int i, std::string* pstrValue)
//
//  - Purpose   : Returns the resource value (name of the resource used) given
//                its index.
//
// -----------------------------------------------------------------------------
bool CEffect::GetResourceValue(int i, std::string* pstrValue)
{
	assert(pstrValue);

	// TODO: improve search

	MAPRESOURCES::iterator it;

	int n;

	for(it = m_mapResources.begin(), n = 0; it != m_mapResources.end(); ++it, n++)
	{
		if(n == i)
		{
			*pstrValue = it->second.strValue;
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool GetResourceValue(int i, std::string* pstrValue)
//
//  - Purpose   : Returns the resource value (name of the resource used) given
//                its index.
//
// -----------------------------------------------------------------------------
bool CEffect::SetResourceValue(int i, const std::string& strValue)
{
	// TODO: improve search

	MAPRESOURCES::iterator it;

	int n;

	for(it = m_mapResources.begin(), n = 0; it != m_mapResources.end(); ++it, n++)
	{
		if(n == i)
		{
			it->second.strValue = strValue;
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : void AddCommand(float fTime, const std::string& strCommand)
//
//  - Purpose   : Adds a command string to be executed at the given effect
//                relative time (seconds).
//
// -----------------------------------------------------------------------------
void CEffect::AddCommand(float fTime, const std::string& strCommand)
{
	SCommand command;

	command.fTime      = fTime;
	command.strCommand = strCommand;

	m_vecCommands.push_back(command);
	std::sort(m_vecCommands.begin(), m_vecCommands.end(), SCommand::Sort);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : int GetCommandCount()
//
//  - Purpose   : Returns the number of commands in the list.
//
// -----------------------------------------------------------------------------
int	CEffect::GetCommandCount()
{
	return m_vecCommands.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool GetCommand(int i, float* pfTimeOut, std::string* pstrCommandOut)
//
//  - Purpose   : Returns i-th command info (time and command string).
//
// -----------------------------------------------------------------------------
bool CEffect::GetCommand(int i, float* pfTimeOut, std::string* pstrCommandOut)
{
	if(i < 0 || i >= m_vecCommands.size())
	{
		return false;
	}

	assert(pstrCommandOut);
	assert(pfTimeOut);

	*pstrCommandOut = m_vecCommands[i].strCommand;
	*pfTimeOut      = m_vecCommands[i].fTime;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : void FXLoadError(const char* szError, ...)
//
//  - Purpose   : Notifies an effect critical error in loading time.
//                Behaviour : show error with messagebox.
//
// -----------------------------------------------------------------------------
void CEffect::FXLoadError(const char* szError, ...)
{
	char szFinalMessage[1000];
	memset(szFinalMessage, 0, 100);

	va_list arguments;

	va_start(arguments, szError);
	vsprintf(szFinalMessage, szError, arguments);
	va_end(arguments);

	if(s_ShowErrors)
	{
		CLogger::ErrorWindow("Effect %s error:\n%s", this->GetFXName().data(), szFinalMessage);
	}
	else
	{
		LOG.Write("Effect %s error:\n%s", this->GetFXName().data(), szFinalMessage);
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : void FXRuntimeError(const char* szError, ...)
//
//  - Purpose   : Notifies a non critical error in execution time.
//                Behaviour : output info through the log file.
//
// -----------------------------------------------------------------------------
void CEffect::FXRuntimeError(const char* szError, ...)
{
	char szFinalMessage[1000];
	memset(szFinalMessage, 0, 100);

	va_list arguments;

	va_start(arguments, szError);
	vsprintf(szFinalMessage, szError, arguments);
	va_end(arguments);

	LOG.Write("\nERROR - Effect %s at runtime: %s", this->GetFXName().data(), szFinalMessage);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool SortByLayerPredicate(CEffect* pfx1, CEffect* pfx2)
//
//  - Purpose   : Compares fx1 with fx2 and returns true if the layer of the
//                first is lower (and thus, has to be drawn first).
//
// -----------------------------------------------------------------------------
bool CEffect::SortByLayerPredicate(CEffect* pfx1, CEffect* pfx2)
{
	assert(pfx1 && pfx2);
	return pfx1->GetLayer() < pfx2->GetLayer();

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : bool SortByTimePredicate(CEffect* pfx1, CEffect* pfx2)
//
//  - Purpose   : Compares fx1 with fx2 and returns true if the start time of
//                the first is lower (and thus, has to be put before in the list).
//
// -----------------------------------------------------------------------------
bool CEffect::SortByTimePredicate(CEffect* pfx1, CEffect* pfx2)
{
	assert(pfx1 && pfx2);
	return pfx1->GetBegin() < pfx2->GetBegin();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEffect
//  - prototype : CResource* FindResource(CResourceList* pResourceList, const std::string& strName, const std::string& strClassName)
//
//  - Purpose   : Returns a resource interface from the list given its name and
//                class name.
//
// -----------------------------------------------------------------------------
CResource* CEffect::FindResource(CResourceList* pResourceList, const std::string& strName, const std::string& strClassName)
{
	if(!pResourceList)
	{
		FXLoadError("Error accessing resource \"%s\" (No resource list available)", strName.data());
		return NULL;
	}

	CResource*  pResource;
	std::string strResValue;

	int i;

	// Get resource index

	if(-1 == (i = GetResourceIndex(strName)))
	{
		FXLoadError("Resource \"%s\" not found", strName.data());
		return NULL;
	}

	// Get value (string)

	if(!GetResourceValue(i, &strResValue))
	{
		FXLoadError("Error getting resource \"%s\" value", strName.data());
		return NULL;
	}

	// Get resource from list
	
	if(!(pResource = pResourceList->GetResource(strResValue)))
	{
		FXLoadError("Error getting resource \"%s\" (%s) from list", strName.data(), strResValue.data());
		return NULL;
	}

	// Check validity

	if(pResource->GetClassName() != strClassName)
	{
		FXLoadError("Resource \"%s\" (%s) is not from class %s", strName.data(), strResValue.data(), strClassName);
		return NULL;
	}

	return pResource;
}