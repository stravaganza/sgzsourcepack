// ==[ File ]===================================================================================
//
//  -Name     : Plane.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPlane implementation.
//
// =============================================================================================

/*

	25/09/2001  -  File created.


*/



// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementations ]==================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CPlane data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CPlane::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	return pInput->ReadRawData(sizeof(float) * 4, 1, &m_fA) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CPlane data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CPlane::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	return pOutput->WriteRawData(sizeof(float) * 4, 1, &m_fA) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - prototype : void Set(float fA, float fB, float fC, float fD)
//
//  - Purpose   : Sets the four coefficients of the plane equation.
//                (Given a plane Ax+By+Cz+D=0)
//
// -----------------------------------------------------------------------------
void CPlane::Set(float fA, float fB, float fC, float fD)
{
	m_fA = fA;
	m_fB = fB;
	m_fC = fC;
	m_fD = fD;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : void Build(const CVector3& point, const CVector3& normal)
//
//  - Purpose   : Builds a plane given a point laying on it and the plane's
//                normal.
//
// ---------------------------------------------------------------------------
void CPlane::Build(const CVector3& point, const CVector3& normal)
{
	m_fA = normal.X();
	m_fB = normal.Y();
	m_fC = normal.Z();

	m_fD = -((m_fA * point.X()) + (m_fB * point.Y()) + (m_fC * point.Z()));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : void Build(const CVector3& point1,
//                           const CVector3& point2,
//                           const CVector3& point3)
//
//  - Purpose   : Builds a plane given 3 points laying on it.
//
// ---------------------------------------------------------------------------
void CPlane::Build(const CVector3& point1, const CVector3& point2, const CVector3& point3)
{
	Build(point1, CVector3(point1, point2) ^ CVector3(point1, point3));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : float Angle(const CPlane& plane)
//
//  - Purpose   : Returns the angle between the two planes.
//
// ---------------------------------------------------------------------------
float CPlane::Angle(const CPlane& plane) const
{
	return Normal().Angle(plane.Normal());
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : float Angle(const CRay& ray)
//
//  - Purpose   : Returns the angle between the plane and a ray.
//
// ---------------------------------------------------------------------------
float CPlane::Angle(const CRay& ray) const
{
	return ray.Angle(*this);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : float Angle(const CVector3& vector)
//
//  - Purpose   : Returns the angle between the plane and a vector.
//
// ---------------------------------------------------------------------------
float CPlane::Angle(const CVector3& vector) const
{
	return vector.Angle(*this);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : bool Intersection(const CRay& ray, CVector3* intersection)
//
//  - Purpose   : Computes the intersection between the plane and a given ray.
//                The result is a point and it's stored in the second argument.
//                If there is a result, the method returns true and the
//                point is stored. If there isn't any result (Plane and ray
//                are parallel), the method returns false and the second
//                parameter remains unmodified.
//
// ---------------------------------------------------------------------------
bool CPlane::Intersection(const CRay& ray, CVector3* intersection) const
{
	return ray.Intersection(*this, intersection);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : bool Intersection(const CPlane& plane, CRay* intersection)
//
//  - Purpose   : The same as the previous method, but this one computes
//                the intersection between two planes. The result is a ray.
//
// ---------------------------------------------------------------------------
bool CPlane::Intersection(const CPlane& plano, CRay* intersection) const
{

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPlane
//  - Prototype : bool Intersection(const CPlane& plane, CRay* intersection)
//
//  - Purpose   : Assignement operator.
//
// ---------------------------------------------------------------------------
CPlane& CPlane::operator = (const CPlane& plane)
{
	m_fA = plane.m_fA;
	m_fB = plane.m_fB;
	m_fC = plane.m_fC;
	m_fD = plane.m_fD;

	return *this;
}

