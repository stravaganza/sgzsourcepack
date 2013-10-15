// ==[ File ]===================================================================================
//
//  -Name     : ResourceSpline.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CResourceSpline definition.
//
// =============================================================================================

/*

	30/12/2003  -  File created.


*/


#ifndef __RESOURCESPLINE_H
#define __RESOURCESPLINE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CResourceSpline
//
//  - Purpose   : Spline resource.
//
// ---------------------------------------------------------------------------
class CResourceSpline : public CResource
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CResourceSpline();
	~CResourceSpline();

	// From CResource

	bool NeedsAdditionalPath() const { return false;                      }

	bool Load				(const std::string& strDatFile,
							 UtilGL::Texturing::STexLoadOptions* pTexOptions);
	bool Free				(bool bFreeForReload);

	int								GetSplineCount() const { return m_vecSplines.size(); }
	const CSplineCatmullRom*		GetSpline(int i) const { return m_vecSplines[i];     }


private:


	typedef std::vector<CSplineCatmullRom*> VECPSPLINES;
	VECPSPLINES m_vecSplines;

};


#endif