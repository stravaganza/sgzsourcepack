#include "Plugins.h"
#include "FXDiscoFloor.h"


const std::string CFXDiscoFloor::CLASSNAME("Disco Floor");

REGISTER_PLUGIN(CPluginFactory, CFXDiscoFloor)


CFXDiscoFloor::CFXDiscoFloor()
{
	DefineVar("Start Time",     CVarFloat::CLASSNAME);
	DefineVar("Speed",          CVarSpeed::CLASSNAME);
	DefineVar("Camera",         CVarString::CLASSNAME);
	DefineVar("Loop Length",    CVarFloat::CLASSNAME);
	DefineVar("Loop Start",     CVarFloat::CLASSNAME);
	DefineVar("Render Mode",    CVarCombo::CLASSNAME);
	DefineVar("Tile Substring", CVarString::CLASSNAME);
	DefineVar("Change Speed",   CVarFloat::CLASSNAME);
	DefineVar("Color1",         CVarColor::CLASSNAME);
	DefineVar("Color2",         CVarColor::CLASSNAME);

	DefineResourceReference("Scene", CResourceScene::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Wire");

	// Set the rest of default values

	((CVarFloat*)GetVar("Start Time"))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time"))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Speed"     ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Change Speed"))->LockConstant(true); // must be constant

	((CVarFloat*)GetVar("Loop Start" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Loop Length"))->SetConstant(-1.0f);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_fAnimStart = m_fAnimEnd = 0.0f;
	m_pResScene  = NULL;
	m_nPeriod    = -1;
}

CFXDiscoFloor::~CFXDiscoFloor()
{
	Free();
}

bool CFXDiscoFloor::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXDiscoFloor::LoadData(CResourceList* pResourceList)
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

			CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
			enumVisitor.Reset();
			pNode->AcceptVisitor(&enumVisitor);

			CVarString::CValueString valueTile;

			if(!EvaluateVar("Tile Substring", 0.0f, &valueTile))
			{
				FXLoadError("Unable to evaluate camera from scene");
				return false;
			}

			for(int i = 0; i < enumVisitor.GetNumObjects(); i++)
			{
				if(strstr(enumVisitor.GetObject(i)->GetNodeName().data(), valueTile.GetString().data()))
				{
					m_vecTiles.push_back(enumVisitor.GetObject(i));
				}
			}
		}

		m_fAnimStart = pSceneView->GetAnimationStart();
		m_fAnimEnd   = pSceneView->GetAnimationEnd();
	}

	m_nPeriod     = -1;
	m_strCamera   = valueCamera.GetString();

	return true;
}

bool CFXDiscoFloor::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	m_nPeriod = -1;
	m_vecTiles.clear();
	m_strCamera.erase();
	return true;
}

bool CFXDiscoFloor::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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
	CVarFloat::CValueFloat   valueChangeSpeed;
	CVarColor::CValueColor   valueColor1;
	CVarColor::CValueColor   valueColor2;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;

	EvaluateVar("Start Time",   fEffectTime, &valueStartTime);
	EvaluateVar("Speed",        fEffectTime, &valueSpeed);
	EvaluateVar("Render Mode",  fEffectTime, &valueRenderMode);
	EvaluateVar("Camera",       fEffectTime, &valueCamera);
	EvaluateVar("Change Speed", fEffectTime, &valueChangeSpeed);
	EvaluateVar("Color2",       fEffectTime, &valueColor1);
	EvaluateVar("Color1",       fEffectTime, &valueColor2);
	EvaluateVar("Loop Length",  fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",   fEffectTime, &valueLoopStart);

	float fPeriod = valueChangeSpeed.GetValue();
	if(fPeriod < 0.01f) fPeriod = 0.01f;

	int nPeriod = fEffectTime / fPeriod;

	if(m_nPeriod != nPeriod)
	{
		m_nPeriod = nPeriod;
		srand(m_nPeriod);

		for(int i = 0; i < m_vecTiles.size(); i++)
		{
			CShaderStandard* pShader = (CShaderStandard*)m_vecTiles[i]->GetShader();

			if(pShader)
			{
				bool bOne = (rand() % 2) == 1;

				if(bOne) pShader->SetDiffuse(valueColor1.GetValue());
				else     pShader->SetDiffuse(valueColor2.GetValue());
			}
		}
	}

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
	return pSceneView->Render(pDemo, fTime, eRenderMode, false);
}
