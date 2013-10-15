// ==[ File ]===================================================================================
//
//  -Name     : CurveSegmentSetStep.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSegmentSetStep definition.
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


#ifndef __CURVESEGMENTSETSTEP_H
#define __CURVESEGMENTSETSTEP_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSegmentSetStep
//
//  - Purpose   : Set of control points which evaluation is done returning
//                the first value between the two that define the segment.
//
// ---------------------------------------------------------------------------
class CSegmentSetStep : public CSegmentSet
{

	
public:


	static const std::string CLASSNAME;
	std::string GetClassName() const { return CLASSNAME; }

	CSegmentSetStep()  {           }
	~CSegmentSetStep() { Delete(); }

	CVector3 Interpolate(const CVector3& v3Start, const CVector3& v3End, float fT) const;

};


#endif