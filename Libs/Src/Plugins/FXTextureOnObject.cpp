#include "Plugins.h"
#include "FXTextureOnObject.h"


const std::string CFXTextureOnObject::CLASSNAME("Texture On Objects");

REGISTER_PLUGIN(CPluginFactory, CFXTextureOnObject)


CFXTextureOnObject::CFXTextureOnObject()
{
	DefineVar("Start Time",       CVarFloat::CLASSNAME);
	DefineVar("Speed",            CVarSpeed::CLASSNAME);
	DefineVar("Camera",           CVarString::CLASSNAME);
	DefineVar("Loop Length",      CVarFloat::CLASSNAME);
	DefineVar("Loop Start",       CVarFloat::CLASSNAME);
	DefineVar("Render Mode",      CVarCombo::CLASSNAME);
	DefineVar("Shader Channel",   CVarCombo::CLASSNAME);
	DefineVar("Object Substring", CVarString::CLASSNAME);

	DefineResourceReference("Scene",   CResourceScene::CLASSNAME);
	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Wire");

	((CVarCombo*)GetVar("Shader Channel"))->AddOption("Diffuse");
	((CVarCombo*)GetVar("Shader Channel"))->AddOption("Map2");
	((CVarCombo*)GetVar("Shader Channel"))->AddOption("Bump");

	// Set the rest of default values

	((CVarFloat*)GetVar("Start Time"))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time"))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Speed"     ))->SetConstant(1.0f);

	((CVarFloat*)GetVar("Loop Start" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Loop Length"))->SetConstant(-1.0f);

	GetVar("Shader Channel"   )->ReloadFXOnChange(this);
	GetVar("Object Substring" )->ReloadFXOnChange(this);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_pResScene   = NULL;
	m_pResTexture = NULL;

	m_fAnimStart = m_fAnimEnd = 0.0f;
}

CFXTextureOnObject::~CFXTextureOnObject()
{
	Free();
}

bool CFXTextureOnObject::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXTextureOnObject::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResScene = this->FindResource(pResourceList, "Scene", CResourceScene::CLASSNAME)))
	{
		FXLoadError("Can't find scene resource");
		return false;
	}

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResScene->NotifyDependency(this);
	m_pResTexture->NotifyDependency(this);

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
			CTransformVisitor transformVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);;
			transformVisitor.Reset();
			transformVisitor.SetEvalTime(fTime);
			pNode->AcceptVisitor(&transformVisitor);
			transformVisitor.DoPendingTransformations();

			// Assign textures

			CVarCombo::CValueCombo   valueChannel;
			CVarString::CValueString valueObjectString;

			EvaluateVar("Shader Channel",   0.0f, &valueChannel);
			EvaluateVar("Object Substring", 0.0f, &valueObjectString);

			CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
			pNode->AcceptVisitor(&enumVisitor);

			CShaderStandard::ELayerType eChannel = CShaderStandard::LAYER_DIFF;

				 if(valueChannel.GetValue() == "Diffuse") eChannel = CShaderStandard::LAYER_DIFF;
			else if(valueChannel.GetValue() == "Map2")    eChannel = CShaderStandard::LAYER_MAP2;
			else if(valueChannel.GetValue() == "Bump")    eChannel = CShaderStandard::LAYER_BUMP;

			UtilGL::Texturing::CTexture2D* pTexture = (UtilGL::Texturing::CTexture2D*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();

			for(int i = 0; i < enumVisitor.GetNumObjects(); i++)
			{
				CObject* pObject = enumVisitor.GetObject(i);

				if(pObject)
				{
					if(strstr(pObject->GetNodeName().data(), valueObjectString.GetString().data()))
					{
						CShaderStandard* pShader = (CShaderStandard*)pObject->GetShader(); // TODO: change cast. can be wrong.

						if(pShader)
						{
							m_vecShadersChanged.push_back(SShaderChanged(pShader, eChannel, pShader->GetTexture(eChannel), pTexture));
						}
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

bool CFXTextureOnObject::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	if(m_pResTexture)
	{
		UtilGL::Texturing::CTexture2D* pTexture = (UtilGL::Texturing::CTexture2D*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();

		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	m_vecShadersChanged.clear();
	m_strCamera.erase();
	return true;
}

bool CFXTextureOnObject::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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

	pSceneView->SetActiveCamera(m_strCamera);

	CRenderVisitor::EMode eRenderMode = CRenderVisitor::MODE_FULLDETAIL;

	     if(valueRenderMode.GetValue() == "Full")  eRenderMode = CRenderVisitor::MODE_FULLDETAIL;
	else if(valueRenderMode.GetValue() == "Solid") eRenderMode = CRenderVisitor::MODE_SOLID;
	else if(valueRenderMode.GetValue() == "Wire")  eRenderMode = CRenderVisitor::MODE_WIREFRAME;

	VECSHADERSCHANGED::iterator it;

	for(it = m_vecShadersChanged.begin(); it != m_vecShadersChanged.end(); ++it)
	{
		it->pShader->SetTexture(it->eLayer, it->pNewTexture, false);
	}

	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	pSceneView->Transform(pDemo, fTime, mtxAddLocal, mtxAddWorld, v3AddCamPos, v3AddCamAngles);
	bool bResult = pSceneView->Render(pDemo, fTime, eRenderMode, false);

	for(it = m_vecShadersChanged.begin(); it != m_vecShadersChanged.end(); ++it)
	{
		it->pShader->SetTexture(it->eLayer, it->pOldTexture, false);
	}

	return bResult;
}
