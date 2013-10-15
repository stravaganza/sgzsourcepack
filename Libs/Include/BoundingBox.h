// ==[ File ]===================================================================================
//
//  -Name     : BoundingBox.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CBoundingBox definition.
//
// =============================================================================================

/*

	04/12/2002  -  File created.


*/


#ifndef __BOUNDINGBOX_H
#define __BOUNDINGBOX_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CBoundingBox
//
//  - Purpose   : Bounding box class.
//
// ---------------------------------------------------------------------------
class CBoundingBox : public CBoundingVolume
{


public:


	CBoundingBox();
	~CBoundingBox();

	bool Build    (const CObject* pObject);
	bool Visible  (const CCamera* pCamera);
	bool Transform(const CMatrix& matrix);


protected:


	CVector3 m_pv3LocalCorners[8];
	CVector3 m_pv3WorldCorners[8];
	CVector3 m_v3LocalCenter;
	CVector3 m_v3WorldCenter;

};


#endif