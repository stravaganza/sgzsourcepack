#include "Plugins.h"
#include "FXRTTRadialHilights.h"


const std::string CFXRTTRadialHilights::CLASSNAME("Render Radial Hilights");

REGISTER_PLUGIN(CPluginFactory, CFXRTTRadialHilights)


CFXRTTRadialHilights::CFXRTTRadialHilights()
{
	DefineVar("Blur Radius (Pw2)", CVarInt::CLASSNAME);
	DefineVar("Intensity Cut",     CVarFloat::CLASSNAME);
	DefineVar("Post Multiply",     CVarInt::CLASSNAME);
	DefineVar("X Origin",          CVarFloat::CLASSNAME);
	DefineVar("Y Origin",          CVarFloat::CLASSNAME);

	DefineResourceReference("Texture Target", CResourceTexture2D::CLASSNAME);

	((CVarInt*)  GetVar("Blur Radius (Pw2)" ))->SetConstant(4);
	((CVarFloat*)GetVar("Intensity Cut"     ))->SetConstant(0.8f);
	((CVarInt*)  GetVar("Post Multiply"     ))->SetConstant(4);
	((CVarFloat*)GetVar("X Origin"          ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin"          ))->SetConstant(0.5f);

	m_pResTexture = NULL;
}

CFXRTTRadialHilights::~CFXRTTRadialHilights()
{
	Free();
}

void CFXRTTRadialHilights::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture) return;

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(pTexture)
	{
		pDemo->SetViewport(0, 0, pTexture->GetWidth(), pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXRTTRadialHilights::LoadData(CResourceList* pResourceList)
{
	m_pResTexture = FindResource(pResourceList, "Texture Target", CResourceTexture2D::CLASSNAME);

	if(!m_pResTexture)
	{
		FXLoadError("Can't find Texture Target resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXRTTRadialHilights::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

bool CFXRTTRadialHilights::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not available");
		pDemo->SetDemoViewport();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("No texture target available");
		pDemo->SetDemoViewport();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return false;
	}

	// Evaluate vars

	CVarInt::CValueInt     valueRadius;
	CVarFloat::CValueFloat valueIntensityCut;
	CVarInt::CValueInt     valuePostMultiply;
	CVarFloat::CValueFloat valueOriginX;
	CVarFloat::CValueFloat valueOriginY;

	EvaluateVar("Blur Radius (Pw2)", fEffectTime, &valueRadius);
	EvaluateVar("Intensity Cut",     fEffectTime, &valueIntensityCut);
	EvaluateVar("Post Multiply",     fEffectTime, &valuePostMultiply);
	EvaluateVar("X Origin",			 fEffectTime, &valueOriginX);
	EvaluateVar("Y Origin",			 fEffectTime, &valueOriginY);

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

	if(valueRadius.GetValue() < 1) valueRadius.SetValue(1);

	int nRadiusPwr = MYROUND(log10f(valueRadius.GetValue()) / log10f(2));
	int nRadius    = pow(2, nRadiusPwr);

	// Compute blur direction

	CVector3 v3Vector;
	v3Vector.Build(CVector3(valueOriginX.GetValue(), valueOriginY.GetValue(), 0.0f), CVector3(0.5f, 0.5f, 0.0f));

	v3Vector = v3Vector * ((float)nRadius / (float)pTexture->GetWidth()) * 2.0f;

	if(nRadius > 0)
	{
		// Set States

		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
		UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_INVSRCALPHA);

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, pTexture->GetWidth(), pTexture->GetHeight(), 0, 0.0f, 10.0f);

		pTexture->SetActive();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// Blur loop

		int   nCurRadius = 1;
		float fAlpha     = 0.5f;//1.0f / (log10f(nRadius) / log10f(2.0f));

		while(nRadius >= nCurRadius)
		{
			float fPos = (float)nCurRadius / (float)nRadius;
			float fCenterX = 0.5f + (v3Vector * fPos).X();
			float fCenterY = 0.5f + (v3Vector * fPos).Y();

			pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());
			DrawQuad(pTexture, fCenterX * pTexture->GetWidth(), fCenterY * pTexture->GetHeight(), nCurRadius, fAlpha);

			nCurRadius = nCurRadius * 2;
		}
	}

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	
	return true;
}

bool CFXRTTRadialHilights::DrawQuad(UtilGL::Texturing::CTexture2D* pTexture,
							  float fCenterX, float fCenterY, float fRadius, float fAlpha)
{
	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, fAlpha));

	// Compute some parameters

	float fHalfWidth  = pTexture->GetWidth()  / 2;
	float fHalfHeight = pTexture->GetHeight() / 2;

	float fLeft   = fCenterX - fHalfWidth  - fRadius;
	float fTop    = fCenterY - fHalfHeight - fRadius;
	float fRight  = fCenterX + fHalfWidth  + fRadius;
	float fBottom = fCenterY + fHalfHeight + fRadius;

	float fTexOffsetX = (1.0f / pTexture->GetWidth()) * 0.5f; // Texels are sampled from center
	float fTexOffsetY = (1.0f / pTexture->GetWidth()) * 0.5f;

	// Draw Quad

	glBegin(GL_QUADS);

	glTexCoord2f(fTexOffsetX, 1.0f - fTexOffsetY);
	glVertex3f  (fLeft, fTop, -1);

	glTexCoord2f(fTexOffsetX, fTexOffsetY);
	glVertex3f  (fLeft, fBottom, -1);

	glTexCoord2f(1.0f - fTexOffsetX, fTexOffsetY);
	glVertex3f  (fRight, fBottom, -1);

	glTexCoord2f(1.0f - fTexOffsetX, 1.0f - fTexOffsetY);
	glVertex3f  (fRight, fTop, -1);

	glEnd();

	return true;
}
