// ==[ File ]===================================================================================
//
//  -Name     : InputRarFile.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInputRarFile definition.
//
// =============================================================================================

/*

	26/02/2004  -  File created.


*/


#ifndef __INPUTRARFILE_H
#define __INPUTRARFILE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CInputRarFile
//
//  - Purpose   : Class to handle rar file data reads.
//
//  - Note      : See CInput in Input.h
//
// ---------------------------------------------------------------------------
class CInputRarFile : public CInput
{


public:


	CInputRarFile();
	CInputRarFile(const std::string& strRarFile, const std::string& strFile)
	{
		Open(strRarFile, strFile);
	}

	~CInputRarFile() { Close(); }

	bool Open(const std::string& strRarFile, const std::string& strFile);

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


	static void SetPassword(const std::string& strPassword) { s_strPassword = strPassword; }


private:


	static std::string s_strPassword;

	CInputMemory m_FileInMemory;

	void* m_pBuffer;

};


#endif
