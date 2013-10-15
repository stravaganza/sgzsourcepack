// ==[ File ]===================================================================================
//
//  -Name     : InputFile.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputFile implementation.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Functions ]==============================================================================


// Returns true if the given file exists
bool FileExists(const std::string& strFile)
{
	if(strFile != "")
	{
		FILE* fp;
		bool bExists = (fp = fopen(strFile.data(), "rb")) != NULL;

		if(bExists)
		{
			fclose(fp);
		}

		return bExists;
	}

	return false;
}

// Returns the given file's extension (f.e. "c:\data.dat" -> "dat")
std::string GetFileExt(const std::string& strFile)
{
	assert(strFile != "");

	int nExtStart = strFile.size();

	while(nExtStart > 0)
	{
		nExtStart--;

		if(strFile[nExtStart] == '.')
		{
			break;
		}
	}

	if(nExtStart)
	{
		return strFile.substr(nExtStart + 1, strFile.length() - nExtStart + 1);
	}

	return "";
}

// Returns only the filename (excluding whole path) of a given file. (f.e. "c:\folder\data.dat" -> "data.dat")
std::string GetFileFromPath(const std::string& strPath)
{
	assert(strPath != "");

	int nFileStart = strPath.size();

	while(nFileStart > 0)
	{
		nFileStart--;

		if(strPath[nFileStart] == '\\')
		{
			break;
		}
	}

	if(nFileStart)
	{
		return strPath.substr(nFileStart + 1, strPath.length() - nFileStart + 1);
	}

	return strPath;
}

// Returns the path excluding filename (f.e. "c:\folder\data.dat" -> "c:\folder\")
std::string GetFileFolder(const std::string& strPath)
{
	assert(strPath != "");

	int nFileStart = strPath.size();

	while(nFileStart > 0)
	{
		nFileStart--;

		if(strPath[nFileStart] == '\\')
		{
			break;
		}
	}

	if(nFileStart)
	{
		return strPath.substr(0, nFileStart + 1);
	}

	return "";
}

// Returns the relative path from strFile to strBaseDir (f.e. "c:\folder\", "c:\folder\data\1.dat" -> "data\1.dat")
std::string GetRelativePath(const std::string& strBaseDir, const std::string& strFile)
{
	if(strFile.find(strBaseDir, 0) != 0)
	{
		return strFile;
	}

	return strFile.substr(strBaseDir.size(), strFile.size() - strBaseDir.size());
}

// Builds a full path from basedir + file (f.e. "c:\folder\", "data\1.dat" -> "c:\folder\data\1.dat")
std::string BuildFileFullPath(const std::string& strBaseDir, const std::string& strFile)
{
	assert(strBaseDir != "");
	assert(strFile    != "");

	std::string strFullPath = strBaseDir;

	if(strBaseDir[strBaseDir.length() - 1] == '\\')
	{
		if(strFile[0] == '\\')
		{
			strFullPath = strFullPath + strFile.substr(1, strFile.length() - 1);
		}
		else
		{
			strFullPath = strFullPath + strFile;
		}
	}
	else
	{
		if(strFile[0] == '\\')
		{
			strFullPath = strFullPath + strFile;
		}
		else
		{
			strFullPath = strFullPath + "\\" + strFile;
		}
	}

	return strFullPath;
}



// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : CInputFile()
//
//  - Purpose   : CInputFile's default constructor
//
// -----------------------------------------------------------------------------
CInputFile::CInputFile()
{
	m_nLength = 0;
	m_pFile   = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : bool Open(const std::string& strFile, bool bAsText)
//
//  - Purpose   : Opens a file and returns true if succesfull, otherwise false.
//                If bAsText is true, file will be opened as text. Otherwise
//                it will be opened in binary mode.
//
// -----------------------------------------------------------------------------
bool CInputFile::Open(const std::string& strFile, bool bAsText)
{
	assert(strFile.data());

	if(m_pFile)
	{
		fclose(m_pFile);
	}

	// Try to open

	if(bAsText)
	{
		m_pFile = fopen(strFile.data(), "rt");
	}
	else
	{
		m_pFile = fopen(strFile.data(), "rb");
	}

	if(m_pFile == NULL)
	{
		return false;
	}

	// Get file length

	fseek(m_pFile, 0, SEEK_END);
	m_nLength = ftell(m_pFile);

	fseek(m_pFile, 0, SEEK_SET);

	return m_pFile ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : bool Close()
//
//  - Purpose   : Closes the file.
//
// -----------------------------------------------------------------------------
bool CInputFile::Close()
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
//  - Class     : CInputFile
//  - Prototype : bool ReadChar(int* pchDest)
//
//  - Purpose   : Reads a char and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputFile::ReadChar(char* pchDest)
{
	if(m_pFile == NULL)
	{
		return false;
	}

	int nCar = fgetc(m_pFile);

	if(nCar == EOF)
	{
		return false;
	}

	*pchDest = nCar;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : bool ReadInt(int* pnDest)
//
//  - Purpose   : Reads an int and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputFile::ReadInt(int* pnDest)
{
	assert(pnDest);

	if(m_pFile == NULL)
	{
		return false;
	}

	return fread(pnDest, sizeof(int), 1, m_pFile) == 1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : bool ReadFloat(float* pfDest)
//
//  - Purpose   : Reads a float and returns true if succesfull, otherwise false.
//
// -----------------------------------------------------------------------------
bool CInputFile::ReadFloat(float* pfDest)
{
	assert(pfDest);

	if(m_pFile == NULL)
	{
		return false;
	}

	return fread(pfDest, sizeof(float), 1, m_pFile) == 1;

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : int ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
//
//  - Purpose   : Reads nNumBytes bytes nNumTimes times and puts each of them
//                in pDestiny.
//                Returns the number of times actually fully read, which may
//                be less or equal to nNumTimes.
//
// -----------------------------------------------------------------------------
int CInputFile::ReadRawData(int nNumBytes, int nNumTimes, void* pDestiny)
{
	assert(pDestiny);

	if(m_pFile == NULL)
	{
		return false;
	}

	return fread(pDestiny, nNumBytes, nNumTimes, m_pFile);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : int ReadStringZero(std::string *pstrDestString)
//
//  - Purpose   : Reads a null-terminated string and returns the number of
//                chars (excluding null termination) read.
//
// -----------------------------------------------------------------------------
int CInputFile::ReadStringZero(std::string *pstrDestString)
{
	assert(pstrDestString);

	pstrDestString->erase();

	if(m_pFile == NULL)
	{
		return false;
	}

	int  nCount = 0;
	char szInsertString[2];

	szInsertString[1] = '\0';

	while(true)
	{
		szInsertString[0] = fgetc(m_pFile);

		if(feof(m_pFile))
		{
			pstrDestString->append("\0", 1);
			break;
		}

		pstrDestString->append(szInsertString, 1);
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
//  - Class     : CInputFile
//  - Prototype : int GetPosition()
//
//  - Purpose   : Returns file's read cursor position.
//
// -----------------------------------------------------------------------------
int CInputFile::GetPosition() const
{
	if(m_pFile == NULL)
	{
		return 0;
	}

	return ftell(m_pFile);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CInputFile
//  - Prototype : bool SetPosition(int nPosition, ESeekFrom eSeekFrom)
//
//  - Purpose   : Sets file's read cursor position.
//
// -----------------------------------------------------------------------------
bool CInputFile::SetPosition(int nPosition, ESeekFrom eSeekFrom)
{
	if(m_pFile == NULL)
	{
		return false;
	}

	switch(eSeekFrom)
	{

	case SEEKFROM_START  : fseek(m_pFile, nPosition, SEEK_SET); break;
	case SEEKFROM_END    : fseek(m_pFile, nPosition, SEEK_END); break;
	case SEEKFROM_CURPOS : fseek(m_pFile, nPosition, SEEK_CUR); break;

	default: return false;

	}

	return true;
}


