// ==[ File ]===================================================================================
//
//  -Name     : OutputFile.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : COutputFile definition.
//
// =============================================================================================

/*

	27/09/2001  -  File created.


*/


#ifndef __OUTPUTFILE_H
#define __OUTPUTFILE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : COutputFile
//
//  - Purpose   : File data output.
//
//  - Note      : Data output is limited to secuential order.
//
// ---------------------------------------------------------------------------
class COutputFile : public COutput
{


public:


	COutputFile();
	~COutputFile() { Close(); }

	bool Open(const std::string& strFile, bool bAsText, bool bAppend);

	bool Ready() const { return m_pFile != NULL; }
	bool Close();

	bool WriteChar (char chValue);
	bool WriteInt  (int   nValue);
	bool WriteFloat(float fValue);

	int  WriteRawData   (int nNumBytes, int nNumTimes, const void *pSource);
	int  WriteStringZero(const std::string& strStringZero);


private:


	FILE* m_pFile;

};


#endif
