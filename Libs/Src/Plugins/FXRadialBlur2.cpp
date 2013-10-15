#include "Plugins.h"
#include "FXRadialBlur2.h"


const std::string CFXRadialBlur2::CLASSNAME("Radial Blur 2");

REGISTER_PLUGIN(CPluginFactory, CFXRadialBlur2)


CFXRadialBlur2::CFXRadialBlur2()
{
	DefineVar("BlurTexture Width",   CVarInt::CLASSNAME);
	DefineVar("BlurTexture Height",  CVarInt::CLASSNAME);
	DefineVar("Blur Radius (Pw2)",   CVarInt::CLASSNAME);
	DefineVar("X Origin",            CVarFloat::CLASSNAME);
	DefineVar("Y Origin",            CVarFloat::CLASSNAME);

	// Set default values

	((CVarInt*)GetVar("BlurTexture Width" ))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Width" ))->SetConstant(512);
	((CVarInt*)GetVar("BlurTexture Height"))->LockConstant(true);
	((CVarInt*)GetVar("BlurTexture Height"))->SetConstant(512);

	((CVarInt*)GetVar("Blur Radius (Pw2)"))->SetConstant(8);

	((CVarFloat*)GetVar("X Origin" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Origin" ))->SetConstant(0.5f);

	GetVar("BlurTexture Width" )->ReloadFXOnChange(this);
	GetVar("BlurTexture Height")->ReloadFXOnChange(this);
}

CFXRadialBlur2::~CFXRadialBlur2()
{
	Free();
}

void CFXRadialBlur2::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_texture.GetWidth(), m_texture.GetHeight(), pDemo->ScissorActive());
}

bool CFXRadialBlur2::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("BlurTexture Width",  0.0f, &valueInt);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2.0f));

	EvaluateVar("BlurTexture Height", 0.0f, &valueInt);
	int nHeightPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2.0f));

	int nWidth  = pow(2, nWidthPwr);
	int nHeight = pow(2, nHeightPwr);

	// Create texture

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();
	texOptions.eFilter = UtilGL::Texturing::FILTER_LINEAR;

	m_texture.LoadFlat(nWidth, nHeight, CVector4(0.0f, 0.0f, 0.0f, 1.0f), false, false, &texOptions);

	return true;
}

bool CFXRadialBlur2::Free()
{
	m_texture.Free();

	return true;
}

bool CFXRadialBlur2::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Evaluate vars

	CVarInt::CValueInt     valueRadius;
	CVarFloat::CValueFloat valueOriginX;
	CVarFloat::CValueFloat valueOriginY;

	EvaluateVar("Blur Radius (Pw2)", fEffectTime, &valueRadius);
	EvaluateVar("X Origin",			 fEffectTime, &valueOriginX);
	EvaluateVar("Y Origin",			 fEffectTime, &valueOriginY);

	if(valueRadius.GetValue() < 1) valueRadius.SetValue(1);

	int nRadiusPwr = MYROUND(log10f(valueRadius.GetValue()) / log10f(2));
	int nRadius    = pow(2, nRadiusPwr);

	if(nRadius < 1) nRadius = 1;

	// Compute blur direction

	CVector3 v3Vector;
	v3Vector.Build(CVector3(valueOriginX.GetValue(), valueOriginY.GetValue(), 0.0f), CVector3(0.5f, 0.5f, 0.0f));

	v3Vector = v3Vector * ((float)nRadius / (float)m_texture.GetWidth()) * 2.0f;

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
	glOrtho(0, m_texture.GetWidth(), m_texture.GetHeight(), 0, 0.0f, 10.0f);

	m_texture.SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Blur loop

	int   nCurRadius = 1;
	float fAlpha     = 0.5f;//1.0f / (log10f(nRadius) / log10f(2.0f));

	while(nRadius >= nCurRadius)
	{
		float fPos     = (float)nCurRadius / (float)nRadius;
		float fCenterX = 0.5f + (v3Vector * fPos).X();
		float fCenterY = 0.5f + (v3Vector * fPos).Y();

		m_texture.CopyFromFramebuffer(0, 0, 0, 0, m_texture.GetWidth(), m_texture.GetHeight());
		DrawQuad(fCenterX * m_texture.GetWidth(), fCenterY * m_texture.GetHeight(), nCurRadius, fAlpha);

		nCurRadius = nCurRadius * 2;
	}

	// Restore demo viewport

	pDemo->SetDemoViewport();

	// Clear framebuffer

	m_texture.CopyFromFramebuffer(0, 0, 0, 0, m_texture.GetWidth(), m_texture.GetHeight());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Draw final blur

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);

	DrawQuad(m_texture.GetWidth() / 2, m_texture.GetHeight() / 2, 0, 1.0f);

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	
	return true;
}

bool CFXRadialBlur2::DrawQuad(float fCenterX, float fCenterY, float fRadius, float fAlpha)
{
	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, fAlpha));

	// Compute some parameters

	float fHalfWidth  = m_texture.GetWidth()  / 2;
	float fHalfHeight = m_texture.GetHeight() / 2;

	float fLeft   = fCenterX - fHalfWidth  - fRadius;
	float fTop    = fCenterY - fHalfHeight - fRadius;
	float fRight  = fCenterX + fHalfWidth  + fRadius;
	float fBottom = fCenterY + fHalfHeight + fRadius;

	float fTexOffsetX = (1.0f / m_texture.GetWidth()) * 0.5f; // Texels are sampled from center
	float fTexOffsetY = (1.0f / m_texture.GetWidth()) * 0.5f;

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