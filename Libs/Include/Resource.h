// ==[ File ]===================================================================================
//
//  -Name     : Resource.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResource definition.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


#ifndef __RESOURCE_H
#define __RESOURCE_H


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CResource
//
//  - Purpose   : Resource class. In our demosystem resources can be textures,
//                scenes etc. Resources can be shared between different
//                effects, this can be nice f.e. to use a render to texture
//                result from one to a texture applied in another.
//                This class only contains the name of the resource. Use this
//                name to access the resource itself on CEffect::Load()
//                through the resource list given as parameter.
//
// ---------------------------------------------------------------------------

class CEffect;

class CResource : public CPlugin
{


public:


	CResource() { }
	virtual ~CResource() { }

	static std::string	GETCLASSTYPE	();
	std::string			GetClassType	() const { return GETCLASSTYPE(); }

	// Virtual

	virtual bool	NeedsAdditionalPath	() const = 0;
	virtual bool	Load				(const std::string& strDatFile,
										UtilGL::Texturing::STexLoadOptions* pTexOptions) = 0;
	virtual bool	Free				(bool bFreeForReload)                            = 0;

	// Non virtual

	std::string GetResourceName		 () const;
	std::string GetResourceFile      () const;
	std::string GetAdditionalPath	 () const;
	void        SetResourceName		 (const std::string& strName);
	void        SetResourceFile		 (const std::string& strFile);
	void        SetAdditionalPath	 (const std::string& strAddPath);

	bool        NotifyDependency     (CEffect* pEffect);
	bool        RemoveDependency     (CEffect* pEffect);
	bool        HasDependencies      () const;
	bool        ShowDependencyWarning() const;


private:


	typedef std::vector<CEffect*> VECDEPENDENCIES;

	VECDEPENDENCIES m_vecDependencies;
	std::string m_strName, m_strAddPath, m_strFile;
};


#endif
