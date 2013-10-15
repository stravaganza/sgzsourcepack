// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineCatmullRom.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineCatmullRom definition.
//
// =============================================================================================

/*

	28/09/2002  -  File created.


*/


#ifndef __CURVESPLINECATMULLROM_H
#define __CURVESPLINECATMULLROM_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSplineCatmullRom
//
//  - Purpose   : Catmull-Rom spline routines.
//
//  - Note      : Spline whose p tangents are parallel to the p-1 p+1 segment.
//
// ---------------------------------------------------------------------------
class CSplineCatmullRom : public CSpline
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	// Key (time, value)

	struct TKey
	{
		float    fTime;
		CVector3 v3Value;
	};

	CSplineCatmullRom()  { m_bRemoveTangentsYZ = false; }
	~CSplineCatmullRom() { Delete();                    }

	// From CEvaluatableKeySet

	bool	Parse		     (CAdvancedParser* pParser);
	bool	WriteASCII	     (CWriterASCII*    pWriter);

	// From CSpline

	CMatrix GetMatrix	     () const;

	// CSplineCatmullRom

	bool    AddKey		     (const TKey& key);
	
	void    RemoveTangentsYZ ();



protected:

	
	void NotifyKeyValueChanged(int nIndex);

	void GetTangents(	const CSpline::TKey& keyPrev, 
						const CSpline::TKey& keyNext,
						CVector3* pv3TanIn,
						CVector3* pv3TanOut);


	bool m_bRemoveTangentsYZ;

};


#endif