// ==[ File ]===================================================================================
//
//  -Name     : Parser.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CParser definition.
//
// =============================================================================================

/*

	28/09/2001  -  File created.


*/


#ifndef __PARSER_H
#define __PARSER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CParser
//
//  - Purpose   : Provides some simple hight level parsing functions.
//
// ---------------------------------------------------------------------------
class CParser
{


public:


	// --[ Enumeration ]----------------------------------------------------------
	//
	//  - Name      : ESearchMode
	//
	//  - Purpose   : Enumerate key search modes.
	//
	// ---------------------------------------------------------------------------
	enum ESearchMode
	{
		SEARCH_WHOLEDATA,
		SEARCH_FROMCURPOS,
		SEARCH_FROMCURPOSTOLINEEND,
		SEARCH_FROMCURLINE,
		SEARCH_INCURLINE
	};

	CParser()  { m_pInput = NULL; }
	~CParser() { Close();         }

	// Init / Close

	bool Init(CInput *pInput);
	bool Close() { m_pInput = NULL; return true; }

	bool Ready() const { return (m_pInput && m_pInput->Ready()); }

	// Access to low level interface:

	CInput* GetInputInterface() { return m_pInput; }

	// Key search:

	int Find(const std::string& strKeyName, ESearchMode eSearchMode, bool bMatchCase = false);

	// Positioning:

	bool ResetPos();
	bool SetPosAtCurLineStart();
	bool IncPos(int nNumPositions = 1);
	bool DecPos(int nNumPositions = 1);
	int  GetPos();

	int  JumpSpacing();
	bool StepIntoNextLine();

	// Key reads:

	bool ReadStringUntilSpacing(std::string *pstrOut);
	bool ReadStringUntilChar   (std::string *pstrOut, char chEndCar);
	
	bool ReadKeyRaw   (const std::string& strKeyName, char chSeparator, std::string *pstrOut, ESearchMode eSearchMode);
	bool ReadKeyInt   (const std::string& strKeyName, char chSeparator, int   *pnValueOut,    ESearchMode eSearchMode);
	bool ReadKeyFloat (const std::string& strKeyName, char chSeparator, float *pfValueOut,    ESearchMode eSearchMode);
	bool ReadKeyChar  (const std::string& strKeyName, char chSeparator, char  *pchValueOut,   ESearchMode eSearchMode);
	bool ReadKeyBool  (const std::string& strKeyName, char chSeparator, bool  *pbValueOut,    ESearchMode eSearchMode);
	bool ReadKeyString(const std::string& strKeyName, char chSeparator, std::string *pstrOut, ESearchMode eSearchMode);

	bool ReadStringBetweenChars(char chStartChar, char chEndChar, std::string *pstrOut, ESearchMode eSearchMode, bool bNested);


protected:


	CInput* m_pInput;

};


#endif
