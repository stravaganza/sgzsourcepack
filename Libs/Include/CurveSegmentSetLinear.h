// ==[ File ]===================================================================================
//
//  -Name     : CurveSegmentSetLinear.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSegmentSetLinear definition.
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


#ifndef __CURVESEGMENTSETLINEAR_H
#define __CURVESEGMENTSETLINEAR_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSegmentSetLinear
//
//  - Purpose   : Set of control points which evaluation is done interpolating
//                linearly between the two values that define a segment.
//
// ---------------------------------------------------------------------------
class CSegmentSetLinear : public CSegmentSet
{

	
public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CSegmentSetLinear()  {           }
	~CSegmentSetLinear() { Delete(); }

	CVector3 Interpolate(const CVector3& v3Start, const CVector3& v3End, float fT) const;

};


#endif