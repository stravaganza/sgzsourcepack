// ==[ File ]===================================================================================
//
//  -Name     : Logger.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLogger definition.
//
// =============================================================================================

/*

	25/09/2001  -  File created.
	30/04/2003  -  Created CWriterASCII (from old CLogger's structure) and inherited CLogger.


*/


#ifndef __LOGGER_H
#define __LOGGER_H


// ==[ Class definitions ]======================================================================


#define LOG CLogger::Instance()


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CLogger
//
//  - Purpose   : Singleton logger.
//
// ---------------------------------------------------------------------------
class CLogger : public CWriterASCII
{


private:


	CLogger();
	CLogger(const CLogger& logger)                   { }
	CLogger& operator = (const CLogger& logger)      { }

	bool WriteStartHeader();
	bool WriteEndHeader();

	static CLogger* s_pSharedInstance;


public:


	~CLogger();


	// Access to global logger

	static CLogger& Instance();
	static void ShareFrom	(CLogger& logger);
	
	// Utilities

	static bool ErrorWindow (const char *szMessage, ...);
	static bool NotifyWindow(const char *szMessage, ...);

	// Methods

	bool		Start(const std::string& strFile);

};


#endif
