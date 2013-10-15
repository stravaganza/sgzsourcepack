// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineTCB.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineTCB definition.
//
// =============================================================================================

/*

	28/09/2002  -  File created.


*/


#ifndef __CURVESPLINETCB_H
#define __CURVESPLINETCB_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSplineTCB
//
//  - Purpose   : Kochanek-Bartels spline (aka TCB splines).
//
// ---------------------------------------------------------------------------
class CSplineTCB : public CSpline
{


public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	// Key (time, tension, bias, continuity, value)

	struct TKey
	{
		float    fTime;
		float    fTension;
		float    fBias;
		float    fContinuity;
		CVector3 v3Value;
	};

	CSplineTCB()          {           }
	virtual ~CSplineTCB() { Delete(); }

	// From CEvaluatableKeySet

	bool	Parse			(CAdvancedParser* pParser);
	bool	WriteASCII		(CWriterASCII*    pWriter);

	// From CSpline

	CMatrix GetMatrix		() const;

	// CSplineTCB

	bool    AddKey			(const TKey& key);

	bool	GetKeyTension   (int nIndex, float* pfDest);
	bool	GetKeyBias      (int nIndex, float* pfDest);
	bool	GetKeyContinuity(int nIndex, float* pfDest);
	bool	SetKeyTension   (int nIndex, float  fValue);
	bool	SetKeyBias      (int nIndex, float  fValue);
	bool	SetKeyContinuity(int nIndex, float  fValue);


protected:


	void NotifyKeyValueChanged(int nIndex);
	void NotifyKeyTCBChanged  (int nIndex);

	void GetTangents(	const CSpline::TKey& keyPrev, 
						const TKey& key,
						const CSpline::TKey& keyNext,
						CVector3* pv3TanIn,
						CVector3* pv3TanOut);

	std::vector<TKey> m_vecTCBKeys;

};


#endif