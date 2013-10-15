// ==[ File ]===================================================================================
//
//  -Name     : OutputMemory.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : COutputMemory definition.
//
// =============================================================================================

/*

	27/09/2001  -  File created.


*/


#ifndef __OUTPUTMEMORY_H
#define __OUTPUTMEMORY_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : COutputMemory
//
//  - Purpose   : Memory data writing.
//
//  - Note      : Data output is limited to secuential order.
//
// ---------------------------------------------------------------------------
class COutputMemory : public COutput
{


public:


	COutputMemory();
	~COutputMemory() { Close(); }

	bool Open(void *pDestiny, int nLength);

	bool Ready() const { return m_pDestiny != NULL; }
	bool Close();

	bool WriteChar (char chValue);
	bool WriteInt  (int   nValue);
	bool WriteFloat(float fValue);

	int  WriteRawData   (int nNumBytes, int nNumTimes, const void *pSource);
	int  WriteStringZero(const std::string& strStringZero);


private:


	void* m_pDestiny;
	int   m_nLength;
	int   m_nCursor;

};


#endif
