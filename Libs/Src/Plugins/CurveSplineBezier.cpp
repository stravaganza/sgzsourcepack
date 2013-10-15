// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineBezier.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineBezier implementation (see SplineBezier.h).
//
// =============================================================================================

/*

	27/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSplineBezier.h"


const std::string CSplineBezier::CLASSNAME("Spline Bezier");

REGISTER_PLUGIN(CPluginFactory, CSplineBezier)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : virtual CMatrix GetMatrix() const;
//
//  - Purpose   : Returns the bezier matrix.
//
// -----------------------------------------------------------------------------
CMatrix CSplineBezier::GetMatrix() const
{
	return CMatrix(	-1.0f,  3.0f, -3.0f,  1.0f,
					 3.0f, -6.0f,  3.0f,  0.0f,
					-3.0f,  3.0f,  0.0f,  0.0f,
					 1.0f,  0.0f,  0.0f,  0.0f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Gets the data parsing the keys from a source.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Delete();

	CSplineBezier::TKey key;

	while(pParser->ReadKeyFloat("#time", '=', &key.fTime, CAdvancedParser::SEARCH_FROMCURPOS))
	{
		int nError = 0;
		
		if(!pParser->ReadKeyVector3("#value",    '=', &key.v3Value,    CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyVector3("#pointin",  '=', &key.v3PointIn,  CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyVector3("#pointout", '=', &key.v3PointOut, CAdvancedParser::SEARCH_INCURLINE)) nError++;

		if(!nError)
		{
			AddKey(key);
		}
		else
		{
			LOG.Write("\nERROR - CSplineBezier::Parse(): missed some field parsing a line.");
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the keys in ASCII info to a destiny.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	CSplineBezier::TKey key;

	pWriter->Write("\n#curve=\"%s\"", GetClassName().data());
	pWriter->Write("\n{");

	for(int i = 0; i < GetNumKeys(); i++)
	{
		GetKeyTime    (i, &key.fTime);
		GetKeyValue   (i, &key.v3Value);
		GetKeyPointIn (i, &key.v3PointIn);
		GetKeyPointOut(i, &key.v3PointIn);

		pWriter->Write("\n  #time=%f", key.fTime);
		pWriter->Write(" #value=(%f, %f, %f)",    key.v3Value.X(),    key.v3Value.Y(),    key.v3Value.Z());
		pWriter->Write(" #pointin=(%f, %f, %f)",  key.v3PointIn.X(),  key.v3PointIn.Y(),  key.v3PointIn.Z());
		pWriter->Write(" #pointout=(%f, %f, %f)", key.v3PointOut.X(), key.v3PointOut.Y(), key.v3PointOut.Z());
	}

	pWriter->Write("\n}");
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool AddKey(const TKey& key)
//
//  - Purpose   : Adds a key to the set. Operates like CSplineHermite::AddKey()
//                but we make a couple of simple operations before to get
//                the tangents.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::AddKey(const TKey& key)
{
	CSpline::TKey newKey;

	newKey.fTime    = key.fTime;
	newKey.v3TanIn  = (key.v3Value    - key.v3PointIn) * 3.0f;
	newKey.v3TanOut = (key.v3PointOut - key.v3Value)   * 3.0f;
	newKey.v3Value  = key.v3Value;

	// First do a quick check:

	if(!m_vecKeys.empty())
	{
		if(m_vecKeys.back().fTime < key.fTime)
		{
			m_vecKeys.push_back(newKey);
			return true;
		}
		else if(m_vecKeys.front().fTime > key.fTime)
		{
			m_vecKeys.insert(m_vecKeys.begin(), newKey);
			return true;
		}
	}

	// Secuencial search:

	std::vector<CSpline::TKey>::iterator it;

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		if(it->fTime > key.fTime)
		{
			m_vecKeys.insert(it, newKey);
			return true;
		}
	}

	// Shouldn't happen, I add it just in case some floating point precision issue occurs.

	m_vecKeys.push_back(newKey);

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool GetKeyPointIn(int nIndex, CVector3* pv3Dest)
//
//  - Purpose   : Returns the given key's Bezier in point value.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::GetKeyPointIn(int nIndex, CVector3* pv3Dest)
{
	assert(pv3Dest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Dest = m_vecKeys[nIndex].v3Value - (m_vecKeys[nIndex].v3TanIn * 0.33333333333f);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool GetKeyPointOut(int nIndex, CVector3* pv3Dest)
//
//  - Purpose   : Returns the given key's Bezier out point value.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::GetKeyPointOut(int nIndex, CVector3* pv3Dest)
{
	assert(pv3Dest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Dest = m_vecKeys[nIndex].v3Value + (m_vecKeys[nIndex].v3TanOut * 0.33333333333f);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool SetKeyPointIn(int nIndex, const CVector3& v3PointIn)
//
//  - Purpose   : Sets the given key's Bezier in point value.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::SetKeyPointIn(int nIndex, const CVector3& v3PointIn)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecKeys[nIndex].v3TanIn = (m_vecKeys[nIndex].v3Value - v3PointIn) * 3.0f;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineBezier
//  - Prototype : bool SetKeyPointOut(int nIndex, const CVector3& v3PointIn)
//
//  - Purpose   : Sets the given key's Bezier out point value.
//
// -----------------------------------------------------------------------------
bool CSplineBezier::SetKeyPointOut(int nIndex, const CVector3& v3PointOut)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecKeys[nIndex].v3TanOut = (v3PointOut - m_vecKeys[nIndex].v3Value) * 3.0f;
	return true;
}
