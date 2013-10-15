#include "Plugins.h"
#include "FXRenderToTexture.h"


const std::string CFXRenderToTexture::CLASSNAME("Render To Texture");

REGISTER_PLUGIN(CPluginFactory, CFXRenderToTexture)


CFXRenderToTexture::CFXRenderToTexture()
{
	DefineResourceReference("Texture Target", CResourceTexture2D::CLASSNAME);

	m_pResTexture = NULL;
}

CFXRenderToTexture::~CFXRenderToTexture()
{
	Free();
}

void CFXRenderToTexture::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
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

bool CFXRenderToTexture::LoadData(CResourceList* pResourceList)
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

bool CFXRenderToTexture::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

bool CFXRenderToTexture::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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

	// Remove mipmapping filter from texture

	pTexture->SetActive();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Framebuffer to texture
	pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}
