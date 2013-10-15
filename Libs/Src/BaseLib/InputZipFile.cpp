// ==[ File ]===================================================================================
//
//  -Name     : InputZipFile.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputZipFile implementation.
//
//  -Notes    : Gilles Vollant's zipreader together with zlib were used to decompress zipped
//              files into memory, so main credits go to them :)
//              I previously used a zipreader coded by Jare/Iguana but it sometimes failed
//              when trying to open a file inside a zip, dunno why.
//
// =============================================================================================

/*

	27/09/2001  -  File created.
	03/10/2002  -  Changed to Gilles Vollant's zipreader.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"
#include "unzip.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : CInputZipFile()
//
//  - Purpose   : CInputZipFile's default constructor
//
// -----------------------------------------------------------------------------
CInputZipFile::CInputZipFile()
{
	m_pBuffer  = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : bool Open(const std::string& strZipFile, 
//                          const std::string& strFile)
//
//  - Purpose   : Opens a file inside a zip and sets it ready to read from.
//                Closes the previous opened zip if any.
//
// -----------------------------------------------------------------------------
bool CInputZipFile::Open(const std::string& strZipFile, const std::string& strFile)
{
	assert(strZipFile.data());
	assert(strFile.data());

	if(Ready())
	{
		Close();
	}

	// Open Zip

	unzFile       zipHandle;
	unz_file_info zipInfo;

	if(!(zipHandle = unzOpen(strZipFile.data())))
	{
		LOG.Write("\nERROR - Couldn't open zip %s", strZipFile.data());
		return false;
	}

	// Try to locate the file inside (2 means without case sensivity).

	if(unzLocateFile(zipHandle, strFile.data(), 2) != UNZ_OK)
	{
		LOG.Write("\nERROR - Couldn't locate file %s inside zip %s", strFile.data(), strZipFile.data());
		unzClose(zipHandle);
		return false;
	}

	// Try to open the located file.

	if(unzOpenCurrentFile(zipHandle) != UNZ_OK)
	{
		LOG.Write("\nERROR - Couldn't open file %s inside zip %s", strFile.data(), strZipFile.data());
		unzClose(zipHandle);
		return false;
	}

	// Get length

	if(unzGetCurrentFileInfo(zipHandle, &zipInfo, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
	{
		LOG.Write("\nERROR - Couldn't get file %s length. File is inside zip %s", strFile.data(), strZipFile.data());
		unzCloseCurrentFile(zipHandle);
		unzClose(zipHandle);
		return false;
	}

	// Copy file to buffer.

	char* pcBuffer = new char[zipInfo.uncompressed_size];

	if(pcBuffer == NULL)
	{
		LOG.Write("\nERROR - Out of memory reading file %s inside zip %s", strFile.data(), strZipFile.data());
		unzCloseCurrentFile(zipHandle);
		unzClose(zipHandle);
		return false;
	}

	int nBytesRead = unzReadCurrentFile(zipHandle, pcBuffer, zipInfo.uncompressed_size);

	if(nBytesRead != zipInfo.uncompressed_size)
	{
		LOG.Write("\nERROR - File %s inside zip %s not completely read (%u/%u)", strFile.data(), strZipFile.data(), nBytesRead, zipInfo.uncompressed_size);
		unzCloseCurrentFile(zipHandle);
		unzClose(zipHandle);
		SAFE_DELETE_A(pcBuffer);
		return false;
	}

	// Close and open our internal memory reader.

	unzCloseCurrentFile(zipHandle);
	unzClose(zipHandle);

	m_pBuffer = pcBuffer;

	if(m_FileInMemory.Open(m_pBuffer, nBytesRead) == false)
	{
		SAFE_DELETE_A(m_pBuffer);
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : bool Close()
//
//  - Purpose   : Closes the file.
//
// -----------------------------------------------------------------------------
bool CInputZipFile::Close()
{
	SAFE_DELETE_A(m_pBuffer);

	return m_FileInMemory.Close();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : bool ReadChar(int* pchDest)
//
//  - Purpose   : Reads a char and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputZipFile::ReadChar(char* pchDest)
{
	return m_FileInMemory.ReadChar(pchDest);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : bool ReadInt(int* pnDest)
//
//  - Purpose   : Reads an int and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputZipFile::ReadInt(int* pnDest)
{
	return m_FileInMemory.ReadInt(pnDest);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : bool ReadFloat(float* pfDest)
//
//  - Purpose   : Reads a float and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputZipFile::ReadFloat(float* pfDest)
{
	return m_FileInMemory.ReadFloat(pfDest);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : int ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
//
//  - Purpose   : Reads nNumBytes bytes nNumTimes times and puts each of them
//                in pDestiny.
//                Returns the number of times actually fully read, which may
//                be less or equal to nNumTimes.
//
// -----------------------------------------------------------------------------
int CInputZipFile::ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
{
	return m_FileInMemory.ReadRawData(nNumBytes, nNumTimes, pDestiny);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : int ReadStringZero(std::string *pstrDestString)
//
//  - Purpose   : Reads a null-terminated string and returns the number of
//                chars (excluding null termination) read.
//
// -----------------------------------------------------------------------------
int CInputZipFile::ReadStringZero(std::string *pstrDestString)
{
	return m_FileInMemory.ReadStringZero(pstrDestString);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : int GetPosition()
//
//  - Purpose   : Returns file's read cursor position.
//
// -----------------------------------------------------------------------------
int CInputZipFile::GetPosition() const
{
	return m_FileInMemory.GetPosition();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputZipFile
//  - Prototype : bool SetPosition(int nPosition, ESeekFrom eSeekFrom)
//
//  - Purpose   : Sets file's read cursor position.
//
// -----------------------------------------------------------------------------
bool CInputZipFile::SetPosition(int nPosition, ESeekFrom eSeekFrom)
{
	return m_FileInMemory.SetPosition(nPosition, eSeekFrom);
}

