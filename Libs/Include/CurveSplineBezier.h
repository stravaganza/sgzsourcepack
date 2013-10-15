// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineBezier.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineBezier definition.
//
// =============================================================================================

/*

	26/09/2002  -  File created.


*/


#ifndef __CURVESPLINEBEZIER_H
#define __CURVESPLINEBEZIER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSplineBezier
//
//  - Purpose   : Bezier spline.
//
// ---------------------------------------------------------------------------
class CSplineBezier : public CSpline
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	// Key (time, control points, value)

	struct TKey
	{
		float    fTime;
		CVector3 v3PointIn;
		CVector3 v3PointOut;
		CVector3 v3Value;
	};

	CSplineBezier()  {           }
	~CSplineBezier() { Delete(); }

	// From CEvaluatableKeySet

	bool	Parse			(CAdvancedParser* pParser);
	bool	WriteASCII		(CWriterASCII*    pWriter);

	// From CSpline

	CMatrix	GetMatrix		() const;

	// CSplineBezier

	bool	AddKey			(const TKey& key);

	bool	GetKeyPointIn	(int nIndex, CVector3* pv3Dest);
	bool	GetKeyPointOut	(int nIndex, CVector3* pv3Dest);
	bool	SetKeyPointIn	(int nIndex, const CVector3& v3PointIn);
	bool	SetKeyPointOut	(int nIndex, const CVector3& v3PointOut);

};


#endif