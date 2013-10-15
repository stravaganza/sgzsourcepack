// ==[ File ]===================================================================================
//
//  -Name     : CShaderList.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CShaderList ìmplementation.
//
// =============================================================================================

/*

	18/03/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : CShaderList()
//
//  - Purpose   : CShaderList's default constructor.
//
// -----------------------------------------------------------------------------
CShaderList::CShaderList()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : ~CShaderList()
//
//  - Purpose   : CShaderList's destructor.
//
// -----------------------------------------------------------------------------
CShaderList::~CShaderList()
{
	Free(true);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : bool ParseAndAssignShaders(CNode* pRootNode, CInput* pInput)
//
//  - Purpose   : Parses from pInput all shaders that the objects from the
//                given hierarchy need, and assigns them.
//                Shaders will be stored also in the internal vector.
//                This method is used when we have a node hierarchy but
//                no shaders have been loaded nor assigned to the objects.
//
//  - Notes     : A given shader can be shared by many different objects.
//
// -----------------------------------------------------------------------------
bool CShaderList::ParseAndAssignShaders(CNode* pRootNode, CInput* pInput)
{
	assert(pRootNode);
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;

	CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
	enumVisitor.Reset();

	pRootNode->AcceptVisitor(&enumVisitor);

	for(int i = 0; i < enumVisitor.GetNumObjects(); i++)
	{
		CObject* pObject = enumVisitor.GetObject(i);
		pObject->SetShader(NULL);

		if(pObject)
		{
			if(pObject->GetShaderName())
			{
				if(strlen(pObject->GetShaderName()) > 0)
				{
					std::vector<CShader*>::iterator it;

					// Check if already loaded

					for(it = m_vecShaders.begin(); it != m_vecShaders.end(); ++it)
					{
						assert(*it);

						if((*it)->GetName() == pObject->GetShaderName())
						{
							pObject->SetShader(*it);
						}
					}

					// If not loaded, parse and assign

					if(!pObject->GetShader())
					{
						CShader* pShader = CShader::Parse(pInput, pObject->GetShaderName());

						if(!pShader)
						{
							uError++;
						}
						else
						{
							pObject->SetShader(pShader);
							m_vecShaders.push_back(pShader);
						}
					}
				}
			}
		}
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : bool BuildListFromNodeTree(CNode* pRootNode)
//
//  - Purpose   : Builds the internal vector with the shaders of a node
//                hierarchy which has already shaders assigned to the objects.
//
// -----------------------------------------------------------------------------
bool CShaderList::BuildListFromNodeTree(CNode* pRootNode)
{
	assert(pRootNode);

	Free(true);

	CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
	enumVisitor.Reset();

	pRootNode->AcceptVisitor(&enumVisitor);

	for(int i = 0; i < enumVisitor.GetNumObjects(); i++)
	{
		CObject* pObject = enumVisitor.GetObject(i);
		CShader* pShader = pObject ? const_cast<CShader*>(pObject->GetShader()) : NULL;

		if(pShader)
		{
			std::vector<CShader*>::iterator it;
			bool bFound = false;

			for(it = m_vecShaders.begin(); it != m_vecShaders.end(); ++it)
			{
				if(*it == pShader)
				{
					bFound = true;
				}
			}

			if(!bFound)
			{
				m_vecShaders.push_back(pShader);
			}
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : bool LoadShaderResources(	const std::string& strDatfile   = "",
//											const std::string& strTexFolder = "",
//											UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL)
//
//  - Purpose   : Loads all resources (i.e. mainly textures) from the shaders
//                in the list.
//
// -----------------------------------------------------------------------------
bool CShaderList::LoadShaderResources(	const std::string& strDatfile,
										const std::string& strTexFolder,
										UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	unsigned uError = 0;
	std::vector<CShader*>::iterator it;

	for(it = m_vecShaders.begin(); it != m_vecShaders.end(); ++it)
	{
		assert(*it);

		if(!(*it)->LoadResources(strDatfile, strTexFolder, pTexOptions))
		{
			uError++;
		}
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : bool SetEvalTime(float fTime)
//
//  - Purpose   : Evaluates all shaders in the list with the given time.
//
// -----------------------------------------------------------------------------
bool CShaderList::SetEvalTime(float fTime)
{
	unsigned uError = 0;
	std::vector<CShader*>::iterator it;

	for(it = m_vecShaders.begin(); it != m_vecShaders.end(); ++it)
	{
		assert(*it);

		if(!(*it)->SetEvalTime(fTime))
		{
			uError++;
		}
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : int GetNumShaders()
//
//  - Purpose   : Returns the number of shaders in the list.
//
// -----------------------------------------------------------------------------
int CShaderList::GetNumShaders()
{
	return m_vecShaders.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : CShader* GetShader(int nIndex)
//
//  - Purpose   : Returns the i-th shader in the list.
//
// -----------------------------------------------------------------------------
CShader* CShaderList::GetShader(int nIndex)
{
	assert(nIndex > 0 && nIndex < m_vecShaders.size());

	return m_vecShaders[nIndex];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderList
//  - prototype : bool Free(bool bDeleteShaders)
//
//  - Purpose   : Deletes the shader list and (if bDeleteShaders = true) all
//                shaders themselves.
//
// -----------------------------------------------------------------------------
void CShaderList::Free(bool bDeleteShaders)
{
	if(bDeleteShaders && m_vecShaders.size() > 0)
	{
		LOG.Write("\n\nDeleting shaders:");

		std::vector<CShader*>::iterator it;

		for(it = m_vecShaders.begin(); it != m_vecShaders.end(); ++it)
		{
			assert(*it);
			LOG.Write("\n  %s", (*it)->GetName().data());
			delete *it;
		}
	}

	m_vecShaders.clear();
}
