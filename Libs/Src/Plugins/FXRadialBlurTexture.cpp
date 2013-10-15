#include "Plugins.h"
#include "FXRadialBlurTexture.h"


const std::string CFXRadialBlurTexture::CLASSNAME("Radial Blur 1 Texture");

REGISTER_PLUGIN(CPluginFactory, CFXRadialBlurTexture)


CFXRadialBlurTexture::CFXRadialBlurTexture()
{
	DefineVar("Steps",     CVarInt::CLASSNAME);
	DefineVar("Width",     CVarFloat::CLASSNAME);
	DefineVar("Height",    CVarFloat::CLASSNAME);
	DefineVar("X Origin",  CVarFloat::CLASSNAME);
	DefineVar("Y Origin",  CVarFloat::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("Steps"))->SetConstant(10);

	((CVarFloat*)GetVar("Width"   ))->SetConstant(1.2f);
	((CVarFloat*)GetVar("Height"  ))->SetConstant(1.2f);
	((CVarFloat*)GetVar("X Origin" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin" ))->SetConstant(0.5f);

	m_pResTexture = NULL;
	m_pTexture    = NULL;
}

CFXRadialBlurTexture::~CFXRadialBlurTexture()
{
	Free();
}

void CFXRadialBlurTexture::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(m_pTexture)
	{
		pDemo->SetViewport(0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXRadialBlurTexture::LoadData(CResourceList* pResourceList)
{
	assert(pResourceList);

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);
	m_pTexture = (UtilGL::Texturing::CTexture2D*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();

	return true;
}

bool CFXRadialBlurTexture::Free()
{
	m_pTexture = NULL;

	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
		return false;
	}

	return true;
}

bool CFXRadialBlurTexture::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pTexture) return false;

	// Framebuffer to texture

	m_pTexture->CopyFromFramebuffer(0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());
	m_pTexture->SetActive();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Evaluate vars

	CVarInt::CValueInt     valueSteps;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueOriginX;
	CVarFloat::CValueFloat valueOriginY;

	EvaluateVar("Steps",    fEffectTime, &valueSteps);
	EvaluateVar("Width",    fEffectTime, &valueWidth);
	EvaluateVar("Height",   fEffectTime, &valueHeight);
	EvaluateVar("X Origin", fEffectTime, &valueOriginX);
	EvaluateVar("Y Origin", fEffectTime, &valueOriginY);

	// Compute blur direction

	CVector3 v3Vector;
	v3Vector.Build(CVector3(valueOriginX.GetValue(), valueOriginY.GetValue(), 0.0f), CVector3(0.5f, 0.5f, 0.0f));

	v3Vector.SetX(v3Vector.X() * (valueWidth.GetValue()  - 1.0f));
	v3Vector.SetY(v3Vector.Y() * (valueHeight.GetValue() - 1.0f));

	// Draw quads

	int   nSteps = valueSteps.GetValue();
	float fAlpha = 1.0f / nSteps;

	for(int i = 0; i < nSteps; i++)
	{
		float fT       = (float)i / (nSteps == 1 ? 1 : nSteps - 1);
		float fWidth   = ComputeLERP(1.0f, valueWidth.GetValue(),  fT);
		float fHeight  = ComputeLERP(1.0f, valueHeight.GetValue(), fT);
		float fCenterX = 0.5f + (v3Vector * fT).X();
		float fCenterY = 0.5f + (v3Vector * fT).Y();

		UtilGL::Rendering::DrawCenteredQuad(m_pTexture,
											fCenterX, fCenterY,
											fWidth, fHeight,
											0.0f, CVector4(1.0f, 1.0f, 1.0f, fAlpha),
											UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_ONE);
	}

	m_pTexture->CopyFromFramebuffer(0, 0, 0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}
