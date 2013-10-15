// ==[ File ]===================================================================================
//
//  -Name     : InputMemory.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputMemory implementation.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : CInputMemory()
//
//  - Purpose   : CInputMemory's default constructor
//
// -----------------------------------------------------------------------------
CInputMemory::CInputMemory()
{
	m_nLength = 0;
	m_nCursor = 0;
	m_pMemory = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : bool Open(void *pMemory, int nLength)
//
//  - Purpose   : Grabs a memory pointer to get data from. Returns true if
//                succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputMemory::Open(void *pMemory, int nLength)
{
	assert(pMemory);
	assert(nLength > 0);

	Close();

	m_pMemory = pMemory;
	m_nCursor = 0;
	m_nLength = nLength;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : bool Close()
//
//  - Purpose   : Invalids the memory source.
//
// -----------------------------------------------------------------------------
bool CInputMemory::Close()
{
	m_pMemory = NULL;
	m_nCursor = 0;
	m_nLength = 0;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : bool ReadChar(char* pchDest)
//
//  - Purpose   : Reads a char and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputMemory::ReadChar(char* pchDest)
{
	assert(pchDest);

	if(m_pMemory == NULL)
	{
		return false;
	}

	if(m_nCursor < 0 || m_nCursor >= m_nLength)
	{
		return false;
	}

	*pchDest = ((char *)m_pMemory)[m_nCursor];

	m_nCursor += sizeof(char);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : bool ReadInt(int* pnDest)
//
//  - Purpose   : Reads an int and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputMemory::ReadInt(int* pnDest)
{
	assert(pnDest);

	if(m_pMemory == NULL)
	{
		return false;
	}

	if(m_nCursor < 0 || m_nCursor >= m_nLength)
	{
		return false;
	}

	if((m_nCursor + (int)sizeof(int)) > m_nLength)
	{
		return false;
	}

	*pnDest = *(int *)((char *)m_pMemory + m_nCursor);

	m_nCursor += sizeof(int);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : bool ReadFloat(float* pfDest)
//
//  - Purpose   : Reads a float and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputMemory::ReadFloat(float* pfDest)
{
	assert(pfDest);

	if(m_pMemory == NULL)
	{
		return false;
	}

	if(m_nCursor < 0 || m_nCursor >= m_nLength)
	{
		return false;
	}

	if((m_nCursor + (int)sizeof(float)) > m_nLength)
	{
		return false;
	}

	*pfDest = *(float *)((char *)m_pMemory + m_nCursor);

	m_nCursor += sizeof(float);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : int ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
//
//  - Purpose   : Reads nNumBytes bytes nNumTimes times and puts each of them
//                in pDestiny.
//                Returns the number of times actually fully read, which may
//                be less or equal to nNumTimes.
//
// -----------------------------------------------------------------------------
int CInputMemory::ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
{
	assert(pDestiny);

	if(m_pMemory == NULL)
	{
		return 0;
	}

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

	char *pchDestiny = (char *)pDestiny;

	for(int nCount = 0; nCount < nValidSize; nCount++, m_nCursor++)
	{
		pchDestiny[nCount] = ((char *)m_pMemory)[m_nCursor];
	}
	
	return nValidSize / nNumBytes;

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : int ReadStringZero(std::string *pstrDestString)
//
//  - Purpose   : Reads a null-terminated string and returns the number of
//                chars (excluding null termination) read.
//
// -----------------------------------------------------------------------------
int CInputMemory::ReadStringZero(std::string *pstrDestString)
{
	assert(pstrDestString);

	pstrDestString->erase();

	if(m_pMemory == NULL)
	{
		return 0;
	}

	char* pchMemory = (char *)m_pMemory;
	int   nCount    = 0;
	char  szInsertString[2];

	szInsertString[1] = '\0';

	while(true)
	{
		if(m_nCursor < 0 || m_nCursor >= m_nLength)
		{
			pstrDestString->append("\0", 1);
			break;
		}

		szInsertString[0] = pchMemory[m_nCursor];

		pstrDestString->append(szInsertString, 1);
		m_nCursor++;
		nCount++;

		if(szInsertString[0] == '\0')
		{
			break;
		}
	}

	return nCount;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : int GetPosition()
//
//  - Purpose   : Returns memory's read cursor position.
//
// -----------------------------------------------------------------------------
int CInputMemory::GetPosition() const
{
	return m_nCursor;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputMemory
//  - Prototype : bool SetPosition(int nPosition, ESeekFrom eSeekFrom)
//
//  - Purpose   : Sets memory's read cursor position.
//
// -----------------------------------------------------------------------------
bool CInputMemory::SetPosition(int nPosition, ESeekFrom eSeekFrom)
{
	if(m_pMemory == NULL)
	{
		return false;
	}

	switch(eSeekFrom)
	{

	case SEEKFROM_START  : m_nCursor = nPosition;             break;
	case SEEKFROM_END    : m_nCursor = m_nLength + nPosition; break;
	case SEEKFROM_CURPOS : m_nCursor = m_nCursor + nPosition; break;

	default: return false;

	}

	return true;
}


