#include "Plugins.h"
#include "FXBoxBlurTexture.h"


const std::string CFXBoxBlurTexture::CLASSNAME("Box Blur Texture");

REGISTER_PLUGIN(CPluginFactory, CFXBoxBlurTexture)


CFXBoxBlurTexture::CFXBoxBlurTexture()
{
	DefineVar("Blur Radius (Pw2)",  CVarInt::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("Blur Radius (Pw2)" ))->SetConstant(16);

	m_pResTexture = NULL;
	m_pTexture    = NULL;
}

CFXBoxBlurTexture::~CFXBoxBlurTexture()
{
	Free();
}

void CFXBoxBlurTexture::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	if(m_pTexture)
	{
		pDemo->SetViewport(0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), pDemo->ScissorActive());
	}
}

bool CFXBoxBlurTexture::LoadData(CResourceList* pResourceList)
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

bool CFXBoxBlurTexture::Free()
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

bool CFXBoxBlurTexture::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pTexture) return false;

	// Evaluate vars

	CVarInt::CValueInt valueRadius;

	EvaluateVar("Blur Radius (Pw2)", fEffectTime, &valueRadius);

	if(valueRadius.GetValue() < 1) valueRadius.SetValue(1);

	int nRadiusPwr = MYROUND(log10f(valueRadius.GetValue()) / log10f(2));
	int nRadius    = pow(2, nRadiusPwr);

	if(nRadius < 1) nRadius = 1;

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
	glOrtho(0, m_pTexture->GetWidth(), m_pTexture->GetHeight(), 0, 0.0f, 10.0f);

	m_pTexture->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Blur loop

	UtilGL::Rendering::ComputeBoxBlur(nRadius, m_pTexture);

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	
	return true;
}
