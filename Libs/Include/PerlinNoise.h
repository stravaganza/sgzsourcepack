// ==[ File ]===================================================================================
//
//  -Name     : PerlinNoise.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CPerlinNoise definition
//
// =============================================================================================

/*

	14/02/2004  -  File created.

*/


#ifndef __PERLINNOISE_H
#define __PERLINNOISE_H


#define MAXOCTAVES 12


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CPerlinNoise2D
//
//  - Purpose   : Encapsulate 2D perlin noise functionality.
//
// ---------------------------------------------------------------------------
class CPerlinNoise2D
{


public:


	CPerlinNoise2D();
	~CPerlinNoise2D();

	bool  Init    (int nSeed, int nOctaves, float fStartFrequency, float fPersistence);
	float Evaluate(int x, int y);


private:


	float Noise(int nOctave, int x, int y);


	int   m_nSeed;
	int   m_nOctaves;
	float m_fStartFrequency;
	float m_fPersistence;

	int   m_nOctavePrimes[MAXOCTAVES][3];

};


#endif
