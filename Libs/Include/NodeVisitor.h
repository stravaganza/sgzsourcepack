// ==[ File ]===================================================================================
//
//  -Name     : NodeVisitor.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CNodeVisitor definition.
//
// =============================================================================================

/*

	13/12/2002  -  File created.


*/


#ifndef __NODEVISITOR_H
#define __NODEVISITOR_H


// ==[ Class definitions ]======================================================================


class CNode;
class CCamera;
class CHelper;
class CLight;
class CObject;
class CScene;
class CLensFlare;
class CParticleSystem;


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CNodeVisitor
//
//  - Purpose   : Node visitors base class.
//
// ---------------------------------------------------------------------------
class CNodeVisitor
{


public:


	enum ETraversalMode
	{
		TRAVERSAL_UPWARDS,
		TRAVERSAL_DOWNWARDS
	};

	CNodeVisitor();
	virtual ~CNodeVisitor() { }

	void         SetTraversalMode   (ETraversalMode eMode) { m_eTraversalMode = eMode; }
	virtual bool Reset              () = 0;

	virtual bool VisitCamera        (CCamera*         pCamera);
	virtual bool VisitHelper        (CHelper*         pHelper);
	virtual bool VisitLight         (CLight*          pLight );
	virtual bool VisitObject        (CObject*         pObject);
	virtual bool VisitScene         (CScene*          pScene );
	virtual bool VisitLensFlare     (CLensFlare*      pLensFlare);
	virtual bool VisitParticleSystem(CParticleSystem* pParticleSystem);


protected:


	bool         Traverse           (CNode*   pCurVisit);


private:


	ETraversalMode m_eTraversalMode;
	
};


#endif