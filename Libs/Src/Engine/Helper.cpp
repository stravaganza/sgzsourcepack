// ==[ File ]===================================================================================
//
//  -Name     : Helper.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CHelper ìmplementation.
//
// =============================================================================================

/*

	03/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CHelper
//  - prototype : CHelper()
//
//  - Purpose   : CHelper's default constructor.
//
// -----------------------------------------------------------------------------
CHelper::CHelper()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CHelper
//  - prototype : ~CHelper()
//
//  - Purpose   : CHelper's destructor.
//
// -----------------------------------------------------------------------------
CHelper::~CHelper()
{
	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CHelper
//  - prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the helper data.
//
// -----------------------------------------------------------------------------
bool CHelper::FreeNodeData()
{
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CHelper
//  - prototype : bool ReadNodeData(CInput* pInput)
//
//  - Purpose   : Reads the helper data from a source.
//
// -----------------------------------------------------------------------------
bool CHelper::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;

	if(!ReadAnimation(pInput)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CHelper
//  - prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Writes the helper data in a given destiny.
//
// -----------------------------------------------------------------------------
bool CHelper::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(!WriteAnimation(pOutput)) uError++;

	return uError == 0;
}

