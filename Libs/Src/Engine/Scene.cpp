// ==[ File ]===================================================================================
//
//  -Name     : Scene.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CScene ìmplementation.
//
// =============================================================================================

/*

	01/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CScene
//  - prototype : CScene()
//
//  - Purpose   : CScene's default constructor.
//
// -----------------------------------------------------------------------------
CScene::CScene() : CNode()
{
	m_v4Ambient.Set (0.2f, 0.2f, 0.2f, 1.0f);
	m_v4FogColor.Set(0.0f, 0.0f, 0.0f, 1.0f);

	m_nFogType    = ID_FOG_NONE;
	m_fFogNear    = 0.0f; // 0.0 means camera.near
	m_fFogFar     = 1.0f; // 1.0 means camera.far
	m_fFogDensity = 1.0f;

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CScene
//  - prototype : ~CScene()
//
//  - Purpose   : CScene's destructor
//
// -----------------------------------------------------------------------------
CScene::~CScene()
{
	// Call CNode::DeleteNode(true);

	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CScene
//  - prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the scene data.
//
// -----------------------------------------------------------------------------
bool CScene::FreeNodeData()
{
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CScene
//  - prototype : bool ReadNodeData(CInput* pInput)
//
//  - Purpose   : Reads the scene properties from a data source.
//
// -----------------------------------------------------------------------------
bool CScene::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;

	if(!m_v4Ambient.Read (pInput)) uError++;
	if(!m_v4FogColor.Read(pInput)) uError++;

	if(!pInput->ReadInt  (&m_nFogType))    uError++;
	if(!pInput->ReadFloat(&m_fFogNear))    uError++;
	if(!pInput->ReadFloat(&m_fFogFar))     uError++;
	if(!pInput->ReadFloat(&m_fFogDensity)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CScene
//  - prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Writes the scene properties to a data target.
//
// -----------------------------------------------------------------------------
bool CScene::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	m_v4Ambient.Write (pOutput);
	m_v4FogColor.Write(pOutput);

	if(!pOutput->WriteInt  (m_nFogType))    uError++;
	if(!pOutput->WriteFloat(m_fFogNear))    uError++;
	if(!pOutput->WriteFloat(m_fFogFar))     uError++;
	if(!pOutput->WriteFloat(m_fFogDensity)) uError++;

	return true;
}
