// ==[ File ]===================================================================================
//
//  -Name     : Animation.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimation implementation (see Animation.h).
//
// =============================================================================================

/*

	10/11/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimation
//  - Prototype : static CAnimation* GetNewInstance(int nID)
//
//  - Purpose   : Static method that returns a new CAnimation child instance
//                based on an ID.
//                This is mainly used to get the proper instance when we nead
//                to read animation data from a file, and we only know which
//                kind of animation it is from an ID. With this instance we
//                can call then the Read() method.
//
// -----------------------------------------------------------------------------
CAnimation* CAnimation::GetNewInstance(int nID)
{
	switch(nID)
	{

	case ID_ANIM_POSSAMPLED: return new CAnimPosSampled;
	case ID_ANIM_ROTSAMPLED: return new CAnimRotSampled;
	case ID_ANIM_SCLSAMPLED: return new CAnimSclSampled;

	default: break;

	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimation
//  - Prototype : bool ValidTime(float fTime) const
//
//  - Purpose   : Checks if fTime is in the animation's valid time range.
//
// -----------------------------------------------------------------------------
bool CAnimation::ValidTime(float fTime) const
{
	if(Looped())
	{
		return true;
	}

	float fTStart, fTEnd;

	if(!GetStartTime(&fTStart)) return false;
	if(!GetEndTime(&fTEnd))     return false;

	if(fTStart > fTime) return false;
	if(fTEnd   < fTime) return false;

	return true;
}