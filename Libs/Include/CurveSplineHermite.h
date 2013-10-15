// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineHermite.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineHermite definition.
//
// =============================================================================================

/*

	26/09/2002  -  File created.


*/


#ifndef __CURVESPLINEHERMITE_H
#define __CURVESPLINEHERMITE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSplineHermite
//
//  - Purpose   : Hermite spline.
//
// ---------------------------------------------------------------------------
class CSplineHermite : public CSpline
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	// Key (time, in/out tangents, value)

	struct TKey
	{
		float    fTime;
		CVector3 v3TanIn;
		CVector3 v3TanOut;
		CVector3 v3Value;
	};

	CSplineHermite()  {           }
	~CSplineHermite() { Delete(); }

	// From CEvaluatableKeySet

	bool	Parse		(CAdvancedParser* pParser);
	bool	WriteASCII	(CWriterASCII*    pWriter);

	// From CSpline

	CMatrix GetMatrix	() const;

	// CSplineHermite

	bool    AddKey		(const TKey& key);

	bool	GetKeyTanIn (int nIndex, CVector3* pv3Dest);
	bool	GetKeyTanOut(int nIndex, CVector3* pv3Dest);
	bool	SetKeyTanIn (int nIndex, const CVector3& v3TanIn);
	bool	SetKeyTanOut(int nIndex, const CVector3& v3TanOut);

};

#endif