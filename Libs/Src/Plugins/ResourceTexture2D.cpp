#include "Plugins.h"
#include "PluginFactory.h"
#include "ResourceTexture2D.h"


const std::string CResourceTexture2D::CLASSNAME("2D Texture");

REGISTER_PLUGIN(CPluginFactory, CResourceTexture2D)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceTexture2D
//  - Prototype : CResourceTexture2D()
//
//  - Purpose   : CResourceTexture2D's default constructor.
//
// -----------------------------------------------------------------------------
CResourceTexture2D::CResourceTexture2D()
{
	m_pTexture = new UtilGL::Texturing::CTexture2D;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceTexture2D
//  - Prototype : ~CResourceTexture2D()
//
//  - Purpose   : CResourceTexture2D's destructor
//
// -----------------------------------------------------------------------------
CResourceTexture2D::~CResourceTexture2D()
{
	Free(false);
	SAFE_DELETE(m_pTexture);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceTexture2D
//  - Prototype : bool Load(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
//
//  - Purpose   : Loads the texture from the given strDatFile (specify ""
//                if it's a stand-alone file) and using the options
//                specified by pTexOptions.
//
// -----------------------------------------------------------------------------
bool CResourceTexture2D::Load(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	Free(true);
	
	assert(m_pTexture);

	// Load Texture

	CInput* pInput = LoadFileFromDat(GetResourceFile(), false, strDatFile);
	if(!pInput)
	{
		return false;
	}

	if(!m_pTexture->Load(pInput, GetResourceFile(), GetFileExt(GetResourceFile()), false, pTexOptions))
	{
		delete pInput;
		return false;
	}

	// Load bitmap separately, which can be useful sometimes if we want to access the pixel data loaded.

	pInput->SetPosition(0, CInput::SEEKFROM_START);

	if(!m_bitmap.Load(pInput, GetFileExt(GetResourceFile())))
	{
		delete pInput;
		m_pTexture->Free();
		return false;
	}

	delete pInput;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceTexture2D
//  - Prototype : bool Free(bool bFreeForReload)
//
//  - Purpose   : Frees the texture from memory. bFreeForReload is used for the
//                editor, to avoid complaining when the resource is reloaded
//                (has to be freed first) and has effects that reference it.
//
// -----------------------------------------------------------------------------
bool CResourceTexture2D::Free(bool bFreeForReload)
{
	if(HasDependencies() && !bFreeForReload)
	{
		ShowDependencyWarning();
	}

	m_pTexture->Free();
	m_bitmap.Free();
	return true;
}