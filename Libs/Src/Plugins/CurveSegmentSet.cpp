// ==[ File ]===================================================================================
//
//  -Name     : CurveSegmentSet.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSegmentSet implementation (see SegmentSet.h).
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSegmentSet.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool Delete();
//
//  - Purpose   : Frees all memory causing the information to be losed.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::Delete()
{
	m_vecKeys.clear();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool GetStartTime(float* pfOut) const
//
//  - Purpose   : Gets (pfOut) the time value of the first key.
//                Returns false if there are no keys.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::GetStartTime(float* pfOut) const
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
//  - Class     : CSegmentSet
//  - Prototype : bool GetEndTime(float* pfOut) const
//
//  - Purpose   : Gets (pfOut) the time value of the last key.
//                Returns false if there are no keys.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::GetEndTime(float* pfOut) const
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
//  - Class     : CSegmentSet
//  - Prototype : int GetNumKeys() const
//
//  - Purpose   : Returns the number of keys.
//
// -----------------------------------------------------------------------------
int CSegmentSet::GetNumKeys() const
{
	return m_vecKeys.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool GetKeyTime(int nIndex, float* pfOut) const
//
//  - Purpose   : Returns the time value of the given key.
//                Returns true if succesfull, false otherwise (bad index).
//
// -----------------------------------------------------------------------------
bool CSegmentSet::GetKeyTime(int nIndex, float* pfOut) const
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
//  - Class     : CSegmentSet
//  - Prototype : bool GetKeyValue(int nIndex, CVector3* pv3Out) const
//
//  - Purpose   : Returns the CVector3 value of the given key.
//                Returns true if succesfull, false otherwise (bad index).
//
// -----------------------------------------------------------------------------
bool CSegmentSet::GetKeyValue(int nIndex, CVector3* pv3Out) const
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
//  - Class     : CSegmentSet
//  - Prototype : bool SetKeyTime(int nIndex, float fTime)
//
//  - Purpose   : Sets a key's time value. Key is accesed by its index (0-n).
//                Returns false if nIndex is invalid or if fTime is less than
//                the previous key or greater than the next. That means that a
//                key's time range must remain between this two limits.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::SetKeyTime(int nIndex, float fTime)
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
//  - Class     : CSegmentSet
//  - Prototype : bool SetKeyValue(int nIndex, const CVector3& v3Value)
//
//  - Purpose   : Sets a key's CVector3 value. Key is accesed by its index (0-n).
//                Returns false if nIndex is invalid.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::SetKeyValue(int nIndex, const CVector3& v3Value)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecKeys[nIndex].v3Value = v3Value;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Gets the data parsing the keys from a source.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Delete();

	CSegmentSet::TKey key;

	while(pParser->ReadKeyFloat("#time", '=', &key.fTime, CAdvancedParser::SEARCH_FROMCURPOS))
	{
		if(pParser->ReadKeyVector3("#value", '=', &key.v3Value, CAdvancedParser::SEARCH_FROMCURPOSTOLINEEND))
		{
			AddKey(key);
		}
		else
		{
			LOG.Write("\nERROR - CSegmentSet::Parse(): #value not found when parsing a line.");
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the keys in ASCII info to a destiny.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	std::vector<CSegmentSet::TKey>::const_iterator it;

	pWriter->Write("\n#curve=\"%s\"", GetClassName().data());
	pWriter->Write("\n{");

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		pWriter->Write("\n  #time=%f #value=(%f, %f, %f)", it->fTime, it->v3Value.X(), it->v3Value.Y(), it->v3Value.Z());
	}

	pWriter->Write("\n}");
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool Evaluate(float fTime, CVector3* pv3Out) const
//
//  - Purpose   : Adds a key to the set. Internally it's positioned according to
//                its time value to keep an increasing order.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::Evaluate(float fTime, CVector3* pv3Out) const
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

			*pv3Out = Interpolate(it->v3Value, (it + 1)->v3Value, (fTime - it->fTime) / ((it + 1)->fTime - it->fTime));
			return true;
		}
	}

	// Shouldn't happen

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSet
//  - Prototype : bool AddKey(const TKey& key)
//
//  - Purpose   : Adds a key to the set. Internally it's positioned according to
//                its time value to keep an increasing order.
//
// -----------------------------------------------------------------------------
bool CSegmentSet::AddKey(const TKey& key)
{
	// First do a quick check:

	if(!m_vecKeys.empty())
	{
		if(m_vecKeys.back().fTime < key.fTime)
		{
			m_vecKeys.push_back(key);
			return true;
		}
		else if(m_vecKeys.front().fTime > key.fTime)
		{
			m_vecKeys.insert(m_vecKeys.begin(), key);
			return true;
		}
	}

	// Secuencial search:

	std::vector<TKey>::iterator it;

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		if(it->fTime > key.fTime)
		{
			m_vecKeys.insert(it, key);
			return true;
		}
	}

	// Shouldn't happen, I add it just in case some floating point precision issue occurs.

	m_vecKeys.push_back(key);

	return false;
}
