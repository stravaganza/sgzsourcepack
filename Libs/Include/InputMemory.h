// ==[ File ]===================================================================================
//
//  -Name     : InputMemory.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputMemory definition.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


#ifndef __INPUTMEMORY_H
#define __INPUTMEMORY_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CInputMemory
//
//  - Purpose   : Class to handle memory data reads.
//
//  - Note      : See CInput in Input.h
//
// ---------------------------------------------------------------------------
class CInputMemory : public CInput
{


public:


	CInputMemory();
	CInputMemory(void *pMemory, int nLength) { Open(pMemory, nLength); }
	~CInputMemory()                          { Close();                }

	bool Open(void *pMemory, int nLength);

	bool Ready() const { return m_pMemory != NULL; }
	bool Close();

	bool ReadChar (char*  pchDest);
	bool ReadInt  (int*   pnDest);
	bool ReadFloat(float* pfDest);

	int  ReadRawData   (int nNumBytes, int nNumTimes, void* pDestiny);
	int  ReadStringZero(std::string *pstrDestString);

	int  GetLength()   const { return m_nLength; }
	int  GetPosition() const;
	bool SetPosition(int nPosition, ESeekFrom eSeekFrom);


private:


	int   m_nLength;
	int   m_nCursor;
	void* m_pMemory;

};


#endif
