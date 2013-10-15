// ==[ File ]===================================================================================
//
//  -Name     : ResourceList.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResourceList implementation.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : bool AddResource(CResource* pResource)
//
//  - Purpose   : Adds a resource to the list.
//
// -----------------------------------------------------------------------------
bool CResourceList::AddResource(CResource* pResource)
{
	assert(pResource);
	m_vecResources.push_back(pResource);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : bool RemoveResource(const std::string & strName, bool bAlsoFree)
//
//  - Purpose   : Removes the given resource from the list.
//                If bAlsoFree is true, it will also free its data.
//
// -----------------------------------------------------------------------------
bool CResourceList::RemoveResource(const std::string & strName, bool bAlsoFree)
{
	VECRESOURCES::iterator it;

	for(it = m_vecResources.begin(); it != m_vecResources.end(); ++it)
	{
		assert(*it);

		if(((CResource*)(*it))->GetResourceName() == strName)
		{
			if(bAlsoFree)
			{
				SAFE_DELETE(*it);
			}

			m_vecResources.erase(it);
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : bool RemoveAll()
//
//  - Purpose   : Removes all resources from the list, also freeing all data.
//
// -----------------------------------------------------------------------------
bool CResourceList::RemoveAll()
{
	VECRESOURCES::iterator it;

	for(it = m_vecResources.begin(); it != m_vecResources.end(); ++it)
	{
		assert(*it);
		SAFE_DELETE(*it);
	}

	m_vecResources.clear();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : bool RemoveAll()
//
//  - Purpose   : Frees all resources in the list, without removing them.
//
// -----------------------------------------------------------------------------
bool CResourceList::FreeAll()
{
	VECRESOURCES::iterator it;

	for(it = m_vecResources.begin(); it != m_vecResources.end(); ++it)
	{
		CResource* pResource = (CResource*)(*it);
		assert(pResource);
		pResource->Free(false);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : int GetResourceCount() const
//
//  - Purpose   : Returns the number of resources in the list.
//
// -----------------------------------------------------------------------------
int CResourceList::GetResourceCount() const
{
	return m_vecResources.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : CResource* GetResource(int i)
//
//  - Purpose   : Returns a pointer to the i-th resource.
//
// -----------------------------------------------------------------------------
CResource* CResourceList::GetResource(int i)
{
	// TODO: improve search

	VECRESOURCES::iterator it;

	int n;

	for(it = m_vecResources.begin(), n = 0; it != m_vecResources.end(); ++it, n++)
	{
		if(n == i)
		{
			return *it;
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceList
//  - prototype : CResource* GetResource(const std::string & strName)
//
//  - Purpose   : Returns a pointer to the resource with the given name.
//
// -----------------------------------------------------------------------------
CResource* CResourceList::GetResource(const std::string & strName)
{
	// TODO: improve search

	VECRESOURCES::iterator it;

	for(it = m_vecResources.begin(); it != m_vecResources.end(); ++it)
	{
		assert(*it);

		if(((CResource*)(*it))->GetResourceName() == strName)
		{
			return *it;
		}
	}

	return NULL;
}
