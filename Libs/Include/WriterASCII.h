// ==[ File ]===================================================================================
//
//  -Name     : WriterASCII.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CWriterASCII definition.
//
// =============================================================================================

/*

	30/04/2003  -  File created.


*/


#ifndef __WRITERASCII_H
#define __WRITERASCII_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CWriterASCII
//
//  - Purpose   : Provides routines to output ascii strings (stdout/file/window).
//
// ---------------------------------------------------------------------------
class CWriterASCII
{


protected:


	enum EOutputType
	{
		OUTPUT_NONE = 0,
		OUTPUT_STDOUT,
		OUTPUT_FILE,
		OUTPUT_WINDOW
	};


public:


	CWriterASCII();
	CWriterASCII(EOutputType eType, const std::string& strName);
	~CWriterASCII();

	bool InitStdout();
	bool InitFile  (const std::string& strFile);
	bool InitWindow(const std::string& strTitle);

	bool Ready() const { return m_eOutputType != OUTPUT_NONE; }
	bool Close();

	// Standard printf-like output.

	bool Write(const char *szMessage, ...) const;

	// Indentation level control:

	int  Indentation()              const { return m_nIndentation;          }
	void SetIndentation(int nIndentChars) { m_nIndentation  = nIndentChars; }
	void IncIndentation(int nInc)         { m_nIndentation += nInc;         }
	void DecIndentation(int nDec)
	{
		m_nIndentation -= nDec;

		if(m_nIndentation < 0)
		{
			m_nIndentation = 0;
		}
	}


private:


	bool WriteChar(char chCar) const;
	bool WriteIndentation()    const;
	bool Flush()               const;

	EOutputType m_eOutputType;

	FILE* m_pFileOut;
	HWND  m_hWndOut;

	int m_nIndentation;

};


#endif
