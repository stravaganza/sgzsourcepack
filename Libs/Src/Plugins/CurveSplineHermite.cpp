// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineHermite.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineHermite implementation (see SplineHermite.h).
//
// =============================================================================================

/*

	26/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSplineHermite.h"


const std::string CSplineHermite::CLASSNAME("Spline Hermite");

REGISTER_PLUGIN(CPluginFactory, CSplineHermite)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Gets the data parsing the keys from a source.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Delete();

	CSplineHermite::TKey key;

	while(pParser->ReadKeyFloat("#time", '=', &key.fTime, CAdvancedParser::SEARCH_FROMCURPOS))
	{
		int nError = 0;
		
		if(!pParser->ReadKeyVector3("#value",  '=', &key.v3Value,  CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyVector3("#tanin",  '=', &key.v3TanIn,  CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyVector3("#tanout", '=', &key.v3TanOut, CAdvancedParser::SEARCH_INCURLINE)) nError++;

		if(!nError)
		{
			AddKey(key);
		}
		else
		{
			LOG.Write("\nERROR - CSplineHermite::Parse(): missed some field parsing a line.");
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the keys in ASCII info to a destiny.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	std::vector<CSpline::TKey>::const_iterator it;

	pWriter->Write("\n#curve=\"%s\"", GetClassName().data());
	pWriter->Write("\n{");

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		pWriter->Write("\n  #time=%f", it->fTime);
		pWriter->Write(" #value=(%f, %f, %f)",  it->v3Value.X(),  it->v3Value.Y(),  it->v3Value.Z());
		pWriter->Write(" #tanin=(%f, %f, %f)",  it->v3TanIn.X(),  it->v3TanIn.Y(),  it->v3TanIn.Z());
		pWriter->Write(" #tanout=(%f, %f, %f)", it->v3TanOut.X(), it->v3TanOut.Y(), it->v3TanOut.Z());
	}

	pWriter->Write("\n}");
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : CMatrix GetMatrix() const;
//
//  - Purpose   : Returns the hermite matrix.
//
// -----------------------------------------------------------------------------
CMatrix CSplineHermite::GetMatrix() const
{
	return CMatrix(	 2.0f, -2.0f,  1.0f,  1.0f,
					-3.0f,  3.0f, -2.0f, -1.0f,
					 0.0f,  0.0f,  1.0f,  0.0f,
					 1.0f,  0.0f,  0.0f,  0.0f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : bool AddKey(const TKey& key)
//
//  - Purpose   : Adds a key to the set. Internally it's positioned according to
//                its time value to keep an increasing order.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::AddKey(const TKey& key)
{
	CSpline::TKey newKey;

	newKey.fTime    = key.fTime;
	newKey.v3TanIn  = key.v3TanIn;
	newKey.v3TanOut = key.v3TanOut;
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
//  - Class     : CSplineHermite
//  - Prototype : bool GetKeyTanIn(int nIndex, CVector3* pv3Dest)
//
//  - Purpose   : Returns the given key's in tangent value.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::GetKeyTanIn(int nIndex, CVector3* pv3Dest)
{
	assert(pv3Dest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Dest = m_vecKeys[nIndex].v3TanIn;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : bool GetKeyTanOut(int nIndex, CVector3* pv3Dest)
//
//  - Purpose   : Returns the given key's out tangent value.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::GetKeyTanOut(int nIndex, CVector3* pv3Dest)
{
	assert(pv3Dest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pv3Dest = m_vecKeys[nIndex].v3TanOut;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : bool SetKeyTanIn(int nIndex, const CVector3& v3TanIn)
//
//  - Purpose   : Sets the given key's in tangent value.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::SetKeyTanIn(int nIndex, const CVector3& v3TanIn)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecKeys[nIndex].v3TanIn = v3TanIn;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineHermite
//  - Prototype : bool SetKeyTanOut(int nIndex, const CVector3& v3TanOut)
//
//  - Purpose   : Sets the given key's out tangent value.
//
// -----------------------------------------------------------------------------
bool CSplineHermite::SetKeyTanOut(int nIndex, const CVector3& v3TanOut)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecKeys[nIndex].v3TanOut = v3TanOut;
	return true;
}