// ==[ File ]===================================================================================
//
//  -Name     : AnimRot.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimRot definition.
//
// =============================================================================================

/*

	22/10/2002  -  File created.


*/


#ifndef __ANIMROT_H
#define __ANIMROT_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimRot
//
//  - Purpose   : Evaluatable 3D rotation value base class.
//
// ---------------------------------------------------------------------------
class CAnimRot : public CAnimation
{


public:


	CAnimRot() : CAnimation() { }
	virtual ~CAnimRot() { }

	virtual bool Evaluate(float fTime, CQuaternion* pQuatOut) const = 0;

};


#endif