// ==[ File ]===================================================================================
//
//  -Name     : Ray.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CRay implementation.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CRay data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CRay::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	int nError = 0;

	if(!m_point.Read(pInput))     nError++;
	if(!m_direction.Read(pInput)) nError++;

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CRay data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CRay::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	int nError = 0;

	if(!m_point.Write    (pOutput)) nError++;
	if(!m_direction.Write(pOutput)) nError++;

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - prototype : float Angle(const CRay& ray)
//
//  - Purpose   : Computes the angle between 2 rays.
//
//  - Note      : Computes the smallest of the two possible angles.
//
// -----------------------------------------------------------------------------
float CRay::Angle(const CRay& ray) const
{
	return m_direction.Angle(ray);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - prototype : float Angle(const CPlane& plane)
//
//  - Purpose   : Computes the angle between the ray and a plane.
//
//  - Note      : Computes the smallest of the two possible angles.
//
// -----------------------------------------------------------------------------
float CRay::Angle(const CPlane& plane) const
{
	return m_direction.Angle(plane);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - prototype : float Angle(const CVector3& vector)
//
//  - Purpose   : Computes the angle between the ray and a vector.
//
//  - Note      : Computes the smallest of the two possible angles.
//
// -----------------------------------------------------------------------------
float CRay::Angle(const CVector3& vector) const
{
	return vector.Angle(*this);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - prototype : bool Intersection(const CPlane& plane, CVector3* intersection)
//
//  - Purpose   : Computes the intersection between the ray and a plane.
//
//  - Note      : Returns false if intersection doesn't exist (ray and plane
//                are parallel), true otherwise.
//                If succesfull, the result is returned in the second parameter.
//
// -----------------------------------------------------------------------------
bool CRay::Intersection(const CPlane& plane, CVector3* intersection) const
{
	float fT;
	float fNumerator, fDenominator;

	fDenominator = plane.A() * m_direction.X() + plane.B() * m_direction.Y() + plane.C() * m_direction.Z();

	if(IS_ZERO(fDenominator)) // ray and plane are parallel
	{
		return false;
	}

	fNumerator = plane.A() * m_point.X() + plane.B() * m_point.Y() + plane.C() * m_point.Z() + plane.D();

	fT = -(fNumerator / fDenominator);

	intersection->SetX(m_point.X() + m_direction.X() * fT);
	intersection->SetY(m_point.Y() + m_direction.Y() * fT);
	intersection->SetZ(m_point.Z() + m_direction.Z() * fT);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRay
//  - prototype : bool Reflexion(const CPlane& plane, CRay* reflexion)
//
//  - Purpose   : Computes the reflexion of the ray when hitting the given
//                plane.
//
//  - Note      : Returns false if intersection doesn't exist (ray and plane
//                are parallel), true otherwise.
//                If succesfull, the result is returned in the second parameter.
//
// -----------------------------------------------------------------------------
bool CRay::Reflexion(const CPlane& plane, CRay* reflexion) const
{
	CVector3 intersection;
	CVector3 reflexionDir;

	if(Intersection(plane, &intersection) == false)
	{
		return false;
	}

	CVector3 normal(plane.Normal());
	normal.Normalize();

	// Calculamos el vector reflejado.

	reflexionDir = (normal * ((-m_direction * normal) * 2.0f)) + m_direction;

	// Define the reflexion ray using the intersection (ray-plane) point and
	// the direction of the reflexion.

	reflexion->Build(intersection, reflexionDir);

	return true;
}