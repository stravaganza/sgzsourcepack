// ==[ File ]===================================================================================
//
//  -Name     : ResourceScene.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResourceScene definition.
//
// =============================================================================================

/*

	28/04/2003  -  File created.


*/


#ifndef __RESOURCESCENE_H
#define __RESOURCESCENE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CResourceScene
//
//  - Purpose   : Texture 2D resource.
//
// ---------------------------------------------------------------------------
class CResourceScene : public CResource
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CResourceScene();
	~CResourceScene();

	// From CResource

	bool NeedsAdditionalPath() const { return true;                      }

	bool Load				(const std::string& strDatFile,
							 UtilGL::Texturing::STexLoadOptions* pTexOptions);
	bool Free				(bool bFreeForReload);

	const CSceneView* GetSceneView() const { return m_pSceneView; }


private:


	CSceneView* m_pSceneView;

};


#endif
