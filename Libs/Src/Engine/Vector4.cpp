// ==[ File ]===================================================================================
//
//  -Name     : CVector4.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVector4 implementation.
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
//  - Class     : CVector4
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CVector4 data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CVector4::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	return pInput->ReadRawData(sizeof(float) * 4, 1, &m_fX) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector4
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CVector4 data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CVector4::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	return pOutput->WriteRawData(sizeof(float) * 4, 1, &m_fX) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector4
//  - Prototype : CVector4 LERP(const CVector4& dst, float fT) const
//
//  - Purpose   : Computes the linear interpolation between this and a
//                destiny CVector4.
//
// -----------------------------------------------------------------------------
CVector4 CVector4::LERP(const CVector4& dst, float fT) const
{
	CVector4 v4Ret;

	v4Ret.m_fX = m_fX + (dst.m_fX - m_fX) * fT;
	v4Ret.m_fY = m_fY + (dst.m_fY - m_fY) * fT;
	v4Ret.m_fZ = m_fZ + (dst.m_fZ - m_fZ) * fT;
	v4Ret.m_fW = m_fW + (dst.m_fW - m_fW) * fT;

	return v4Ret;
}
