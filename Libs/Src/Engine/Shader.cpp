// ==[ File ]===================================================================================
//
//  -Name     : Shader.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CShader implementation.
//
// =============================================================================================

/*

	07/01/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShader
//  - prototype : CShader()
//
//  - Purpose   : CShader's default constructor
//
// -----------------------------------------------------------------------------
CShader::CShader()
{
	m_strName = "<no shader>";
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShader
//  - prototype : static CShader* Parse(CInput* pInput, const std::string& strShaderName);
//
//  - Purpose   : Instances and parses a shader (name based) from a data source.
//                Returns a pointer to the shader (NULL means error).
//
// -----------------------------------------------------------------------------
CShader* CShader::Parse(CInput* pInput, const std::string& strShaderName)
{
	assert(pInput);
	assert(pInput->Ready());

	CParser parser;

	LOG.Write("\nReading shader %s", strShaderName.data());
	LOG.Write("\n  Initializing parser.");

	if(!parser.Init(pInput))
	{
		LOG.Write("\nCShader::Parse(): Error initializing parser for shader %s.", strShaderName.data());
		return NULL;
	}

	parser.ResetPos();

	bool bFound = false;
	std::string strName, strType, strShaderText;

	// Search for the requested shader.

	LOG.Write("\n  Searching for shader.");

	while(!bFound)
	{
		// Find a shader

		if(parser.Find("#Shader", CParser::SEARCH_FROMCURPOS) == -1)
		{
			LOG.Write("\nCShader::Parse(): Shader %s not found.", strShaderName.data());
			return NULL;
		}

		// Read its name

		if(!parser.ReadKeyString("#Name", '=', &strName, CParser::SEARCH_INCURLINE))
		{
			LOG.Write("\nWARNING - CShader::Parse(): Name field not found parsing a shader. Name must be in the same line.");
			continue;
		}

		if(strName == strShaderName)
		{
			if(!parser.ReadKeyRaw("#Type", '=', &strType, CParser::SEARCH_INCURLINE))
			{
				LOG.Write("\nCShader::Parse(): Error reading shader %s'type (was searching for %s). Type before name?", strName.data(), strShaderName.data());
				return NULL;
			}

			// Read shader content

			if(!parser.ReadStringBetweenChars('{', '}', &strShaderText, CParser::SEARCH_FROMCURPOS, true))
			{
				LOG.Write("\nCShader::Parse(): Error reading shader %s's content (was searching for %s).", strName.data(), strShaderName.data());
				return NULL;
			}

			bFound = true;
		}
	}

	if(bFound)
	{
		LOG.Write("\n  Found, parsing.");

		// Instance correct shader class based on strType.

		CInputMemory inputMemory;

		if(!inputMemory.Open((void *)strShaderText.data(), strShaderText.length()))
		{
			LOG.Write("\nCShader::Parse(): Error on CInputMemory::Open() to parse shader text.");
			return NULL;
		}

		// Switch-case like:

		CShader* pShader = NULL;

		if(!stricmp(strType.data(), "Standard"))
		{
			pShader = new CShaderStandard;
		}
		else
		{
			LOG.Write("\nCShader::Parse(): Unknown shader type (%s) reading shader %s", strType.data(), strShaderName.data());
			return NULL;
		}

		assert(pShader);

		pShader->SetName(strName);

		if(!pShader->ParseShader(&inputMemory))
		{
			LOG.Write("  Error!");
			delete pShader;
			return NULL;
		}

		LOG.Write("\nShader loaded.");

		return pShader;
	}

	LOG.Write("\nShader not found!");

	return NULL;
}
