// ==[ File ]===================================================================================
//
//  -Name     : LightingVisitor.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLightingVisitor definition.
//
// =============================================================================================

/*

	13/12/2002  -  File created.


*/


#ifndef __LIGHTINGVISITOR_H
#define __LIGHTINGVISITOR_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CLightingVisitor
//
//  - Purpose   : Traverse a node hierarchy accumulating lighting data.
//                Establish OpenGL lighting with it.
//
// ---------------------------------------------------------------------------
class CLightingVisitor : public CNodeVisitor
{


public:


	CLightingVisitor (CNodeVisitor::ETraversalMode eTraversalMode);
	~CLightingVisitor();

	bool Reset();
	bool VisitLight(CLight* pLight);
	bool VisitScene(CScene* pScene);
	bool ApplyLighting();


private:


	CVector4             m_v4GlobalAmbient;
	std::vector<CLight*> m_vecLights;
	
};


#endif