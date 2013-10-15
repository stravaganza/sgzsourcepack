// ==[ File ]===================================================================================
//
//  -Name     : Transform.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CTransformVisitor definition.
//
// =============================================================================================

/*

	19/02/2003  -  File created.


*/


#ifndef __TRANSFORMVISITOR_H
#define __TRANSFORMVISITOR_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CTransformVisitor
//
//  - Purpose   : Transform to a given time the internal state of a
//                node hierarchy:
//
//                   pTransformVisitor->Reset();
//                   pNode = GimmeRootNode();
//                   pTransformVisitor->SetEvalTime(timer.Seconds());
//                   pNode->AcceptVisitor(pTransformVisitor);
//
// ---------------------------------------------------------------------------
class CTransformVisitor : public CNodeVisitor
{


public:


	CTransformVisitor (CNodeVisitor::ETraversalMode eTraversalMode);
	~CTransformVisitor();

	bool Reset			    ();
	bool VisitObject	    (CObject*         pObject);
	bool VisitCamera        (CCamera*         pCamera);
	bool VisitLight		    (CLight*          pLight);
	bool VisitHelper        (CHelper*         pHelper);
	bool VisitLensFlare     (CLensFlare*      pLensFlare);
	bool VisitParticleSystem(CParticleSystem* pParticleSystem);

	void SetEvalTime        (float fTime)           { m_fEvalTime = fTime;  }

	bool DoPendingTransformations();

	// Adds flexibility to do some effects. Default behaviour should be not using them:

	void SetWorldMatrix     (const CMatrix& matrix) { m_accumTM = matrix; m_invWorldTM    = matrix.Inverse(); }
	void SetLocalMatrix     (const CMatrix& matrix) { m_localTM = matrix; m_bPerformLocal = true;             }


private:


	bool    m_bPerformLocal, m_bPerformLocalPrevious;
	float   m_fEvalTime;
	CMatrix m_accumTM, m_localTM, m_invWorldTM;

	std::vector<CObject*> m_vecBonedObjects;
	
};


#endif