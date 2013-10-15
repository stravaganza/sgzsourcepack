// ==[ File ]===================================================================================
//
//  -Name     : CurveSplineCatmullRom.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSplineCatmullRom implementation (see SplineCatmullRom.h).
//              Important note! The spline is actually faked to be compatible with the base
//              class. It's not really a catmull-rom spline.
//
// =============================================================================================

/*

	28/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSplineCatmullRom.h"


const std::string CSplineCatmullRom::CLASSNAME("Spline Catmull-Rom");

REGISTER_PLUGIN(CPluginFactory, CSplineCatmullRom)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineCatmullRom
//  - Prototype : bool Parse(CAdvancedParser* pParser)
//
//  - Purpose   : Gets the data parsing the keys from a source.
//
// -----------------------------------------------------------------------------
bool CSplineCatmullRom::Parse(CAdvancedParser* pParser)
{
	assert(pParser);
	assert(pParser->Ready());

	Delete();

	CSplineCatmullRom::TKey key;

	while(pParser->ReadKeyFloat("#time", '=', &key.fTime, CAdvancedParser::SEARCH_FROMCURPOS))
	{
		if(pParser->ReadKeyVector3("#value", '=', &key.v3Value, CAdvancedParser::SEARCH_FROMCURPOSTOLINEEND))
		{
			AddKey(key);
		}
		else
		{
			LOG.Write("\nERROR - CSplineCatmullRom::Parse(): #value not found when parsing a line.");
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineCatmullRom
//  - Prototype : bool WriteASCII(CWriterASCII* pWriter)
//
//  - Purpose   : Writes the keys in ASCII info to a destiny.
//
// -----------------------------------------------------------------------------
bool CSplineCatmullRom::WriteASCII(CWriterASCII* pWriter)
{
	assert(pWriter);
	assert(pWriter->Ready());

	std::vector<CSpline::TKey>::const_iterator it;

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
//  - Class     : CSplineCatmullRom
//  - Prototype : CMatrix GetMatrix() const;
//
//  - Purpose   : Returns the CatmullRom matrix (the Hermite one).
//
// -----------------------------------------------------------------------------
CMatrix CSplineCatmullRom::GetMatrix() const
{
	return CMatrix(	 2.0f, -2.0f,  1.0f,  1.0f,
					-3.0f,  3.0f, -2.0f, -1.0f,
					 0.0f,  0.0f,  1.0f,  0.0f,
					 1.0f,  0.0f,  0.0f,  0.0f);
/*	return CMatrix(	-1.0f * 0.5f,  3.0f * 0.5f, -3.0f * 0.5f,  1.0f * 0.5f,
					 2.0f * 0.5f, -5.0f * 0.5f,  4.0f * 0.5f, -1.0f * 0.5f,
					-1.0f * 0.5f,  0.0f       ,  1.0f * 0.5f,  0.0f,
					 0.0f       ,  2.0f * 0.5f,  0.0f       ,  0.0f);*/
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineCatmullRom
//  - Prototype : bool AddKey(const TKey& key)
//
//  - Purpose   : Adds a key to the set. Tangent values are computed using
//                the previous and the next keys Ti = 0.5 * (p[i+1] - p[i-1])
//
//  - Note      : Special cases first and last key are dealed duplicating
//                these keys (remember, we need p-1 and p+1). Duplicating means
//                only that we consider the previous key to the first as the
//                same and also the next to the last as the last itself.
//
// -----------------------------------------------------------------------------
bool CSplineCatmullRom::AddKey(const TKey& key)
{
	CSpline::TKey newKey;

	newKey.fTime   = key.fTime;
	newKey.v3Value = key.v3Value;

	// List is empty?

	if(m_vecKeys.empty())
	{
		newKey.v3TanIn.Set(0, 0, 0);
		newKey.v3TanOut.Set(0, 0, 0);
		m_vecKeys.push_back(newKey);
		return true;
	}

	// Secuencial search for a t greater than key.time:

	std::vector<CSpline::TKey>::iterator it;

	for(it = m_vecKeys.begin(); it != m_vecKeys.end(); ++it)
	{
		if(it->fTime > key.fTime)
		{
			// We know there's at least a key after the one being inserted
			// (key.time < keylist[i].time), so the insertion can be only
			// at the list's start or between two keys.

			int nIndex, nLast;
			
			nIndex = it - m_vecKeys.begin();
			nLast  = GetNumKeys() - 1;

			if(nIndex == 0)
			{
				// Added as the first key.
				GetTangents(newKey, m_vecKeys.front(), &newKey.v3TanIn, &newKey.v3TanOut);
			}
			else
			{
				// Added in between two keys:
				GetTangents(m_vecKeys[nIndex - 1], m_vecKeys[nIndex], &newKey.v3TanIn, &newKey.v3TanOut);
			}

			m_vecKeys.insert(it, newKey);

			NotifyKeyValueChanged(nIndex);
			return true;
		}
	}

	// Added at the end of the list.

	GetTangents(m_vecKeys.back(), newKey, &newKey.v3TanIn, &newKey.v3TanOut);
	m_vecKeys.push_back(newKey);

	NotifyKeyValueChanged(GetNumKeys() - 1);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineCatmullRom
//  - Prototype : void NotifyKeyValueChanged(int nIndex)
//
//  - Purpose   : This method is called when a key value has been changed in
//                order to recompute its neighbour tangent values.
//                Special cases are treated as explained in AddKey().
//
// -----------------------------------------------------------------------------
void CSplineCatmullRom::NotifyKeyValueChanged(int nIndex)
{
	// Adjust previous key's tangents if there's one

	if(nIndex > 0)
	{
		// Is key[nIndex-1] the first? Duplicate then, otherwise no need to duplicate.

		if(nIndex == 1)
		{
			GetTangents(m_vecKeys[0], m_vecKeys[1], &m_vecKeys[0].v3TanIn, &m_vecKeys[0].v3TanOut);
		}
		else
		{
			GetTangents(m_vecKeys[nIndex - 2], m_vecKeys[nIndex], &m_vecKeys[nIndex - 1].v3TanIn, &m_vecKeys[nIndex - 1].v3TanOut);
		}
	}

	// Adjust next key's tangents if there's one

	if(nIndex < GetNumKeys() - 1)
	{
		// Is key[nIndex+1] the last? Duplicate then, otherwise no need to duplicate.

		int nLast = GetNumKeys() - 1;

		if(nIndex + 1 == nLast)
		{
			GetTangents(m_vecKeys[nIndex], m_vecKeys[nLast], &m_vecKeys[nLast].v3TanIn, &m_vecKeys[nLast].v3TanOut);
		}
		else
		{
			GetTangents(m_vecKeys[nIndex], m_vecKeys[nIndex + 2], &m_vecKeys[nIndex + 1].v3TanIn, &m_vecKeys[nIndex + 1].v3TanOut);
		}
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineCatmullRom
//  - Prototype : void GetTangents(	const CSpline::TKey& keyPrev, 
//									const CSpline::TKey& keyNext,
//									CVector3* pv3TanIn,
//									CVector3* pv3TanOut)
//
//
//  - Purpose   : Computes key's tangents using catmull rom formula.
//
// -----------------------------------------------------------------------------
void CSplineCatmullRom::GetTangents(const CSpline::TKey& keyPrev, 
									const CSpline::TKey& keyNext,
									CVector3* pv3TanIn,
									CVector3* pv3TanOut)
{
	assert(pv3TanIn);
	assert(pv3TanOut);

	*pv3TanIn  = (keyNext.v3Value - keyPrev.v3Value) * 0.5f;
	*pv3TanOut = (keyNext.v3Value - keyPrev.v3Value) * 0.5f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSplineCatmullRom
//  - Prototype : void RemoveTangents()
//
//
//  - Purpose   : Removes the Y and Z components of the spline tangents, useful
//                when we're editing 2D (X, Y) splines and we don't want values
//                to go outside the vertical range.
//
// -----------------------------------------------------------------------------
void CSplineCatmullRom::RemoveTangentsYZ()
{
	m_bRemoveTangentsYZ = true;
}
