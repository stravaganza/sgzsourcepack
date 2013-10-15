// ==[ File ]===================================================================================
//
//  -Name     : WriterASCII.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CWriterASCII implementation (see WriterASCII.h).
//
// =============================================================================================

/*

	30/04/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Class implementations ]==================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : CWriterASCII()
//
//  - Purpose   : CWriterASCII's default constructor
//
//  - Notes     : Starts with no possibility to write data. Could've started
//                with stdout though.
//
// -----------------------------------------------------------------------------
CWriterASCII::CWriterASCII()
{
	m_eOutputType  = OUTPUT_NONE;

	m_pFileOut     = NULL;
	m_hWndOut      = NULL;

	m_nIndentation = 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : CWriterASCII(EOutputType eType, const std::string& strName)
//
//  - Purpose   : CWriterASCII constructor. Can create every way of output.
//                strName is only used if either OUTPUT_FILE (file name) or
//                OUTPUT_WINDOW (window caption) are specified.
//
// -----------------------------------------------------------------------------
CWriterASCII::CWriterASCII(EOutputType eType, const std::string& strName)
{
	m_pFileOut     = NULL;
	m_hWndOut      = NULL;
	m_nIndentation = 0;

	switch(eType)
	{

	case OUTPUT_NONE:

		break;

	case OUTPUT_STDOUT:

		InitStdout();
		break;

	case OUTPUT_FILE:

		InitFile(strName);
		break;

	case OUTPUT_WINDOW:

		InitWindow(strName);
		break;

	default:

		break;
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : ~CWriterASCII()
//
//  - Purpose   : CWriterASCII's destructor.
//
//  - Notes     : See Close()
//
// -----------------------------------------------------------------------------
CWriterASCII::~CWriterASCII()
{
	Close();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool InitStdout()
//
//  - Purpose   : Starts writing to stdout.
//
// -----------------------------------------------------------------------------
bool CWriterASCII::InitStdout()
{
	if(Ready())
	{
		Close();
	}

	m_pFileOut = stdout;

	if(m_pFileOut == NULL)
	{
		return false;
	}

	m_eOutputType = OUTPUT_STDOUT;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool InitFile(const std::string& strFile)
//
//  - Purpose   : Starts writing to a file.
//
// -----------------------------------------------------------------------------
bool CWriterASCII::InitFile(const std::string& strFile)
{
	if(Ready())
	{
		Close();
	}

	m_pFileOut = fopen(strFile.data(), "wt");

	if(m_pFileOut == NULL)
	{
		CLogger::ErrorWindow("Couldn't open %s for output logging", strFile.data());
		return false;
	}

	m_eOutputType = OUTPUT_FILE;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool InitWindow(const std::string& strTitle)
//
//  - Purpose   : Starts writing to a text window.
//
// -----------------------------------------------------------------------------
bool CWriterASCII::InitWindow(const std::string& strTitle)
{
	if(Ready())
	{
		Close();
	}

	m_hWndOut = CreateWindow(	"EDIT", strTitle.data(),
								WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_WANTRETURN | ES_READONLY,
								20, 20, 640, 480,
								NULL, NULL, NULL, NULL);

	if(m_hWndOut == NULL)
	{
		CLogger::ErrorWindow("Error creating a log window");
		return false;
	}

	UpdateWindow(m_hWndOut);
	m_eOutputType = OUTPUT_WINDOW;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool Close()
//
//  - Purpose   : Closes output.
//
// -----------------------------------------------------------------------------
bool CWriterASCII::Close()
{
	switch(m_eOutputType)
	{

	case OUTPUT_NONE:

		break;

	case OUTPUT_STDOUT:

		break;

	case OUTPUT_FILE:

		if(m_pFileOut)
		{
			fclose(m_pFileOut);
			m_pFileOut = NULL;
		}

		break;

	case OUTPUT_WINDOW:

		if(m_hWndOut)
		{
			DestroyWindow(m_hWndOut);
			m_hWndOut = NULL;
		}

		break;

	default:

		break;
	}

	m_eOutputType  = OUTPUT_NONE;
	m_nIndentation = 0;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool Write(const char *szMessage, ...)
//
//  - Purpose   : Writes a string the same way as printf().
//
// -----------------------------------------------------------------------------
bool CWriterASCII::Write(const char *szMessage, ...) const
{
	if(m_eOutputType == OUTPUT_NONE)
	{
		return true;
	}

	if(Ready() == false)
	{
		return false;
	}

	// Create the formatted string

	static char szFormattedMessage[1000];

	va_list arguments;

	va_start(arguments, szMessage);
	vsprintf(szFormattedMessage, szMessage, arguments);
	va_end(arguments);

#ifdef _DEBUG
	OutputDebugString(szFormattedMessage);
#endif

	// Get it out taking care of the line jumps

	int nCount, nLength = strlen(szFormattedMessage);

	for(nCount = 0; nCount < nLength; nCount++)
	{
		if(szFormattedMessage[nCount] == '\n')
		{
			WriteChar('\n');

			if(m_eOutputType == OUTPUT_WINDOW)
			{
				WriteChar('\r');
			}

			WriteIndentation();
		}
		else
		{
			WriteChar(szFormattedMessage[nCount]);
		}
	}

	Flush();
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : WriteChar(char chCar)
//
//  - Purpose   : Writes a char.
//
//  - Notes     : Internal method called by Output()
//
// -----------------------------------------------------------------------------
bool CWriterASCII::WriteChar(char chCar) const
{
	int nWindowTextLength;

	switch(m_eOutputType)
	{

	case OUTPUT_NONE:

		return true;
		
	case OUTPUT_STDOUT:
	case OUTPUT_FILE:

		fputc(chCar, m_pFileOut);

		break;

	case OUTPUT_WINDOW:

		nWindowTextLength = GetWindowTextLength(m_hWndOut) + 1;

		std::string newText;

		newText.reserve(nWindowTextLength + 1);
		GetWindowText(m_hWndOut, (char *)newText.data(), nWindowTextLength);

		newText[nWindowTextLength] = chCar;

		SetWindowText(m_hWndOut, newText.data());

		break;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool WriteIndentation()
//
//  - Purpose   : Writes a number of blanks equal to m_nIndentation.
//
// -----------------------------------------------------------------------------
bool CWriterASCII::WriteIndentation() const
{
	if(m_eOutputType == OUTPUT_NONE)
	{
		return true;
	}

	for(int nCount = 0; nCount < m_nIndentation; nCount++)
	{
		WriteChar(' ');
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWriterASCII
//  - Prototype : bool Flush()
//
//  - Purpose   : Flushes all pendent output.
//
// -----------------------------------------------------------------------------
bool CWriterASCII::Flush() const
{
	if(Ready() == false)
	{
		return false;
	}

	switch(m_eOutputType)
	{

	case OUTPUT_NONE:

		return true;

	case OUTPUT_STDOUT:
	case OUTPUT_FILE:

		if(m_pFileOut)
		{
			fflush(m_pFileOut);
		}

		break;

	case OUTPUT_WINDOW:

		//UpdateWindow(m_hWndOut);

		break;
	}

	return true;
}