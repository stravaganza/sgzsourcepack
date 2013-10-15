// ==[ File ]===================================================================================
//
//  -Name     : SceneView.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSceneView definition.
//
// =============================================================================================

/*

	19/03/2003  -  File created.


*/


#ifndef __SCENEVIEW_H
#define __SCENEVIEW_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSceneView
//
//  - Purpose   : Provide high level functionality to load, setup and render
//                scenes.
//
// ---------------------------------------------------------------------------
class CSceneView
{


public:
	

	CSceneView();
	~CSceneView();


	bool SetNodeData     (CNode* pNode);

	bool LoadNodeData    (const std::string& strFile,
		                  const std::string& strShaderFile,
						  const std::string& strDatfile = "");

	bool LoadResources   (const std::string& strDatfile   = "",
						  const std::string& strTexFolder = "",
						  UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL);

	bool Free            ();

	bool SetActiveCamera (const std::string& strCamera);

	bool Transform		 (	CWindowGL* pWindowGL,
							float fTime,
							const CMatrix&  mtxLocal    = CMatrix::IDENTITY,
							const CMatrix&  mtxWorld    = CMatrix::IDENTITY,
							const CVector3& v3CamPosAdd = CVector3::NULLVECTOR,
							const CVector3& v3CamAngAdd = CVector3::NULLVECTOR);

	bool Render          (	CWindowGL* pWindowGL, float fTime,
							CRenderVisitor::EMode eRenderMode = CRenderVisitor::MODE_FULLDETAIL,
							bool bShowStats = false);

	float GetAnimationStart() const;
	float GetAnimationEnd()   const;

	const CNode* GetRootNode() const { return m_pRootNode; }


private:


	CNode*      m_pRootNode;
	CCamera*    m_pActiveCamera;
	CShaderList m_shaderList;

	CTransformVisitor m_visitorTransform;
	CLightingVisitor  m_visitorLighting;
	CRenderVisitor    m_visitorRender;

	CTimer m_timer;

};


#endif