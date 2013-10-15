// ==[ File ]===================================================================================
//
//  -Name     : CurveSpline.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSpline implementation (see SplineHermite.h).
//
// =============================================================================================

/*

	26/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSpline.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool Delete();
//
//  - Purpose   : Frees all memory causing the information to be losed.
//
// -----------------------------------------------------------------------------
bool CSpline::Delete()
{
	m_vecKeys.clear();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool GetStartTime(float* pfOut) const
//
//  - Purpose   : Gets (pfOut) the time value of the first key.
//                Returns false if there are no keys.
//
// -----------------------------------------------------------------------------
bool CSpline::GetStartTime(float* pfOut) const
{
	assert(pfOut);

	if(m_vecKeys.size() > 0)
	{
		*pfOut = m_vecKeys.front().fTime;
		return true;
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool GetEndTime(float* pfOut) const
//
//  - Purpose   : Gets (pfOut) the time value of the last key.
//                Returns false if there are no keys.
//
// -----------------------------------------------------------------------------
bool CSpline::GetEndTime(float* pfOut) const
{
	assert(pfOut);

	if(m_vecKeys.size() > 0)
	{
		*pfOut = m_vecKeys.back().fTime;
		return true;
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : int GetNumKeys() const
//
//  - Purpose   : Returns the number of keys.
//
// -----------------------------------------------------------------------------
int CSpline::GetNumKeys() const
{
	return m_vecKeys.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool GetKeyTime(int nIndex, float* pfOut) const
//
//  - Purpose   : Returns the time value of the given key.
//                Returns true if succesfull, false otherwise (bad index).
//
// -----------------------------------------------------------------------------
bool CSpline::GetKeyTime(int nIndex, float* pfOut) const
{
	assert(pfOut);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pfOut = m_vecKeys[nIndex].fTime;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool GetKeyValue(int nIndex, CVector3* pv3Out) const
//
//  - Purpose   : Returns the CVector3 value of the given key.
//                Returns true if succesfull, false otherwise (bad index).
//
// -----------------------------------------------------------------------------
bool CSpline::GetKeyValue(int nIndex, CVector3* pv3Out) const
{
	assert(pv3Out);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Out = m_vecKeys[nIndex].v3Value;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool SetKeyTime(int nIndex, float fTime)
//
//  - Purpose   : Sets a key's time value. Key is accesed by its index (0-n).
//                Returns false if nIndex is invalid or if fTime is less than
//                the previous key or greater than the next. That means that a
//                key's time range must remain between this two limits.
//
// -----------------------------------------------------------------------------
bool CSpline::SetKeyTime(int nIndex, float fTime)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	// Check limits

	if(nIndex > 0)
	{
		if(m_vecKeys[nIndex - 1].fTime > fTime)
		{
			return false;
		}
	}

	if(nIndex < GetNumKeys() - 1)
	{
		if(m_vecKeys[nIndex + 1].fTime < fTime)
		{
			return false;
		}
	}

	// Ok, set value.

	m_vecKeys[nIndex].fTime = fTime;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool SetKeyValue(int nIndex, const CVector3& v3Value)
//
//  - Purpose   : Sets a key's CVector3 value. Key is accesed by its index (0-n).
//                Returns false if nIndex is invalid.
//
// -----------------------------------------------------------------------------
bool CSpline::SetKeyValue(int nIndex, const CVector3& v3Value)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecKeys[nIndex].v3Value = v3Value;

	// We need to notify a value change since in some child classes
	// adjacent control point properties (tangents) depend on this value.

	NotifyKeyValueChanged(nIndex);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool Evaluate(float fTime, CVector3* pv3Out) const
//
//  - Purpose   : Evaluates the spline, returning the CVector3 corresponding
//                to the given time value.
//
// -----------------------------------------------------------------------------
bool CSpline::Evaluate(float fTime, CVector3* pv3Out) const
{
	assert(pv3Out);

	// First do our quick checks:

	if(m_vecKeys.size() < 1)
	{
		return false;
	}
	else if(m_vecKeys.size() < 2)
	{
		if(ARE_EQUAL(fTime, m_vecKeys.front().fTime))
		{
			*pv3Out = m_vecKeys.front().v3Value;
			return true;
		}

		return false;
	}
	else if(fTime < m_vecKeys.front().fTime || fTime > m_vecKeys.back().fTime)
	{
		return false;
	}

	// Necessary?

	if(ARE_EQUAL(fTime, m_vecKeys.front().fTime))
	{
		*pv3Out = m_vecKeys.front().v3Value;
		return true;
	}

	if(ARE_EQUAL(fTime, m_vecKeys.back().fTime))
	{
		*pv3Out = m_vecKeys.back().v3Value;
		return true;
	}

	// We'll try to get the right two points to interpolate between:

	std::vector<TKey>::const_iterator it;

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		if(fTime >= it->fTime && fTime < (it + 1)->fTime)
		{
			// Call interpolate:

			float fT = (fTime - it->fTime) / ((it + 1)->fTime - it->fTime);
			*pv3Out = Interpolate(it - m_vecKeys.begin(), (it - m_vecKeys.begin()) + 1, fT);
			return true;
		}
	}

	// Shouldn't happen

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool GetKeyTanIn(int nIndex, CVector3* pv3Out) const
//
//  - Purpose   : Returns the in tangent of the i-th key.
//
// -----------------------------------------------------------------------------
bool CSpline::GetKeyTanIn(int nIndex, CVector3* pv3Out) const
{
	assert(pv3Out);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Out = m_vecKeys[nIndex].v3TanIn;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : bool GetKeyTanIn(int nIndex, CVector3* pv3Out) const
//
//  - Purpose   : Returns the out tangent of the i-th key.
//
// -----------------------------------------------------------------------------
bool CSpline::GetKeyTanOut(int nIndex, CVector3* pv3Out) const
{
	assert(pv3Out);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Out = m_vecKeys[nIndex].v3TanOut;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : void AdjustTangentRange(const CVector3& v3Min,
//                                        const CVector3& v3Max)
//
//  - Purpose   : See EnableAdjustTangentRange()
//
// -----------------------------------------------------------------------------
void CSpline::AdjustTangentRange(const CVector3& v3Min, const CVector3& v3Max)
{
	m_v3Min = v3Min;
	m_v3Max = v3Max;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : void EnableAdjustTangentRange(bool bTrue)
//
//  - Purpose   : When enabled, tangents are adjusted (i.e. clamped) so that no
//                spline value can go out of the (v3Min, v3Max) range.
//                This is useful to create curves and assure that no value is
//                going to be outside the given range when evaluating the curve.
//
//  - Notes     : Exceptions are cases when keys outside that range exist.
//
// -----------------------------------------------------------------------------
void CSpline::EnableAdjustTangentRange(bool bTrue)
{
	m_bAdjustTangentRange = bTrue;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSpline
//  - Prototype : CVector3 Interpolate(int nKeyStart, int nKeyEnd, float fT) const
//
//  - Purpose   : Interpolates between two keys. 
//
// -----------------------------------------------------------------------------
CVector3 CSpline::Interpolate(int nKeyStart, int nKeyEnd, float fT) const
{
	CVector3 result;

	// Call virtual method:

	CMatrix matrix = GetMatrix();

	// Compute:

	float fTQuad = fT * fT;
	float fTCube = fTQuad * fT;

	float fX = fTCube * matrix(0,0) + fTQuad * matrix(1,0) + fT * matrix(2,0) + matrix(3,0);
	float fY = fTCube * matrix(0,1) + fTQuad * matrix(1,1) + fT * matrix(2,1) + matrix(3,1);
	float fZ = fTCube * matrix(0,2) + fTQuad * matrix(1,2) + fT * matrix(2,2) + matrix(3,2);
	float fW = fTCube * matrix(0,3) + fTQuad * matrix(1,3) + fT * matrix(2,3) + matrix(3,3);

	CVector3 v3Start  = m_vecKeys[nKeyStart].v3Value;
	CVector3 v3End    = m_vecKeys[nKeyEnd  ].v3Value;
	CVector3 v3TanOut = m_vecKeys[nKeyStart].v3TanOut;
	CVector3 v3TanIn  = m_vecKeys[nKeyEnd  ].v3TanIn;

	result.SetX(fX * v3Start.X() + fY * v3End.X() + fZ * v3TanOut.X() + fW * v3TanIn.X());
	result.SetY(fX * v3Start.Y() + fY * v3End.Y() + fZ * v3TanOut.Y() + fW * v3TanIn.Y());
	result.SetZ(fX * v3Start.Z() + fY * v3End.Z() + fZ * v3TanOut.Z() + fW * v3TanIn.Z());

	return result;
}
