// ==[ File ]===================================================================================
//
//  -Name     : AnimSclSampled.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimSclSampled definition.
//
// =============================================================================================

/*

	22/10/2002  -  File created.


*/


#ifndef __ANIMSCLSAMPLED_H
#define __ANIMSCLSAMPLED_H


#pragma pack(1)


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimSclSampled
//
//  - Purpose   : Sampled 3D scaling animation.
//
// ---------------------------------------------------------------------------
class CAnimSclSampled : public CAnimScl
{


public:


	struct TSample
	{
		CQuaternion axes;
		CVector3    scale;
	};

	CAnimSclSampled();
	~CAnimSclSampled() { Delete(); }

	// From CAnimation:

	bool HasData() const { return m_pSamples != NULL; }
	bool Delete();
	bool GetStartTime(float* pfOut) const;
	bool GetEndTime  (float* pfOut) const;

	bool Read (CInput*  pInput);
	bool Write(COutput* pOutput);

	// From CAnimScl:

	bool Evaluate(float fTime, CQuaternion* pAxesOut, CVector3* pv3ScaleOut) const;

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