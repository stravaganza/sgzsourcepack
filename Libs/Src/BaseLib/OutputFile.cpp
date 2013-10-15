// ==[ File ]===================================================================================
//
//  -Name     : OutputFile.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : COutputFile implementation.
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
//  - Class     : COutputFile
//  - Prototype : COutputFile()
//
//  - Purpose   : COutputFile's default constructor
//
// -----------------------------------------------------------------------------
COutputFile::COutputFile()
{
	m_pFile = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : bool Open(const std::string& strFile, bool bAsText, bool bAppend)
//
//  - Purpose   : Opens a file and returns true if succesfull, otherwise false.
//                If bAsText is true, file will be opened as text. Otherwise
//                it will be opened in binary mode.
//                If bAppend is true, file will be opened in append mode, thus
//                data will be written at the end of the file, saving previous
//                file content. Otherwise, file will be overwritten.
//
// -----------------------------------------------------------------------------
bool COutputFile::Open(const std::string& strFile, bool bAsText, bool bAppend)
{
	assert(strFile.data());

	if(m_pFile)
	{
		fclose(m_pFile);
	}

	switch(bAsText)
	{

	case true:

		if(bAppend)
		{
			m_pFile = fopen(strFile.data(), "a+");
		}
		else
		{
			m_pFile = fopen(strFile.data(), "wt");
		}

		break;

	case false:

		if(bAppend)
		{
			m_pFile = fopen(strFile.data(), "a+b");
		}
		else
		{
			m_pFile = fopen(strFile.data(), "wb");
		}

		break;

	}

	return m_pFile ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : bool Close()
//
//  - Purpose   : Closes the file.
//
// -----------------------------------------------------------------------------
bool COutputFile::Close()
{
	if(m_pFile)
	{
		fclose(m_pFile);
	}

	m_pFile = NULL;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : bool WriteChar(char chValue)
//
//  - Purpose   : Writes a char and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputFile::WriteChar(char chValue)
{
	assert(m_pFile);

	return fputc(chValue, m_pFile) != EOF;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : bool WriteInt(int nValue)
//
//  - Purpose   : Writes an int and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputFile::WriteInt(int nValue)
{
	assert(m_pFile);

	return fwrite(&nValue, sizeof(int), 1, m_pFile) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : bool WriteFloat(float fValue)
//
//  - Purpose   : Writes a float and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool COutputFile::WriteFloat(float fValue)
{
	assert(m_pFile);

	return fwrite(&fValue, sizeof(float), 1, m_pFile) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : int WriteRawData(int nNumBytes, int nNumTimes, const void* pSource)
//
//  - Purpose   : Writes nNumBytes bytes nNumTimes times.
//                Returns the number of times actually fully written, which may
//                be less or equal to nNumTimes.
//
// -----------------------------------------------------------------------------
int COutputFile::WriteRawData(int nNumBytes, int nNumTimes, const void* pSource)
{
	assert(pSource);
	assert(m_pFile);

	return fwrite(pSource, nNumBytes, nNumTimes, m_pFile);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : COutputFile
//  - Prototype : int WriteStringZero(const std::string& strStringZero)
//
//  - Purpose   : Writes a null-terminated string and returns the number of
//                chars (including null termination) written.
//
// -----------------------------------------------------------------------------
int COutputFile::WriteStringZero(const std::string& strStringZero)
{
	assert(strStringZero.data());
	assert(m_pFile);

	bool bOK = true;

	int nLen = strStringZero.size();

	if(nLen > 0)
	{
		if(fwrite(strStringZero.data(), nLen, 1, m_pFile) != 1)
		{
			bOK = false;
		}
	}

	if(fputc('\0', m_pFile) == EOF)
	{
		bOK = false;
	}

	return bOK;
}
