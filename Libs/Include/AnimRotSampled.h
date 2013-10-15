// ==[ File ]===================================================================================
//
//  -Name     : AnimRotSampled.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimRotSampled definition.
//
// =============================================================================================

/*

	22/10/2002  -  File created.


*/


#ifndef __ANIMROTSAMPLED_H
#define __ANIMROTSAMPLED_H


#pragma pack(1)


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimRotSampled
//
//  - Purpose   : Sampled 3D rotation animation.
//
// ---------------------------------------------------------------------------
class CAnimRotSampled : public CAnimRot
{


public:


	struct TSample
	{
		CQuaternion quat;
	};

	CAnimRotSampled();
	~CAnimRotSampled() { Delete(); }

	// From CAnimation:

	bool HasData() const { return m_pSamples != NULL; }
	bool Delete();
	bool GetStartTime(float* pfOut) const;
	bool GetEndTime  (float* pfOut) const;

	bool Read (CInput*  pInput);
	bool Write(COutput* pOutput);

	// From CAnimRot:

	bool Evaluate(float fTime, CQuaternion* pQuatOut) const;

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