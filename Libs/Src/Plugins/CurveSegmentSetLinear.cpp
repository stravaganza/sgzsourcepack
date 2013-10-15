// ==[ File ]===================================================================================
//
//  -Name     : CurveSegmentSetLinear.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSegmentSetLinear implementation (see SegmentSetLinear.h).
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSegmentSetLinear.h"


const std::string CSegmentSetLinear::CLASSNAME("Linear Segment Set");

REGISTER_PLUGIN(CPluginFactory, CSegmentSetLinear)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSetLinear
//  - Prototype : CVector3 Interpolate(const CVector3& v3Start,
//                                     const CVector3& v3End,
//                                     float fT) const
//
//  - Purpose   : Linear interpolation algorithm.
//
// -----------------------------------------------------------------------------
CVector3 CSegmentSetLinear::Interpolate(const CVector3& v3Start, const CVector3& v3End, float fT) const
{
	return v3Start.LERP(v3End, fT);
}