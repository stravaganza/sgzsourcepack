// ==[ File ]===================================================================================
//
//  -Name     : GeoMath.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Class definitions to deal with the geometry and maths behind the engine.
//
// =============================================================================================

/*

	25/09/2001  -  File created.
	01/12/2002  -  Added Read/Write methods.


*/


#ifndef __GEOMATH_H
#define __GEOMATH_H


#ifndef _MSC_VER
  #define __forceinline inline
  inline float floorf(float fArg) { return (float)floor(fArg); }
  inline float sinf  (float fArg) { return (float)sin(fArg);   }
  inline float cosf  (float fArg) { return (float)cos(fArg);   }
  inline float tanf  (float fArg) { return (float)tan(fArg);   }
  inline float acosf (float fArg) { return (float)acos(fArg);  }
  inline float atanf (float fArg) { return (float)atan(fArg);  }
  inline float log10f(float fArg) { return (float)log10(fArg); }
#endif

// ==[ Constants ]==============================================================================


#ifndef ALMOST_ZERO
  #define ALMOST_ZERO 0.0001f
#endif

#ifndef FLOAT_MAX
  #define FLOAT_MAX 3.4e38f // not max, but will do the work
#endif

#ifndef FLOAT_MIN
  #define FLOAT_MIN -FLOAT_MAX // not min, but will do the work
#endif

#ifndef PI
  #define PI 3.1415926535897932384626433832795f
#endif

#ifndef HALFPI
  #define HALFPI 1.5707963267948966192313216916398f
#endif

#ifndef TWOPI
  #define TWOPI 6.283185307179586476925286766559f
#endif


// ==[  Macros  ]===============================================================================


#define MYFABSF(value)      (((value) > 0.0f) ? (value) : -(value))

#define MYROUND(value)      ((int)((value) + 0.5f))


#define IS_ZERO(value)      ((MYFABSF(value)) < ALMOST_ZERO ? true : false)
#define ARE_EQUAL(v1, v2)   (IS_ZERO((v1) - (v2)))

#ifndef DEG_TO_RAD
  #define DEG_TO_RAD(n)     ((n) * 0.017453292f)
#endif

#ifndef RAD_TO_DEG
  #define RAD_TO_DEG(n)     ((n) * 57.29577951f)
#endif


// ==[ Functions ]==============================================================================


void  ComputeLoop(float fStart, float fEnd, float* pfValue);
float ComputeLERP(float fStart, float fEnd, float fT);
float ComputeRand(float fMin,   float fMax);
float ComputeRandWithVariation(float fValue, float fVariation);
float ComputeRandWithVariation(float fMin, float fMax, float fVariation);


// ==[ Class definitions ]======================================================================


#pragma pack(1) // To ensure Read/Write methods operate properly.


class CVector2;
class CVector3;
class CVector4;
class CMatrix;
class CQuaternion;
class CPlane;
class CRay;



// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVector2
//
//  - Purpose   : 2 dimension vector routines. Used to store XY or UV f.e.
//
// ---------------------------------------------------------------------------
class CVector2
{


public:


	CVector2()                   { m_fX = m_fY = 0.0f; }
	CVector2(float fX, float fY) { Set(fX, fY);        }
	~CVector2() { }

	// Read/Write

	bool Read (CInput*  pInput);
	bool Write(COutput* pOutput) const;

	//

	__forceinline const float* Data()            const { return &m_fX;         }

	void                       Set(float fX, float fY) { m_fX = fX; m_fY = fY; }

	__forceinline float        X()               const { return m_fX; }
	__forceinline float        Y()               const { return m_fY; }

	__forceinline void         SetX(float fX)          { m_fX = fX; }
	__forceinline void         SetY(float fY)          { m_fY = fY; }


private:


	float m_fX, m_fY;

};

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVector3
//
//  - Purpose   : Three-dimensional vector routines.
//
// ---------------------------------------------------------------------------
class CVector3
{


public:


	static CVector3 NULLVECTOR;

	CVector3()                              { m_fX = m_fY = m_fZ = 0.0f; }
	CVector3(const CVector3& vector)        { *this = vector;            }
	CVector3(float fX, float fY, float fZ)  { Set(fX, fY, fZ);           }

	CVector3(const CVector3& src, const CVector3& dst) { Build(src, dst); }

	~CVector3() { }

	// Read/Write

	bool Read (CInput*  pInput);
	bool Write(COutput* pOutput) const;

	// Access

	__forceinline const float* Data()        const { return &m_fX; }

	__forceinline void         Set(float fX, float fY, float fZ) { m_fX = fX; m_fY = fY; m_fZ = fZ; }

	void                       Build(const CVector3& src, const CVector3& dst);

	__forceinline float        X()            const { return m_fX; }
	__forceinline float        Y()            const { return m_fY; }
	__forceinline float        Z()            const { return m_fZ; }

	__forceinline void         SetX(float fX)       { m_fX = fX; }
	__forceinline void         SetY(float fY)       { m_fY = fY; }
	__forceinline void         SetZ(float fZ)       { m_fZ = fZ; }

	float    Length() const;
	void     Normalize();

	// Geometric computations

	CVector3 LERP(const CVector3& dst, float fT) const;

	float    Angle(const CVector3& vector) const;
	float    Angle(const CRay&     ray)    const;
	float    Angle(const CPlane&   plane)  const;

	bool     IsOnPositiveSide(const CPlane& plane) const;

	float    Distance    (CVector3&     vector) const { return (vector - *this).Length(); }
	float    Distance    (const CPlane& plane)  const;
	float    DistanceFast(const CPlane& plane)  const;

	// Operators

	CVector3  operator *  (float fValue) const;
	CVector3  operator /  (float fValue) const;

	float     operator *  (const CVector3& vector) const;
	CVector3  operator +  (const CVector3& vector) const;
	CVector3  operator -  (const CVector3& vector) const;
	CVector3  operator ^  (const CVector3& vector) const;
	CVector3& operator =  (const CVector3& vector);
	bool      operator == (const CVector3& vector) const;
	bool      operator != (const CVector3& vector) const;


private:


	float m_fX, m_fY, m_fZ;

};

__forceinline CVector3 operator - (const CVector3 &vector)
{
	return CVector3(-vector.X(), -vector.Y(), -vector.Z());
}


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CVector4
//
//  - Purpose   : 4 dimension vector routines. Used to store information
//                of an XYZW value or RGBA for example.
//
// ---------------------------------------------------------------------------
class CVector4
{


public:


	CVector4()                                        { m_fX = m_fY = m_fZ = 0.0f; m_fW = 1.0f;     }
	CVector4(float fX, float fY, float fZ, float fW)  { Set(fX, fY, fZ, fW);                        }
	CVector4(const CVector3& v3Vec)                   { Set(v3Vec.X(), v3Vec.Y(), v3Vec.Z(), 1.0f); }
	~CVector4() { }

	bool					   Read (CInput*  pInput);
	bool					   Write(COutput* pOutput) const;

	__forceinline const float* Data()         const { return &m_fX; }

	__forceinline float        X()            const { return m_fX;  }
	__forceinline float        Y()            const { return m_fY;  }
	__forceinline float        Z()            const { return m_fZ;  }
	__forceinline float        W()            const { return m_fW;  }

	__forceinline void         SetX(float fX)       { m_fX = fX;    }
	__forceinline void         SetY(float fY)       { m_fY = fY;    }
	__forceinline void         SetZ(float fZ)       { m_fZ = fZ;    }
	__forceinline void         SetW(float fW)       { m_fW = fW;    }


	__forceinline void Set(float fX, float fY, float fZ, float fW)
	{
		m_fX = fX; m_fY = fY; m_fZ = fZ; m_fW = fW;
	}

	CVector4 LERP(const CVector4& dst, float fT) const;

	CVector4& operator = (const CVector3& v3Vec)
	{
		Set(v3Vec.X(), v3Vec.Y(), v3Vec.Z(), 1.0f);
		return *this;
	}

	CVector4 operator * (float fValue)
	{
		return CVector4(m_fX * fValue, m_fY * fValue, m_fZ * fValue, m_fW * fValue);
	}


private:


	float m_fX, m_fY, m_fZ, m_fW;

};


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CMatrix
//
//  - Purpose   : 4x4 Matrix routines.
//
//  - Notes     : Operations are composed from right to left, so Vector3
//                objects, when operating, are handled as columns:
//
//                [x' y' z']T = Matrix2 * Matrix1 * [x y z]T.
// 
//                As you know, order does matter.
//
// ---------------------------------------------------------------------------
class CMatrix
{


public:


	static CMatrix IDENTITY;

	CMatrix();
	CMatrix(const CMatrix& matrix);

	CMatrix(float f00, float f01, float f02, float f03,
			float f10, float f11, float f12, float f13,
			float f20, float f21, float f22, float f23,
			float f30, float f31, float f32, float f33);

	CMatrix(const CVector3& pos, const CVector3& xAxis, const CVector3& yAxis, const CVector3& zAxis);

	CMatrix(float fAngle, float fAxisX, float fAxisY, float fAxisZ);

	CMatrix(CQuaternion& quaternion);

	~CMatrix() { }

	// Read/Write

	bool		 Read (CInput*  pInput);
	bool		 Write(COutput* pOutput) const;

	// Access

	const float* Data() const { return &m_fM[0][0]; }
	
	void         SetRow0(float fC0, float fC1, float fC2, float fC3);
	void         SetRow1(float fC0, float fC1, float fC2, float fC3);
	void         SetRow2(float fC0, float fC1, float fC2, float fC3);
	void         SetRow3(float fC0, float fC1, float fC2, float fC3);

	CVector3     XAxis()    const { return CVector3(m_fM[0][0], m_fM[0][1], m_fM[0][2]); }
	CVector3     YAxis()    const { return CVector3(m_fM[1][0], m_fM[1][1], m_fM[1][2]); }
	CVector3     ZAxis()    const { return CVector3(m_fM[2][0], m_fM[2][1], m_fM[2][2]); }
	CVector3     Position() const { return CVector3(m_fM[0][3], m_fM[1][3], m_fM[2][3]); }
	CVector3     Scale()    const;
	CVector3     Angles()   const;

	void         RemovePosition();
	void         RemoveScale();
	void         RemoveRotation();

	bool         IsIdentity()       const;
	bool         HasNegativeScale() const;

	void         SetPosition(const CVector3& position);
	void         SetXAxis(const CVector3& xAxis);
	void         SetYAxis(const CVector3& yAxis);
	void         SetZAxis(const CVector3& zAxis);

	CMatrix      Transposed() const;
	CMatrix      Inverse()    const;

	CQuaternion  Quaternion() const;

	// Specific matrix creation:

	void         SetIdentity();

	void         BuildTranslation(float fX, float fY, float fZ);
	void         BuildScaling    (float fX, float fY, float fZ);

	void         BuildRotationX(float fDegrees);
	void         BuildRotationY(float fDegrees);
	void         BuildRotationZ(float fDegrees);

	void         BuildRotation(float fAngle, float fAxisX, float fAxisY, float fAxisZ);

	// Composing matrices:

	void         PreTranslate(float fX, float fY, float fZ);
	void         PreScale    (float fX, float fY, float fZ);
	void         PreRotateX  (float fDegrees);
	void         PreRotateY  (float fDegrees);
	void         PreRotateZ  (float fDegrees);
	void         PreRotate   (float fDegrees, float fAxisX, float fAxisY, float fAxisZ);

	void         Translate(float fX, float fY, float fZ);
	void         Scale    (float fX, float fY, float fZ);
	void         RotateX  (float fDegrees);
	void         RotateY  (float fDegrees);
	void         RotateZ  (float fDegrees);
	void         Rotate   (float fDegrees, float fAxisX, float fAxisY, float fAxisZ);

	// Transforming:

	void         Transform(CPlane*   plane) const;
	void         Transform(CRay*     ray)   const;
	void         Transform(CVector3* vector, bool bIsNormal = false) const;

	// Operators

	float        operator() (int nRow, int nColumn) const; // Get
	float&       operator() (int nRow, int nColumn);       // Set

	CMatrix&     operator = (const CMatrix& matrix);

	CMatrix      operator * (const CMatrix&  matrix) const;
	CPlane       operator * (const CPlane&   plane)  const;
	CRay         operator * (const CRay&     ray)    const;
	CVector3     operator * (const CVector3& vector) const;


private:


	float m_fM[4][4];

};


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CQuaternion
//
//  - Purpose   : Quaternion routines
//
//  - Notes     : Should add some more interesting methods.
//
// ---------------------------------------------------------------------------
class CQuaternion
{


public:


	CQuaternion()										{ Set(1.0f, 0.0f, 0.0f, 0.0f);		}
	CQuaternion(const CQuaternion& quaternion)			{ *this = quaternion;               }
	CQuaternion(float fW, float fX, float fY, float fZ)	{ Set(fW, fX, fY, fZ);				}
	CQuaternion(float fW, const CVector3& vector)		{ Set(fW, vector);					}
	CQuaternion(CMatrix& rotMatrix)						{ *this = rotMatrix.Quaternion();	}
	~CQuaternion() { }

	// Read/Write

	bool                   Read (CInput*  pInput);
	bool                   Write(COutput* pOutput) const;

	// Access

	void                   Set   (float fW, float fX, float fY, float fZ) { m_fW = fW; m_v.Set(fX, fY, fZ);	}
	void                   Set   (float fW, const CVector3& vector)       { m_fW = fW; m_v = vector;			}

	__forceinline float    W     ()                                 const { return m_fW;   }
	__forceinline CVector3 V     ()                                 const { return m_v;    }

	__forceinline void     SetW  (float fW)                               { m_fW = fW;     }
	__forceinline void     SetV  (const CVector3& vector)                 { m_v  = vector; }

	CMatrix                Matrix() const;

	// Spherical linear interpolation between 2 quaternions. 0.0f <= fT <= 1.0f

	CQuaternion            SLERP (CQuaternion quat2, float fT) const;

	// Operadores

	CQuaternion& operator = (const CQuaternion& quaternion)
	{
		m_fW = quaternion.m_fW;
		m_v  = quaternion.m_v;

		return *this;
	}

	CQuaternion operator * (const CQuaternion& quaternion)
	{
		CQuaternion quatResult;

		quatResult.m_fW = (m_fW * quaternion.m_fW) - (m_v * quaternion.m_v);
		quatResult.m_v  = (quaternion.m_v * m_fW)  + (m_v * quaternion.m_fW) + (m_v ^ quaternion.m_v);

		return quatResult;
	}


private:


	float    m_fW;
	CVector3 m_v;

};


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CRay
//
//  - Purpose   : Ray routines
//
// ---------------------------------------------------------------------------
class CRay
{


public:


	CRay()					{				}
	CRay(const CRay &ray)	{ *this = ray;	}
	CRay(const CVector3 &point, const CVector3 &direction) { Build(point, direction); }
	~CRay() { }

	// Read/Write

	bool				   Read (CInput*  pInput);
	bool				   Write(COutput* pOutput) const;

	// Access.

	void                   Build(const CVector3 &point, const CVector3 &direction) { m_point = point; m_direction = direction; }

	__forceinline CVector3 Point()     const                     { return m_point;      }
	__forceinline CVector3 Direction() const                     { return m_direction;  }

	__forceinline void     SetPoint    (const CVector3& point)   { m_point     = point; }
	__forceinline void     SetDirection(const CVector3& dir)     { m_direction = dir;   }

	// Geometric operations.

	float                  Angle       (const CRay&		ray)     const;
	float                  Angle       (const CPlane&    plane)  const;
	float                  Angle       (const CVector3&  vector) const;

	bool                   Intersection(const CPlane& plane, CVector3* intersection) const;
	bool                   Reflexion   (const CPlane& plane, CRay*     reflexion)    const;

	// Operators.

	CRay& operator = (const CRay& ray)
	{
		m_point     = ray.m_point;
		m_direction = ray.m_direction;

		return *this;
	}


private:


	CVector3 m_point;
	CVector3 m_direction;

};


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CPlane
//
//  - Purpose   : Plane routines
//
// ---------------------------------------------------------------------------
class CPlane
{


public:


	CPlane()                                              { m_fA = m_fB = m_fC = m_fD = 0.0f;	}
	CPlane(const CPlane& plane)                           { *this = plane;						}
	CPlane(float fA, float fB, float fC, float fD)        { Set(fA, fB, fC, fD);				}
	CPlane(const CVector3& point, const CVector3& normal) { Build(point, normal);				}

	CPlane(const CVector3& point1, const CVector3& point2, const CVector3& point3)
	{
		Build(point1, point2, point3);
	}

	~CPlane() { }

	// Read/Write

	bool				Read (CInput*  pInput);
	bool				Write(COutput* pOutput) const;

	// Access

	void                Set(float fA, float fB, float fC, float fD);

	void                Build(const CVector3& point,  const CVector3& normal);
	void                Build(const CVector3& point1, const CVector3& point2, const CVector3& point3);

	__forceinline float A()      const { return m_fA; }
	__forceinline float B()      const { return m_fB; }
	__forceinline float C()      const { return m_fC; }
	__forceinline float D()      const { return m_fD; }

	__forceinline void  SetA(float fA) { m_fA = fA; }
	__forceinline void  SetB(float fB) { m_fB = fB; }
	__forceinline void  SetC(float fC) { m_fC = fC; }
	__forceinline void  SetD(float fD) { m_fD = fD; }

	CVector3 Normal() const { return CVector3(m_fA, m_fB, m_fC); }

	// Geometric operations

	float Angle        (const CPlane&   plane)  const;
	float Angle        (const CRay&		ray)    const;
	float Angle        (const CVector3& vector) const;

	bool  Intersection (const CRay&   ray,   CVector3* intersection) const;
	bool  Intersection (const CPlane& plane, CRay*     intersection) const;

	// Operators.

	CPlane& operator = (const CPlane& plane);


private:


	float m_fA, m_fB, m_fC, m_fD;

};


#pragma pack() // Restore default alignment


#endif
