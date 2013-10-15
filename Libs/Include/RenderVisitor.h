// ==[ File ]===================================================================================
//
//  -Name     : RenderVisitor.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CRenderVisitor definition.
//
// =============================================================================================

/*

	17/02/2003  -  File created.


*/


#ifndef __RENDERVISITOR_H
#define __RENDERVISITOR_H


// ==[ Class definitions ]======================================================================

class CRenderable;

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CRenderVisitor
//
//  - Purpose   : Give support to render nodes in a hierarchy.
//                This should be the correct way to render nodes using
//                the engine:
//
//                   pRenderVisitor->Reset();
//                   pNode = GimmeRootNode();
//                   pNode->AcceptVisitor(pRenderVisitor);
//                   pRenderVisitor->Render();
//
// ---------------------------------------------------------------------------
class CRenderVisitor : public CNodeVisitor
{


public:


	enum EMode
	{
		MODE_POINTS,
		MODE_WIREFRAME,
		MODE_SOLID,
		MODE_FULLDETAIL
	};

	CRenderVisitor (CNodeVisitor::ETraversalMode eTraversalMode);
	~CRenderVisitor();

	bool Reset			    ();
	bool VisitScene		    (CScene*          pScene);
	bool VisitObject	    (CObject*         pObject);
	bool VisitLensFlare     (CLensFlare*      pLensFlare);
	bool VisitParticleSystem(CParticleSystem* pParticleSystem);
	void SetRenderMode	    (EMode            eMode)	{ m_eRenderMode = eMode; }
	bool Render				(CCamera*         pCamera);

	int  GetDrawnObjects    () const { return m_nDrawnObjs;  }
	int  GetTotalObjects    () const { return m_nTotalObjs;  }
	int  GetDrawnPolys      () const { return m_nDrawnPolys; }
	int  GetTotalPolys      () const { return m_nTotalPolys; }
	int  GetDrawnVerts      () const { return m_nDrawnVerts; }
	int  GetTotalVerts      () const { return m_nTotalVerts; }
	int  GetDrawnParticles  () const { return m_nDrawnParts; }
	int  GetTotalParticles  () const { return m_nTotalParts; }


private:


	bool WireRender    (CObject* pObject, bool bLightingEnabled);
	bool DefaultRender (CObject* pObject, bool bLightingEnabled);

	std::vector<CRenderable*> m_vecRenderables;

	CScene* m_pScene; // for fog

	EMode m_eRenderMode;

	int m_nDrawnObjs,  m_nTotalObjs;
	int m_nDrawnPolys, m_nTotalPolys;
	int m_nDrawnVerts, m_nTotalVerts;
	int m_nDrawnParts, m_nTotalParts;
	
};


#endif