// ==[ File ]===================================================================================
//
//  -Name     : Matrix.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CMatrix implementation.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementations ]==================================================================


CMatrix CMatrix::IDENTITY = CMatrix();


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CMatrix data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CMatrix::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	return pInput->ReadRawData(sizeof(float) * 16, 1, &m_fM[0][0]) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CMatrix data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CMatrix::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	return pOutput->WriteRawData(sizeof(float) * 16, 1, &m_fM[0][0]) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//  - prototype : CMatrix()
//
//  - Purpose   : Default CMatrix constructor.
//
//  - Note      : Set's an 4x4 identity matrix.
//
// -----------------------------------------------------------------------------
CMatrix::CMatrix()
{
	SetIdentity();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//  - prototype : CMatrix(const CMatrix& matrix)
//
//  - Purpose   : Copy constructor.
//
// -----------------------------------------------------------------------------
CMatrix::CMatrix(const CMatrix& matrix)
{
	*this = matrix;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//  - prototype : CMatrix(	float f00, float f01, float f02, float f03,
//							float f10, float f11, float f12, float f13,
//							float f20, float f21, float f22, float f23,
//							float f30, float f31, float f32, float f33)
//
//  - Purpose   : Constructor for specifing all the individual matrix elements.
//
// -----------------------------------------------------------------------------
CMatrix::CMatrix(	float f00, float f01, float f02, float f03,
					float f10, float f11, float f12, float f13,
					float f20, float f21, float f22, float f23,
					float f30, float f31, float f32, float f33)
{
	m_fM[0][0] = f00; m_fM[0][1] = f01; m_fM[0][2] = f02; m_fM[0][3] = f03;
	m_fM[1][0] = f10; m_fM[1][1] = f11; m_fM[1][2] = f12; m_fM[1][3] = f13;
	m_fM[2][0] = f20; m_fM[2][1] = f21; m_fM[2][2] = f22; m_fM[2][3] = f23;
	m_fM[3][0] = f30; m_fM[3][1] = f31; m_fM[3][2] = f32; m_fM[3][3] = f33;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//  - prototype : CMatrix(const CVector3& pos,
//                        const CVector3& xAxis,
//                        const CVector3& yAxis,
//                        const CVector3& zAxis)
//
//  - Purpose   : Constructor based on a space position and 3 axis.
//
// -----------------------------------------------------------------------------
CMatrix::CMatrix(const CVector3& pos, const CVector3& xAxis, const CVector3& yAxis, const CVector3& zAxis)
{
	m_fM[0][0] = xAxis.X(); m_fM[0][1] = xAxis.Y(); m_fM[0][2] = xAxis.Z(); m_fM[0][3] = pos.X();
	m_fM[1][0] = yAxis.X(); m_fM[1][1] = yAxis.Y(); m_fM[1][2] = yAxis.Z(); m_fM[1][3] = pos.Y();
	m_fM[2][0] = zAxis.X(); m_fM[2][1] = zAxis.Y(); m_fM[2][2] = zAxis.Z(); m_fM[2][3] = pos.Z();
	m_fM[3][0] = 0.0f;      m_fM[3][1] = 0.0f;      m_fM[3][2] = 0.0f;      m_fM[3][3] = 1.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix(float fAngle, float fAxisX, float fAxisY, float fAxisZ)
//
//  - Purpose   : Constructor based on a rotation of the identity matrix around
//                a three-component axis.
//
// -----------------------------------------------------------------------------
CMatrix::CMatrix(float fAngle, float fAxisX, float fAxisY, float fAxisZ)
{
	BuildRotation(fAngle, fAxisX, fAxisY, fAxisZ);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix(CQuaternion& quaternion)
//
//  - Purpose   : Constructor based on a quaternion. Creates a rotation matrix.
//
// -----------------------------------------------------------------------------
CMatrix::CMatrix(CQuaternion& quaternion)
{
	*this = quaternion.Matrix();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : SetRow0(float fC0, float fC1, float fC2, float fC3)
//
//  - Purpose   : Sets matrix's row 0 elements.
//
// -----------------------------------------------------------------------------
void CMatrix::SetRow0(float fC0, float fC1, float fC2, float fC3)
{
	m_fM[0][0] = fC0; m_fM[0][1] = fC1; m_fM[0][2] = fC2; m_fM[0][3] = fC3;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : SetRow1(float fC0, float fC1, float fC2, float fC3)
//
//  - Purpose   : Sets matrix's row 1 elements.
//
// -----------------------------------------------------------------------------
void CMatrix::SetRow1(float fC0, float fC1, float fC2, float fC3)
{
	m_fM[1][0] = fC0; m_fM[1][1] = fC1; m_fM[1][2] = fC2; m_fM[1][3] = fC3;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : SetRow2(float fC0, float fC1, float fC2, float fC3)
//
//  - Purpose   : Sets matrix's row 2 elements.
//
// -----------------------------------------------------------------------------
void CMatrix::SetRow2(float fC0, float fC1, float fC2, float fC3)
{
	m_fM[2][0] = fC0; m_fM[2][1] = fC1; m_fM[2][2] = fC2; m_fM[2][3] = fC3;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : SetRow3(float fC0, float fC1, float fC2, float fC3)
//
//  - Purpose   : Sets matrix's row 3 elements.
//
// -----------------------------------------------------------------------------
void CMatrix::SetRow3(float fC0, float fC1, float fC2, float fC3)
{
	m_fM[3][0] = fC0; m_fM[3][1] = fC1; m_fM[3][2] = fC2; m_fM[3][3] = fC3;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CVector Scale()
//
//  - Purpose   : Returns matrix's (x, y, z) scale values.
//
// -----------------------------------------------------------------------------
CVector3 CMatrix::Scale() const
{
	return CVector3(XAxis().Length(), YAxis().Length(), ZAxis().Length());
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CVector3 Angles()
//
//  - Purpose   : Returns matrix's (x, y, z) rotation angles.
//
// -----------------------------------------------------------------------------
CVector3 CMatrix::Angles() const
{
	float fAngleX = ZAxis().Angle(CVector3(0.0f, 0.0f, 1.0f));
	float fAngleY = XAxis().Angle(CVector3(1.0f, 0.0f, 0.0f));
	float fAngleZ = YAxis().Angle(CVector3(0.0f, 1.0f, 0.0f));

	if(YAxis().Z() < 0) fAngleX += 180 - fAngleY;
	if(XAxis().Z() > 0) fAngleY += 180 - fAngleY;
	if(YAxis().X() > 0) fAngleZ += 180 - fAngleZ;

	return CVector3(fAngleX, fAngleY, fAngleZ);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void RemovePosition()
//
//  - Purpose   : Removes matrix's position values.
//
// -----------------------------------------------------------------------------
void CMatrix::RemovePosition()
{
	m_fM[0][3] = m_fM[1][3] = m_fM[2][3] = 0.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void RemoveScale()
//
//  - Purpose   : Removes matrix's scale values.
//
// -----------------------------------------------------------------------------
void CMatrix::RemoveScale()
{
	CVector3 xAxis = XAxis();
	CVector3 yAxis = YAxis();
	CVector3 zAxis = ZAxis();

	xAxis.Normalize();
	yAxis.Normalize();
	zAxis.Normalize();

	SetXAxis(xAxis);
	SetYAxis(yAxis);
	SetZAxis(zAxis);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void RemoveRotation()
//
//  - Purpose   : Removes matrix's rotation values.
//
// -----------------------------------------------------------------------------
void CMatrix::RemoveRotation()
{
	float fLengthX = XAxis().Length();
	float fLengthY = YAxis().Length();
	float fLengthZ = ZAxis().Length();

	SetXAxis(CVector3(fLengthX, 0.0f, 0.0f));
	SetYAxis(CVector3(0.0f, fLengthY, 0.0f));
	SetZAxis(CVector3(0.0f, 0.0f, fLengthZ));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : bool IsIdentity() const
//
//  - Purpose   : Returns true if matrix is the identity.
//                Use only if it avoids some computationally intense path.
//
// -----------------------------------------------------------------------------
bool CMatrix::IsIdentity() const
{
	return (ARE_EQUAL(m_fM[0][0], 1.0f) && ARE_EQUAL(m_fM[1][1], 1.0f) &&
			ARE_EQUAL(m_fM[2][2], 1.0f) && ARE_EQUAL(m_fM[3][3], 1.0f)) &&
		   (ARE_EQUAL(m_fM[0][1], 0.0f) && ARE_EQUAL(m_fM[0][2], 0.0f) &&
			ARE_EQUAL(m_fM[0][3], 0.0f) && ARE_EQUAL(m_fM[1][0], 0.0f) &&
			ARE_EQUAL(m_fM[1][2], 0.0f) && ARE_EQUAL(m_fM[1][3], 0.0f) &&
			ARE_EQUAL(m_fM[2][0], 0.0f) && ARE_EQUAL(m_fM[2][1], 0.0f) &&
			ARE_EQUAL(m_fM[2][3], 0.0f) && ARE_EQUAL(m_fM[3][0], 0.0f) &&
			ARE_EQUAL(m_fM[3][1], 0.0f) && ARE_EQUAL(m_fM[3][2], 0.0f));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : bool HasNegativeScale()
//
//  - Purpose   : Returns true if matrix has some negative scaling, otherwise
//                false.
//
// -----------------------------------------------------------------------------
bool CMatrix::HasNegativeScale() const
{
	CVector3 xAxis = XAxis();
	CVector3 yAxis = YAxis();
	CVector3 zAxis = ZAxis();
	
	CVector3 c0(xAxis.X(), yAxis.X(), zAxis.X());
	CVector3 c1(xAxis.Y(), yAxis.Y(), zAxis.Y());
	CVector3 c2(xAxis.Z(), yAxis.Z(), zAxis.Z());

	return ((c0 ^ c1) * c2) < 0.0f ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void SetPosition(const CVector3& position)
//
//  - Purpose   : Sets matrix's position values.
//
// -----------------------------------------------------------------------------
void CMatrix::SetPosition(const CVector3& position)
{
	m_fM[0][3] = position.X();
	m_fM[1][3] = position.Y();
	m_fM[2][3] = position.Z();

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void SetXAxis(const CVector3& xAxis)
//
//  - Purpose   : Sets matrix's X axis.
//
// -----------------------------------------------------------------------------
void CMatrix::SetXAxis(const CVector3& xAxis)
{
	m_fM[0][0] = xAxis.X();	m_fM[0][1] = xAxis.Y(); m_fM[0][2] = xAxis.Z();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void SetYAxis(const CVector3& yAxis)
//
//  - Purpose   : Sets matrix's Y axis.
//
// -----------------------------------------------------------------------------
void CMatrix::SetYAxis(const CVector3& yAxis)
{
	m_fM[1][0] = yAxis.X();	m_fM[1][1] = yAxis.Y(); m_fM[1][2] = yAxis.Z();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void SetZAxis(const CVector3& zAxis)
//
//  - Purpose   : Sets matrix's Z axis.
//
// -----------------------------------------------------------------------------
void CMatrix::SetZAxis(const CVector3& zAxis)
{
	m_fM[2][0] = zAxis.X();	m_fM[2][1] = zAxis.Y(); m_fM[2][2] = zAxis.Z();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix Transposed()
//
//  - Purpose   : Returns the transposed matrix.
//
// -----------------------------------------------------------------------------
CMatrix CMatrix::Transposed() const
{
	return CMatrix(	m_fM[0][0], m_fM[1][0], m_fM[2][0], m_fM[3][0],
					m_fM[0][1], m_fM[1][1], m_fM[2][1], m_fM[3][1],
					m_fM[0][2], m_fM[1][2], m_fM[2][2], m_fM[3][2],
					m_fM[0][3], m_fM[1][3], m_fM[2][3], m_fM[3][3]	);

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix Inverse()
//
//  - Purpose   : Returns the inverse transformation matrix.
//
//  - Note      : IMPORTANT: Algorithm only valid for orthogonal matrices!
//
// -----------------------------------------------------------------------------
CMatrix CMatrix::Inverse() const
{
	CMatrix result;

	// Transpose rotation submatrix

	CVector3 row0(m_fM[0][0], m_fM[1][0], m_fM[2][0]);
	CVector3 row1(m_fM[0][1], m_fM[1][1], m_fM[2][1]);
	CVector3 row2(m_fM[0][2], m_fM[1][2], m_fM[2][2]);

	CVector3 position(m_fM[0][3], m_fM[1][3], m_fM[2][3]);
	CVector3 invPosition;

	// Solve ecuation system

	invPosition.SetX((-row0) * position);
	invPosition.SetY((-row1) * position);
	invPosition.SetZ((-row2) * position);

	// Get scale values

	CVector3 scale = Scale();

	float sqrSclX = scale.X(); sqrSclX *= sqrSclX;
	float sqrSclY = scale.Y(); sqrSclY *= sqrSclY;
	float sqrSclZ = scale.Z(); sqrSclZ *= sqrSclZ;

	// Shouldn't happen:

	assert(!IS_ZERO(sqrSclX));
	assert(!IS_ZERO(sqrSclY));
	assert(!IS_ZERO(sqrSclZ));

	// Normalize axis and multiply by the inverse scale.

	row0 = row0 / sqrSclX;
	row1 = row1 / sqrSclY;
	row2 = row2 / sqrSclZ;

	// Insert values

	result.SetRow0(row0.X(), row0.Y(), row0.Z(), invPosition.X());
	result.SetRow1(row1.X(), row1.Y(), row1.Z(), invPosition.Y());
	result.SetRow2(row2.X(), row2.Y(), row2.Z(), invPosition.Z());
	result.SetRow3(  0.0f,      0.0f,      0.0f,         1.0f);

	return result;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix Quaternion()
//
//  - Purpose   : Returns the equivalent quaternion.
//
// -----------------------------------------------------------------------------
CQuaternion CMatrix::Quaternion() const
{
	CQuaternion result;
	float fTrace, fS;

	fTrace = 1.0f + m_fM[0][0] + m_fM[1][1] + m_fM[2][2];

	// Check trace value

	if(fTrace > 0.000001f)
	{
		fS = 0.5f / sqrtf(fTrace);
		result.SetW(0.25f / fS);

		CVector3 v;

		v.SetX((m_fM[2][1] - m_fM[1][2]) * fS);
		v.SetY((m_fM[0][2] - m_fM[2][0]) * fS);
		v.SetZ((m_fM[1][0] - m_fM[0][1]) * fS);

		result.SetV(v);

		return result;
	}
	else
	{
		float qx, qy, qz, qw;

		if(m_fM[0][0] > m_fM[1][1] && m_fM[0][0] > m_fM[2][2])
		{
			fS = sqrtf(1.0f + m_fM[0][0] - m_fM[1][1] - m_fM[2][2]) * 2.0f;
			qx = 0.25f * fS;
			qy = (m_fM[0][1] + m_fM[1][0] ) / fS;
			qz = (m_fM[0][2] + m_fM[2][0] ) / fS;
			qw = (m_fM[1][2] - m_fM[2][1] ) / fS;
		}
		else if(m_fM[1][1] > m_fM[2][2])
		{ 
			fS = sqrt( 1.0 + m_fM[1][1] - m_fM[0][0] - m_fM[2][2] ) * 2;
			qx = (m_fM[0][1] + m_fM[1][0] ) / fS;
			qy = 0.25f * fS;
			qz = (m_fM[1][2] + m_fM[2][1] ) / fS;
			qw = (m_fM[0][2] - m_fM[2][0] ) / fS;
		}
		else
		{
			fS = sqrt( 1.0 + m_fM[2][2] - m_fM[0][0] - m_fM[1][1] ) * 2;
			qx = (m_fM[0][2] + m_fM[2][0] ) / fS;
			qy = (m_fM[1][2] + m_fM[2][1] ) / fS;
			qz = 0.25f * fS;
			qw = (m_fM[0][1] - m_fM[1][0] ) / fS;
		}

		result.SetW(qw);
		result.SetV(CVector3(qx, qy, qz));
	}

	return result;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix SetIdentity()
//
//  - Purpose   : Sets the matrix to the 4x4 identity.
//
// -----------------------------------------------------------------------------
void CMatrix::SetIdentity()
{
	memset(m_fM, 0, sizeof(float) * 16);

	m_fM[0][0] = m_fM[1][1] = m_fM[2][2] = m_fM[3][3] = 1.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void BuildTranslation(float fX, float fY, float fZ)
//
//  - Purpose   : Builds a translation matrix given 3 space position components.
//
// -----------------------------------------------------------------------------
void CMatrix::BuildTranslation(float fX, float fY, float fZ)
{
	SetIdentity();
	SetPosition(CVector3(fX, fY, fZ));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void BuildScaling(float fX, float fY, float fZ)
//
//  - Purpose   : Builds a scale matrix given 3 scale components.
//
// -----------------------------------------------------------------------------
void CMatrix::BuildScaling(float fX, float fY, float fZ)
{
	memset(m_fM, 0, sizeof(float) * 16);

	m_fM[0][0] = fX;
	m_fM[1][1] = fY;
	m_fM[2][2] = fZ;
	m_fM[3][3] = 1.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void BuildRotationX(float fDegrees)
//
//  - Purpose   : Builds a rotation matrix around x axis. Angle in degrees.
//
// -----------------------------------------------------------------------------
void CMatrix::BuildRotationX(float fDegrees)
{
	float fRad	  = DEG_TO_RAD(fDegrees);
	float fSine   = sinf(fRad);
	float fCosine = cosf(fRad);

	SetIdentity();
	
	m_fM[1][1] = fCosine;
	m_fM[1][2] = -fSine;
	m_fM[2][1] = fSine;
	m_fM[2][2] = fCosine;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void BuildRotationY(float fDegrees)
//
//  - Purpose   : Builds a rotation matrix around y axis. Angle in degrees.
//
// -----------------------------------------------------------------------------
void CMatrix::BuildRotationY(float fDegrees)
{
	float fRad	  = DEG_TO_RAD(fDegrees);
	float fSine   = sinf(fRad);
	float fCosine = cosf(fRad);

	SetIdentity();
	
	m_fM[0][0] = fCosine;
	m_fM[0][2] = fSine;
	m_fM[2][0] = -fSine;
	m_fM[2][2] = fCosine;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void BuildRotationZ(float fDegrees)
//
//  - Purpose   : Builds a rotation matrix around z axis. Angle in degrees.
//
// -----------------------------------------------------------------------------
void CMatrix::BuildRotationZ(float fDegrees)
{
	float fRad	  = DEG_TO_RAD(fDegrees);
	float fSine   = sinf(fRad);
	float fCosine = cosf(fRad);

	SetIdentity();
	
	m_fM[0][0] = fCosine;
	m_fM[0][1] = -fSine;
	m_fM[1][0] = fSine;
	m_fM[1][1] = fCosine;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : BuildRotation(float fAngle, float fAxisX, float fAxisY, float fAxisZ)
//
//  - Purpose   : Builds a rotation matrix of fAngle degrees around the axis
//                given by (fAxisX, fAxisY, fAxisZ).
//
// -----------------------------------------------------------------------------
void CMatrix::BuildRotation(float fAngle, float fAxisX, float fAxisY, float fAxisZ)
{
	CVector3 axis(fAxisX, fAxisY, fAxisZ);
	axis.Normalize();

	float fRad  = DEG_TO_RAD(fAngle * 0.5f);
	float fSine = sinf(fRad);

	CQuaternion quat(cosf(fRad), fSine * fAxisX, fSine * fAxisY, fSine * fAxisZ);

	*this = quat.Matrix();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : PreTranslate(float fX, float fY, float fZ)
//
//  - Purpose   : Inserts a translation transformation _before_ all other
//                transformations stored in the matrix.
//                So, if tranforming a vertex, it works as it is first being
//                translated and then transformed by the other ones stored before.
//
// -----------------------------------------------------------------------------
void CMatrix::PreTranslate(float fX, float fY, float fZ)
{
	CMatrix translation;
	translation.Translate(fX, fY, fZ);

	*this = *this * translation;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : PreScale(float fX, float fY, float fZ)
//
//  - Purpose   : Inserts a scale transformation _before_ all other
//                transformations stored in the matrix.
//                So, if tranforming a vertex, it works as it is first being
//                scaled then transformed by the other ones stored before.
//
// -----------------------------------------------------------------------------
void CMatrix::PreScale(float fX, float fY, float fZ)
{
	CMatrix scale;
	scale.BuildScaling(fX, fY, fZ);

	*this = *this * scale;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void PreRotateX(float fDegrees)
//
//  - Purpose   : Inserts a X axis rotation transformation _before_ all other
//                transformations stored in the matrix.
//                So, if tranforming a vertex, it works as it is first being
//                rotated around the X axis, and then transformed by the other
//                ones stored before.
//
// -----------------------------------------------------------------------------
void CMatrix::PreRotateX(float fDegrees)
{
	CMatrix rotation(fDegrees, 1.0f, 0.0f, 0.0f);

	*this = *this * rotation;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void PreRotateY(float fDegrees)
//
//  - Purpose   : Inserts a Y axis rotation transformation _before_ all other
//                transformations stored in the matrix.
//                So, if tranforming a vertex, it works as it is first being
//                rotated around the Y axis, and then transformed by the other
//                ones stored before.
//
// -----------------------------------------------------------------------------
void CMatrix::PreRotateY(float fDegrees)
{
	CMatrix rotation(fDegrees, 0.0f, 1.0f, 0.0f);

	*this = *this * rotation;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void PreRotateZ(float fDegrees)
//
//  - Purpose   : Inserts a Z axis rotation transformation _before_ all other
//                transformations stored in the matrix.
//                So, if tranforming a vertex, it works as it is first being
//                rotated around the Z axis, and then transformed by the other
//                ones stored before.
//
// -----------------------------------------------------------------------------
void CMatrix::PreRotateZ(float fDegrees)
{
	CMatrix rotation(fDegrees, 0.0f, 0.0f, 1.0f);

	*this = *this * rotation;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void PreRotate(float fDegrees, float fAxisX, float fAxisY, float fAxisZ)
//
//  - Purpose   : Inserts a rotation transformation _before_ all other
//                transformations stored in the matrix.
//                So, if tranforming a vertex, it works as it is first being
//                rotated around the (fAxisX, fAxisY, fAxisZ) axis and then
//                transformed by the other ones stored before.
//
// -----------------------------------------------------------------------------
void CMatrix::PreRotate(float fDegrees, float fAxisX, float fAxisY, float fAxisZ)
{
	CMatrix rotation(fDegrees, fAxisX, fAxisY, fAxisZ);

	*this = *this * rotation;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void Translate(float fX, float fY, float fZ)
//
//  - Purpose   : Appends a translation transformation in the matrix.
//                So, if tranforming a vertex, it will first be transformed
//                by the 'original' transformation and then translated.
//
// -----------------------------------------------------------------------------
void CMatrix::Translate(float fX, float fY, float fZ)
{
	m_fM[0][3] += fX;
	m_fM[1][3] += fY;
	m_fM[2][3] += fZ;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void Scale(float fX, float fY, float fZ)
//
//  - Purpose   : Appends a scaling as last transformation in the matrix.
//                So, if tranforming a vertex, it will first be transformed
//                by the 'original' transformation and then scaled.
//
// -----------------------------------------------------------------------------
void CMatrix::Scale(float fX, float fY, float fZ)
{
	CMatrix scale;
	scale.BuildScaling(fX, fY, fZ);

	*this = scale * *this;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void RotateX(float fDegrees)
//
//  - Purpose   : Appends a X rotation as last transformation in the matrix.
//                So, if tranforming a vertex, it will first be transformed
//                by the 'original' transformation and then rotated around X.
//
// -----------------------------------------------------------------------------
void CMatrix::RotateX(float fDegrees)
{
	CMatrix rotation(fDegrees, 1.0f, 0.0f, 0.0f);

	*this = rotation * *this;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void RotateY(float fDegrees)
//
//  - Purpose   : Appends a Y rotation as last transformation in the matrix.
//                So, if tranforming a vertex, it will first be transformed
//                by the 'original' transformation and then rotated around Y.
//
// -----------------------------------------------------------------------------
void CMatrix::RotateY(float fDegrees)
{
	CMatrix rotation(fDegrees, 0.0f, 1.0f, 0.0f);

	*this = rotation * *this;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void RotateZ(float fDegrees)
//
//  - Purpose   : Appends a Z rotation as last transformation in the matrix.
//                So, if tranforming a vertex, it will first be transformed
//                by the 'original' transformation and then rotated around Z.
//
// -----------------------------------------------------------------------------
void CMatrix::RotateZ(float fDegrees)
{
	CMatrix rotation(fDegrees, 0.0f, 0.0f, 1.0f);

	*this = rotation * *this;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void Rotate(float fDegrees, float fAxisX, float fAxisY, float fAxisZ)
//
//  - Purpose   : Appends a rotation as last transformation in the matrix.
//                So, if tranforming a vertex, it will first be transformed
//                by the 'original' transformation and then rotated around
//                the given axis.
//
// -----------------------------------------------------------------------------
void CMatrix::Rotate(float fDegrees, float fAxisX, float fAxisY, float fAxisZ)
{
	CMatrix rotation(fDegrees, fAxisX, fAxisY, fAxisZ);

	*this = rotation * *this;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void Transform(CPlane* plane)
//
//  - Purpose   : Transforms a plane.
//
// -----------------------------------------------------------------------------
void CMatrix::Transform(CPlane* plane) const
{
	CVector3 planePoint;

	// Get a point of the plane:

	if(IS_ZERO(plane->A()) == false)
	{
		planePoint.Set(-plane->D() / plane->A(), 0.0f, 0.0f);
	}
	else if(IS_ZERO(plane->B()) == false)
	{
		planePoint.Set(0.0f, -plane->D() / plane->B(), 0.0f);
	}
	else if(IS_ZERO(plane->C()) == false)
	{
		planePoint.Set(0.0f, 0.0f, -plane->D() / plane->C());
	}

	// Transform both point of the plane and plane's normal.

	CVector3 normal = plane->Normal();

	Transform(&planePoint);
	Transform(&normal, true);

	plane->Build(planePoint, normal);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void Transform(CRay* ray)
//
//  - Purpose   : Transforms a ray.
//
// -----------------------------------------------------------------------------
void CMatrix::Transform(CRay* ray) const
{
	CVector3 point     = ray->Point();
	CVector3 direction = ray->Direction();

	Transform(&point);
	Transform(&direction, true);

	ray->Build(point, direction);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : void Transform(CVector3* vector, bool bIsNormal = false)
//
//  - Purpose   : Transforms a vector3.
//
//  - Note      : When bIsNormal is true, the vector will be transformed by
//                the inverse transposed matrix instead. This is the derived
//                matrix used to transform normals correctly.
//                If transforming a vertex, bIsNormal should be false. No
//                derived matrix will be used.
//
// -----------------------------------------------------------------------------
void CMatrix::Transform(CVector3* vector, bool bIsNormal) const
{
	if(!bIsNormal)
	{
		*vector = *this * *vector;
	}
	else
	{
		*vector = this->Inverse().Transposed() * *vector;
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : float operator() (int nRow, int nColumn) const
//
//  - Purpose   : Parenthesis operator, used to get a matrix component.
//
// -----------------------------------------------------------------------------
float CMatrix::operator() (int nRow, int nColumn) const
{
	assert(nRow    >= 0);
	assert(nRow    <  4);
	assert(nColumn >= 0);
	assert(nColumn <  4);

	return m_fM[nRow][nColumn];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : float operator() (int nRow, int nColumn)
//
//  - Purpose   : Parenthesis operator, used to set a matrix component.
//
// -----------------------------------------------------------------------------
float& CMatrix::operator() (int nRow, int nColumn)
{
	assert(nRow    >= 0);
	assert(nRow    <  4);
	assert(nColumn >= 0);
	assert(nColumn <  4);

	return m_fM[nRow][nColumn];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix& operator = (const CMatrix& matrix)
//
//  - Purpose   : Assignement operator.
//
// -----------------------------------------------------------------------------
CMatrix& CMatrix::operator = (const CMatrix& matrix)
{
	memcpy(m_fM, matrix.m_fM, sizeof(float) * 16);

	return *this;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CMatrix operator * (const CMatrix& matrix)
//
//  - Purpose   : Multiplies two matrices.
//                The order is essencial, transformations are read from right
//                to left, so if he have:
//
//                 matrix = translationmatrix * rotationmatrix * scalematrix
//
//                matrix will contain a transformation that would first scale,
//                then rotate, and finally translate.
//
// -----------------------------------------------------------------------------
CMatrix CMatrix::operator * (const CMatrix& matrix) const
{
	CMatrix result;
	int nRow, nColumn;

	for(nRow = 0; nRow < 4; nRow++)
	{
		for(nColumn = 0; nColumn < 4; nColumn++)
		{
			result.m_fM[nRow][nColumn] =	m_fM[nRow][0] * matrix.m_fM[0][nColumn] +
											m_fM[nRow][1] * matrix.m_fM[1][nColumn] +
											m_fM[nRow][2] * matrix.m_fM[2][nColumn] +
											m_fM[nRow][3] * matrix.m_fM[3][nColumn];
		}
	}
	
	return result;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CPlane operator * (const CPlane& plane)
//
//  - Purpose   : Transforms a given plane using the matrix.
//
// -----------------------------------------------------------------------------
CPlane CMatrix::operator * (const CPlane& plane) const
{
	CPlane result(plane);

	Transform(&result);

	return result;

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CRay operator * (const CRay& ray)
//
//  - Purpose   : Transforms a given ray using the matrix.
//
// -----------------------------------------------------------------------------
CRay CMatrix::operator * (const CRay& ray) const
{
	CRay result(ray);

	Transform(&result);

	return result;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMatrix
//
//  - prototype : CVector3 operator * (const CVector3& vector)
//
//  - Purpose   : Transforms a given vector3 using the matrix.
//
// -----------------------------------------------------------------------------
CVector3 CMatrix::operator * (const CVector3& vector) const
{
	CVector3 result;
	CVector3 v3temp(vector);

	result.SetX(m_fM[0][0] * v3temp.X() + m_fM[0][1] * v3temp.Y() + m_fM[0][2] * v3temp.Z() + m_fM[0][3]);
	result.SetY(m_fM[1][0] * v3temp.X() + m_fM[1][1] * v3temp.Y() + m_fM[1][2] * v3temp.Z() + m_fM[1][3]);
	result.SetZ(m_fM[2][0] * v3temp.X() + m_fM[2][1] * v3temp.Y() + m_fM[2][2] * v3temp.Z() + m_fM[2][3]);

	return result;
}
