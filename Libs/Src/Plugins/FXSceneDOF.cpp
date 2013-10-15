#include "Plugins.h"
#include "FXSceneDOF.h"


const std::string CFXSceneDOF::CLASSNAME("Scene With Depth Of Field");

REGISTER_PLUGIN(CPluginFactory, CFXSceneDOF)


CFXSceneDOF::CFXSceneDOF()
{
	DefineVar("Blur Radius (Pw2)",     CVarInt::CLASSNAME);

	DefineVar("Start Time",          CVarFloat::CLASSNAME);
	DefineVar("Speed",               CVarSpeed::CLASSNAME);
	DefineVar("Focus",				 CVarFloat::CLASSNAME);
	DefineVar("Near-Far Multiplier", CVarFloat::CLASSNAME);
	DefineVar("Sharpness",           CVarFloat::CLASSNAME);
	DefineVar("Camera",              CVarString::CLASSNAME);
	DefineVar("Loop Length",         CVarFloat::CLASSNAME);
	DefineVar("Loop Start",          CVarFloat::CLASSNAME);
	DefineVar("Render Mode",         CVarCombo::CLASSNAME);

	DefineVar("Debug Alpha",         CVarCombo::CLASSNAME);

	DefineResourceReference("Scene",        CResourceScene::CLASSNAME);
	DefineResourceReference("DOF Texture",  CResourceTexture2D::CLASSNAME);
	DefineResourceReference("Blur Texture", CResourceTexture2D::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Render Mode"))->AddOption("Wire");

	((CVarCombo*)GetVar("Debug Alpha"))->AddOption("No");
	((CVarCombo*)GetVar("Debug Alpha"))->AddOption("Yes");

	// Set the rest of default values

	((CVarFloat*)GetVar("Start Time"         ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Start Time"         ))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Speed"              ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Focus"              ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Near-Far Multiplier"))->SetConstant(0.3f);
	((CVarFloat*)GetVar("Sharpness"          ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Loop Length"        ))->SetConstant(-1.0f);

	((CVarFloat*)GetVar("Loop Start" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Loop Length"))->SetConstant(-1.0f);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_pResScene       = NULL;
	m_pResDOFTexture  = NULL;
	m_pResBlurTexture = NULL;
	m_pTexDOF         = NULL;
	m_pTexBlur        = NULL;
	m_gluiDOFVertexProgram = 0;

	m_fAnimStart = m_fAnimEnd = 0.0f;
}

CFXSceneDOF::~CFXSceneDOF()
{
	Free();
}

void CFXSceneDOF::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(m_pTexDOF && m_pTexBlur)
	{
		if(m_pTexDOF->GetWidth() > 0)
		{
			pDemo->SetViewport(0, 0, m_pTexDOF->GetWidth(), m_pTexDOF->GetHeight(), pDemo->ScissorActive());
		}
	}
}

bool CFXSceneDOF::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXSceneDOF::LoadData(CResourceList* pResourceList)
{
	if(!UtilGL::Extensions::GetAvailableExtensions()->ARB_vertex_program)
	{
		return false;
	}

	if(!(m_pResScene = this->FindResource(pResourceList, "Scene", CResourceScene::CLASSNAME)))
	{
		FXLoadError("Can't find scene resource");
		return false;
	}

	m_pResScene->NotifyDependency(this);

	if(!(m_pResDOFTexture = this->FindResource(pResourceList, "DOF Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find DOF Texture resource");
		return false;
	}

	m_pResDOFTexture->NotifyDependency(this);

	if(!(m_pResBlurTexture = this->FindResource(pResourceList, "Blur Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find Blur Texture resource");
		return false;
	}

	m_pResBlurTexture->NotifyDependency(this);

	m_pTexDOF  = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResDOFTexture)->GetTexture2D());
	m_pTexBlur = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResBlurTexture)->GetTexture2D());

	if(!m_pTexDOF || !m_pTexBlur)
	{
		FXLoadError("Can't get DOF/Blur Texture resource");
		return false;
	}

	m_pTexBlur->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_pTexDOF->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	// Gen vertex program

	glGenProgramsARB(1, &m_gluiDOFVertexProgram);

	std::string strvprogram =	"!!ARBvp1.0                                      \n\
								OPTION ARB_position_invariant;                   \n\
								ATTRIB pos         = vertex.position;            \n\
								PARAM  mv_mat[4]   = { state.matrix.modelview }; \n\
								TEMP   eyevertex, dist, col;                     \n\
\
								# Compute absolute distance from vertex to focus plane \n\
\
								DP4 eyevertex.z, mv_mat[2], pos;		 \n\
								SUB dist.z, eyevertex, program.local[0]; \n\
								ABS dist, dist;							 \n\
\
								# Normalize distance					 \n\
\
								MUL dist, dist, program.local[1];		 \n\
\
								# Output normalized distance to result.color.w (alpha) and output.color.RGB to (1, 1, 1) \n\
\
								MOV col, { 1.0, 1.0, 1.0, 1.0 }; \n\
								MOV col.w, dist.xywz;            \n\
								MOV result.color, col;           \n\
\
								END";

	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, m_gluiDOFVertexProgram);
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strvprogram.size(), strvprogram.c_str());

	if(GL_INVALID_OPERATION == glGetError())
	{
		GLint gliErrorPos;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &gliErrorPos);
		GLubyte* glubErrorString;
		glubErrorString = (GLubyte*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		FXRuntimeError("Error loading vertex program. Position %d\nError:\n%s", gliErrorPos, glubErrorString);
		return false;
	}

	return true;
}

bool CFXSceneDOF::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	if(m_pResDOFTexture)
	{
		m_pResDOFTexture->RemoveDependency(this);
		m_pResDOFTexture = NULL;
	}

	if(m_pResBlurTexture)
	{
		m_pResBlurTexture->RemoveDependency(this);
		m_pResBlurTexture = NULL;
	}

	m_pResScene       = NULL;
	m_pResDOFTexture  = NULL;
	m_pResBlurTexture = NULL;
	m_pTexDOF         = NULL;
	m_pTexBlur        = NULL;

	if(UtilGL::Extensions::GetAvailableExtensions()->ARB_vertex_program && m_gluiDOFVertexProgram)
	{
		glDeleteProgramsARB(1, &m_gluiDOFVertexProgram);
	}

	m_gluiDOFVertexProgram = 0;

	m_strCamera.erase();

	return true;
}

bool CFXSceneDOF::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResScene)
	{
		FXRuntimeError("Scene resource not available");
		pDemo->SetDemoViewport();
		return false;
	}

	if(!m_pTexDOF || !m_pTexBlur)
	{
		FXRuntimeError("DOF / Blur Texture not available");
		pDemo->SetDemoViewport();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return false;
	}

	CSceneView* pSceneView = const_cast<CSceneView*>(((CResourceScene*)m_pResScene)->GetSceneView());

	if(!pSceneView)
	{
		FXRuntimeError("NULL pSceneView");
		pDemo->SetDemoViewport();
		return false;
	}

	bool bAA = false;

	if(UtilGL::Extensions::GetAvailableExtensions()->ARB_multisample)
	{
		// Disable AA since it doesn't work well with this effect.

		bAA = glIsEnabled(GL_MULTISAMPLE_ARB) == GL_TRUE;
		glDisable(GL_MULTISAMPLE_ARB);
	}

	CVarFloat::CValueFloat   valueStartTime;
	CVarSpeed::CValueSpeed   valueSpeed;
	CVarFloat::CValueFloat   valueFocus;
	CVarFloat::CValueFloat   valueNearFarMultiplier;
	CVarFloat::CValueFloat   valueSharpness;
	CVarCombo::CValueCombo   valueRenderMode;
	CVarCombo::CValueCombo   valueDebugAlpha;
	CVarString::CValueString valueCamera;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;
	CVarInt::CValueInt       valueRadius;

	EvaluateVar("Start Time",          fEffectTime, &valueStartTime);
	EvaluateVar("Speed",               fEffectTime, &valueSpeed);
	EvaluateVar("Focus",               fEffectTime, &valueFocus);
	EvaluateVar("Near-Far Multiplier", fEffectTime, &valueNearFarMultiplier);
	EvaluateVar("Sharpness",           fEffectTime, &valueSharpness);
	EvaluateVar("Render Mode",         fEffectTime, &valueRenderMode);
	EvaluateVar("Debug Alpha",         fEffectTime, &valueDebugAlpha);
	EvaluateVar("Camera",              fEffectTime, &valueCamera);
	EvaluateVar("Loop Length",         fEffectTime, &valueLoopLength);
	EvaluateVar("Loop Start",          fEffectTime, &valueLoopStart);
	EvaluateVar("Blur Radius (Pw2)",   fEffectTime, &valueRadius);

	bool bDebug = valueDebugAlpha.GetValue() == "Yes";

	if(valueRadius.GetValue() < 1) valueRadius.SetValue(1);

	int nRadiusPwr = MYROUND(log10f(valueRadius.GetValue()) / log10f(2));
	int nRadius    = pow(2, nRadiusPwr);

	if(nRadius < 1) nRadius = 1;

	// RENDER

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

	// Render depth with vertex shaders

	glEnable(GL_VERTEX_PROGRAM_ARB);
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, m_gluiDOFVertexProgram);

	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	pSceneView->Transform(pDemo, fTime, mtxAddLocal, mtxAddWorld, v3AddCamPos, v3AddCamAngles);

	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	float    fNearFar = 1000.0f;
	CCamera* pCamera  = (CCamera*)((CNode*)pSceneView->GetRootNode())->FindNode(m_strCamera);

	if(pCamera)
	{
		fNearFar = pCamera->Far() - pCamera->Near();
	}

	fNearFar *= valueNearFarMultiplier.GetValue();
	if(fNearFar < 1.0f) fNearFar = 1.0f;

	float fNearFarMult = fNearFar * (1.0f - valueSharpness.GetValue());
	if(fNearFarMult < 0.001f) fNearFarMult = 0.001f;

	float fRecNearFarDist = 1.0f / fNearFarMult;
	float fFocusDist      = -(valueFocus.GetValue() * fNearFar);

	// Local param 0 = focus dist
	// Local param 1 = 1/(far-near) to normalize length from vertex to focus dist

	glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, 0.0f, 0.0f, fFocusDist,      0.0f);
	glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, 0.0f, 0.0f, fRecNearFarDist, 0.0f);

	UtilGL::States::Lock(UtilGL::States::LIGHTING, true);
	UtilGL::States::Set (UtilGL::States::LIGHTING, UtilGL::States::DISABLED);

	pSceneView->Render(pDemo, fTime, CRenderVisitor::MODE_SOLID, false);

	UtilGL::States::Lock(UtilGL::States::LIGHTING, false);
	glDisable(GL_VERTEX_PROGRAM_ARB);

	// Copy result to texture

	m_pTexDOF->CopyFromFramebuffer(0, 0, 0, 0, m_pTexDOF->GetWidth(), m_pTexDOF->GetHeight());

	// OR ComputeBoxBlur(1, m_pTexDOF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Render normally and compute box blur

	pDemo->SetViewport(0, 0, m_pTexBlur->GetWidth(), m_pTexBlur->GetHeight(), pDemo->ScissorActive());

	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_VIEW);

	pSceneView->Render(pDemo, fTime, eRenderMode, false);

	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	// Perform box blur

	UtilGL::Rendering::ComputeBoxBlur(nRadius, m_pTexBlur);

	// Final render

	pDemo->SetDemoViewport();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_VIEW);
	pSceneView->Render(pDemo, fTime, eRenderMode, false);

	// Draw blur+dof texture, mixing RGBA blur and Alpha DOF.

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_INVSRCALPHA);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_pTexDOF->GetWidth(), m_pTexDOF->GetHeight(), 0, 0.0f, 10.0f);

	// Setup both texture units

	UtilGL::Texturing::SetCurrentUnit(0);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	m_pTexBlur->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	UtilGL::Texturing::SetCurrentUnit(1);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);

	m_pTexDOF->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// QUAD

	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));

	glBegin(GL_QUADS);

	glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f, 1.0f);
	glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f, 1.0f);
	glVertex3i  (0, 0, -1);

	glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f, 0.0f);
	glVertex3i  (0, m_pTexDOF->GetHeight(), -1);

	glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f, 0.0f);
	glVertex3i  (m_pTexDOF->GetWidth(), m_pTexDOF->GetHeight(), -1);

	glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f, 1.0f);
	glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f, 1.0f);
	glVertex3i  (m_pTexDOF->GetWidth(), 0, -1);

	glEnd();

	// Restore

	UtilGL::Texturing::SetCurrentUnit(1);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);

	UtilGL::Texturing::SetCurrentUnit(0);

	if(bDebug)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		UtilGL::Rendering::DrawCenteredQuad(m_pTexDOF, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
											UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);
	}

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);

	if(bAA)
	{
		glEnable(GL_MULTISAMPLE_ARB);
	}

	return true;
}
