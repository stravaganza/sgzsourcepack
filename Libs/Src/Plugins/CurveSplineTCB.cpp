// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineTCB.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineTCB implementation (see SplineTCB.h).
//
// =============================================================================================

/*

	28/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSplineTCB.h"


const std::string CSplineTCB::CLASSNAME("Spline TCB");

REGISTER_PLUGIN(CPluginFactory, CSplineTCB)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Gets the data parsing the keys from a source.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Delete();

	CSplineTCB::TKey key;

	while(pParser->ReadKeyFloat("#time", '=', &key.fTime, CAdvancedParser::SEARCH_FROMCURPOS))
	{
		int nError = 0;

		if(!pParser->ReadKeyVector3("#value",      '=', &key.v3Value,     CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyFloat  ("#tension",    '=', &key.fTension,    CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyFloat  ("#continuity", '=', &key.fContinuity, CAdvancedParser::SEARCH_INCURLINE)) nError++;
		if(!pParser->ReadKeyFloat  ("#bias",       '=', &key.fBias,       CAdvancedParser::SEARCH_INCURLINE)) nError++;

		if(!nError)
		{
			AddKey(key);
		}
		else
		{
			LOG.Write("\nERROR - CSplineTCB::Parse(): missed some field parsing a line.");
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the keys in ASCII info to a destiny.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	std::vector<CSplineTCB::TKey>::const_iterator it;

	pWriter->Write("\n#curve=\"%s\"", GetClassName().data());
	pWriter->Write("\n{");

	for(it = m_vecTCBKeys.begin(); it != m_vecTCBKeys.end(); ++it)
	{
		pWriter->Write("\n  #time=%f #tension=%f #continuity=%f #bias=%f",
							it->fTime, it->fTension, it->fContinuity, it->fBias);
	}

	pWriter->Write("\n}");
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : CMatrix GetMatrix() const;
//
//  - Purpose   : Returns the TCB matrix (the Hermite one).
//
// -----------------------------------------------------------------------------
CMatrix CSplineTCB::GetMatrix() const
{
	return CMatrix(	 2.0f, -2.0f,  1.0f,  1.0f,
					-3.0f,  3.0f, -2.0f, -1.0f,
					 0.0f,  0.0f,  1.0f,  0.0f,
					 1.0f,  0.0f,  0.0f,  0.0f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool AddKey(const TKey& tcbKey)
//
//  - Purpose   : Adds a key to the set. Operates like CSplineTCB::AddKey()
//                but we make some operations with tension, bias and continuity
//                to compute the tangents. We also need p-1 and p+1's CVector3
//                values.
//
//  - Note      : Special cases first and last key are dealed duplicating
//                these keys (remember, we need p-1 and p+1). Duplicating means
//                that we consider the previous key to the first as the
//                same and also the next to the last as the last itself.
//                We also need to keep a secondary TCB key list to recompute
//                tangents of the neighbour keys when a key is changed or a new
//                one is inserted.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::AddKey(const TKey& tcbKey)
{
	CSpline::TKey newKey;

	newKey.fTime   = tcbKey.fTime;
	newKey.v3Value = tcbKey.v3Value;

	// Secuencial search for a t greater than tcbKey.time:

	std::vector<CSpline::TKey>::iterator it;

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		if(it->fTime > tcbKey.fTime)
		{
			// We know there's at least a key after the one being inserted
			// (key.time < keylist[i].time), so the insertion can be only
			// at the list's start or between two keys.

			int nIndex, nLast;
			
			nIndex = it - m_vecKeys.begin();
			nLast  = GetNumKeys() - 1;

			if(nIndex == 0)
			{
				// Added as the first key. Duplicate it to compute its tangents.
				GetTangents(newKey, tcbKey, m_vecKeys.front(), &newKey.v3TanIn, &newKey.v3TanOut);
			}
			else
			{
				// Added in between two keys:
				GetTangents(m_vecKeys[nIndex - 1], tcbKey, m_vecKeys[nIndex], &newKey.v3TanIn, &newKey.v3TanOut);
			}

			m_vecKeys.insert(it, newKey);
			m_vecTCBKeys.insert(m_vecTCBKeys.begin() + (it - m_vecKeys.begin()), tcbKey);

			NotifyKeyValueChanged(nIndex);
			return true;
		}
	}

	// List is empty?

	if(m_vecKeys.empty())
	{
		m_vecKeys.push_back(newKey);
		m_vecTCBKeys.push_back(tcbKey);
		return true;
	}

	// Added at the end of the list. Duplicate it to compute its tangents.

	GetTangents(m_vecKeys.back(), tcbKey, newKey, &newKey.v3TanIn, &newKey.v3TanOut);
	m_vecKeys.push_back(newKey);
	m_vecTCBKeys.push_back(tcbKey);

	NotifyKeyValueChanged(GetNumKeys() - 1);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : GetKeyTension(int nIndex, float* pfDest)
//
//  - Purpose   : Returns the given key's tension value.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::GetKeyTension(int nIndex, float* pfDest)
{
	assert(pfDest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pfDest = m_vecTCBKeys[nIndex].fTension;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool GetKeyBias(int nIndex, float* pfDest)
//
//  - Purpose   : Returns the given key's bias value.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::GetKeyBias(int nIndex, float* pfDest)
{
	assert(pfDest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pfDest = m_vecTCBKeys[nIndex].fBias;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool GetKeyContinuity(int nIndex, float* pfDest)
//
//  - Purpose   : Returns the given key's continuity value.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::GetKeyContinuity(int nIndex, float* pfDest)
{
	assert(pfDest);

	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	*pfDest = m_vecTCBKeys[nIndex].fContinuity;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : SetKeyTension(int nIndex, float fValue)
//
//  - Purpose   : Sets the given key's tension value.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::SetKeyTension(int nIndex, float fValue)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecTCBKeys[nIndex].fTension = fValue;

	NotifyKeyValueChanged(nIndex);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool SetKeyBias(int nIndex, float fValue)
//
//  - Purpose   : Sets the given key's bias value.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::SetKeyBias(int nIndex, float fValue)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecTCBKeys[nIndex].fBias = fValue;

	NotifyKeyValueChanged(nIndex);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : bool SetKeyContinuity(int nIndex, float fValue)
//
//  - Purpose   : Sets the given key's continuity value.
//
// -----------------------------------------------------------------------------
bool CSplineTCB::SetKeyContinuity(int nIndex, float fValue)
{
	if(!ValidKeyIndex(nIndex))
	{
		return false;
	}

	m_vecTCBKeys[nIndex].fContinuity = fValue;

	NotifyKeyValueChanged(nIndex);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : void NotifyKeyValueChanged(int nIndex)
//
//  - Purpose   : This method is called when a key value has been changed in
//                order to recompute its neighbour tangent values.
//                Special cases are treated as explained in AddKey().
//
// -----------------------------------------------------------------------------
void CSplineTCB::NotifyKeyValueChanged(int nIndex)
{
	// Adjust previous key's tangents if there's one

	if(nIndex > 0)
	{
		// Is key[nIndex-1] the first? Duplicate then, otherwise no need to duplicate.

		if(nIndex == 1)
		{
			GetTangents(m_vecKeys[0], m_vecTCBKeys[0], m_vecKeys[nIndex], &m_vecKeys[0].v3TanIn, &m_vecKeys[0].v3TanOut);
		}
		else
		{
			GetTangents(m_vecKeys[nIndex - 2], m_vecTCBKeys[nIndex - 1], m_vecKeys[nIndex], &m_vecKeys[nIndex - 1].v3TanIn, &m_vecKeys[nIndex - 1].v3TanOut);
		}
	}

	// Adjust next key's tangents if there's one

	if(nIndex < GetNumKeys() - 1)
	{
		// Is key[nIndex+1] the last? Duplicate then, otherwise no need to duplicate.

		int nLast = GetNumKeys() - 1;

		if(nIndex + 1 == nLast)
		{
			GetTangents(m_vecKeys[nIndex], m_vecTCBKeys[nLast], m_vecKeys[nLast], &m_vecKeys[nLast].v3TanIn, &m_vecKeys[nLast].v3TanOut);
		}
		else
		{
			GetTangents(m_vecKeys[nIndex], m_vecTCBKeys[nIndex + 1], m_vecKeys[nIndex + 2], &m_vecKeys[nIndex + 1].v3TanIn, &m_vecKeys[nIndex + 1].v3TanOut);
		}
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : void NotifyKeyValueChanged(int nIndex)
//
//  - Purpose   : This method is called when a key's T/C/B value(s) has/have
//                been changed in order to recompute its tangents.
//
// -----------------------------------------------------------------------------
void CSplineTCB::NotifyKeyTCBChanged(int nIndex)
{
	assert(ValidKeyIndex(nIndex));

	if(GetNumKeys() < 2)
	{
		return;
	}

	int nPrevious = nIndex - 1, nNext = nIndex + 1;

	if(nIndex == 0)
	{
		nPrevious = 0;
	}

	if(nIndex == GetNumKeys() - 1)
	{
		nNext = GetNumKeys() - 1;
	}

	GetTangents(m_vecKeys[nPrevious], m_vecTCBKeys[nIndex], m_vecKeys[nNext], &m_vecKeys[nIndex].v3TanIn, &m_vecKeys[nIndex].v3TanOut);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineTCB
//  - Prototype : void GetTangents(	const CSpline::TKey& keyPrev, 
//									const TKey& key,
//									const CSpline::TKey& keyNext,
//									CVector3* pv3TanIn,
//									CVector3* pv3TanOut)
//
//
//  - Purpose   : Computes key's tangents using its tcb values and the
//                previous and next key.
//
// -----------------------------------------------------------------------------
void CSplineTCB::GetTangents(	const CSpline::TKey& keyPrev, 
								const TKey& key,
								const CSpline::TKey& keyNext,
								CVector3* pv3TanIn,
								CVector3* pv3TanOut)
{
	assert(pv3TanIn);
	assert(pv3TanOut);

	*pv3TanIn	=	((key.v3Value - keyPrev.v3Value) *
					(((1.0f - key.fTension) * (1.0f - key.fContinuity) * (1.0f + key.fBias)) * 0.5f)) +
					((keyNext.v3Value - key.v3Value) *
					(((1.0f - key.fTension) * (1.0f + key.fContinuity) * (1.0f - key.fBias)) * 0.5f));

	*pv3TanOut	=	((key.v3Value - keyPrev.v3Value) *
					(((1.0f - key.fTension) * (1.0f + key.fContinuity) * (1.0f + key.fBias)) * 0.5f)) +
					((keyNext.v3Value - key.v3Value) *
					(((1.0f - key.fTension) * (1.0f - key.fContinuity) * (1.0f - key.fBias)) * 0.5f));					
}