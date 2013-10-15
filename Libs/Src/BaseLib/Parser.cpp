// ==[ File ]===================================================================================
//
//  -Name     : Parser.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CParser implementation.
//
// =============================================================================================

/*

	02/10/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool Init(CInput *pInput)
//
//  - Purpose   : Sets parser ready to operate.
//
// -----------------------------------------------------------------------------
bool CParser::Init(CInput *pInput)
{
	assert(pInput);

	m_pInput = pInput;

	return m_pInput ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : Find(const std::string& strString, ESearchMode eSearchMode, bool bMatchCase = false)
//
//  - Purpose   : Tries to find the string using the given search mode with
//                optional case matching .
//                If the string is found it returns the position of the first
//                character and also sets the current reading position at that
//                point. If the string wasn't found -1 is returned and no changes
//                are made.
//
// -----------------------------------------------------------------------------
int CParser::Find(const std::string& strString,	ESearchMode eSearchMode, bool bMatchCase)
{
	if(Ready() == false)
	{
		return -1;
	}

	// Get the current position

	int nCurPos = m_pInput->GetPosition();

	// Set the position depending on the search mode.

	switch(eSearchMode)
	{

	case SEARCH_WHOLEDATA :

		m_pInput->SetPosition(0, CInput::SEEKFROM_START);

		break;

	case SEARCH_FROMCURPOS :

		// Doesn't need position change.
		
		break;

	case SEARCH_FROMCURPOSTOLINEEND :

		// Doesn't need position change.
		
		break;

	case SEARCH_FROMCURLINE :

		SetPosAtCurLineStart();

		break;

	case SEARCH_INCURLINE :

		SetPosAtCurLineStart();

		break;

	default: return -1;

	}

	char chCar;
	int  nStringLength = strString.length();
	int  nLengthFound  = 0;

	// Read characters from input trying to find the string.

	while(m_pInput->ReadChar(&chCar))
	{
		// chCar matches the one we are looking for?

		char chCar2 = strString[nLengthFound];

		if(!bMatchCase)
		{
			if(isupper(chCar))  chCar  = tolower(chCar);
			if(isupper(chCar2)) chCar2 = tolower(chCar2);
		}

		if(chCar == chCar2)
		{
			// Yes, point the counter to the next we have to find.
			nLengthFound++;

			// Found all?
			if(nLengthFound >= nStringLength)
			{
				// Return the position of the first string's character found.

				int nFoundPos = m_pInput->GetPosition() - nStringLength;

				m_pInput->SetPosition(nFoundPos, CInput::SEEKFROM_START);
				//m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);

				return nFoundPos;
			}
		}
		// Not found, set the characters found counter to 0.
		else
		{
			nLengthFound = 0;
		}

		if(chCar == '\n')
		{
			if((eSearchMode == SEARCH_INCURLINE) || (eSearchMode == SEARCH_FROMCURPOSTOLINEEND))
			{
				// Not found in current line.
				break;
			}
		}
	}

	m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);

	return -1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ResetPos()
//
//  - Purpose   : Resets the current read position.
//
// -----------------------------------------------------------------------------
bool CParser::ResetPos()
{
	if(Ready() == false)
	{
		return false;
	}

	return m_pInput->SetPosition(0, CInput::SEEKFROM_START);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool SetPosAtCurLineStart()
//
//  - Purpose   : Sets the read position at the current's line start.
//
// -----------------------------------------------------------------------------
bool CParser::SetPosAtCurLineStart()
{
	if(Ready() == false)
	{
		return false;
	}

	int nCurPos = m_pInput->GetPosition();

	if(nCurPos <= 1)
	{
		m_pInput->SetPosition(0, CInput::SEEKFROM_START);
		return true;
	}

	char chCar;

	// Handle first a special situation.

	m_pInput->ReadChar(&chCar);

	if(chCar == '\n')
	{
		// Now we have to check if the previous one is also a \n, to see if we are positioned in a
		// line wich only a carriage return.

		// I don't know what the fuck happens here, I have to move back 3 positions instead of 2.
		// Probably it has to be something like:

		DecPos(1); // \r
		DecPos(1); // \n
		DecPos(1); // previous char.

		m_pInput->ReadChar(&chCar);

		if(chCar == '\n')
		{
			// It actually is a line with only a carriage return. Let the position be the initial.
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
			return true;
		}

		// It actually is a line whos current position is a carriage return, but has another characters
		// as well. We can move back without fear :)

		DecPos();
	}
	else
	{
		DecPos();
	}

	m_pInput->ReadChar(&chCar);

	while(chCar != '\n')
	{
		if(m_pInput->GetPosition() <= 1)
		{
			m_pInput->SetPosition(0, CInput::SEEKFROM_START);
			return true;
		}

		DecPos(2); // chCar + back move.

		m_pInput->ReadChar(&chCar);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool IncPos(int nNumPositions = 1)
//
//  - Purpose   : Increments the current read position by a given number of
//                positions.
//
// -----------------------------------------------------------------------------
bool CParser::IncPos(int nNumPositions)
{
	if(Ready() == false)
	{
		return false;
	}

	return m_pInput->SetPosition(nNumPositions, CInput::SEEKFROM_CURPOS);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool DecPos(int nNumPositions = 1)
//
//  - Purpose   : Decrements the current read position by a given number of
//                positions.
//
// -----------------------------------------------------------------------------
bool CParser::DecPos(int nNumPositions)
{
	if(Ready() == false)
	{
		return false;
	}

	return m_pInput->SetPosition(-nNumPositions, CInput::SEEKFROM_CURPOS);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : int GetPos()
//
//  - Purpose   : Returns the current read position.
//
// -----------------------------------------------------------------------------
int CParser::GetPos()
{
	if(Ready() == false)
	{
		return 0;
	}

	return m_pInput->GetPosition();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : int JumpSpacing()
//
//  - Purpose   : If the current position is a spacing character, skips all
//                consecutive chars until a first non-spacing character is
//                found. The read position is placed there.
//                Returns the number of line jumps made if any.
//
// -----------------------------------------------------------------------------
int CParser::JumpSpacing()
{
	if(Ready() == false)
	{
		return 0;
	}

	int  nCurPos    = m_pInput->GetPosition();
	int  nLineJumps = 0;
	char chCar;

	while(m_pInput->ReadChar(&chCar))
	{
		if(chCar == '\t' || chCar == '\n' || chCar == ' ' || chCar == '\r')
		{
			if(chCar == '\n')
			{
				nLineJumps++;
			}
		}
		else
		{
			DecPos(); // Position over the visible character.
			return nLineJumps;
		}
	}

	m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_CURPOS);

	return 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool StepIntoNextLine()
//
//  - Purpose   : 
//
// -----------------------------------------------------------------------------
bool CParser::StepIntoNextLine()
{
	if(Ready() == false)
	{
		return false;
	}

	int nCurPos = m_pInput->GetPosition();

	// Set position after a carriage return.

	char chCar;

	while(m_pInput->ReadChar(&chCar))
	{
		if(chCar == '\n')
		{
			return true;
		}
	}

	m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadStringUntilSpacing(std::string *pStrOut)
//
//  - Purpose   : Reads a whole string starting from the current position until
//                a spacing character is found.
//                Leaves the cursor over the first spacing character found if
//                succesfull, otherwise it doesn't change.
//
// -----------------------------------------------------------------------------
bool CParser::ReadStringUntilSpacing(std::string *pStrOut)
{
	assert(pStrOut);

	if(Ready() == false)
	{
		return false;
	}

	int  nCurPos  = m_pInput->GetPosition();
	int  nSizeOut = 0;
	char chCar, szInsertString[2];

	szInsertString[1] = '\0';

	while(m_pInput->ReadChar(&chCar))
	{
		// End?

		if(chCar == '\t' || chCar == '\n' || chCar == ' ' || chCar == '\r' || chCar == '\0')
		{
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);

			pStrOut->erase();

			if(nSizeOut == 0)
			{
				return true;
			}

			for(int nCount = 0; nCount < nSizeOut; nCount++)
			{
				m_pInput->ReadChar(&szInsertString[0]);
				pStrOut->append(szInsertString, 1);
			}

			pStrOut->append("\0");
			return true;
		}

		// No, continue searching.

		nSizeOut++;
	}

	// EOF reached.

	m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
	pStrOut->erase();

	if(nSizeOut == 0)
	{
		return true;
	}

	for(int nCount = 0; nCount < nSizeOut; nCount++)
	{
		m_pInput->ReadChar(&szInsertString[0]);
		pStrOut->append(szInsertString, 1);
	}

	pStrOut->append("\0");
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadStringUntilChar(std::string *pStrOut, char chEndCar)
//
//  - Purpose   : 
//
//  - Purpose   : Reads a whole string starting from the current position until
//                the given character is found.
//                Leaves the cursor over the ending character if succesfull,
//                otherwise it doesn't change.
//
// -----------------------------------------------------------------------------
bool CParser::ReadStringUntilChar(std::string *pStrOut, char chEndCar)
{
	assert(pStrOut);

	if(Ready() == false)
	{
		return false;
	}

	int  nCurPos  = m_pInput->GetPosition();
	int  nSizeOut = 0;
	char chCar, szInsertString[2];

	szInsertString[1] = '\0';

	while(m_pInput->ReadChar(&chCar))
	{
		// End?

		if(chCar == chEndCar)
		{
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);

			if(nSizeOut == 0)
			{
				pStrOut->append("\0");
				return true;
			}

			for(int nCount = 0; nCount < nSizeOut; nCount++)
			{
				m_pInput->ReadChar(&szInsertString[0]);
				pStrOut->append(szInsertString, 1);
			}

			pStrOut->append("\0");
			return true;
		}

		// No, continue searching.

		nSizeOut++;
	}

	// EOF reached.

	m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);

	return false;
}
	
// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadKeyRaw(const std::string& strKeyName, char chSeparator,
//                                std::string *pStrOut, ESearchMode eSearchMode)
//
//  - Purpose   : Reads a key value (text format being key-separator-value)
//                using the given search mode. The value is being stored in raw
//                text format in pStrOut.
//                The cursor is stored over the first spacing character found
//                after the value if succesfull, otherwise it doesn't change.
//
//  - Notes     : Example: ReadKeyRaw("variable", '=', SEEKFROM_START) would
//                read the first variable=something found (spaces can be between
//                the separator and the two parts). 'something' would be stored
//                in text format in pStrOut.
//
// -----------------------------------------------------------------------------
bool CParser::ReadKeyRaw(const std::string& strKeyName, char chSeparator, std::string *pStrOut, ESearchMode eSearchMode)
{
	assert(pStrOut);

	if(Ready() == false)
	{
		return false;
	}

	int nCurPos = m_pInput->GetPosition();
	int nJumpedLines;

	// Search for the key.

	if(Find(strKeyName, eSearchMode) == -1)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	// Step over the key.

	m_pInput->SetPosition(strKeyName.length(), CInput::SEEKFROM_CURPOS);

	// Step over spacing (if any)

	nJumpedLines = JumpSpacing();

	if(nJumpedLines == -1)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	if(nJumpedLines > 0)
	{
		if((eSearchMode == SEARCH_FROMCURPOSTOLINEEND) || (eSearchMode == SEARCH_INCURLINE))
		{
			// Separator in other line.
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
			return false;
		}
	}

	char chCar;

	m_pInput->ReadChar(&chCar);

	// Step over separator, checking first if it is correct.

	if(chCar != chSeparator)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	nJumpedLines = JumpSpacing();

	// Step over spacing again (if any).

	if(nJumpedLines == -1)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	if(nJumpedLines > 0)
	{
		if((eSearchMode == SEARCH_FROMCURPOSTOLINEEND) || (eSearchMode == SEARCH_INCURLINE))
		{
			// Key value in other line.
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
			return false;
		}
	}

	// Read key value.

	std::string tempString;

	if(ReadStringUntilSpacing(&tempString) == false)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	*pStrOut = tempString;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadKeyInt(const std::string& strKeyName, char chSeparator,
//                                int*  pnValueOut, ESearchMode eSearchMode)
//
//  - Purpose   : Like ReadKeyRaw but converting the value to an integer.
//
// -----------------------------------------------------------------------------
bool CParser::ReadKeyInt(const std::string& strKeyName, char chSeparator, int *pnValueOut, ESearchMode eSearchMode)
{
	assert(pnValueOut);

	if(Ready() == false)
	{
		return false;
	}

	std::string tempString;

	if(ReadKeyRaw(strKeyName, chSeparator, &tempString, eSearchMode) == false)
	{
		return false;
	}

	*pnValueOut = atoi(tempString.data());

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : ReadKeyFloat(char*  strKeyName, char chSeparator,
//                             float* pfValueOut, ESearchMode eSearchMode)
//
//  - Purpose   : 
//
// -----------------------------------------------------------------------------
bool CParser::ReadKeyFloat(const std::string& strKeyName, char chSeparator, float *pfValueOut, ESearchMode eSearchMode)
{
	assert(pfValueOut);

	if(Ready() == false)
	{
		return false;
	}

	std::string tempString;

	if(ReadKeyRaw(strKeyName, chSeparator, &tempString, eSearchMode) == false)
	{
		return false;
	}

	*pfValueOut = (float)atof(tempString.data());

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadKeyChar(const std::string& strKeyName, char chSeparator,
//                                 char* pchValueOut, ESearchMode eSearchMode)
//
//  - Purpose   : Like ReadKeyRaw but converting the value to a float.
//
// -----------------------------------------------------------------------------
bool CParser::ReadKeyChar(const std::string& strKeyName, char chSeparator, char *pchValueOut, ESearchMode eSearchMode)
{
	assert(pchValueOut);

	if(Ready() == false)
	{
		return false;
	}

	std::string tempString;

	if(ReadKeyRaw(strKeyName, chSeparator, &tempString, eSearchMode) == false)
	{
		return false;
	}

	*pchValueOut = tempString[0];

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadKeyBool(const std::string& strKeyName, char chSeparator,
//                                 bool* pbValueOut, ESearchMode eSearchMode)
//
//  - Purpose   : Like ReadKeyRaw but converting the value to a boolean.
//                Valid raw text values are True and False (no case matching
//                is done).
//
// -----------------------------------------------------------------------------
bool CParser::ReadKeyBool(const std::string& strKeyName, char chSeparator, bool  *pbValueOut, ESearchMode eSearchMode)
{
	if(Ready() == false)
	{
		return false;
	}

	std::string tempString;

	if(ReadKeyRaw(strKeyName, chSeparator, &tempString, eSearchMode) == false)
	{
		return false;
	}

	if(!stricmp(tempString.data(), "true"))
	{
		*pbValueOut = true;
	}
	else if(!stricmp(tempString.data(), "false"))
	{
		*pbValueOut = false;
	}
	else
	{
		return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadKeyString(const std::string& strKeyName, char chSeparator,
//                                   std::string *pStrOut, ESearchMode eSearchMode)
//
//  - Purpose   : Like ReadKeyRaw but converting the value to a string.
//                The value must be between double quotes, but the stored string
//                contains the value without them
//
// -----------------------------------------------------------------------------
bool CParser::ReadKeyString(const std::string& strKeyName, char chSeparator, std::string *pStrOut, ESearchMode eSearchMode)
{
	assert(pStrOut);

	if(Ready() == false)
	{
		return false;
	}

	int nCurPos = m_pInput->GetPosition();
	int nJumpedLines;

	// Search for the key.

	if(Find(strKeyName, eSearchMode) == -1)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	// Step over the key.

	m_pInput->SetPosition(strKeyName.length(), CInput::SEEKFROM_CURPOS);

	// Step over spacing (if any)

	nJumpedLines = JumpSpacing();

	if(nJumpedLines == -1)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	if(nJumpedLines > 0)
	{
		if((eSearchMode == SEARCH_FROMCURPOSTOLINEEND) || (eSearchMode == SEARCH_INCURLINE))
		{
			// Separator in other line.
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
			return false;
		}
	}

	char chCar;

	m_pInput->ReadChar(&chCar);

	// Step over separator, checking first if it is correct.

	if(chCar != chSeparator)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	nJumpedLines = JumpSpacing();

	// Step over spacing again (if any).

	if(nJumpedLines == -1)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	if(nJumpedLines > 0)
	{
		if((eSearchMode == SEARCH_FROMCURPOSTOLINEEND) || (eSearchMode == SEARCH_INCURLINE))
		{
			// Key value in other line.
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
			return false;
		}
	}

	// Read key value.

	std::string tempString;

	if(!m_pInput->ReadChar(&chCar))
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	if(chCar != '"')
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	if(ReadStringUntilChar(&tempString, '"') == false)
	{
		m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
		return false;
	}

	*pStrOut = tempString;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CParser
//  - Prototype : bool ReadStringBetweenChars(char chStartChar, char chEndChar,
//                                            std::string *pStrOut,
//                                            ESearchMode eSearchMode,
//                                            bool bNested = false)
//
//  - Purpose   : Reads all characters between two chars with optional nesting.
//                If succesfull the string is returned in pStrOut and the cursor
//                placed just after the end character.
//
// -----------------------------------------------------------------------------
bool CParser::ReadStringBetweenChars(char chStartChar, char chEndChar, std::string *pStrOut, ESearchMode eSearchMode, bool bNested)
{
	assert(pStrOut);

	if(m_pInput == NULL)
	{
		return false;
	}

	int nCurPos = m_pInput->GetPosition();

	if(Find(&chStartChar, eSearchMode) == -1)
	{
		return false;
	}

	std::string tempString;

	IncPos();  // start char

	if(bNested == false)
	{
		// No nesting sensitive, read whole text until chEndChar found

		if(ReadStringUntilChar(&tempString, chEndChar) == false) // key
		{
			m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
			return false;
		}

		IncPos(); // end char
		*pStrOut = tempString;
		return true;
	}
	else
	{
		int nTest = 1; // Zero means finished.

		char chCar, szInsertString[2];

		szInsertString[1] = '\0';

		while(m_pInput->ReadChar(&chCar))
		{
			// Handle nesting

			if     (chCar == chStartChar) nTest++;
			else if(chCar == chEndChar)   nTest--;

			if(nTest == 0)
			{
				// Found and finished

				tempString.append("\0");
				*pStrOut = tempString;
				return true;
			}
			else
			{
				szInsertString[0] = chCar;
				tempString.append(szInsertString, 1);
			}
		}
	}

	// End char not found or error.

	m_pInput->SetPosition(nCurPos, CInput::SEEKFROM_START);
	return false;
}
