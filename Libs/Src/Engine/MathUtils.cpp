// ==[ File ]===================================================================================
//
//  -Name     : MathUtils.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Some utility functions.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/



// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Functions ]================================================================================


// If pfValue is outside the range (start, end), this function
// recomputes the value as if it had a looping behaviour.
void ComputeLoop(float fStart, float fEnd, float* pfValue)
{
	assert(pfValue);

	float fNumLoops, fLength = fEnd - fStart;

	if(*pfValue > fEnd)
	{
		fNumLoops = floorf((*pfValue - fStart) / fLength); // Represents integer.
		*pfValue  = *pfValue - (fNumLoops * fLength);
	}
	else if(*pfValue < fStart)
	{
		fNumLoops = floorf((fEnd - *pfValue) / fLength); // Represents integer.
		*pfValue  = *pfValue + (fNumLoops * fLength);
	}
}

// Returns the linear interpolation between start and end with parameter fT
float ComputeLERP(float fStart, float fEnd, float fT)
{
	return fStart + (fEnd - fStart) * fT;
}

// Gets a random float value in the range (min, max)
float ComputeRand(float fMin, float fMax)
{
	// 0.0f <= fNormRand <= 1.0f

	float fNormRand = rand() / (float)RAND_MAX;

	return (fMin + ((fMax - fMin) * fNormRand));
}

// Gets a random float value with a variation (percentage/100)
float ComputeRandWithVariation(float fValue, float fVariation)
{
	// 0.0f <= fNormRand <= 1.0f

	float fNormRand = rand() / (float)RAND_MAX;
	float fAmp      = fValue * fVariation;
	float fRand     = -fAmp + ((fAmp * 2.0f) * fNormRand);

	return fValue + fRand;
}

// Gets a random float value in the range (min, max) with a random variation (percentage/100)
float ComputeRandWithVariation(float fMin, float fMax, float fVariation)
{
	// 0.0f <= fNormRand <= 1.0f

	float fNormRand = rand() / (float)RAND_MAX;
	float fRand     = fMin + ((fMax - fMin) * fNormRand);

	return fRand + ((fMax - fMin) * ComputeRand(-fVariation, fVariation));
}

