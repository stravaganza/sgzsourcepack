// ==[ File ]===================================================================================
//
//  -Name     : EnumVisitor.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CEnumVisitor ìmplementation.
//
// =============================================================================================

/*

	19/03/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : CEnumVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
//
//  - Purpose   : CEnumVisitor's constructor. Initializes with the given
//                traversal mode.
//
// -----------------------------------------------------------------------------
CEnumVisitor::CEnumVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
{
	SetTraversalMode(eTraversalMode);
	m_nNumNodes = 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : ~CEnumVisitor()
//
//  - Purpose   : CEnumVisitor's destructor
//
// -----------------------------------------------------------------------------
CEnumVisitor::~CEnumVisitor()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool Reset()
//
//  - Purpose   : Deletes visitor's accumulated data.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::Reset()
{
	m_vecCameras.clear();
	m_vecHelpers.clear();
	m_vecLights.clear();
	m_vecObjects.clear();
	m_vecPSystems.clear();

	m_nNumNodes = 0;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool VisitCamera(CCamera* pCamera)
//
//  - Purpose   : Visits a camera. Stores it in the camera vector.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::VisitCamera(CCamera* pCamera)
{
	assert(pCamera);
	m_vecNodes.push_back  (pCamera);
	m_vecCameras.push_back(pCamera);
	m_nNumNodes++;
	return Traverse(pCamera);

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool VisitHelper(CHelper* pHelper)
//
//  - Purpose   : Visits a helper. Stores it in the helper vector.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::VisitHelper(CHelper* pHelper)
{
	assert(pHelper);
	m_vecNodes.push_back  (pHelper);
	m_vecHelpers.push_back(pHelper);
	m_nNumNodes++;
	return Traverse(pHelper);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool VisitLight(CLight* pLight)
//
//  - Purpose   : Visits a light. Stores it in the light vector.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::VisitLight(CLight* pLight)
{
	assert(pLight);
	m_vecNodes.push_back (pLight);
	m_vecLights.push_back(pLight);
	m_nNumNodes++;
	return Traverse(pLight);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool VisitLensFlare(CLensFlare* pLensFlare)
//
//  - Purpose   : Visits a lensflare. Stores it in the object vector.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::VisitLensFlare(CLensFlare* pLensFlare)
{
	assert(pLensFlare);
	m_vecNodes.push_back     (pLensFlare);
	m_vecLensFlares.push_back(pLensFlare);
	m_nNumNodes++;
	return Traverse(pLensFlare);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool VisitObject(CObject* pObject)
//
//  - Purpose   : Visits an object. Stores it in the object vector.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::VisitObject(CObject* pObject)
{
	assert(pObject);
	m_vecNodes.push_back  (pObject);
	m_vecObjects.push_back(pObject);
	m_nNumNodes++;
	return Traverse(pObject);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CEnumVisitor
//  - prototype : bool VisitParticleSystem(CParticleSystem* pParticleSystem)
//
//  - Purpose   : Visits a particle system node. Stores it in the psystem vector.
//
// -----------------------------------------------------------------------------
bool CEnumVisitor::VisitParticleSystem(CParticleSystem* pParticleSystem)
{
	assert(pParticleSystem);
	m_vecNodes.push_back   (pParticleSystem);
	m_vecPSystems.push_back(pParticleSystem);
	m_nNumNodes++;
	return Traverse(pParticleSystem);
}