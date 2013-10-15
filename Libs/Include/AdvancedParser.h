// ==[ File ]===================================================================================
//
//  -Name     : AdvancedParser.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CAdvancedParser definition.
//
// =============================================================================================

/*

	16/01/2002  -  File created.
	28/04/2003  -  Changed from CShaderParser to CAdvancedParser.


*/


#ifndef __ADVANCEDPARSER_H
#define __ADVANCEDPARSER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CAdvancedParser
//
//  - Purpose   : Inherited from CParser, provides extra parsing methods.
//
// ---------------------------------------------------------------------------
class CAdvancedParser : public CParser
{


public:


	CAdvancedParser()  { }
	~CAdvancedParser() { }

	// Key reads:

	bool ReadKeyVector2(const std::string& strKeyName, char chSeparator, CVector2* pv2Out, ESearchMode eSearchMode);
	bool ReadKeyVector3(const std::string& strKeyName, char chSeparator, CVector3* pv3Out, ESearchMode eSearchMode);
	bool ReadKeyVector4(const std::string& strKeyName, char chSeparator, CVector4* pv4Out, ESearchMode eSearchMode);

	// For CShaderStandard

	bool ReadTextureInfo(const std::string& strKeyName, CShaderStandard::STextureInfo* pTextureInfo, ESearchMode eSearchMode);

};


#endif
