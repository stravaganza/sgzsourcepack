#include "Plugins.h"
#include "FXBillboardScene.h"


const std::string CFXBillboardScene::CLASSNAME("Billboard Scene");

REGISTER_PLUGIN(CPluginFactory, CFXBillboardScene)


CFXBillboardScene::CFXBillboardScene()
{
	DefineVar("Start Time",  CVarFloat::CLASSNAME);
	DefineVar("Speed",       CVarSpeed::CLASSNAME);
	DefineVar("Camera",      CVarString::CLASSNAME);
	DefineVar("Loop Length", CVarFloat::CLASSNAME);
	DefineVar("Loop Start",  CVarFloat::CLASSNAME);
	DefineVar("Render Mode", CVarCombo::CLASSNAME);

	DefineVar("Color",      CVarColor::CLASSNAME);
	DefineVar("Size",       CVarFloat::CLASSNAME);
	DefineVar("Alpha",      CVarFloat::CLASSNAME);
	DefineVar("Angle",      CVarFloat::CLASSNAME);
	DefineVar("Blend Mode", CVarCombo::CLASSNAME);
	DefineVar("Filtering",  CVarCombo::CLASSNAME);

	DefineResourceReference("Scene",   CResourceScene::CLASSNAME);
	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Wire");

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes");
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	// Set the rest of default values

	((CVarFloat*)GetVar("Start Time" ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time" ))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Speed"      ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Loop Length"))->SetConstant(-1.0f);
	((CVarFloat*)GetVar("Loop Start" ))->SetConstant(0.0f);

	((CVarColor*)GetVar("Color" ))->SetConstant(CVector3(1.0f, 1.0f, 1.0f));
	((CVarFloat*)GetVar("Size"  ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Alpha" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Angle" ))->SetConstant(0.0f);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_pResScene   = NULL;
	m_pResTexture = NULL;
	m_fAnimStart  = m_fAnimEnd = 0.0f;
}

CFXBillboardScene::~CFXBillboardScene()
{
	Free();
}

bool CFXBillboardScene::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXBillboardScene::LoadData(CResourceList* pResourceList)
{
	// Scene

	if(!(m_pResScene = this->FindResource(pResourceList, "Scene", CResourceScene::CLASSNAME)))
	{
		FXLoadError("Can't find scene resource");
		return false;
	}

	m_pResScene->NotifyDependency(this);

	// Texture

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

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
		}

		m_fAnimStart = pSceneView->GetAnimationStart();
		m_fAnimEnd   = pSceneView->GetAnimationEnd();
	}

	m_strCamera = valueCamera.GetString();

	return true;
}

bool CFXBillboardScene::Free()
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

bool CFXBillboardScene::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Scene

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

	// Texture

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("WARNING: Texture not available, effect will draw flat quad");
	}

	// Vars

	CVarFloat::CValueFloat   valueStartTime;
	CVarSpeed::CValueSpeed   valueSpeed;
	CVarCombo::CValueCombo   valueRenderMode;
	CVarString::CValueString valueCamera;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;

	CVarColor::CValueColor   valueColor;
	CVarFloat::CValueFloat   valueSize;
	CVarFloat::CValueFloat   valueAlpha;
	CVarFloat::CValueFloat   valueAngle;
	CVarCombo::CValueCombo   valueBlendMode;
	CVarCombo::CValueCombo   valueFiltering;

	EvaluateVar("Start Time",  fEffectTime, &valueStartTime);
	EvaluateVar("Speed",       fEffectTime, &valueSpeed);
	EvaluateVar("Render Mode", fEffectTime, &valueRenderMode);
	EvaluateVar("Camera",      fEffectTime, &valueCamera);
	EvaluateVar("Loop Length", fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",  fEffectTime, &valueLoopStart);

	EvaluateVar("Color",       fEffectTime, &valueColor);
	EvaluateVar("Size",        fEffectTime, &valueSize);
	EvaluateVar("Alpha",       fEffectTime, &valueAlpha);
	EvaluateVar("Angle",       fEffectTime, &valueAngle);
	EvaluateVar("Blend Mode",  fEffectTime, &valueBlendMode);
	EvaluateVar("Filtering",   fEffectTime, &valueFiltering);

	float fAlpha = valueAlpha.GetValue();
	CVector4 v4Color = valueColor.GetValue();
	v4Color.SetW(fAlpha);

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

	// States

	UtilGL::States::SetColor(v4Color);

	// Change filter mode

	bool bTextureAlpha = false;
	GLint minFilter, magFilter;

	if(pTexture)
	{
		pTexture->SetActive();

		if(pTexture->HasAlpha())
		{
			bTextureAlpha = true;
		}

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

		if(valueFiltering.GetValue() == "Yes")
		{
			// If it's point filtering, change it to linear, otherwise leave it as it is

			if(minFilter == GL_NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			if(magFilter == GL_NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	// Blending

	int nSrcBlend = UtilGL::States::BLEND_SRCALPHA;
	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !bTextureAlpha)
	{
		nSrcBlend = UtilGL::States::BLEND_ONE;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}

	if(!((nSrcBlend == UtilGL::States::BLEND_ONE  && nDstBlend == UtilGL::States::BLEND_ZERO) ||
		 (nSrcBlend == UtilGL::States::BLEND_ZERO && nDstBlend == UtilGL::States::BLEND_ONE)))
	{
		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND, nSrcBlend);
		UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	}

	UtilGL::States::Set(UtilGL::States::LIGHTING,     UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ALPHATEST,    UtilGL::States::ENABLED);
	glAlphaFunc(GL_GREATER, 0.0f);

	CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
	enumVisitor.Reset();

	if(pSceneView->GetRootNode())
	{
		CNode* pRootNode = (CNode*)pSceneView->GetRootNode();
		pRootNode->AcceptVisitor(&enumVisitor);
	}

	if(valueRenderMode.GetValue() == "Solid")
	{
		pTexture = NULL;
	}
	else if(valueRenderMode.GetValue() == "Wire")
	{
		pTexture = NULL;
		UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_LINE);
	}

	for(int i = 0; i < enumVisitor.GetNumObjects(); i++)
	{
		CObject* pObject = enumVisitor.GetObject(i);

		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pObject->GetAbsoluteTM());

		UtilGL::Rendering::DrawBillboards(	pObject->GetNumVertices(),
											pTexture,
											pObject->GetVertices(),
											NULL,
											valueSize.GetValue(),
											valueAngle.GetValue());
	}

	if(pTexture)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	}

	UtilGL::States::Set(UtilGL::States::BLENDING,     UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ALPHATEST,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::POLYGONMODE,  UtilGL::States::POLYGONMODE_FILL);
	UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::ENABLED);

	return true;
}
