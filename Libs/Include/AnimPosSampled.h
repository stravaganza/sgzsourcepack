// ==[ File ]===================================================================================
//
//  -Name     : AnimPosSampled.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimPosSampled definition.
//
// =============================================================================================

/*

	22/10/2002  -  File created.


*/


#ifndef __ANIMPOSSAMPLED_H
#define __ANIMPOSSAMPLED_H


#pragma pack(1)


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimPosSampled
//
//  - Purpose   : Sampled 3D position animation.
//
// ---------------------------------------------------------------------------
class CAnimPosSampled : public CAnimPos
{


public:


	struct TSample
	{
		CVector3 pos;
	};

	CAnimPosSampled();
	~CAnimPosSampled() { Delete(); }

	// From CAnimation:

	bool HasData() const { return m_pSamples != NULL; }
	bool Delete();
	bool GetStartTime(float* pfOut) const;
	bool GetEndTime  (float* pfOut) const;

	bool Read (CInput*  pInput);
	bool Write(COutput* pOutput);

	// From CAnimPos:

	bool Evaluate(float fTime, CVector3* pv3Out) const;

	// Own:

	bool SetData(TSample* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime);


private:


	TSample* m_pSamples;
	float    m_fStartTime;
	float    m_fEndTime;
	unsigned m_uNumSamples;
	unsigned m_uFrecuency;
};


#pragma pack()


#endif