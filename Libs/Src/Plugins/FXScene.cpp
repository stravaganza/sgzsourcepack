#include "Plugins.h"
#include "FXScene.h"


const std::string CFXScene::CLASSNAME("Standard Scene");

REGISTER_PLUGIN(CPluginFactory, CFXScene)


CFXScene::CFXScene()
{
	DefineVar("Start Time",  CVarFloat::CLASSNAME);
	DefineVar("Speed",       CVarSpeed::CLASSNAME);
	DefineVar("Camera",      CVarString::CLASSNAME);
	DefineVar("Loop Length", CVarFloat::CLASSNAME);
	DefineVar("Loop Start",  CVarFloat::CLASSNAME);
	DefineVar("Render Mode", CVarCombo::CLASSNAME);

	DefineResourceReference("Scene", CResourceScene::CLASSNAME);

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

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_fAnimStart = m_fAnimEnd = 0.0f;

	m_pResScene = NULL;
}

CFXScene::~CFXScene()
{
	Free();
}

bool CFXScene::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXScene::LoadData(CResourceList* pResourceList)
{
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

bool CFXScene::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	m_strCamera.erase();
	return true;
}

bool CFXScene::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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

	CVarFloat::CValueFloat   valueStartTime;
	CVarSpeed::CValueSpeed   valueSpeed;
	CVarCombo::CValueCombo   valueRenderMode;
	CVarString::CValueString valueCamera;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;

	EvaluateVar("Start Time",  fEffectTime, &valueStartTime);
	EvaluateVar("Speed",       fEffectTime, &valueSpeed);
	EvaluateVar("Render Mode", fEffectTime, &valueRenderMode);
	EvaluateVar("Camera",      fEffectTime, &valueCamera);
	EvaluateVar("Loop Length", fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",  fEffectTime, &valueLoopStart);

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

	pSceneView->Transform    (pDemo, fTime, mtxAddLocal, mtxAddWorld, v3AddCamPos, v3AddCamAngles);
	return pSceneView->Render(pDemo, fTime, eRenderMode, false);
}
