#include "Plugins.h"
#include "PluginFactory.h"
#include "ResourceScene.h"


const std::string CResourceScene::CLASSNAME("Scene");

REGISTER_PLUGIN(CPluginFactory, CResourceScene)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceScene
//  - Prototype : CResourceScene()
//
//  - Purpose   : CResourceScene's default constructor.
//
// -----------------------------------------------------------------------------
CResourceScene::CResourceScene()
{
	m_pSceneView = new CSceneView;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceScene
//  - Prototype : ~CResourceScene()
//
//  - Purpose   : CResourceScene's destructor.
//
// -----------------------------------------------------------------------------
CResourceScene::~CResourceScene()
{
	Free(false);
	SAFE_DELETE(m_pSceneView);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceScene
//  - Prototype : Load(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
//
//  - Purpose   : Loads the scene specifying optionally a datafile ("" for no
//                datafile) and using the texture loading options pointed
//                by pTexOptions.
//
// -----------------------------------------------------------------------------
bool CResourceScene::Load(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	Free(true);

	if(!m_pSceneView->LoadNodeData(GetResourceFile(), GetResourceFile().substr(0, GetResourceFile().length() - 4) + ".sha", strDatFile))
	{
		return false;
	}

	if(!m_pSceneView->LoadResources(strDatFile, this->GetAdditionalPath(), pTexOptions))
	{
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceScene
//  - Prototype : bool Free(bool bFreeForReload)
//
//  - Purpose   : Frees the scene resource.
//
// -----------------------------------------------------------------------------
bool CResourceScene::Free(bool bFreeForReload)
{
	if(HasDependencies() && !bFreeForReload)
	{
		ShowDependencyWarning();
	}

	m_pSceneView->Free();
	
	return true;
}