// ==[ File ]===================================================================================
//
//  -Name     : ResourceTexture2D.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResourceTexture2D definition.
//
// =============================================================================================

/*

	28/04/2003  -  File created.


*/


#ifndef __RESOURCETEXTURE2D_H
#define __RESOURCETEXTURE2D_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CResourceTexture2D
//
//  - Purpose   : Texture 2D resource.
//
// ---------------------------------------------------------------------------
class CResourceTexture2D : public CResource
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CResourceTexture2D();
	~CResourceTexture2D();

	// From CResource

	bool NeedsAdditionalPath() const { return false;                         }

	bool Load				(const std::string& strDatFile,
							 UtilGL::Texturing::STexLoadOptions* pTexOptions);
	bool Free				(bool bFreeForReload);

	const CBitmap*						 GetBitmap()    const { return &m_bitmap;  }
	const UtilGL::Texturing::CTexture2D* GetTexture2D() const { return m_pTexture; }


private:


	CBitmap						   m_bitmap;
	UtilGL::Texturing::CTexture2D* m_pTexture;

};


#endif