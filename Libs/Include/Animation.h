// ==[ File ]===================================================================================
//
//  -Name     : Animation.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAnimation definition.
//
// =============================================================================================

/*

	22/11-2002  -  File created.


*/


#ifndef __ANIMATION_H
#define __ANIMATION_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAnimation
//
//  - Purpose   : Animation base class.
//
// ---------------------------------------------------------------------------
class CAnimation
{


public:


	CAnimation()          { m_bLoop = false; }
	virtual ~CAnimation() {                  }

	inline bool  Looped       ()            const { return m_bLoop;  }
	inline void  SetLoop      (bool bLoop)        { m_bLoop = bLoop; }

	virtual bool HasData      () const = 0;
	virtual bool Delete       ()       = 0;
	virtual bool GetStartTime (float* pfOut) const = 0;
	virtual bool GetEndTime   (float* pfOut) const = 0;

	virtual bool Read         (CInput*  pInput)  = 0;
	virtual bool Write        (COutput* pOutput) = 0;

	bool ValidTime            (float fTime) const;

	// Static methods:

	static CAnimation* GetNewInstance(int nID);


private:


	bool m_bLoop;
};


#endif