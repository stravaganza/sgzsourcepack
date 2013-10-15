// ==[ File ]===================================================================================
//
//  -Name     : ResourceList.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResourceList definition.
//
// =============================================================================================

/*

	20/10/2003  -  File created.


*/


#ifndef __RESOURCELIST_H
#define __RESOURCELIST_H


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CResourceList
//
//  - Purpose   : Stores resources and provide methods to access them by name.
//
// ---------------------------------------------------------------------------
class CResourceList
{


public:


	CResourceList()  {            }
	~CResourceList() { FreeAll(); }

	bool		AddResource		(CResource* pResource);
	bool		RemoveResource	(const std::string & strName, bool bAlsoFree);
	bool        RemoveAll       ();
	bool        FreeAll         ();
	int         GetResourceCount() const;
	CResource*  GetResource     (int i);
	CResource*	GetResource		(const std::string & strName);


private:


	typedef std::vector<CResource*> VECRESOURCES;

	VECRESOURCES m_vecResources;

};


#endif
