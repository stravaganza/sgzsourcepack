// ==[ File ]===================================================================================
//
//  -Name     : CVector3.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVector3 implementation.
//
// =============================================================================================

/*

	25/09/2001  -  File created.
	01/12/2002  -  Added Read/Write methods


*/



// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementations ]==================================================================


CVector3 CVector3::NULLVECTOR = CVector3(0.0f, 0.0f, 0.0f);


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CVector3 data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CVector3::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	return pInput->ReadRawData(sizeof(float) * 3, 1, &m_fX) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CVector3 data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CVector3::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	return pOutput->WriteRawData(sizeof(float) * 3, 1, &m_fX) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : void Build(const CVector3& src, const CVector3& dst)
//
//  - Purpose   : Builds a vector given 2 points, from src to dst.
//
// -----------------------------------------------------------------------------
void CVector3::Build(const CVector3& src, const CVector3& dst)
{
	m_fX = dst.m_fX - src.m_fX;
	m_fY = dst.m_fY - src.m_fY;
	m_fZ = dst.m_fZ - src.m_fZ;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : float Length()
//
//  - Purpose   : Returns vector's length.
//
// -----------------------------------------------------------------------------
float CVector3::Length() const
{
	return sqrtf((m_fX * m_fX) + (m_fY * m_fY) + (m_fZ * m_fZ));
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : void Normalize()
//
//  - Purpose   : Normalizes the vector.
//
// ---------------------------------------------------------------------------
void CVector3::Normalize()
{

#ifdef _DEBUG
	if(IS_ZERO(Length()))
	{
		//LOG.Write("\nWARNING - CVector3::Normalize(): Length = %f", Length());
		return;
	}
#endif

	float fRecModulus = 1.0f / Length();

	m_fX *= fRecModulus;
	m_fY *= fRecModulus;
	m_fZ *= fRecModulus;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 LERP(const CVector3 dst, float fT)
//
//  - Purpose   : Returns the result of interpolating linearly to dst by a
//                factor t, being 0.0f <= t <= 1.0f.
//
// ---------------------------------------------------------------------------
CVector3 CVector3::LERP(const CVector3& dst, float fT) const
{
	CVector3 result;

	result.m_fX = m_fX + (dst.m_fX - m_fX) * fT;
	result.m_fY = m_fY + (dst.m_fY - m_fY) * fT;
	result.m_fZ = m_fZ + (dst.m_fZ - m_fZ) * fT;

	return result;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : float Angle(const CVector3& vector)
//
//  - Purpose   : Returns the angle between two vectors.
//
// ---------------------------------------------------------------------------
float CVector3::Angle(const CVector3& vector) const
{
	float fDotProduct = m_fX * vector.m_fX + m_fY * vector.m_fY + m_fZ * vector.m_fZ;
	float fArg        = fDotProduct / (Length() * vector.Length());

#ifdef _DEBUG

	if(fArg < -1.0f || fArg > 1.0f)
	{
		CLogger::ErrorWindow("acosf arg %f (%f, %f, %f)*(%f, %f, %f)", fArg, m_fX, m_fY, m_fZ, vector.m_fX, vector.m_fY, vector.m_fZ);
	}

#endif

	return RAD_TO_DEG(acosf(fArg));
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : float Angle(const CRay& ray)
//
//  - Purpose   : Returns the angle between the vector and a ray.
//
// ---------------------------------------------------------------------------
float CVector3::Angle(const CRay& ray) const
{
	float fAngle = Angle(ray.Direction());

	if(fAngle > 90.0f) fAngle = 180.0f - fAngle;

	return fAngle;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : float Angle(const CRay& ray)
//
//  - Purpose   : Returns the angle between the vector and a plane.
//
// ---------------------------------------------------------------------------
float CVector3::Angle(const CPlane& plane) const
{
	float fAngle = Angle(plane.Normal());

	if(fAngle > 90.0f) fAngle = 180.0f - fAngle;

	return 90.0f - fAngle;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : bool IsOnPositiveSide(const CPlane& plane)
//
//  - Purpose   : Returns true if the (handled as) point is on the positive
//                side of the plane, otherwise false.
//                The positive side is the pointed by the plane's normal.
//
// ---------------------------------------------------------------------------
bool CVector3::IsOnPositiveSide(const CPlane& plane) const
{
	return ((m_fX * plane.A() + m_fY * plane.B() + m_fZ * plane.C() + plane.D()) > 0.0f);
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : float Distance(const CPlane& plane)
//
//  - Purpose   : Computes the distance from the vector3 (handled as vertex)
//                to the plane.
//
// ---------------------------------------------------------------------------
float CVector3::Distance(const CPlane& plane) const
{
	return (MYFABSF(m_fX * plane.A() + m_fY * plane.B() + m_fZ * plane.C() + plane.D()) /
				sqrtf(plane.A() * plane.A() + plane.B() * plane.B() + plane.C() * plane.C()));
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : bool DistanceFast(const CPlane& plane)
//
//  - Purpose   : Does the same as the previous one but much faster.
//                Requires the plane to be defined with a unity length normal.
//
// ---------------------------------------------------------------------------
float CVector3::DistanceFast(const CPlane& plane) const
{
	return MYFABSF(m_fX * plane.A() + m_fY * plane.B() + m_fZ * plane.C() + plane.D());
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator * (float fValue)
//
//  - Purpose   : Operator vector multiplied by a scalar.
//
// ---------------------------------------------------------------------------
CVector3 CVector3::operator * (float fValue) const
{
	return CVector3(m_fX * fValue, m_fY * fValue, m_fZ * fValue);
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator / (float fValue)
//
//  - Purpose   : Operator vector divided by a scalar.
//
// ---------------------------------------------------------------------------
CVector3 CVector3::operator / (float fValue) const
{
	// 1 division + 3 mults cheaper than 3 divisions.

	float fReciprocal = 1.0f / fValue;

	return CVector3(m_fX * fReciprocal, m_fY * fReciprocal, m_fZ * fReciprocal);
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator * (const CVector3& vector)
//
//  - Purpose   : Operator * to multiply 2 vectors (Dot product).
//
// ---------------------------------------------------------------------------
float CVector3::operator * (const CVector3& vector) const
{
	return m_fX * vector.m_fX + m_fY * vector.m_fY + m_fZ * vector.m_fZ;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator + (const CVector3& vector)
//
//  - Purpose   : Operator + to add 2 vectors together.
//
// ---------------------------------------------------------------------------
CVector3 CVector3::operator + (const CVector3& vector) const
{
	return CVector3(m_fX + vector.m_fX, m_fY + vector.m_fY, m_fZ + vector.m_fZ);
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator - (const CVector3& vector)
//
//  - Purpose   : Operator - to substract a vector from this one.
//
// ---------------------------------------------------------------------------
CVector3 CVector3::operator - (const CVector3& vector) const
{
	return CVector3(m_fX - vector.m_fX, m_fY - vector.m_fY, m_fZ - vector.m_fZ);
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator ^ (const CVector3& vector)
//
//  - Purpose   : Operator ^ to compute the cross product between 2 vectors.
//
// ---------------------------------------------------------------------------
CVector3 CVector3::operator ^ (const CVector3& vector) const
{
	CVector3 result;

	result.m_fX =   (m_fY * vector.m_fZ) - (m_fZ * vector.m_fY);
	result.m_fY = -((m_fX * vector.m_fZ) - (m_fZ * vector.m_fX));
	result.m_fZ =   (m_fX * vector.m_fY) - (m_fY * vector.m_fX);

	return result;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : CVector3 operator = (const CVector3& vector)
//
//  - Purpose   : Assignement operator.
//
// ---------------------------------------------------------------------------
CVector3& CVector3::operator = (const CVector3& vector)
{
	m_fX = vector.m_fX;
	m_fY = vector.m_fY;
	m_fZ = vector.m_fZ;

	return *this;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : bool operator == (const CVector3& vector) const
//
//  - Purpose   : Comparison operator
//
// ---------------------------------------------------------------------------
bool CVector3::operator == (const CVector3& vector) const
{
	if(!ARE_EQUAL(vector.X(), m_fX)) return false;
	if(!ARE_EQUAL(vector.Y(), m_fY)) return false;
	if(!ARE_EQUAL(vector.Z(), m_fZ)) return false;

	return true;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CVector3
//  - Prototype : bool operator != (const CVector3& vector) const
//
//  - Purpose   : Comparison operator
//
// ---------------------------------------------------------------------------
bool CVector3::operator != (const CVector3& vector) const
{
	if(!ARE_EQUAL(vector.X(), m_fX)) return true;
	if(!ARE_EQUAL(vector.Y(), m_fY)) return true;
	if(!ARE_EQUAL(vector.Z(), m_fZ)) return true;

	return false;
}