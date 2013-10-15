// ==[ File ]===================================================================================
//
//  -Name     : Logger.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLogger implementation (see Logger.h).
//
// =============================================================================================

/*

	21/09/2001  -  File created.
	30/04/2003  -  Transferred almost all functionality to CWriterASCII. Created singleton.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Class implementations ]==================================================================


CLogger* CLogger::s_pSharedInstance = NULL;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : static CLogger& Instance()
//
//  - Purpose   : Access to the global logger.
//
// -----------------------------------------------------------------------------
CLogger& CLogger::Instance()
{
	static CLogger s_instance;

	if(s_pSharedInstance)
	{
		return *s_pSharedInstance;
	}

	return s_instance;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : void ShareFrom(CLogger& memTracer)
//
//  - Purpose   : Allows the global instance to be shared between other modules,
//				  such as a dll.
//
// -----------------------------------------------------------------------------
void CLogger::ShareFrom(CLogger& logger)
{
	s_pSharedInstance = &logger;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : CLogger::CLogger()
//
//  - Purpose   : CLogger constructor.
//
// -----------------------------------------------------------------------------
CLogger::CLogger()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : CLogger::~CLogger()
//
//  - Purpose   : CLogger destructor. Closes the log file.
//
// -----------------------------------------------------------------------------
CLogger::~CLogger()
{
	if(!s_pSharedInstance)
	{
		this->WriteEndHeader();
		this->Close();
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : static bool ErrorWindow(const char *szMessage, ...)
//
//  - Purpose   : Shows an error window with the given message.
//
//  - Note      : static method.
//
// -----------------------------------------------------------------------------
bool CLogger::ErrorWindow(const char *szMessage, ...)
{
	char szFinalMessage[1000];
	memset(szFinalMessage, 0, 100);

	va_list arguments;

	va_start(arguments, szMessage);
	vsprintf(szFinalMessage, szMessage, arguments);
	va_end(arguments);

	MessageBox(NULL, szFinalMessage, "Error!", MB_TASKMODAL | MB_TOPMOST | MB_ICONERROR);

	LOG.Write("\nERROR - ");
	LOG.Write(szFinalMessage);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : bool NotifyWindow(const char *szMessage, ...)
//
//  - Purpose   : Shows a notify window with the given message.
//
//  - Note      : static method.
//
// -----------------------------------------------------------------------------
bool CLogger::NotifyWindow(const char *szMessage, ...)
{
	static char szFinalMessage[1000];

	va_list arguments;

	va_start(arguments, szMessage);
	vsprintf(szFinalMessage, szMessage, arguments);
	va_end(arguments);

	MessageBox(NULL, szFinalMessage, "Notification", MB_TASKMODAL | MB_TOPMOST | MB_ICONINFORMATION);

	LOG.Write("\nNOTIFICATION - ");
	LOG.Write(szFinalMessage);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : bool WriteStartHeader()
//
//  - Purpose   : Outputs a fancy start header.
//
// -----------------------------------------------------------------------------
bool CLogger::WriteStartHeader()
{
	SYSTEMTIME sysTime;

	GetLocalTime(&sysTime);

	Write(" ============================================================================ \n");
	Write("   Ithaqua^Stravaganza log.\n");
	Write("   Created on %u-%u-%u, at %u:%u:%u\n", sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	Write(" ============================================================================ ");
	Write("\n\n");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLogger
//  - Prototype : bool Flush()
//
//  - Purpose   : Outputs a fancy end header.
//
// -----------------------------------------------------------------------------
bool CLogger::WriteEndHeader()
{
	SYSTEMTIME sysTime;

	GetLocalTime(&sysTime);

	Write("\n\n");
	Write(" ============================================================================ \n");
	Write("   Log closed on %u-%u-%u, at %u:%u:%u\n", sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	Write(" ============================================================================ ");
	Write("\n\n");

	return true;
}

bool CLogger::Start(const std::string& strFile)
{
	this->InitFile(strFile);
	this->WriteStartHeader();

	return true;
}