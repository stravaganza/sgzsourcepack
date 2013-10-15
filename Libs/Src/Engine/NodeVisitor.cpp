// ==[ File ]===================================================================================
//
//  -Name     : NodeVisitor.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CNodeVisitor ìmplementation.
//
// =============================================================================================

/*

	17/02/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNodeVisitor
//  - prototype : CNodeVisitor()
//
//  - Purpose   : CNodeVisitor's default constructor. Set default traversal
//                mode to downwards..
//
// -----------------------------------------------------------------------------
CNodeVisitor::CNodeVisitor()
{
	m_eTraversalMode = TRAVERSAL_DOWNWARDS;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNodeVisitor
//  - prototype : bool Traverse(CNode* pCurVisit)
//
//  - Purpose   : Continues traversing the hierarchy tree from pCurVisit
//                depending on our active traversal mode.
//
// -----------------------------------------------------------------------------
bool CNodeVisitor::Traverse(CNode* pCurVisit)
{
	assert(pCurVisit);

	unsigned uError = 0, uCount = 0;

	switch(m_eTraversalMode)
	{

	case TRAVERSAL_DOWNWARDS:

		// From node to all childs

		for(uCount = 0; uCount < pCurVisit->GetNumChilds(); uCount++)
		{
			if(!pCurVisit->GetChild(uCount)->AcceptVisitor(this))
			{
				uError++;
			}
		}

		break;

	case TRAVERSAL_UPWARDS:

		// From node to parent

		if(pCurVisit->GetParent())
		{
			if(!pCurVisit->GetParent()->AcceptVisitor(this))
			{
				uError++;
			}
		}
	}

	return uError == 0;
}

// Default visits:

bool CNodeVisitor::VisitCamera        (CCamera*         pCamera)         { return Traverse(pCamera);         }
bool CNodeVisitor::VisitHelper        (CHelper*         pHelper)         { return Traverse(pHelper);         }
bool CNodeVisitor::VisitLight         (CLight*          pLight )         { return Traverse(pLight);          }
bool CNodeVisitor::VisitObject        (CObject*         pObject)         { return Traverse(pObject);         }
bool CNodeVisitor::VisitScene         (CScene*          pScene )         { return Traverse(pScene);          }
bool CNodeVisitor::VisitLensFlare     (CLensFlare*      pLensFlare)      { return Traverse(pLensFlare);      }
bool CNodeVisitor::VisitParticleSystem(CParticleSystem* pParticleSystem) { return Traverse(pParticleSystem); }