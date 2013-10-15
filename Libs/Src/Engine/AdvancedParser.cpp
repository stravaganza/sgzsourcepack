// ==[ File ]===================================================================================
//
//  -Name     : AdvancedParser.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAdvancedParser implementation.
//
// =============================================================================================

/*

	28/01/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAdvancedParser
//  - prototype : bool ReadKeyVector2(const std::string& strKeyName,
//                                    char chSeparator,
//                                    CVector2* pv2Out,
//                                    ESearchMode eSearchMode);
//
//  - Purpose   : Searches for a key <strKeyName> using search mode <eSearchMode>
//                and reads the vector2 in format (val1, val2) after the
//                separator character <chSeparator>.
//                For example:
//
//                  Code:
//
//                      CVector2 val;
//                      parser.ReadKeyVector2("value", '=', &val)
//
//                  Text to be parsed:
//
//                      value = (1.0, 3)
//
// -----------------------------------------------------------------------------
bool CAdvancedParser::ReadKeyVector2(const std::string& strKeyName, char chSeparator, CVector2* pv2Out, ESearchMode eSearchMode)
{
	assert(pv2Out);
	assert(m_pInput);
	assert(m_pInput->Ready());

	int nStartPos = m_pInput->GetPosition(); // push

	if(Find(strKeyName, eSearchMode, false) == -1)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	// Jump over key name and read separator, checking if found.

	char chSep;

	m_pInput->SetPosition(strKeyName.size(), CInput::SEEKFROM_CURPOS);
	JumpSpacing();
	m_pInput->ReadChar(&chSep);

	if(chSep != chSeparator)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	// Read the two values

	std::string strFirst, strSecond;

	unsigned uError = 0;

	if(!ReadStringBetweenChars('(', ',', &strFirst, CParser::SEARCH_FROMCURPOSTOLINEEND, false)) uError++; IncPos();
	if(!ReadStringUntilChar(&strSecond, ')')) uError++;

	if(uError)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	pv2Out->Set(atof(strFirst.data()), atof(strSecond.data()));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAdvancedParser
//  - prototype : bool ReadKeyVector3(const std::string& strKeyName,
//                                    char chSeparator,
//                                    CVector3* pv3Out,
//                                    ESearchMode eSearchMode);
//
//  - Purpose   : Same as ReadKeyVector2, but with CVector3 format.
//
// -----------------------------------------------------------------------------
bool CAdvancedParser::ReadKeyVector3(const std::string& strKeyName, char chSeparator, CVector3* pv3Out, ESearchMode eSearchMode)
{
	assert(pv3Out);
	assert(m_pInput);
	assert(m_pInput->Ready());

	int nStartPos = m_pInput->GetPosition(); // push

	if(Find(strKeyName, eSearchMode, false) == -1)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	// Jump over key name and read separator, checking if found.

	char chSep;

	m_pInput->SetPosition(strKeyName.size(), CInput::SEEKFROM_CURPOS);
	JumpSpacing();
	m_pInput->ReadChar(&chSep);

	if(chSep != chSeparator)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	// Read the three values

	std::string strFirst, strSecond, strThird;

	unsigned uError = 0;

	if(!ReadStringBetweenChars('(', ',', &strFirst, CParser::SEARCH_FROMCURPOSTOLINEEND, false)) uError++; IncPos();
	if(!ReadStringUntilChar(&strSecond, ',')) uError++; IncPos();
	if(!ReadStringUntilChar(&strThird,  ')')) uError++;

	if(uError)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	pv3Out->Set(atof(strFirst.data()), atof(strSecond.data()), atof(strThird.data()));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAdvancedParser
//  - prototype : bool ReadKeyVector3(const std::string& strKeyName,
//                                    char chSeparator,
//                                    CVector4* pv3Out,
//                                    ESearchMode eSearchMode);
//
//  - Purpose   : Same as ReadKeyVector2, but with CVector4 format.
//
// -----------------------------------------------------------------------------
bool CAdvancedParser::ReadKeyVector4(const std::string& strKeyName, char chSeparator, CVector4* pv4Out, ESearchMode eSearchMode)
{
	assert(pv4Out);
	assert(m_pInput);
	assert(m_pInput->Ready());

	int nStartPos = m_pInput->GetPosition(); // push

	if(Find(strKeyName, eSearchMode, false) == -1)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	// Jump over key name and read separator, checking if found.

	char chSep;

	m_pInput->SetPosition(strKeyName.size(), CInput::SEEKFROM_CURPOS);
	JumpSpacing();
	m_pInput->ReadChar(&chSep);

	if(chSep != chSeparator)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	// Read the four values

	std::string strFirst, strSecond, strThird, strFourth;

	unsigned uError = 0;

	if(!ReadStringBetweenChars('(', ',', &strFirst, CParser::SEARCH_FROMCURPOSTOLINEEND, false)) uError++; IncPos();
	if(!ReadStringUntilChar(&strSecond, ',')) uError++; IncPos();
	if(!ReadStringUntilChar(&strThird,  ',')) uError++; IncPos();
	if(!ReadStringUntilChar(&strFourth, ')')) uError++;

	if(uError)
	{
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	pv4Out->Set(atof(strFirst.data()), atof(strSecond.data()), atof(strThird.data()), atof(strFourth.data()));
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CAdvancedParser
//  - prototype : bool ReadTextureInfo(const std::string& strKeyName,
//                                     CShaderStandard::STextureInfo* pTextureInfo,
//                                     ESearchMode eSearchMode)
//
//  - Purpose   : Reads a texture field.
//
// -----------------------------------------------------------------------------
bool CAdvancedParser::ReadTextureInfo(const std::string& strKeyName, CShaderStandard::STextureInfo* pTextureInfo, ESearchMode eSearchMode)
{
	assert(pTextureInfo);
	assert(m_pInput);
	assert(m_pInput->Ready());

	int nStartPos = m_pInput->GetPosition(); // push

	// No texture?

	if(Find(strKeyName, eSearchMode, false) == -1)
	{
		if(!pTextureInfo->m_vecBitmaps.empty())
		{
			pTextureInfo->m_vecBitmaps.clear();
		}

		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return true;
	}

	// Read texture type

	std::string strTextureType;

	if(!ReadStringBetweenChars('(', ')', &strTextureType, CParser::SEARCH_FROMCURPOSTOLINEEND, false))
	{
		LOG.Write("\nERROR - CAdvancedParser::ReadTextureInfo(): No texture type found in %s field", strKeyName.data());
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	if(strTextureType == "2D")      pTextureInfo->eTextureType = UtilGL::Texturing::CTexture::TEXTURE2D;     else
	if(strTextureType == "2DANIM")  pTextureInfo->eTextureType = UtilGL::Texturing::CTexture::TEXTURE2DANIM; else
	if(strTextureType == "CUBEMAP") pTextureInfo->eTextureType = UtilGL::Texturing::CTexture::TEXTURECUBEMAP;

	// Read texture info

	CParser      auxParser;
	CInputMemory inputMem;
	std::string  strTextureInfo;
	
	if(!ReadStringBetweenChars('{', '}', &strTextureInfo, CParser::SEARCH_FROMCURPOS, true))
	{
		LOG.Write("\nERROR - CAdvancedParser::ReadTextureInfo(): No { } found for %s field", strKeyName.data());
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	inputMem.Open((void *)strTextureInfo.data(), strTextureInfo.length());
	auxParser.Init(&inputMem);

	std::string strFilter;

	if(strTextureType == "2D")
	{
		// Single bitmap

		CShaderStandard::SBitmapInfo bitmap;

		bitmap.bTile           = true;
		bitmap.eFilter         = UtilGL::Texturing::FILTER_TRILINEAR;
		bitmap.bForceFiltering = false;

		// File, tiling and filtering

		if(!auxParser.ReadKeyString("#file", '=', &bitmap.strFile, CParser::SEARCH_WHOLEDATA))
		{
			LOG.Write("\nERROR - CAdvancedParser::ReadTextureInfo(): No #file key found in %s field", strKeyName.data());
			m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
			return false;
		}

		auxParser.ReadKeyBool("#tile", '=', &bitmap.bTile, CParser::SEARCH_WHOLEDATA);

		if(auxParser.ReadKeyRaw("#filter", '=', &strFilter, CParser::SEARCH_WHOLEDATA))
		{
			bitmap.bForceFiltering = true;
			if(strFilter == "POINT")     bitmap.eFilter = UtilGL::Texturing::FILTER_POINT;    else
			if(strFilter == "LINEAR")    bitmap.eFilter = UtilGL::Texturing::FILTER_LINEAR;   else
			if(strFilter == "BILINEAR")  bitmap.eFilter = UtilGL::Texturing::FILTER_BILINEAR; else
			if(strFilter == "TRILINEAR") bitmap.eFilter = UtilGL::Texturing::FILTER_TRILINEAR;
			LOG.Write("\nWARNING - CAdvancedParser::ReadTextureInfo(): (%s) invalid filter mode (%s)", strKeyName.data(), strFilter.data());
		}

		pTextureInfo->bLoop = false;
		pTextureInfo->m_vecBitmaps.push_back(bitmap);		
	}
	else if(strTextureType == "2DANIM" || strTextureType == "CUBEMAP")
	{
		// Multiple bitmaps

		// 2DANIM only:
		pTextureInfo->bLoop = false;
		auxParser.ReadKeyBool("#loop", '=', &pTextureInfo->bLoop, CParser::SEARCH_WHOLEDATA);

		std::string strBitmap;

		auxParser.ResetPos();
		int nBitmapCount = 1;

		// Bitmap read loop

		while(auxParser.ReadStringBetweenChars('{', '}', &strBitmap, CParser::SEARCH_FROMCURPOS, true))
		{
			CParser      bitmapParser;
			CInputMemory inputBitmap;

			// Create a parser for bitmap subfield.

			inputBitmap.Open((void *)strBitmap.data(), strBitmap.length());
			bitmapParser.Init(&inputBitmap);
			
			CShaderStandard::SBitmapInfo bitmap;

			bitmap.bTile    = true;
			bitmap.eFilter  = UtilGL::Texturing::FILTER_TRILINEAR;
			bitmap.fSeconds = 0.0f;

			// File, tiling, filtering and duration

			if(!bitmapParser.ReadKeyString("#file", '=', &bitmap.strFile,  CParser::SEARCH_WHOLEDATA))
			{
				LOG.Write("\nERROR - CAdvancedParser::ReadTextureInfo(): (%s, bitmap %u) no #file found.",strKeyName.data(), nBitmapCount);
				m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
				return false;
			}

			bitmapParser.ReadKeyBool  ("#tile",     '=', &bitmap.bTile,    CParser::SEARCH_WHOLEDATA);
			bitmapParser.ReadKeyFloat ("#duration", '=', &bitmap.fSeconds, CParser::SEARCH_WHOLEDATA);

			if(bitmapParser.ReadKeyRaw("#filter", '=', &strFilter, CParser::SEARCH_WHOLEDATA))
			{
				bitmap.bForceFiltering = true;
				if(strFilter == "POINT")     bitmap.eFilter = UtilGL::Texturing::FILTER_POINT;    else
				if(strFilter == "LINEAR")    bitmap.eFilter = UtilGL::Texturing::FILTER_LINEAR;   else
				if(strFilter == "BILINEAR")  bitmap.eFilter = UtilGL::Texturing::FILTER_BILINEAR; else
				if(strFilter == "TRILINEAR") bitmap.eFilter = UtilGL::Texturing::FILTER_TRILINEAR;
				LOG.Write("\nWARNING - CAdvancedParser::ReadTextureInfo(): (%s) invalid filter mode (%s)", strKeyName.data(), strFilter.data());
			}

			pTextureInfo->m_vecBitmaps.push_back(bitmap);

			bitmapParser.Close();
			inputBitmap.Close();
			nBitmapCount++;
		}
	}
	else
	{
		LOG.Write("\nERROR - Invalid texture type found in %s field (%s)", strKeyName.data(), strTextureType.data());
		m_pInput->SetPosition(nStartPos, CInput::SEEKFROM_START);
		return false;
	}

	auxParser.Close();
	inputMem.Close();

	return true;
}