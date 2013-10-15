// ==[ File ]===================================================================================
//
//  -Name     : AnimSclSampled.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimSclSampled implementation (see AnimSclSampled.h).
//
// =============================================================================================

/*

	09/11/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : CAnimSclSampled()
//
//  - Purpose   : CAnimSclSampled's default constructor
//
// -----------------------------------------------------------------------------
CAnimSclSampled::CAnimSclSampled() : CAnimScl()
{
	m_pSamples      = NULL;
	m_uNumSamples   = 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool Delete()
//
//  - Purpose   : Deletes all animation data.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::Delete()
{
	SAFE_DELETE_A(m_pSamples);

	m_uNumSamples = 0;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool GetStartTime(float* pfOut) const
//
//  - Purpose   : Returns (in pfOut) the animation's start time value.
//                If succesfull returns true, otherwise false. That would mean
//                that no sample data is available.
//
//  - Note      : For looped animations it will return also the start time
//                of the interval, even if times below this range are valid.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::GetStartTime(float* pfOut) const
{
	if(!HasData())
	{
		return false;
	}

	*pfOut = m_fStartTime;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool GetEndTime(float* pfOut) const
//
//  - Purpose   : Returns (in pfOut) the animation's end time value.
//                If succesfull returns true, otherwise false. That would mean
//                that no sample data is available.
//
//  - Note      : For looped animations it will return also the start time
//                of the interval, even if times below this range are valid.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::GetEndTime(float* pfOut) const
{
	if(!HasData())
	{
		return false;
	}

	*pfOut = m_fEndTime;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool Read(CInput* pInput)
//
//  - Purpose   : Reads the animation data from a source.
//
//  - Note      : This method starts reading the animation parameters, thus
//                both the start chunk signature and the animation type
//                signature have already been read. Usually a higher level
//                reader processes them in order to identify which class to
//                instance (using CAnimation::GetNewInstance()) and calls
//                Read() to finally read the whole chunk.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::Read(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	if(HasData())
	{
		Delete();
	}

	unsigned uError = 0;

	// Read parameters:

	int nBool;

	if(!pInput->ReadInt  ((int*)&m_uNumSamples)) uError++;
	if(!pInput->ReadInt  ((int*)&m_uFrecuency))  uError++;
	if(!pInput->ReadFloat(&m_fStartTime))        uError++;
	if(!pInput->ReadInt  (&nBool))               uError++;

	SetLoop(nBool ? true : false);

	m_fEndTime = m_fStartTime + (m_uNumSamples * (1.0f / m_uFrecuency));

	LOG.Write("\nSampled scaling animation: %u samples.", m_uNumSamples);

	if(uError)
	{
		m_uNumSamples = 0;
		return false;
	}

	// Read sample data:

	SAFE_DELETE_A(m_pSamples);

	if(m_uNumSamples > 0)
	{
		if(!(m_pSamples = new TSample[m_uNumSamples]))
		{
			LOG.Write("Error! (new).");
			m_uNumSamples = 0;
			return false;
		}

		if(pInput->ReadRawData(sizeof(TSample) * m_uNumSamples, 1, m_pSamples) != 1)
		{
			LOG.Write("Error! (Reading sample data).");
			Delete();
			return false;
		}
	}

	// Check integrity:

	int nChunkEnd = 0;
	pInput->ReadInt(&nChunkEnd);

	if(nChunkEnd != ID_CHUNKEND)
	{
		LOG.Write("Error! (End chunk check).");
		Delete();
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool Write(COutput* pOutput)
//
//  - Purpose   : Writes the animation data to a destiny.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::Write(COutput* pOutput)
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	// Chunk start:

	if(!pOutput->WriteInt(ID_ANIM_CHUNKSCL))   uError++;
	if(!pOutput->WriteInt(ID_ANIM_SCLSAMPLED)) uError++;

	// Write parameters:

	if(!pOutput->WriteInt  (m_uNumSamples))    uError++;
	if(!pOutput->WriteInt  (m_uFrecuency))     uError++;
	if(!pOutput->WriteFloat(m_fStartTime))     uError++;
	if(!pOutput->WriteInt  (Looped() ? 1 : 0)) uError++;

	// Write sample data:

	if(!pOutput->WriteRawData(sizeof(TSample) * m_uNumSamples, 1, m_pSamples))
	{
		uError++;
	}

	// Chunk end:

	if(!pOutput->WriteInt(ID_CHUNKEND))
	{
		uError++;
	}

	return uError == 0 ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool Evaluate(float fTime, CQuaternion* pAxesOut, CVector3* pv3ScaleOut) const
//
//  - Purpose   : Computes (and stores in pAxesOut and pv3ScaleOut if succesfull)
//                the scale and axes application values of a given time. If
//                there's no sample data or if an invalid time value is given,
//                Evaluate() will return false, otherwise true.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::Evaluate(float fTime, CQuaternion* pAxesOut, CVector3* pv3ScaleOut) const
{
	assert(pAxesOut);
	assert(pv3ScaleOut);

	if(!HasData())
	{
		return false;
	}

	if(!Looped())
	{
		if(!ValidTime(fTime))
		{
			return false;
		}
	}
	else
	{
		ComputeLoop(m_fStartTime, m_fEndTime, &fTime);
	}

	assert(m_uFrecuency);

	unsigned uSample = m_uFrecuency * (fTime - m_fStartTime);
	float    fPeriod = 1.0f / m_uFrecuency;
	float    fT      = (fTime - (m_fStartTime + (uSample * fPeriod))) * m_uFrecuency;

	if(uSample < 0 || uSample >= (m_uNumSamples - 1))
	{
		return false;
	}

	*pAxesOut    = m_pSamples[uSample].axes.SLERP(m_pSamples[uSample + 1].axes,  fT);
	*pv3ScaleOut = m_pSamples[uSample].scale.LERP(m_pSamples[uSample + 1].scale, fT);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAnimSclSampled
//  - Prototype : bool SetData(TSample* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime)
//
//  - Purpose   : Sets the animation data. The sample values, the number of them,
//                the frecuency (a constant value telling how many samples per
//                second the animation has), and the time value of the first
//                sample.
//
//  - Note      : It grabs the pointer, so pSamples must not be freed after
//                calling this method.
//
// -----------------------------------------------------------------------------
bool CAnimSclSampled::SetData(TSample* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime)
{
	assert(pSamples);

	m_pSamples    = pSamples;
	m_uNumSamples = uNumSamples;
	m_uFrecuency  = uFrecuency;
	m_fStartTime  = fStartTime;
	m_fEndTime    = fStartTime + (uNumSamples * (1.0f / uFrecuency));

	return pSamples ? true : false;
}
