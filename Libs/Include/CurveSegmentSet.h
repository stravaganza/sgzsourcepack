// ==[ File ]===================================================================================
//
//  -Name     : CurveSegmentSet.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSegmentSet definition.
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


#ifndef __CURVESEGMENTSET_H
#define __CURVESEGMENTSET_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSegmentSet
//
//  - Purpose   : Set of control points which evaluation results in a set
//                of segments.
//
// ---------------------------------------------------------------------------
class CSegmentSet : public CCurve
{


public:


	// Key (time, value)

	struct TKey
	{
		float    fTime;
		CVector3 v3Value;
	};

	CSegmentSet()  {           }
	~CSegmentSet() { Delete(); }

	// From CEvaluatableKeySet

	bool Delete();

	bool GetStartTime(float* pfOut) const;
	bool GetEndTime  (float* pfOut) const;

	int  GetNumKeys  () const;
	bool GetKeyTime  (int nIndex, float*    pfOut)  const;
	bool GetKeyValue (int nIndex, CVector3* pv3Out) const;
	bool SetKeyTime  (int nIndex, float     fTime);
	bool SetKeyValue (int nIndex, const CVector3& v3Value);

	bool Parse       (CAdvancedParser* pParser);
	bool WriteASCII  (CWriterASCII*    pWriter);

	bool Evaluate    (float fTime, CVector3* pv3Out) const;

	// CSegmentSet

	bool AddKey      (const TKey& key);

	// Pure Virtual method to implement childs with different
	// interpolation algorithms (usually linear and step).

	virtual CVector3 Interpolate(const CVector3& v3Start, const CVector3& v3End, float fT) const = 0;


private:


	std::vector<TKey> m_vecKeys;

};


#endif