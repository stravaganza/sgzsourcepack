// ==[ File ]===================================================================================
//
//  -Name     : InputRarFile.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputRarFile implementation.
//
//  -Notes    : Uses Unique RAR file library.
//				Copyright (C) 2000-2002 by Christian Scheurer (www.ChristianScheurer.ch)
//
// =============================================================================================

/*

	26/02/2004  -  File created.

*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"
#include "unrarlib.h"


// ==[ Class implementation ]===================================================================


std::string CInputRarFile::s_strPassword = "";

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : CInputRarFile()
//
//  - Purpose   : CInputRarFile's default constructor
//
// -----------------------------------------------------------------------------
CInputRarFile::CInputRarFile()
{
	m_pBuffer  = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : bool Open(const std::string& strRarFile, 
//                          const std::string& strFile)
//
//  - Purpose   : Opens a file inside a rar and sets it ready to read from.
//                Closes previously opened rar if any.
//
// -----------------------------------------------------------------------------
bool CInputRarFile::Open(const std::string& strRarFile, const std::string& strFile)
{
	assert(strRarFile.data());
	assert(strFile.data());

	if(Ready())
	{
		Close();
	}

	// Open rar

	unsigned long ulSize;

	if(urarlib_get(	&m_pBuffer, &ulSize, (char*)strFile.data(), (char*)strRarFile.data(),
					s_strPassword == "" ? NULL : (char*)s_strPassword.data()) == FALSE)
	{
		return false;
	}

	if(m_FileInMemory.Open(m_pBuffer, ulSize) == false)
	{
		SAFE_DELETE_A(m_pBuffer);
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : bool Close()
//
//  - Purpose   : Closes the file.
//
// -----------------------------------------------------------------------------
bool CInputRarFile::Close()
{
	SAFE_DELETE_A(m_pBuffer);

	return m_FileInMemory.Close();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : bool ReadChar(int* pchDest)
//
//  - Purpose   : Reads a char and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputRarFile::ReadChar(char* pchDest)
{
	return m_FileInMemory.ReadChar(pchDest);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : bool ReadInt(int* pnDest)
//
//  - Purpose   : Reads an int and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputRarFile::ReadInt(int* pnDest)
{
	return m_FileInMemory.ReadInt(pnDest);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : bool ReadFloat(float* pfDest)
//
//  - Purpose   : Reads a float and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputRarFile::ReadFloat(float* pfDest)
{
	return m_FileInMemory.ReadFloat(pfDest);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : int ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
//
//  - Purpose   : Reads nNumBytes bytes nNumTimes times and puts each of them
//                in pDestiny.
//                Returns the number of times actually fully read, which may
//                be less or equal to nNumTimes.
//
// -----------------------------------------------------------------------------
int CInputRarFile::ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
{
	return m_FileInMemory.ReadRawData(nNumBytes, nNumTimes, pDestiny);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : int ReadStringZero(std::string *pstrDestString)
//
//  - Purpose   : Reads a null-terminated string and returns the number of
//                chars (excluding null termination) read.
//
// -----------------------------------------------------------------------------
int CInputRarFile::ReadStringZero(std::string *pstrDestString)
{
	return m_FileInMemory.ReadStringZero(pstrDestString);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : int GetPosition()
//
//  - Purpose   : Returns file's read cursor position.
//
// -----------------------------------------------------------------------------
int CInputRarFile::GetPosition() const
{
	return m_FileInMemory.GetPosition();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputRarFile
//  - Prototype : bool SetPosition(int nPosition, ESeekFrom eSeekFrom)
//
//  - Purpose   : Sets file's read cursor position.
//
// -----------------------------------------------------------------------------
bool CInputRarFile::SetPosition(int nPosition, ESeekFrom eSeekFrom)
{
	return m_FileInMemory.SetPosition(nPosition, eSeekFrom);
}

