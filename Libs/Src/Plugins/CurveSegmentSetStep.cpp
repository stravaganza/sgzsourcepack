// ==[ File ]===================================================================================
//
//  -Name     : CurveSegmentSetStep.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSegmentSetStep implementation (see SegmentSetStep.h).
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"
#include "CurveSegmentSetStep.h"


const std::string CSegmentSetStep::CLASSNAME("Step Segment Set");

REGISTER_PLUGIN(CPluginFactory, CSegmentSetStep)


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSegmentSetStep
//  - Prototype : CVector3 Interpolate(const CVector3& v3Start,
//                                     const CVector3& v3End,
//                                     float fT) const
//
//  - Purpose   : No interpolation, only takes the first one of the two values.
//                The result is that a value remains unchanged until a new
//                key appears.
//
// -----------------------------------------------------------------------------
CVector3 CSegmentSetStep::Interpolate(const CVector3& v3Start, const CVector3& v3End, float fT) const
{
	return v3Start;
}