// ==[ File ]===================================================================================
//
//  -Name     : CurveSpline.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSpline definition.
//
// =============================================================================================

/*

	27/09/2002  -  File created.


*/


#ifndef __CURVESPLINE_H
#define __CURVESPLINE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSpline
//
//  - Purpose   : Spline base class
//
//  - Note      : Represents all spline types that can be seen as a set of
//                keys with an incoming and outgoing tangent.
//
// ---------------------------------------------------------------------------
class CSpline : public CCurve
{


public:


	CSpline()          { }
	virtual ~CSpline() { }

	// From CEvaluatableKeySet

	bool Delete();

	bool GetStartTime(float* pfOut) const;
	bool GetEndTime  (float* pfOut) const;

	int  GetNumKeys  () const;
	bool GetKeyTime  (int nIndex, float*    pfOut)  const;
	bool GetKeyValue (int nIndex, CVector3* pv3Out) const;
	bool SetKeyTime  (int nIndex, float     fTime);
	bool SetKeyValue (int nIndex, const CVector3& v3Value);

	bool Evaluate    (float fTime, CVector3* pv3Out) const;

	// CSpline

	bool GetKeyTanIn (int nIndex, CVector3* pv3Out) const;
	bool GetKeyTanOut(int nIndex, CVector3* pv3Out) const;

	void AdjustTangentRange       (const CVector3& v3Min, const CVector3& v3Max);
	void EnableAdjustTangentRange (bool bTrue);

	CVector3 Interpolate       (int nKeyStart, int nKeyEnd, float fT) const;

	// Implement in CSpline childs: 

	virtual CMatrix  GetMatrix() const = 0;


private:


	bool     m_bAdjustTangentRange;
	CVector3 m_v3Min, m_v3Max;


protected:

	
	virtual void NotifyKeyValueChanged(int nIndex) { }

	// Key (time, in/out tangents, value)

	struct TKey
	{
		float    fTime;
		CVector3 v3TanIn;
		CVector3 v3TanOut;
		CVector3 v3Value;
	};

	std::vector<TKey> m_vecKeys;

};


#endif