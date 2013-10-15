// ==[ File ]===================================================================================
//
//  -Name     : CVector2.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVector2 implementation.
//
// =============================================================================================

/*

	01/12/2002  -  File created.


*/



// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementations ]==================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector2
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CVector2 data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CVector2::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	return pInput->ReadRawData(sizeof(float) * 2, 1, &m_fX) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector2
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CVector2 data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CVector2::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	return pOutput->WriteRawData(sizeof(float) * 2, 1, &m_fX) == 1;
}

