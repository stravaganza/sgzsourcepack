// ==[ File ]===================================================================================
//
//  -Name     : BoundingVolume.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CBoundingVolume definition.
//
// =============================================================================================

/*

	04/12/2002  -  File created.


*/


#ifndef __BOUNDINGVOLUME_H
#define __BOUNDINGVOLUME_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CBoundingVolume
//
//  - Purpose   : Bounding box class.
//
// ---------------------------------------------------------------------------
class CBoundingVolume
{


public:


	CBoundingVolume()          { }
	virtual ~CBoundingVolume() { }

	virtual bool Build    (const CObject* pObject) = 0;
	virtual bool Visible  (const CCamera* pCamera) = 0;
	virtual bool Transform(const CMatrix& matrix)  = 0;
};


#endif