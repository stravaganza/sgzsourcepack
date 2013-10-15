// ==[ File ]===================================================================================
//
//  -Name     : BoundingBox.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CBoundingBox ìmplementation.
//
// =============================================================================================

/*

	04/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBoundingBox
//  - prototype : CBoundingBox()
//
//  - Purpose   : CBoundingBox's default constructor.
//
// -----------------------------------------------------------------------------
CBoundingBox::CBoundingBox()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBoundingBox
//  - prototype : ~CBoundingBox()
//
//  - Purpose   : CBoundingBox's destructor.
//
// -----------------------------------------------------------------------------
CBoundingBox::~CBoundingBox()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBoundingBox
//  - prototype : bool Build(const CObject* pObject)
//
//  - Purpose   : Builds the bounding box using the geometry of a given object.
//
// -----------------------------------------------------------------------------
bool CBoundingBox::Build(const CObject* pObject)
{
	CVector3 v3Min(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX);
	CVector3 v3Max(FLOAT_MIN, FLOAT_MIN, FLOAT_MIN);

	// Iterate over all vertices, getting min and max limits:

	for(int i = 0; i < pObject->GetNumVertices(); i++)
	{
		CVector3 v = pObject->GetVertices()[i];

		if(v.X() < v3Min.X()) v3Min.SetX(v.X());
		if(v.Y() < v3Min.Y()) v3Min.SetY(v.Y());
		if(v.Z() < v3Min.Z()) v3Min.SetZ(v.Z());

		if(v.X() > v3Max.X()) v3Max.SetX(v.X());
		if(v.Y() > v3Max.Y()) v3Max.SetY(v.Y());
		if(v.Z() > v3Max.Z()) v3Max.SetZ(v.Z());
	}

	// Build corner data:

	m_pv3LocalCorners[0].Set(v3Min.X(), v3Min.Y(), v3Min.Z());
	m_pv3LocalCorners[1].Set(v3Min.X(), v3Min.Y(), v3Max.Z());
	m_pv3LocalCorners[2].Set(v3Min.X(), v3Max.Y(), v3Min.Z());
	m_pv3LocalCorners[3].Set(v3Min.X(), v3Max.Y(), v3Max.Z());
	m_pv3LocalCorners[4].Set(v3Max.X(), v3Min.Y(), v3Min.Z());
	m_pv3LocalCorners[5].Set(v3Max.X(), v3Min.Y(), v3Max.Z());
	m_pv3LocalCorners[6].Set(v3Max.X(), v3Max.Y(), v3Min.Z());
	m_pv3LocalCorners[7].Set(v3Max.X(), v3Max.Y(), v3Max.Z());

	m_v3LocalCenter = (v3Min + v3Max) * 0.5f;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBoundingBox
//  - prototype : bool Visible(const CCamera* pCamera)
//
//  - Purpose   : Returns true if the bounding box is inside camera's frustum,
//                false if not.
//
// -----------------------------------------------------------------------------
bool CBoundingBox::Visible(const CCamera* pCamera)
{
	int nCornerCount;

	// Center

	if(pCamera->TestAgainstFrustum(m_v3WorldCenter))
	{
		return true;
	}

	for(unsigned uPlaneCount = 0; uPlaneCount < 6; uPlaneCount++)
	{
		for(nCornerCount = 0; nCornerCount < 8; nCornerCount++)
		{
			if(m_pv3WorldCorners[nCornerCount].IsOnPositiveSide(pCamera->FrustumPlane(uPlaneCount)))
			{
				break;
			}
		}

		// ¿All vertices on the negative side?

		if(nCornerCount == 8)
		{
/*			switch(uPlaneCount)
			{

			case CCamera::FRUSTUM_PLANE_NEAR   : LOG.Write("\nCBoundingBox::Visible() - Test failed on plane NEAR");   break;
			case CCamera::FRUSTUM_PLANE_FAR    : LOG.Write("\nCBoundingBox::Visible() - Test failed on plane FAR");    break;
			case CCamera::FRUSTUM_PLANE_TOP    : LOG.Write("\nCBoundingBox::Visible() - Test failed on plane TOP");    break;
			case CCamera::FRUSTUM_PLANE_BOTTOM : LOG.Write("\nCBoundingBox::Visible() - Test failed on plane BOTTOM"); break;
			case CCamera::FRUSTUM_PLANE_LEFT   : LOG.Write("\nCBoundingBox::Visible() - Test failed on plane LEFT");   break;
			case CCamera::FRUSTUM_PLANE_RIGHT  : LOG.Write("\nCBoundingBox::Visible() - Test failed on plane RIGHT");  break;

			default: break;
			}
*/			
			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBoundingBox
//  - prototype : bool Transform(const CMatrix& matrix)
//
//  - Purpose   : Transforms the bounding box with the given transformation matrix.
//
// -----------------------------------------------------------------------------
bool CBoundingBox::Transform(const CMatrix& matrix)
{
	for(int i = 0; i < 8; i++)
	{
		m_pv3WorldCorners[i] = m_pv3LocalCorners[i];
		matrix.Transform(&m_pv3WorldCorners[i]);
	}

	m_v3WorldCenter = m_v3LocalCenter;
	matrix.Transform(&m_v3WorldCenter);

	return true;
}
