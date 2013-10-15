#include "Plugins.h"
#include "FXSceneGlow.h"


const std::string CFXSceneGlow::CLASSNAME("Scene With Glow");

REGISTER_PLUGIN(CPluginFactory, CFXSceneGlow)


CFXSceneGlow::CFXSceneGlow()
{
	DefineVar("Start Time",  CVarFloat::CLASSNAME);
	DefineVar("Speed",       CVarSpeed::CLASSNAME);
	DefineVar("Camera",      CVarString::CLASSNAME);
	DefineVar("Loop Length", CVarFloat::CLASSNAME);
	DefineVar("Loop Start",  CVarFloat::CLASSNAME);
	DefineVar("Render Mode", CVarCombo::CLASSNAME);

	DefineVar("Glow Radius (Pw2)", CVarInt::CLASSNAME);
	DefineVar("Intensity Cut",     CVarFloat::CLASSNAME);
	DefineVar("Post Multiply",     CVarInt::CLASSNAME);
	DefineVar("Alpha",             CVarFloat::CLASSNAME);

	DefineResourceReference("Scene", CResourceScene::CLASSNAME);
	DefineResourceReference("Texture Target", CResourceTexture2D::CLASSNAME);

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

	((CVarInt*)  GetVar("Glow Radius (Pw2)" ))->SetConstant(4);
	((CVarFloat*)GetVar("Intensity Cut"     ))->SetConstant(0.8f);
	((CVarInt*)  GetVar("Post Multiply"     ))->SetConstant(4);
	((CVarFloat*)GetVar("Alpha"             ))->SetConstant(1.0f);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_fAnimStart  = m_fAnimEnd = 0.0f;
	m_pResScene   = NULL;
	m_pResTexture = NULL;
}

CFXSceneGlow::~CFXSceneGlow()
{
	Free();
}

void CFXSceneGlow::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResScene)
	{
		FXRuntimeError("Scene resource not available");
		return;
	}

	CSceneView* pSceneView = const_cast<CSceneView*>(((CResourceScene*)m_pResScene)->GetSceneView());

	if(!pSceneView)
	{
		FXRuntimeError("NULL pSceneView");
		return;
	}

	// Transform

	CVarFloat::CValueFloat   valueStartTime;
	CVarSpeed::CValueSpeed   valueSpeed;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;
	CVarString::CValueString valueCamera;

	EvaluateVar("Start Time",  fEffectTime, &valueStartTime);
	EvaluateVar("Speed",       fEffectTime, &valueSpeed);
	EvaluateVar("Loop Length", fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",  fEffectTime, &valueLoopStart);
	EvaluateVar("Camera",      fEffectTime, &valueCamera);

	m_fTime = valueStartTime.GetValue() + valueSpeed.Integrate(0.0f, fEffectTime);

	if(IS_ZERO(valueLoopLength.GetValue()))
	{
		ComputeLoop(valueLoopStart.GetValue(), m_fAnimEnd, &m_fTime);
	}
	else if(valueLoopLength.GetValue() > 0.0f)
	{
		ComputeLoop(valueLoopStart.GetValue(), valueLoopLength.GetValue(), &m_fTime);
	}

	if(valueCamera.GetString() != m_strCamera)
	{
		// Camera string changed at runtime
		m_strCamera = valueCamera.GetString();
	}

	pSceneView->SetActiveCamera(m_strCamera);

	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	pSceneView->Transform(pDemo, fEffectTime, mtxAddLocal, mtxAddWorld, v3AddCamPos, v3AddCamAngles);

	// Evaluate Vars

	if(!m_pResTexture) return;

	CVarInt::CValueInt       valueRadius;
	CVarFloat::CValueFloat   valueIntensityCut;
	CVarInt::CValueInt       valuePostMultiply;

	EvaluateVar("Glow Radius (Pw2)", fEffectTime, &valueRadius);
	EvaluateVar("Intensity Cut",     fEffectTime, &valueIntensityCut);
	EvaluateVar("Post Multiply",     fEffectTime, &valuePostMultiply);

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(pTexture)
	{
		pDemo->SetViewport(0, 0, pTexture->GetWidth(), pTexture->GetHeight(), pDemo->ScissorActive());
	}
	else
	{
		return;
	}

	m_mtxWorld       = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_WORLD);
	m_mtxView        = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW);
	m_mtxProjection  = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	DrawScene(pDemo, fEffectTime, fDemoTime);

	// Substract

	float fSubstract = valueIntensityCut.GetValue();

	if(fSubstract > 1.0f) fSubstract = 1.0f;

	if(UtilGL::Extensions::GetAvailableExtensions()->EXT_blend_subtract && fSubstract > 0.0f)
	{
		glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);

		UtilGL::Rendering::DrawRect(NULL, 0.0f, 0.0f, 1.0f, 1.0f,
									0.0f, CVector4(1.0f, 1.0f, 1.0f, 0.0f) * fSubstract,
									UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ONE);

		glBlendEquationEXT(GL_FUNC_ADD_EXT);
	}

	// Add

	int nPasses = valuePostMultiply.GetValue() - 1;

	if(nPasses < 0)   nPasses = 0;
	if(nPasses > 100) nPasses = 100;

	if(nPasses > 0)
	{
		pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	}

	for(int i = 0; i < nPasses; i++)
	{
		UtilGL::Rendering::DrawCenteredQuad(pTexture,
											0.5f, 0.5f, 1.0f, 1.0f,
											0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
											UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ONE);
	}

	// Blur

	UtilGL::Rendering::ComputeBoxBlur(valueRadius.GetValue(), pTexture);
	pDemo->SetDemoViewport();

	// Clear framebuffer

	pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);

}

bool CFXSceneGlow::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXSceneGlow::LoadData(CResourceList* pResourceList)
{
	m_pResTexture = FindResource(pResourceList, "Texture Target", CResourceTexture2D::CLASSNAME);

	if(!m_pResTexture)
	{
		FXLoadError("Can't find Texture Target resource");
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
		// Put scene to starttime (to optimize particles, mainly)

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

		m_fAnimStart = pSceneView->GetAnimationStart();
		m_fAnimEnd   = pSceneView->GetAnimationEnd();
	}

	m_strCamera = valueCamera.GetString();

	return true;
}

bool CFXSceneGlow::Free()
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

	m_strCamera.erase();
	return true;
}

bool CFXSceneGlow::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD,      m_mtxWorld);
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW,       m_mtxView);
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_PROJECTION, m_mtxProjection);

	DrawScene(pDemo, fEffectTime, fDemoTime);
	DrawGlow (pDemo, fEffectTime, fDemoTime);

	return true;
}

bool CFXSceneGlow::DrawScene(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

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

	// Evaluate vars

	CVarCombo::CValueCombo   valueRenderMode;

	EvaluateVar("Render Mode", fEffectTime, &valueRenderMode);

	CRenderVisitor::EMode eRenderMode = CRenderVisitor::MODE_FULLDETAIL;

	     if(valueRenderMode.GetValue() == "Full")  eRenderMode = CRenderVisitor::MODE_FULLDETAIL;
	else if(valueRenderMode.GetValue() == "Solid") eRenderMode = CRenderVisitor::MODE_SOLID;
	else if(valueRenderMode.GetValue() == "Wire")  eRenderMode = CRenderVisitor::MODE_WIREFRAME;

	return pSceneView->Render(pDemo, m_fTime, eRenderMode, false);
}

bool CFXSceneGlow::DrawGlow(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not available");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("No texture target available");
		return false;
	}

	// Evaluate vars

	CVarFloat::CValueFloat valueAlpha;

	EvaluateVar("Alpha", fEffectTime, &valueAlpha);

	return UtilGL::Rendering::DrawCenteredQuad(pTexture,
									0.5f, 0.5f, 1.0f, 1.0f,
									0.0f, CVector4(1.0f, 1.0f, 1.0f, valueAlpha.GetValue()),
									UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_ONE);

}
