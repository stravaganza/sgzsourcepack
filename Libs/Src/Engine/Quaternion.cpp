// ==[ File ]===================================================================================
//
//  -Name     : Quaternion.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CQuaternion implementation.
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
//  - Class     : CQuaternion
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the CQuaternion data from a source.
//
//  - Note      : If false is returned, it doesn't mean that data has not been
//                modified.
//
// -----------------------------------------------------------------------------
bool CQuaternion::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	int nError = 0;

	if(!pInput->ReadFloat(&m_fW)) nError++;
	if(!m_v.Read(pInput)) nError++;

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CQuaternion
//  - Prototype : bool Write(COutput* pOutput) const
//
//  - Purpose   : Sends the CQuaternion data to a destiny (file, memory...).
//
// -----------------------------------------------------------------------------
bool CQuaternion::Write(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	int nError = 0;

	if(!pOutput->WriteFloat(m_fW)) nError++;
	if(!m_v.Write(pOutput))        nError++;

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CQuaternion
//  - prototype : CMatrix Matrix()
//
//  - Purpose   : Returns the equivalent (rotation) matrix of the quaternion.
//
// -----------------------------------------------------------------------------
CMatrix CQuaternion::Matrix() const
{
	CMatrix result;
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	// Precalc stuff

	x2 = m_v.X() + m_v.X(); y2 = m_v.Y() + m_v.Y(); 
	z2 = m_v.Z() + m_v.Z();
	xx = m_v.X() * x2;   xy = m_v.X() * y2;   xz = m_v.X() * z2;
	yy = m_v.Y() * y2;   yz = m_v.Y() * z2;   zz = m_v.Z() * z2;
	wx = m_fW    * x2;   wy = m_fW    * y2;   wz = m_fW    * z2;

	// Compute

	result(0, 0) = 1.0f - (yy + zz);   result(0, 1) = xy - wz;
	result(0, 2) = xz + wy;            result(0, 3) = 0.0f;
	
	result(1, 0) = xy + wz;            result(1, 1) = 1.0f - (xx + zz);
	result(1, 2) = yz - wx;            result(1, 3) = 0.0f;

	result(2, 0) = xz - wy;            result(2, 1) = yz + wx;
	result(2, 2) = 1.0f - (xx + yy);   result(2, 3) = 0.0f;

	result(3, 0) = 0.0f;
	result(3, 1) = 0.0f;
	result(3, 2) = 0.0f;
	result(3, 3) = 1.0f;

	return result.Transposed();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CQuaternion
//  - prototype : CQuaternion SLERP(CQuaternion quatTO, float fT)
//
//  - Purpose   : Computes the result of spherically interpolating between
//                the quaternion and the one specified.
//                The spherical interpolation is controlled by the fT parameter
//                being 0.0f <= fT <= 1.0f
//
// -----------------------------------------------------------------------------
CQuaternion CQuaternion::SLERP(CQuaternion quatTO, float fT) const
{
	CQuaternion result, tempQuat;
	float fOmega, fCosOmega, fRecSinOmega, fSclQ1, fSclQ2;

	// Get the cosine of the angle between the two quaternions, we'll use this
	// later to see if we can use simple linear interpolation.

	fCosOmega = m_v.X() * quatTO.m_v.X() + m_v.Y() * quatTO.m_v.Y() + m_v.Z() * quatTO.m_v.Z() + m_fW * quatTO.m_fW;

	// Ajustamos signos si es necesario.

	if(fCosOmega < 0.0f)
	{
		fCosOmega     = -fCosOmega;
		tempQuat.m_fW = -quatTO.m_fW;
		tempQuat.m_v  = -quatTO.m_v;
	}
	else
	{
		tempQuat = quatTO;
	}

	// Get the coefficients of the interpolation depending on the angle.
	// If it's too close we can use simple linear interpolation coefficients.

	if((1.0f - fCosOmega) > ALMOST_ZERO)
	{
		// SLERP

		fOmega	     = acosf(fCosOmega);
		fRecSinOmega = 1.0f / sinf(fOmega);
		fSclQ1	     = sinf((1.0f - fT) * fOmega) * fRecSinOmega;
		fSclQ2	     = sinf(fT * fOmega)          * fRecSinOmega;
	}
	else
	{
		// LERP

		fSclQ1 = 1.0f - fT;
		fSclQ2 = fT;
	}

	// Interpolate

	result.m_fW = m_fW * fSclQ1  + tempQuat.m_fW * fSclQ2;
	result.m_v  = (m_v * fSclQ1) + (tempQuat.m_v * fSclQ2);

	return result;
}

