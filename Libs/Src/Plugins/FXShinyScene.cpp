#include "Plugins.h"
#include "FXShinyScene.h"


const std::string CFXShinyScene::CLASSNAME("Shiny Scene");

REGISTER_PLUGIN(CPluginFactory, CFXShinyScene)


CFXShinyScene::CFXShinyScene() : m_enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS)
{
	DefineVar("Start Time",      CVarFloat::CLASSNAME);
	DefineVar("Speed",           CVarSpeed::CLASSNAME);
	DefineVar("Camera",          CVarString::CLASSNAME);
	DefineVar("Loop Length",     CVarFloat::CLASSNAME);
	DefineVar("Loop Start",      CVarFloat::CLASSNAME);
	DefineVar("Render Mode",     CVarCombo::CLASSNAME);
	DefineVar("Angle Threshold", CVarFloat::CLASSNAME);
	DefineVar("Billboard Size",  CVarFloat::CLASSNAME);

	DefineResourceReference("Scene",         CResourceScene::CLASSNAME);
	DefineResourceReference("Shiny Texture", CResourceTexture2D::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Wire");

	// Set the rest of default values

	((CVarFloat*)GetVar("Start Time" ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time" ))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Speed"      ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Loop Start" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Loop Length"))->SetConstant(-1.0f);

	((CVarFloat*)GetVar("Angle Threshold"))->SetConstant(20.0f);
	((CVarFloat*)GetVar("Billboard Size" ))->SetConstant(10.0f);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_pResScene   = NULL;
	m_pResTexture = NULL;

	m_postRenderData.pLight = NULL;

	m_postRenderData.pBLBPositions = NULL;
	m_postRenderData.pBLBColors    = NULL;
	m_postRenderData.pfBLBSizes    = NULL;

	m_fAnimStart = m_fAnimEnd = 0.0f;
}

CFXShinyScene::~CFXShinyScene()
{
	Free();
}

bool CFXShinyScene::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXShinyScene::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Shiny Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	if(!(m_pResScene = this->FindResource(pResourceList, "Scene", CResourceScene::CLASSNAME)))
	{
		FXLoadError("Can't find scene resource");
		return false;
	}

	m_pResScene->NotifyDependency(this);

	// Get camera name

	CVarString::CValueString valueCamera;

	if(!EvaluateVar("Camera", 0.0f, &valueCamera))
	{
		FXLoadError("Unable to evaluate camera from scene");
		return false;
	}

	CSceneView* pSceneView = const_cast<CSceneView*>(((CResourceScene*)m_pResScene)->GetSceneView());

	if(pSceneView)
	{
		// Take scene to starttime (to optimize particles, mainly)

		CVarFloat::CValueFloat   valueStartTime;
		CVarSpeed::CValueSpeed   valueSpeed;

		EvaluateVar("Start Time",  0.0f,                      &valueStartTime);
		EvaluateVar("Speed",       valueStartTime.GetValue(), &valueSpeed);

		float fTime = valueStartTime.GetValue() + valueSpeed.Integrate(0.0f, valueStartTime.GetValue());

		CNode* pNode = (CNode*)pSceneView->GetRootNode();

		if(pNode)
		{
				CTransformVisitor transformVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
				transformVisitor.Reset();
				transformVisitor.SetEvalTime(fTime);
				pNode->AcceptVisitor(&transformVisitor);
				transformVisitor.DoPendingTransformations();
		}

		// Find the object with largest number of vertices to allocate memory

		m_enumVisitor.Reset();
		pNode->AcceptVisitor(&m_enumVisitor);

		int nMaxVerts = 0;

		for(int i = 0; i < m_enumVisitor.GetNumObjects(); i++)
		{
			if(m_enumVisitor.GetObject(i)->GetNumVertices() > nMaxVerts)
			{
				nMaxVerts = m_enumVisitor.GetObject(i)->GetNumVertices();
			}
		}

		if(nMaxVerts > 0)
		{
			m_postRenderData.pBLBPositions = new CVector3[nMaxVerts];
			m_postRenderData.pBLBColors    = new CVector4[nMaxVerts],
			m_postRenderData.pfBLBSizes    = new float   [nMaxVerts];
		}

		// Try to get the first light of the scene

		if(m_enumVisitor.GetNumLights() > 0)
		{
			m_postRenderData.pLight = m_enumVisitor.GetLight(0);
		}

		m_fAnimStart = pSceneView->GetAnimationStart();
		m_fAnimEnd   = pSceneView->GetAnimationEnd();
	}

	m_strCamera = valueCamera.GetString();

	return true;
}

bool CFXShinyScene::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	SAFE_DELETE_A(m_postRenderData.pBLBPositions);
	SAFE_DELETE_A(m_postRenderData.pBLBColors);
	SAFE_DELETE_A(m_postRenderData.pfBLBSizes);

	m_postRenderData.pLight = NULL;

	m_strCamera.erase();
	return true;
}

bool CFXShinyScene::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	m_postRenderData.pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!m_pResTexture)
	{
		FXRuntimeError("WARNING: Texture not available, effect will draw flat quad");
	}

	if(!m_pResScene)
	{
		FXRuntimeError("Scene resource not available");
		return false;
	}

	CSceneView* pSceneView = const_cast<CSceneView*>(((CResourceScene*)m_pResScene)->GetSceneView());

	if(!pSceneView)
	{
		FXRuntimeError("NULL pSceneView");
		return false;
	}

	int i;

	for(i = 0; i < m_enumVisitor.GetNumObjects(); i++)
	{
		// Set the post render callback
		m_enumVisitor.GetObject(i)->SetPostRenderCallback(&ShinyPostRender, &m_postRenderData);
	}

	CVarFloat::CValueFloat   valueStartTime;
	CVarSpeed::CValueSpeed   valueSpeed;
	CVarCombo::CValueCombo   valueRenderMode;
	CVarString::CValueString valueCamera;
	CVarFloat::CValueFloat   valueAngleThreshold;
	CVarFloat::CValueFloat   valueBillboardSize;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;

	EvaluateVar("Start Time",      fEffectTime, &valueStartTime);
	EvaluateVar("Speed",           fEffectTime, &valueSpeed);
	EvaluateVar("Render Mode",     fEffectTime, &valueRenderMode);
	EvaluateVar("Camera",          fEffectTime, &valueCamera);
	EvaluateVar("Angle Threshold", fEffectTime, &valueAngleThreshold);
	EvaluateVar("Billboard Size",  fEffectTime, &valueBillboardSize);
	EvaluateVar("Loop Length",     fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",      fEffectTime, &valueLoopStart);

	m_postRenderData.fAngleThreshold = valueAngleThreshold.GetValue();
	m_postRenderData.fBillboardSize  = valueBillboardSize.GetValue();

	if(valueCamera.GetString() != m_strCamera)
	{
		// Camera string changed at runtime
		m_strCamera = valueCamera.GetString();
	}

	float fTime = valueStartTime.GetValue() + valueSpeed.Integrate(0.0f, fEffectTime);

	if(IS_ZERO(valueLoopLength.GetValue()))
	{
		ComputeLoop(valueLoopStart.GetValue(), m_fAnimEnd, &fTime);
	}
	else if(valueLoopLength.GetValue() > 0.0f)
	{
		ComputeLoop(valueLoopStart.GetValue(), valueLoopLength.GetValue(), &fTime);
	}

	pSceneView->SetActiveCamera(m_strCamera);

	CRenderVisitor::EMode eRenderMode = CRenderVisitor::MODE_FULLDETAIL;

	     if(valueRenderMode.GetValue() == "Full")  eRenderMode = CRenderVisitor::MODE_FULLDETAIL;
	else if(valueRenderMode.GetValue() == "Solid") eRenderMode = CRenderVisitor::MODE_SOLID;
	else if(valueRenderMode.GetValue() == "Wire")  eRenderMode = CRenderVisitor::MODE_WIREFRAME;

	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	pSceneView->Transform(pDemo, fTime, mtxAddLocal, mtxAddWorld, v3AddCamPos, v3AddCamAngles);

	// The rendervisitor will call for each object its postrender callback

	bool bResult = pSceneView->Render(pDemo, fTime, eRenderMode, false);

	for(i = 0; i < m_enumVisitor.GetNumObjects(); i++)
	{
		// Remove the post render callback to let other effects share this scene without the effect.
		m_enumVisitor.GetObject(i)->SetPostRenderCallback(NULL, NULL);
	}

	return bResult;
}

void CFXShinyScene::ShinyPostRender(CRenderable* pRenderable, void* pData)
{
	assert(pRenderable);

	SPostRenderData* pPostRenderData = (SPostRenderData*)pData;

	if(pPostRenderData->pLight == NULL) return;

	CObject* pObject         = (CObject*)pRenderable;
	CVector3 v3LocalLightPos = pObject->GetAbsoluteTM().Inverse() * pPostRenderData->pLight->GetAbsoluteTM().Position();
	CVector3 v3LightDir;

	int nNumBillboards = 0;

	for(int i = 0; i < pObject->GetNumVertices(); i++)
	{
		v3LightDir.Build(pObject->GetVertices()[i], v3LocalLightPos);
		v3LightDir.Normalize();

		float fT     = -1.0f;

		float fAngle = 180.0f;

		if(pObject->GetVertexNormals()[i] != CVector3::NULLVECTOR)
		{
			fAngle = v3LightDir.Angle(pObject->GetVertexNormals()[i]);
		}

		if(fAngle <= pPostRenderData->fAngleThreshold)
		{
			fT = fAngle / pPostRenderData->fAngleThreshold;

			pPostRenderData->pBLBPositions[nNumBillboards] = pObject->GetVertices()[i];
			pPostRenderData->pfBLBSizes   [nNumBillboards] = pPostRenderData->fBillboardSize * fT;
			pPostRenderData->pBLBColors   [nNumBillboards].Set(1.0f, 1.0f, 1.0f, fT);

			nNumBillboards++;
		}
	}

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_ONE);
	UtilGL::States::Set(UtilGL::States::ALPHATEST, UtilGL::States::ENABLED);
	glAlphaFunc(GL_GREATER, 0.0f);

	UtilGL::Rendering::DrawBillboards(	nNumBillboards,
										pPostRenderData->pTexture,
										pPostRenderData->pBLBPositions,
										pPostRenderData->pBLBColors,
										pPostRenderData->pfBLBSizes);

	UtilGL::States::Set(UtilGL::States::ZBUFFER,      UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,     UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ALPHATEST,    UtilGL::States::DISABLED);
}