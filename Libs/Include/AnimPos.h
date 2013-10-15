// ==[ File ]===================================================================================
//
//  -Name     : AnimPos.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimPos definition.
//
// =============================================================================================

/*

	22/10/2002  -  File created.


*/


#ifndef __ANIMPOS_H
#define __ANIMPOS_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimPos
//
//  - Purpose   : Evaluatable 3D position value base class.
//
// ---------------------------------------------------------------------------
class CAnimPos : public CAnimation
{


public:


	CAnimPos() : CAnimation() { }
	virtual ~CAnimPos() { }

	virtual bool Evaluate(float fTime, CVector3* pv3Out) const = 0;

};


#endif