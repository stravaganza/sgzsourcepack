// ==[ File ]===================================================================================
//
//  -Name     : DatFile.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Some utilities to load files packed into .dat
//
// =============================================================================================

/*

	22/02/2003  -  File created.
	26/02/2004  -  Added rar support

*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Functions  ]=============================================================================


// --[ Function ]--------------------------------------------------------------
//
//  - Prototype : CInput* LoadFileFromDat(const std::string& strFile,
//                                        bool bTextMode,
//                                        const std::string& strDatFile = "")
//
//  - Purpose   : Loads a file packed into a dat file, returns the CInput
//                pointer if succesfull or NULL if there was an error.
//
//  - Notes     : If no dat file was specified (calling the function with
//                only 2 parameters or passing "" as third), we load strFile
//                as it would normally be. The main purpose of this function
//                is to solve in one call:
//                   if(packed) loadpacked() else loadunpacked()
//                and also leaving the door open if we want to support
//				  new packed formats.
//
//                The bTextMode parameter has only meaning if strDatFile
//                was not specified, since packed files are always opened
//                in binary mode.
//
// ---------------------------------------------------------------------------
CInput* LoadFileFromDat(const std::string& strFile, bool bTextMode, const std::string& strDatFile)
{
	assert(strDatFile.data());

	if(strDatFile == "")
	{
		// No dat, normal file

		CInputFile* pInputFile = new CInputFile;

		if(pInputFile->Open(strFile, bTextMode))
		{
			return pInputFile;
		}

		delete pInputFile;
		return NULL;
	}

	// Read some bytes to know the signature

	char signature[4];

	FILE* fp;
	fp = fopen(strDatFile.data(), "rb");

	if(!fp)
	{
		return NULL;
	}

	if(fread(&signature, 4, 1, fp) != 1)
	{
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	// Check signature

	if(signature[0] == 'P' && signature[1] == 'K')
	{
		CInputZipFile* pInputZipFile = new CInputZipFile;

		// Change '\\' -> '/' for zipped flies

		std::string strAux;
		std::string::const_iterator it;

		for(it = strFile.begin(); it != strFile.end(); ++it)
		{
			if(*it == '\\')
			{
				strAux = strAux + "/";
			}
			else
			{
				strAux = strAux + strFile.substr(it - strFile.begin(), 1);
			}
		}

		if(pInputZipFile->Open(strDatFile, strAux))
		{
			return pInputZipFile;
		}

		delete pInputZipFile;
	}
	else if(signature[0] == 'R' && signature[1] == 'a' &&signature[2] == 'r' && signature[3] == '!')
	{
		CInputRarFile* pInputRarFile = new CInputRarFile;

		if(pInputRarFile->Open(strDatFile, strFile))
		{
			return pInputRarFile;
		}

		delete pInputRarFile;
	}

	return NULL;
}