// ==[ File ]===================================================================================
//
//  -Name     : SceneView.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSceneView ìmplementation.
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
//  - Class     : CSceneView
//  - prototype : CSceneView()
//
//  - Purpose   : CSceneView's default constructor.
//
// -----------------------------------------------------------------------------
CSceneView::CSceneView() :	m_visitorTransform(CNodeVisitor::TRAVERSAL_DOWNWARDS),
							m_visitorLighting (CNodeVisitor::TRAVERSAL_DOWNWARDS),
							m_visitorRender   (CNodeVisitor::TRAVERSAL_DOWNWARDS)
{
	m_pRootNode     = NULL;
	m_pActiveCamera = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : ~CSceneView()
//
//  - Purpose   : CSceneView's destructor.
//
// -----------------------------------------------------------------------------
CSceneView::~CSceneView()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool SetNodeData(CNode* pNode)
//
//  - Purpose   : Sets the pointer to the node data.
//
// -----------------------------------------------------------------------------
bool CSceneView::SetNodeData(CNode* pNode)
{

#ifdef _DEBUG

	if(m_pRootNode)
	{
		LOG.Write("\nWARNING - CSceneView::SetNodeData(): Sceneview has already node data:");
		LOG.Write("\n          Old node %s new node %s", m_pRootNode->GetNodeName().data(), pNode->GetNodeName().data());
	}

#endif

	m_pRootNode = pNode;
	return m_shaderList.BuildListFromNodeTree(pNode);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool LoadNodeData(const std::string& strFile,
//                                  const std::string& strShaderFile,
//                                  const std::string& strDatfile = "")
//
//  - Purpose   : Loads the node data from disk, strFile is the scene file,
//                strShaderFile the shader file, and strDatfile an optional
//                data file (zip, rar...)
//
// -----------------------------------------------------------------------------
bool CSceneView::LoadNodeData(const std::string& strFile, const std::string& strShaderFile, const std::string& strDatfile)
{
	assert(strFile.data());
	assert(strShaderFile.data());
	assert(strDatfile.data());

	// Open scene file

	CInput* pInputScene = LoadFileFromDat(strFile, false, strDatfile);

	if(!pInputScene)
	{
		LOG.Write("\nERROR - CSceneView::LoadNodeData(): Unable to open file %s", strFile.data());
		return false;
	}

	// Open shader file

	CInput* pInputShaders = LoadFileFromDat(strShaderFile, true, strDatfile);

	if(!pInputShaders)
	{
		LOG.Write("\nERROR - CSceneView::LoadNodeData(): Unable to open file %s", strShaderFile.data());
		delete pInputScene;
		return false;
	}

	// Read scene

	LOG.Write("\n\nLoading scene %s:", strFile.data());
	LOG.IncIndentation(2);

	m_pRootNode = CNode::ReadNode(pInputScene, NULL);

	LOG.DecIndentation(2);

	if(m_pRootNode == NULL)
	{
		LOG.Write("\nERROR - CSceneView::LoadNodeData(): Unable to open file %s", strShaderFile.data());
		delete pInputScene;
		delete pInputShaders;
		return false;
	}

	// Read shaders

	LOG.Write("\n\nLoading shaders from %s:", strShaderFile.data());
	LOG.IncIndentation(2);

	m_shaderList.Free(true); // just in case

	if(!m_shaderList.ParseAndAssignShaders(m_pRootNode, pInputShaders))
	{
		LOG.DecIndentation(2);
		LOG.Write("\nERROR - CSceneView::LoadNodeData(): Error loading and assigning shaders. (Scene %s)", strFile.data());
		m_shaderList.Free(true);
		delete m_pRootNode;
		delete pInputScene;
		delete pInputShaders;
		return false;
	}

	LOG.DecIndentation(2);

	// Close files

	delete pInputScene;
	delete pInputShaders;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool LoadResources(const std::string& strDatfile   = "",
//									 const std::string& strTexFolder = "",
//									 UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL)
//
//  - Purpose   : Loads all resources needed to render the scene, mainly
//                building display lists for the objects and loading textures.
//
// -----------------------------------------------------------------------------
bool CSceneView::LoadResources(	const std::string& strDatfile, const std::string& strTexFolder,
								UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	assert(strDatfile.data());
	assert(strTexFolder.data());

	if(!m_pRootNode)
	{
		return false;
	}

	// Load shader resources

	LOG.Write("\n\nLoading shaders resources:");
	LOG.IncIndentation(2);

	if(!m_shaderList.LoadShaderResources(strDatfile, strTexFolder, pTexOptions))
	{
		LOG.DecIndentation(2);
		LOG.Write("\nERROR - CSceneView::LoadResources(): Error loading shader resources.");
		return false;
	}

	LOG.DecIndentation(2);

	// Compile geomobjects

	LOG.Write("\n\nCompiling objects:");
	LOG.IncIndentation(2);

	unsigned uError = 0;
	int i;

	CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
	enumVisitor.Reset();
	m_pRootNode->AcceptVisitor(&enumVisitor);

	for(i = 0; i < enumVisitor.GetNumObjects(); i++)
	{
		CObject* pObject = enumVisitor.GetObject(i);
		assert(pObject);

		pObject->ComputeExpVertices();

		if(pObject->NeedsBoneResolve())
		{
			pObject->ResolveBones();
		}

		if(pObject->GetShader())
		{
			if(!pObject->GetShader()->CompileObject(pObject))
			{
				LOG.Write("\nERROR - CSceneView::LoadResources(): Error compiling object %s", pObject->GetNodeName().data());
				uError++;
			}
		}
		else
		{
			LOG.Write("\n%s (no shader)", pObject->GetNodeName().data());
		}
	}

	// Init lensflares

	for(i = 0; i < enumVisitor.GetNumLensFlares(); i++)
	{
		CLensFlare* pLensFlare = enumVisitor.GetLensFlare(i);
		assert(pLensFlare);

		if(!pLensFlare->LoadResources(strDatfile, strTexFolder, pTexOptions))
		{
			uError++;
			LOG.Write("\nERROR - CSceneView::LoadResources(): Error loading lensflare %s's resources", pLensFlare->GetNodeName().data());
		}
	}

	// Init particle systems

	for(i = 0; i < enumVisitor.GetNumParticleSystems(); i++)
	{
		CParticleSystem* pSystem = enumVisitor.GetParticleSystem(i);

		if(!pSystem->InitSystem())
		{
			LOG.Write("\nERROR - CSceneView::LoadResources(): Error initializing particle system %s", pSystem->GetNodeName().data());
			uError++;
		}

		if(!pSystem->LoadResources(strDatfile, strTexFolder, pTexOptions))
		{
			uError++;
			LOG.Write("\nERROR - CSceneView::LoadResources(): Error loading particle system %s's resources", pSystem->GetNodeName().data());
		}
	}

	LOG.DecIndentation(2);

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool Free()
//
//  - Purpose   : Frees all data (node hierarchy, shaders and resources).
//
// -----------------------------------------------------------------------------
bool CSceneView::Free()
{
	if(m_pRootNode)
	{
		LOG.Write("\n\nDeleting scene:");
		LOG.IncIndentation(2);
		SAFE_DELETE(m_pRootNode);
		LOG.DecIndentation(2);
	}

	if(m_shaderList.GetNumShaders() > 0)
	{
		m_shaderList.Free(true);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool SetActiveCamera(const std::string& strCamera)
//
//  - Purpose   : Tells which camera will be the one from which will be rendered
//                the scene.
//
// -----------------------------------------------------------------------------
bool CSceneView::SetActiveCamera(const std::string& strCamera)
{
	if(!m_pRootNode)
	{
		return false;
	}

	m_pActiveCamera = NULL;
	CNode* pNode    = m_pRootNode->FindNode(strCamera);

	if(pNode)
	{
		if(pNode->GetNodeType() == ID_NODE_CAMERA)
		{
			m_pActiveCamera = (CCamera *)pNode;
		}
	}

	return m_pActiveCamera != NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool Transform(	CWindowGL* pWindowGL,
//									float fTime,
//									const CMatrix&  mtxLocal    = CMatrix::IDENTITY,
//									const CMatrix&  mtxWorld    = CMatrix::IDENTITY,
//									const CVector3& v3CamPosAdd = CVector3::NULLVECTOR,
//									const CVector3& v3CamAngAdd = CVector3::NULLVECTOR)
//
//  - Purpose   : Transforms the scene and sets up the view/projection matrices.
//                Parameters:
//					-Window that will be used to render to.
//                  -Time
//                  -Optional local matrix added initially to each object node.
//                  -Optional world matrix applied (posttransform) to the whole scene.
//                  -Optional camera position added (post transform but local axes) to the camera
//                  -Optional camera angles added (pre transform) to the camera.
//
// -----------------------------------------------------------------------------
bool CSceneView::Transform(	CWindowGL* pWindowGL,
							float fTime,
							const CMatrix&  mtxLocal,
							const CMatrix&  mtxWorld,
							const CVector3& v3CamPosAdd,
							const CVector3& v3CamAngAdd)
{
	unsigned uError = 0;

	// Reset visitor

	if(!m_visitorTransform.Reset()) uError++;

	// Transform

	m_visitorTransform.SetEvalTime   (fTime);
	m_visitorTransform.SetWorldMatrix(mtxWorld);

	// The additional local computation increases cpu usage so check if it's not the identity

	if(!mtxLocal.IsIdentity())
	{
		m_visitorTransform.SetLocalMatrix(mtxLocal);
	}

	if(!m_pRootNode->AcceptVisitor(&m_visitorTransform)) uError++;
	if(!m_visitorTransform.DoPendingTransformations()) uError++;

	// Set projection matrix
	
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	glMatrixMode(GL_PROJECTION);

	if(m_pActiveCamera)
	{
		float fVerticalFOV = CCamera::GetVerticalFOV(m_pActiveCamera->FOV(), pWindowGL->GetAspect());

		gluPerspective(	fVerticalFOV, pWindowGL->GetAspect(),
						m_pActiveCamera->Near(), m_pActiveCamera->Far());
	}
	else
	{
		gluPerspective(60.0f, pWindowGL->GetAspect(), 1.0f, 3000.0f);
	}

	CMatrix viewMatrix;

	if(m_pActiveCamera)
	{
		viewMatrix = m_pActiveCamera->GetAbsoluteTM();
	}

	// Additional camera transformations

	if(!IS_ZERO(v3CamAngAdd.X())) viewMatrix.PreRotateX(v3CamAngAdd.X());
	if(!IS_ZERO(v3CamAngAdd.Y())) viewMatrix.PreRotateY(v3CamAngAdd.Y());
	if(!IS_ZERO(v3CamAngAdd.Z())) viewMatrix.PreRotateZ(v3CamAngAdd.Z());

	if(v3CamPosAdd != CVector3::NULLVECTOR)
	{
		CMatrix invview = viewMatrix.Inverse();
		CVector3 v3Aux = invview.XAxis() * v3CamPosAdd.X();
		v3Aux = v3Aux + (invview.YAxis() * v3CamPosAdd.Y());
		v3Aux = v3Aux + (invview.ZAxis() * v3CamPosAdd.Z());

		viewMatrix(0, 3) += v3Aux.X();
		viewMatrix(1, 3) += v3Aux.Y();
		viewMatrix(2, 3) += v3Aux.Z();
	}

	if(m_pActiveCamera)
	{
		// It would be better not to do this, but it's needed in order to compute
		// frustum culling properly (see CRenderVisitor::Render()).
		m_pActiveCamera->SetAbsoluteTM(viewMatrix);
		m_pActiveCamera->ComputeClippingPlanes(pWindowGL->GetAspect());
	}

	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW, viewMatrix.Inverse());

	if(!m_shaderList.SetEvalTime(fTime)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : bool Render(	CWindowGL* pWindowGL, float fTime,
//								CRenderVisitor::EMode eRenderMode,
//								bool bShowStats)
//
//  - Purpose   : Renders the scene.
//                Parameters:
//                  -Window to render to
//                  -Time to evaluate the scene
//                  -Render mode (see CRenderVisitor)
//                  -boolean value telling wheter to output debug stats or not.
//
// -----------------------------------------------------------------------------
bool CSceneView::Render(CWindowGL* pWindowGL, float fTime,
						CRenderVisitor::EMode eRenderMode, bool bShowStats)
{
	assert(pWindowGL);

	// FPS computing works only if the sceneview is not shared within a frame

	float fFrameLength = m_timer.Seconds();

	m_timer.Stop();
	m_timer.Start();

	unsigned uError = 0;

	pWindowGL->SetAsActiveWindow();

	// Reset visitors

	if(!m_visitorLighting.Reset())  uError++;
	if(!m_visitorRender.Reset())    uError++;

	m_visitorRender.SetRenderMode(eRenderMode);

	// Do lighting

	if(!m_pRootNode->AcceptVisitor(&m_visitorLighting))  uError++;
	if(!m_visitorLighting.ApplyLighting())               uError++;

	// Render

	if(!m_pRootNode->AcceptVisitor(&m_visitorRender))    uError++;
	if(!m_visitorRender.Render(m_pActiveCamera)) uError++;

	// Show stats

	if(bShowStats)
	{
		CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
		enumVisitor.Reset();

		m_pRootNode->AcceptVisitor(&enumVisitor);

		UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);
		UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float fYOffset = 0.08f;
		float fYJump   = 0.022f * (600.0f / viewport[3]);

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 0, "Time %.2f",                fTime)) uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 1, "%.2f FPS / %u PPS",        1.0f / fFrameLength, (unsigned)((1.0f / fFrameLength) * m_visitorRender.GetDrawnPolys()))) uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 2, "%u/%u Objects",            m_visitorRender.GetDrawnObjects(),   m_visitorRender.GetTotalObjects()))     uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 3, "%u/%u Polys",              m_visitorRender.GetDrawnPolys(),     m_visitorRender.GetTotalPolys()))       uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 4, "%u/%u Verts",              m_visitorRender.GetDrawnVerts(),     m_visitorRender.GetTotalVerts()))       uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 5, "%u/%u Particles",          m_visitorRender.GetDrawnParticles(), m_visitorRender.GetTotalParticles()))   uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 6, "%u Lights",                enumVisitor.GetNumLights()))  uError++;
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 7, "%u Helpers",               enumVisitor.GetNumHelpers())) uError++;

		char  szCamera[256];
		float fVerticalFOV;

		if(m_pActiveCamera)
		{
			fVerticalFOV = CCamera::GetVerticalFOV(m_pActiveCamera->FOV(), pWindowGL->GetAspect());

			sprintf(szCamera, "%s (%.2f hFOV, %.2f vFOV)",	m_pActiveCamera->GetNodeName().data(),
															m_pActiveCamera->FOV(),
															fVerticalFOV);
		}
		else
		{
			strcpy(szCamera, "NONE");
		}
		
		if(!pWindowGL->TextOut(0.02f, fYOffset + fYJump * 8, "%u Cameras (Active : %s)", enumVisitor.GetNumCameras(), szCamera)) uError++;
	}

	return uError == 0;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : float GetAnimationStart() const
//
//  - Purpose   : Returns the start time of the animation.
//
// -----------------------------------------------------------------------------
float CSceneView::GetAnimationStart() const
{
	if(m_pRootNode == NULL)
	{
		return 0.0f;
	}

	CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
	enumVisitor.Reset();

	float fAnimStart = 0.0f;
	m_pRootNode->AcceptVisitor(&enumVisitor);

	for(int i = 0; i < enumVisitor.GetNumNodes(); i++)
	{
		float fTime;
		CNode* pNode = enumVisitor.GetNode(i);

		if(pNode->GetPosAnimation())
		{
			if(pNode->GetPosAnimation()->GetStartTime(&fTime)) if(fTime < fAnimStart) fAnimStart = fTime;
		}
		if(pNode->GetSclAnimation())
		{
			if(pNode->GetSclAnimation()->GetStartTime(&fTime)) if(fTime < fAnimStart) fAnimStart = fTime;
		}
		if(pNode->GetRotAnimation())
		{
			if(pNode->GetRotAnimation()->GetStartTime(&fTime)) if(fTime < fAnimStart) fAnimStart = fTime;
		}
	}

	return fAnimStart;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSceneView
//  - prototype : float GetAnimationEnd() const
//
//  - Purpose   : Returns the end time of the animation.
//
// -----------------------------------------------------------------------------
float CSceneView::GetAnimationEnd() const
{
	if(m_pRootNode == NULL)
	{
		return 0.0f;
	}

	CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
	enumVisitor.Reset();

	float fAnimEnd = 0.0f;
	m_pRootNode->AcceptVisitor(&enumVisitor);

	for(int i = 0; i < enumVisitor.GetNumNodes(); i++)
	{
		float fTime;
		CNode* pNode = enumVisitor.GetNode(i);

		if(pNode->GetPosAnimation())
		{
			if(pNode->GetPosAnimation()->GetEndTime(&fTime)) if(fTime < fAnimEnd) fAnimEnd = fTime;
		}
		if(pNode->GetSclAnimation())
		{
			if(pNode->GetSclAnimation()->GetEndTime(&fTime)) if(fTime < fAnimEnd) fAnimEnd = fTime;
		}
		if(pNode->GetRotAnimation())
		{
			if(pNode->GetRotAnimation()->GetEndTime(&fTime)) if(fTime < fAnimEnd) fAnimEnd = fTime;
		}
	}

	return fAnimEnd;
}