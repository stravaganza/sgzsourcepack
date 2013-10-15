// ==[ File ]===================================================================================
//
//  -Name     : Resource.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResource implementation.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


std::string CResource::GETCLASSTYPE()
{
	static std::string CLASSTYPE = "Resource";
	return CLASSTYPE;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : std::string GetResourceName() const
//
//  - Purpose   : Returns the resource's name.
//
// -----------------------------------------------------------------------------
std::string CResource::GetResourceName() const
{
	return m_strName;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : std::string GetResourceFile() const
//
//  - Purpose   : Returns the resource's associated file.
//
// -----------------------------------------------------------------------------
std::string CResource::GetResourceFile() const
{
	return m_strFile;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : std::string GetAdditionalPath() const
//
//  - Purpose   : Returns the resource's additional path. Some resources
//                may need additional path information (f.e. scenes may need
//                a texture path to load the textures from).
//
// -----------------------------------------------------------------------------
std::string CResource::GetAdditionalPath() const
{
	return m_strAddPath;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : void SetResourceName(const std::string& strName)
//
//  - Purpose   : Sets the resource name.
//
// -----------------------------------------------------------------------------
void CResource::SetResourceName(const std::string& strName)
{
	m_strName = strName;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : void SetResourceFile(const std::string& strFile)
//
//  - Purpose   : Sets the resource file.
//
// -----------------------------------------------------------------------------
void CResource::SetResourceFile(const std::string& strFile)
{
	m_strFile = strFile;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : void SetAdditionalPath(const std::string& strAddPath)
//
//  - Purpose   : Sets the resource additional path.
//
// -----------------------------------------------------------------------------
void CResource::SetAdditionalPath(const std::string& strAddPath)
{
	m_strAddPath = strAddPath;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : bool NotifyDependency(CEffect* pEffect)
//
//  - Purpose   : Notifies a given effect will use this resource, so we can
//                know when we are allowed to delete a resource.
//
// -----------------------------------------------------------------------------
bool CResource::NotifyDependency(CEffect* pEffect)
{
	assert(pEffect);

	VECDEPENDENCIES::iterator it;

	for(it = m_vecDependencies.begin(); it < m_vecDependencies.end(); ++it)
	{
		if(*it == pEffect)
		{
			// Already notified
			return true;
		}
	}

	m_vecDependencies.push_back(pEffect);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : bool RemoveDependency(CEffect* pEffect)
//
//  - Purpose   : Notifies a given effect won't need the resource anymore.
//
// -----------------------------------------------------------------------------
bool CResource::RemoveDependency(CEffect* pEffect)
{
	// We didn't use std::map<name, effect) (that would have speed up this operation) because the
	// effect name can change at runtime, mainly from the editor.

	assert(pEffect);

	VECDEPENDENCIES::iterator it;

	for(it = m_vecDependencies.begin(); it < m_vecDependencies.end(); ++it)
	{
		if(*it == pEffect)
		{
			m_vecDependencies.erase(it);
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : bool HasDependencies() const
//
//  - Purpose   : Returns true if the resource currently has effects using it.
//
// -----------------------------------------------------------------------------
bool CResource::HasDependencies() const
{
	return m_vecDependencies.size() > 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResource
//  - prototype : bool ShowDependencyWarning() const
//
//  - Purpose   : Shows an error window telling the resource still has
//                effects referencing it when trying to delete.
//
// -----------------------------------------------------------------------------
bool CResource::ShowDependencyWarning() const
{
	if(!HasDependencies())
	{
		return false;
	}

	CLogger::NotifyWindow("Resource %s (%s) is going to be deleted/reloaded.\n There are still effects that reference it. See log for more details.",
						   m_strName.data(), m_strFile.data());

	VECDEPENDENCIES::const_iterator it;

	LOG.Write("\nWARNING - Resouce %s (%s) has still effects with its dependencies:", m_strName.data(), m_strFile.data());
	
	for(it = m_vecDependencies.begin(); it != m_vecDependencies.end(); ++it)
	{
		assert(*it);
		LOG.Write("\n  -Effect %s", (*it)->GetFXName().data());
	}

	return true;
}