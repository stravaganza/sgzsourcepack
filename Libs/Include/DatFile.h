// ==[ File ]===================================================================================
//
//  -Name     : DatFile.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Dat files utility prototypes.
//
// =============================================================================================

/*

	22/02/2003  -  File created.


*/


#ifndef __DATFILE_H
#define __DATFILE_H


// ==[ Functions  ]=============================================================================


CInput* LoadFileFromDat(const std::string& strFile, bool bTextMode, const std::string& strDatFile = "");


#endif

