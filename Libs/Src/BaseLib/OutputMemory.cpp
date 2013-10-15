// ==[ File ]===================================================================================
//
//  -Name     : OutputMemory.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : COutputMemory implementation.
//
// =============================================================================================

/*

	27/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : COutputMemory()
//
//  - Purpose   : COutputMemory's default constructor
//
// -----------------------------------------------------------------------------
COutputMemory::COutputMemory()
{
	m_pDestiny = NULL;
	m_nLength  = 0;
	m_nCursor  = 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : bool Open(void *pDestiny, int nLength)
//
//  - Purpose   : Grabs a memory pointer to write data to. Writes will be
//                restricted in the range (pDestiny, pDestiny + nLength - 1).
//                Returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputMemory::Open(void *pDestiny, int nLength)
{
	assert(pDestiny);
	assert(nLength > 1);

	if(Ready())
	{
		Close();
	}

	m_pDestiny = pDestiny;
	m_nLength  = nLength;
	m_nCursor  = 0;

	return m_pDestiny ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : bool Close()
//
//  - Purpose   : Invalids the memory to avoid further writings.
//
// -----------------------------------------------------------------------------
bool COutputMemory::Close()
{
	m_pDestiny = NULL;
	m_nLength  = 0;
	m_nCursor  = 0;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : bool WriteChar(char chValue)
//
//  - Purpose   : Writes a char and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputMemory::WriteChar(char chValue)
{
	assert(m_pDestiny);

	if(m_nCursor >= m_nLength)
	{
		return false;
	}

	((char *)m_pDestiny)[m_nCursor] = chValue;

	m_nCursor += sizeof(char);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : bool WriteInt(int nValue)
//
//  - Purpose   : Writes an int and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputMemory::WriteInt(int nValue)
{
	assert(m_pDestiny);

	if(m_nCursor >= m_nLength)
	{
		return false;
	}

	if((m_nCursor + (int)sizeof(int)) > m_nLength)
	{
		return false;
	}

	*(int *)((char *)m_pDestiny + m_nCursor) = nValue;

	m_nCursor += sizeof(int);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : bool WriteFloat(float fValue)
//
//  - Purpose   : Writes a float and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputMemory::WriteFloat(float fValue)
{
	assert(m_pDestiny);

	if(m_nCursor >= m_nLength)
	{
		return false;
	}

	if((m_nCursor + (int)sizeof(float)) > m_nLength)
	{
		return false;
	}

	*(float *)((char *)m_pDestiny + m_nCursor) = fValue;

	m_nCursor += sizeof(float);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : int WriteRawData(int nNumBytes, int nNumTimes, const void* pSource)
//
//  - Purpose   : Writes nNumBytes bytes nNumTimes times.
//                Returns the number of times actually fully written, which may
//                be less or equal to nNumTimes.
//
// -----------------------------------------------------------------------------
int COutputMemory::WriteRawData(int nNumBytes, int nNumTimes, const void* pSource)
{
	assert(pSource);
	assert(m_pDestiny);

	if(m_nCursor < 0 || m_nCursor >= m_nLength)
	{
		return 0;
	}

	int nArraySize = nNumBytes * nNumTimes;
	int nValidSize = nArraySize;

	if(m_nCursor + nArraySize > m_nLength)
	{
		nValidSize = m_nLength - m_nCursor;
	}

	char *pchDestiny = (char *)m_pDestiny;

	for(int nCount = 0; nCount < nValidSize; nCount++, m_nCursor++)
	{
		pchDestiny[m_nCursor] = ((char *)pSource)[nCount];
	}
	
	return (nCount / (nArraySize / nValidSize));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputMemory
//  - Prototype : int WriteStringZero(const std::string& strStringZero)
//
//  - Purpose   : Writes a null-terminated string and returns the number of
//                chars (including null termination) written.
//
// -----------------------------------------------------------------------------
int COutputMemory::WriteStringZero(const std::string& strStringZero)
{
	assert(strStringZero.data());
	assert(m_pDestiny);

	bool bOK  = true;
	int  nLen = strStringZero.size();

	if(nLen > 0)
	{
		if(WriteRawData(nLen, 1, strStringZero.data()) != 1)
		{
			bOK = false;
		}
	}

	if(WriteChar('\0') == false)
	{
		bOK = false;
	}

	return bOK;
}
