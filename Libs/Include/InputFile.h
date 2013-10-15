// ==[ File ]===================================================================================
//
//  -Name     : InputFile.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputFile definition.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


#ifndef __INPUTFILE_H
#define __INPUTFILE_H


// ==[ Functions ]==============================================================================


bool		FileExists		 (const std::string& strFile);
std::string GetFileExt       (const std::string& strFile);
std::string GetFileFromPath  (const std::string& strPath);
std::string GetFileFolder    (const std::string& strPath);
std::string GetRelativePath  (const std::string& strBaseDir, const std::string& strFile);
std::string BuildFileFullPath(const std::string& strBaseDir, const std::string& strFile);


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CInputFile
//
//  - Purpose   : Class to handle file data reads.
//
//  - Note      : See CInput in Input.h
//
// ---------------------------------------------------------------------------
class CInputFile : public CInput
{


public:


	CInputFile();
	CInputFile(const std::string& strFile, bool bAsText) { Open(strFile, bAsText); }
	~CInputFile() { Close();}

	bool Open(const std::string& strFile, bool bAsText);

	bool Ready() const { return m_pFile != NULL; }
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
	FILE* m_pFile;

};


#endif
