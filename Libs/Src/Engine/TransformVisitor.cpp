// ==[ File ]===================================================================================
//
//  -Name     : TransformVisitor.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CTransformVisitor ìmplementation.
//
// =============================================================================================

/*

	19/02/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : CTransformVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
//
//  - Purpose   : CTransformVisitor's constructor. Initializes with the given
//                traversal mode.
//
// -----------------------------------------------------------------------------
CTransformVisitor::CTransformVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
{
	SetTraversalMode(eTraversalMode);
	m_fEvalTime     = 0.0f;
	m_bPerformLocal = false;
	m_bPerformLocalPrevious = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : ~CTransformVisitor()
//
//  - Purpose   : CTransformVisitor's destructor
//
// -----------------------------------------------------------------------------
CTransformVisitor::~CTransformVisitor()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool Reset()
//
//  - Purpose   : Deletes visitor's accumulated data.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::Reset()
{
	m_accumTM.SetIdentity();
	m_localTM.SetIdentity();
	m_invWorldTM.SetIdentity();

	m_fEvalTime             = 0.0f;
	m_bPerformLocalPrevious = m_bPerformLocal;
	m_bPerformLocal         = false;

	m_vecBonedObjects.clear();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool VisitObject(CObject* pObject)
//
//  - Purpose   : Visits an object. Compute its relative/absolute transf matrix.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::VisitObject(CObject* pObject)
{
	assert(pObject);

	// Compute relative TM:

	if(pObject->IsAnimated() || m_bPerformLocal || m_bPerformLocalPrevious)
	{
		pObject->SetAnimEvalTime(m_fEvalTime, false);
	}

	CMatrix matrix = m_accumTM; // push
	m_accumTM = m_accumTM * pObject->GetRelativeTM();

	// Additional local transformation?

	if(m_bPerformLocal)
	{
		// Add relative transformation and compute absolute matrix

		pObject->SetRelativeTM(pObject->GetRelativeTM() * m_localTM);
		pObject->SetAbsoluteTM(matrix * pObject->GetRelativeTM());
	}
	else
	{
		// Establish absolute matrix
		pObject->SetAbsoluteTM(m_accumTM);
	}

	// Transform object's bounding volume

	if(pObject->GetBoundingVolume())
	{
		if(pObject->HasStaticMesh() == false)
		{
			pObject->GetBoundingVolume()->Build(pObject);
		}

		pObject->GetBoundingVolume()->Transform(pObject->GetAbsoluteTM());
	}

	pObject->ComputeMassCenter(true);

	// Dynamic objects

	if(pObject->HasStaticMesh() == false)
	{
		if(pObject->HasBones())
		{
			m_vecBonedObjects.push_back(pObject);
		}
		else
		{
			pObject->ComputeNormals(true, true);
		}
	}

	// Continue traversal:

	bool bResult = Traverse(pObject);

	m_accumTM = matrix; // pop
	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool VisitCamera(CCamera* pCamera)
//
//  - Purpose   : Visits a camera. Compute its relative/absolute transf matrix.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::VisitCamera(CCamera* pCamera)
{
	assert(pCamera);

	// Compute relative TM and current animated camera settings:

	if(pCamera->IsAnimated())
	{
		pCamera->SetAnimEvalTime(m_fEvalTime, false);
	}

	if(pCamera->HasAnimCamSettingsData())
	{
		pCamera->SetAnimCamSettingsEvalTime(m_fEvalTime);
	}

	// Compute absolute TM:

	CMatrix matrix = m_accumTM; // push

	m_accumTM = m_accumTM * pCamera->GetRelativeTM();
	pCamera->SetAbsoluteTM(m_invWorldTM * m_accumTM);

	// Compute clipping planes:

	if(CWindowGL::GetActiveWindow())
	{
		pCamera->ComputeClippingPlanes(CWindowGL::GetActiveWindow()->GetAspect());
	}
	else
	{
		pCamera->ComputeClippingPlanes(1.33f);
	}

	// Continue traversal

	bool bResult = Traverse(pCamera);

	m_accumTM = matrix; // pop
	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool VisitLight(CLight* pLight)
//
//  - Purpose   : Visits a light. Compute its relative/absolute transf matrix.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::VisitLight(CLight* pLight)
{
	assert(pLight);

	// Compute relative TM and current animated light settings:

	if(pLight->IsAnimated())
	{
		pLight->SetAnimEvalTime(m_fEvalTime, false);
	}

	if(pLight->HasAnimLightSettingsData())
	{
		pLight->SetAnimLightSettingsEvalTime(m_fEvalTime);
	}

	// Compute absolute TM:

	CMatrix matrix = m_accumTM; // push

	m_accumTM = m_accumTM * pLight->GetRelativeTM();
	pLight->SetAbsoluteTM(m_accumTM);

	// Continue traversal:

	bool bResult = Traverse(pLight);

	m_accumTM = matrix; // pop
	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool VisitHelper(CHelper* pHelper)
//
//  - Purpose   : Visits a helper. Compute its relative/absolute transf matrix.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::VisitHelper(CHelper* pHelper)
{
	assert(pHelper);

	// Compute relative TM

	if(pHelper->IsAnimated())
	{
		pHelper->SetAnimEvalTime(m_fEvalTime, false);
	}

	// Compute absolute TM:

	CMatrix matrix = m_accumTM; // push

	m_accumTM = m_accumTM * pHelper->GetRelativeTM();
	pHelper->SetAbsoluteTM(m_accumTM);

	// Continue traversal:

	bool bResult = Traverse(pHelper);

	m_accumTM = matrix; // pop
	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool VisitLensFlare(CLensFlare* pLensFlare)
//
//  - Purpose   : Visits a lensflare. Compute its relative/absolute
//                transformation matrix and evaluate the lensflare properties.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::VisitLensFlare(CLensFlare* pLensFlare)
{
	assert(pLensFlare);

	// Compute relative TM

	if(pLensFlare->IsAnimated())
	{
		pLensFlare->SetAnimEvalTime(m_fEvalTime, false);
	}

	// Compute absolute TM:

	CMatrix matrix = m_accumTM; // push

	m_accumTM = m_accumTM * pLensFlare->GetRelativeTM();
	pLensFlare->SetAbsoluteTM(m_accumTM);

	// Evaluate the lensflare

	bool bOK = pLensFlare->SetEvalTime(m_fEvalTime);

	// Continue traversal:

	bool bResult = Traverse(pLensFlare);

	m_accumTM = matrix; // pop
	return bResult && bOK;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool VisitParticleSystem(CParticleSystem* pParticleSystem)
//
//  - Purpose   : Visits a particle system. Compute its relative/absolute
//                transformation matrix and evaluate the system itself.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::VisitParticleSystem(CParticleSystem* pParticleSystem)
{
	assert(pParticleSystem);

	// Compute relative TM

	if(pParticleSystem->IsAnimated())
	{
		pParticleSystem->SetAnimEvalTime(m_fEvalTime, false);
	}

	// Compute absolute TM:

	CMatrix matrix = m_accumTM; // push

	m_accumTM = m_accumTM * pParticleSystem->GetRelativeTM();
	pParticleSystem->SetAbsoluteTM(m_accumTM);

	// Evaluate the particle system

	bool bOK = pParticleSystem->SetEvalTime(m_fEvalTime);

	// Continue traversal:

	bool bResult = Traverse(pParticleSystem);

	m_accumTM = matrix; // pop
	return bResult && bOK;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTransformVisitor
//  - prototype : bool DoPendingTransformations()
//
//  - Purpose   : Computes transformations that need to be done after
//                all hierarchy processing.
//
// -----------------------------------------------------------------------------
bool CTransformVisitor::DoPendingTransformations()
{
	std::vector<CObject*>::iterator it;

	for(it = m_vecBonedObjects.begin(); it != m_vecBonedObjects.end(); ++it)
	{
		CObject* pObject = *it;

		pObject->ComputeBlendedVertices();
		pObject->ComputeNormals(true, true);

		// Computed vertices are already in world space

		pObject->SetAbsoluteTM(CMatrix::IDENTITY);
		pObject->SetRelativeTM(CMatrix::IDENTITY);
	}

	return true;
}