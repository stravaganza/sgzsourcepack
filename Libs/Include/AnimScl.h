// ==[ File ]===================================================================================
//
//  -Name     : AnimScl.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimScl definition.
//
// =============================================================================================

/*

	22/10/2002  -  File created.


*/


#ifndef __ANIMSCL_H
#define __ANIMSCL_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimScl
//
//  - Purpose   : Evaluatable 3D scale value base class.
//
// ---------------------------------------------------------------------------
class CAnimScl : public CAnimation
{


public:


	CAnimScl() : CAnimation() { }
	virtual ~CAnimScl() { }

	virtual bool Evaluate(float fTime, CQuaternion* pAxisOut, CVector3* pv3ScaleOut) const = 0;

};


#endif