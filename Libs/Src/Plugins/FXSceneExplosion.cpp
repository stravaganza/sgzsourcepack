#include "Plugins.h"
#include "FXSceneExplosion.h"


const std::string CFXSceneExplosion::CLASSNAME("Scene Explosion");

REGISTER_PLUGIN(CPluginFactory, CFXSceneExplosion)


CFXSceneExplosion::CFXSceneExplosion()
{
	DefineVar("Start Time",           CVarFloat::CLASSNAME);
	DefineVar("Start Time Explosion", CVarFloat::CLASSNAME);
	DefineVar("Speed",                CVarSpeed::CLASSNAME);
	DefineVar("Camera",               CVarString::CLASSNAME);
	DefineVar("Loop Length",          CVarFloat::CLASSNAME);
	DefineVar("Loop Start",           CVarFloat::CLASSNAME);
	DefineVar("Render Mode",          CVarCombo::CLASSNAME);
	DefineVar("Intensity",            CVarFloat::CLASSNAME);
	DefineVar("Gravity",              CVarFloat::CLASSNAME);
	DefineVar("Duration",             CVarFloat::CLASSNAME);
	DefineVar("Object Substring",     CVarString::CLASSNAME);

	DefineResourceReference("Scene", CResourceScene::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Wire");

	// Set the rest of default values

	((CVarFloat*)GetVar("Start Time"          ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time"          ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Start Time Explosion"))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time Explosion"))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Speed"               ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Intensity"           ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Gravity"             ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Duration"            ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Intensity"           ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Gravity"             ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Duration"            ))->SetConstant(01.0f);
	((CVarFloat*)GetVar("Loop Start"          ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Loop Length"         ))->SetConstant(-1.0f);

	GetVar("Start Time"          )->ReloadFXOnChange(this);
	GetVar("Start Time Explosion")->ReloadFXOnChange(this);
	GetVar("Speed"               )->ReloadFXOnChange(this);
	GetVar("Intensity"           )->ReloadFXOnChange(this);
	GetVar("Gravity"             )->ReloadFXOnChange(this);
	GetVar("Duration"            )->ReloadFXOnChange(this);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_pResScene  = NULL;
	m_fAnimStart = m_fAnimEnd = 0.0f;
}

CFXSceneExplosion::~CFXSceneExplosion()
{
	Free();
}

bool CFXSceneExplosion::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXSceneExplosion::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResScene = this->FindResource(pResourceList, "Scene", CResourceScene::CLASSNAME)))
	{
		FXLoadError("Can't find scene resource");
		return false;
	}

	m_pResScene->NotifyDependency(this);

	// Get camera name

	CVarString::CValueString valueCamera;
	CVarFloat::CValueFloat   valueDuration;

	if(!EvaluateVar("Camera", 0.0f, &valueCamera))
	{
		FXLoadError("Unable to evaluate camera from scene");
		return false;
	}

	if(!EvaluateVar("Duration", 0.0f, &valueDuration))
	{
		FXLoadError("Unable to evaluate explosion duration");
		return false;
	}

	CSceneView* pSceneView = const_cast<CSceneView*>(((CResourceScene*)m_pResScene)->GetSceneView());

	if(pSceneView)
	{
		// Put scene to starttime (to optimize particles, mainly)

		CVarFloat::CValueFloat valueStartTime;
		CVarFloat::CValueFloat valueStartTimeExplosion;
		CVarFloat::CValueFloat valueIntensity;
		CVarFloat::CValueFloat valueGravity;
		CVarSpeed::CValueSpeed valueSpeed;

		EvaluateVar("Start Time",           0.0f,                      &valueStartTime);
		EvaluateVar("Start Time Explosion", 0.0f,                      &valueStartTimeExplosion);
		EvaluateVar("Intensity",            0.0f,                      &valueIntensity);
		EvaluateVar("Gravity",              0.0f,                      &valueGravity);
		EvaluateVar("Speed",                valueStartTime.GetValue(), &valueSpeed);

		float fTime = valueStartTime.GetValue() + valueSpeed.Integrate(0.0f, valueStartTime.GetValue());

		CNode* pNode = (CNode*)pSceneView->GetRootNode();

		if(pNode)
		{
			CTransformVisitor transformVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);;
			transformVisitor.Reset();
			transformVisitor.SetEvalTime(fTime);
			pNode->AcceptVisitor(&transformVisitor);
			transformVisitor.DoPendingTransformations();

			// Create object vector

			CVarString::CValueString valueObjectString;

			EvaluateVar("Object Substring", 0.0f, &valueObjectString);

			CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
			pNode->AcceptVisitor(&enumVisitor);

			for(int i = 0; i < enumVisitor.GetNumObjects(); i++)
			{
				CObject* pObject = enumVisitor.GetObject(i);

				if(pObject)
				{
					if(strstr(pObject->GetNodeName().data(), valueObjectString.GetString().data()))
					{
						m_vecExplosions.push_back(SExplosion(pObject, valueIntensity.GetValue(), valueGravity.GetValue()));

						// Get start
						// We can override default start time with the node user defined string

						float fStart = valueStartTimeExplosion.GetValue();

						CInputMemory inputMem;
						inputMem.Open((void*)pObject->GetUserDefinedString().data(), pObject->GetUserDefinedString().length());

						CParser parser;
						parser.Init(&inputMem);
						parser.ReadKeyFloat("#start", '=', &fStart, CParser::SEARCH_WHOLEDATA);
						parser.Close();

						m_vecExplosions.back().fStart    = fStart;
						m_vecExplosions.back().fDuration = valueDuration.GetValue();
					}
				}
			}
		}

		m_fAnimStart = pSceneView->GetAnimationStart();
		m_fAnimEnd   = pSceneView->GetAnimationEnd();
	}

	m_strCamera = valueCamera.GetString();

	return true;
}

bool CFXSceneExplosion::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	VECEXPLOSIONS::iterator it;

	for(it = m_vecExplosions.begin(); it < m_vecExplosions.end(); ++it)
	{
		it->Delete();
	}

	m_vecExplosions.clear();
	m_strCamera.erase();
	return true;
}

bool CFXSceneExplosion::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResScene)
	{
		FXRuntimeError("Scene resource not available");
		return false;
	}

	CSceneView* pSceneView = (CSceneView*)(((CResourceScene*)m_pResScene)->GetSceneView());

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

	EvaluateVar("Start Time",     fEffectTime, &valueStartTime);
	EvaluateVar("Speed",          fEffectTime, &valueSpeed);
	EvaluateVar("Render Mode",    fEffectTime, &valueRenderMode);
	EvaluateVar("Camera",         fEffectTime, &valueCamera);
	EvaluateVar("Loop Length",    fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",     fEffectTime, &valueLoopStart);

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

	// DYNAMICS

	VECEXPLOSIONS::iterator it;

	for(it = m_vecExplosions.begin(); it < m_vecExplosions.end(); ++it)
	{
		it->fTime = fTime;

		SExplosion* pExplosion = &(*it);

		it->pObject->SetPreRenderCallback (PreRender,  pExplosion);
		it->pObject->SetPostRenderCallback(PostRender, pExplosion);
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
	bool bResult = pSceneView->Render(pDemo, fTime, eRenderMode, false);

	for(it = m_vecExplosions.begin(); it < m_vecExplosions.end(); ++it)
	{
		it->pObject->SetPreRenderCallback (NULL, NULL);
		it->pObject->SetPostRenderCallback(NULL, NULL);
	}

	return bResult;
}

void CFXSceneExplosion::PreRender(CRenderable* pRenderable, void* pData)
{
	SExplosion* pExplosion = (SExplosion*)pData;
	CObject*    pObject    = (CObject*)pRenderable;

	float fTime = pExplosion->fTime - pExplosion->fStart;

	if(fTime > 0.0f && fTime < pExplosion->fDuration)
	{
		pObject->SetStaticMesh(false);

		pObject->SetFaceList(pObject->GetNumFaces(),      pExplosion->puNewFaceList);
		pObject->SetVertices(pExplosion->uNewNumVertices, pExplosion->pv3NewVertices);
		pObject->SetVertexNormals(pExplosion->pv3NewVertexNormals);

		int i;

		for(i = 0; i < pObject->GetNumMapChannels(); i++)
		{
			pObject->SetMapChannel(i, pExplosion->m_vecNewMapChannels[i]);
		}

		for(i = 0; i < pExplosion->uNewNumVertices; i++)
		{
			// Physics
			pExplosion->pv3NewVertices[i] =		(pExplosion->pv3ExplVectors[i] * fTime)
											+	(CVector3(0.0f, -pExplosion->fGravity * 0.5f, 0.0f) * 
												fTime * fTime);

			pExplosion->pv3NewVertices[i] = pExplosion->pv3NewVertices[i] + pExplosion->pv3NewStartVertices[i];
		}
	}
}

void CFXSceneExplosion::PostRender(CRenderable* pRenderable, void* pData)
{
	SExplosion* pExplosion = (SExplosion*)pData;
	CObject*    pObject    = (CObject*)pRenderable;

	pObject->SetStaticMesh(pExplosion->bStatic);

	pObject->SetFaceList(pObject->GetNumFaces(),      pExplosion->puOldFaceList);
	pObject->SetVertices(pExplosion->uOldNumVertices, pExplosion->pv3OldVertices);
	pObject->SetVertexNormals(pExplosion->pv3OldVertexNormals);

	int i;

	for(i = 0; i < pObject->GetNumMapChannels(); i++)
	{
		pObject->SetMapChannel(i, pExplosion->m_vecOldMapChannels[i]);
	}
}