// ==[ File ]===================================================================================
//
//  -Name     : InputZipFile.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputZipFile definition.
//
// =============================================================================================

/*

	27/09/2001  -  File created.


*/


#ifndef __INPUTZIPFILE_H
#define __INPUTZIPFILE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CInputZipFile
//
//  - Purpose   : Class to handle zip file data reads.
//
//  - Note      : See CInput in Input.h
//
// ---------------------------------------------------------------------------
class CInputZipFile : public CInput
{


public:


	CInputZipFile();
	CInputZipFile(const std::string& strZipFile, const std::string& strFile)
	{
		Open(strZipFile, strFile);
	}

	~CInputZipFile() { Close(); }

	bool Open(const std::string& strZipFile, const std::string& strFile);

	bool Ready() const { return m_FileInMemory.Ready(); }
	bool Close();

	bool ReadChar (char*  pchDest);
	bool ReadInt  (int*   pnDest);
	bool ReadFloat(float* pfDest);

	int  ReadRawData   (int nNumBytes, int nNumTimes, void* pDestiny);
	int  ReadStringZero(std::string *pstrDestString);

	int  GetLength()   const { return m_FileInMemory.GetLength(); }
	int  GetPosition() const;
	bool SetPosition(int nPosition, ESeekFrom eSeekFrom);


private:


	CInputMemory m_FileInMemory;

	void* m_pBuffer;

};


#endif
