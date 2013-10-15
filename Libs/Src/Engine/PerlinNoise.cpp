// ==[ File ]===================================================================================
//
//  -Name     : PerlinNoise.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPerlinNoise implementation
//
//  -Credits  : Most routines based upon Hugo Elias' perlin noise tutorial.
//              http://freespace.virgin.net/hugo.elias/
//
// =============================================================================================

/*

	14/02/2004  -  File created.

*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


#define MAXPRIMES 36

static int pnPrimes[MAXPRIMES] = {
10007,  17839,  20533,  22093,  22271,  44257,  55633,  66931,  84179,  101723, 113723, 131707,
149867, 174617, 199399, 230653, 256181, 275461, 294979, 320627, 340127, 359581, 379649, 406183,
425989, 506101, 649007, 704023, 759557, 780163, 800693, 828601, 856789, 884497, 990851, 999931 };


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPerlinNoise2D
//  - prototype : CPerlinNoise2D()
//
//  - Purpose   : Default constructor
//
// -----------------------------------------------------------------------------
CPerlinNoise2D::CPerlinNoise2D()
{
	Init(0, 4, 1.0f, 0.5f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPerlinNoise2D
//  - prototype : ~CPerlinNoise2D()
//
//  - Purpose   : Destructor
//
// -----------------------------------------------------------------------------
CPerlinNoise2D::~CPerlinNoise2D()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPerlinNoise2D
//  - prototype : bool Init(int nSeed, int nOctaves, float fStartFrequency, float fPersistence)
//
//  - Purpose   : Configures the perlin noise parameters that will be used:
//
//                nSeed           : Seed number for the noise functions.
//                nOctaves        : How many noise functions to add together.
//                                  Each one with half the previous frequency.
//                fStartFrequency : The frequency of the first octave.
//                fPersistence    : Factor that will be used to compute the
//                                  amplitude of each octave.
//                                  Each one will have the previous amplitude
//                                  multiplied by this value.
//
// -----------------------------------------------------------------------------
bool CPerlinNoise2D::Init(int nSeed, int nOctaves, float fStartFrequency, float fPersistence)
{
	m_nSeed            = nSeed;
	m_nOctaves         = nOctaves;
	m_fStartFrequency  = fStartFrequency;
	m_fPersistence     = fPersistence;

	int nCount = 0;

	for(int i = 0; i < nOctaves; i++)
	{
		m_nOctavePrimes[i][0] = pnPrimes[(nSeed + nCount) % MAXPRIMES]; nCount++;
		m_nOctavePrimes[i][1] = pnPrimes[(nSeed + nCount) % MAXPRIMES]; nCount++;
		m_nOctavePrimes[i][2] = pnPrimes[(nSeed + nCount) % MAXPRIMES]; nCount++;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPerlinNoise2D
//  - prototype : bool Evaluate(int x, int y)
//
//  - Purpose   : Computes the perlin noise value (range 0.0 - 1.0) given the
//                two dimensional coordinates.
//
// -----------------------------------------------------------------------------
float CPerlinNoise2D::Evaluate(int x, int y)
{
	float fAccum     = 0.0f;
	float fFrequency = m_fStartFrequency;
	float fAmplitude = 1.0f;

	for(int i = 0; i < m_nOctaves; i++)
	{
		fAccum += Noise(i, (int)(x * fFrequency), (int)(y * fFrequency)) * fAmplitude;

		fFrequency = fFrequency * 2.0f;
		fAmplitude = fAmplitude * m_fPersistence;
	}

	return (fAccum + 1.0f) * 0.5f; // Adjust range
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CPerlinNoise2D
//  - prototype : float Noise(int nOctave, int x, int y)
//
//  - Purpose   : Evaluates our noise function and returns a value in the
//                -1.0 +1.0 range given the octave and two dimension parameters.
//
// -----------------------------------------------------------------------------
float CPerlinNoise2D::Noise(int nOctave, int x, int y)
{
	int n;

	n = x + y * 57;
	n = n + m_nSeed;
    n = (n << 13) ^ n;

    return (1.0f - ((n * (n * n * m_nOctavePrimes[nOctave][0] + m_nOctavePrimes[nOctave][1]) + m_nOctavePrimes[nOctave][2]) & 0x7fffffff) / 1073741824.0f);  
	//return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);  
}