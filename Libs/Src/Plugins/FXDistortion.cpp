#include "Plugins.h"
#include "FXDistortion.h"


const std::string CFXDistortion::CLASSNAME("Standard Distortion");

REGISTER_PLUGIN(CPluginFactory, CFXDistortion)


CFXDistortion::CFXDistortion()
{
	DefineVar("X Resolution",  CVarInt::CLASSNAME);
	DefineVar("Y Resolution",  CVarInt::CLASSNAME);
	DefineVar("Mesh X Res",    CVarInt::CLASSNAME);
	DefineVar("Mesh Y Res",    CVarInt::CLASSNAME);
	DefineVar("Alpha Range",   CVarFloat::CLASSNAME);
	DefineVar("Distort Range", CVarFloat::CLASSNAME);
	DefineVar("Distort Value", CVarFloat::CLASSNAME);
	DefineVar("Frequency",     CVarFloat::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarInt*)GetVar("X Resolution"  ))->SetConstant(512);
	((CVarInt*)GetVar("Y Resolution"  ))->SetConstant(256);
	((CVarInt*)GetVar("X Resolution"  ))->LockConstant(true);
	((CVarInt*)GetVar("Y Resolution"  ))->LockConstant(true);
	((CVarInt*)GetVar("Mesh X Res"    ))->SetConstant(20);
	((CVarInt*)GetVar("Mesh Y Res"    ))->SetConstant(15);
	((CVarInt*)GetVar("Mesh X Res"    ))->LockConstant(true);
	((CVarInt*)GetVar("Mesh Y Res"    ))->LockConstant(true);
	((CVarFloat*)GetVar("Alpha Range"   ))->SetConstant(0.4f);
	((CVarFloat*)GetVar("Distort Range" ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Distort Value" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Frequency"     ))->SetConstant(10.0f);

	GetVar("X Resolution")->ReloadFXOnChange(this);
	GetVar("Y Resolution")->ReloadFXOnChange(this);
	GetVar("Mesh X Res"  )->ReloadFXOnChange(this);
	GetVar("Mesh Y Res"  )->ReloadFXOnChange(this);

	m_fLastDistort = 0.0f;
	m_fAlpha       = 1.0f;
	m_fDistort     = 0.0f;

	m_nWidth  = 0;
	m_nHeight = 0;

	m_pVertices = NULL;
	m_pMapVerts = NULL;
}

CFXDistortion::~CFXDistortion()
{
	Free();
}

void CFXDistortion::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_texture.GetWidth(), m_texture.GetHeight(), pDemo->ScissorActive());
}

bool CFXDistortion::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("X Resolution",  0.0f, &valueInt);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	EvaluateVar("Y Resolution", 0.0f, &valueInt);
	int nHeightPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	int nResX = pow(2, nWidthPwr);
	int nResY = pow(2, nHeightPwr);

	// Create texture

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();
	texOptions.eFilter = UtilGL::Texturing::FILTER_POINT;

	m_texture.LoadFlat(nResX, nResY, CVector4(0.0f, 0.0f, 0.0f, 1.0f), false, false, &texOptions);

	CVarInt::CValueInt valueMeshWidth;
	CVarInt::CValueInt valueMeshHeight;

	EvaluateVar("Mesh X Res",  0.0f, &valueMeshWidth);
	EvaluateVar("Mesh Y Res",  0.0f, &valueMeshHeight);

	m_nWidth  = valueMeshWidth.GetValue();
	m_nHeight = valueMeshHeight.GetValue();

	int nIndex = 0;
	int nNumVerts = (m_nWidth + 1) * (m_nHeight + 1);

	m_pVertices = new CVector3[nNumVerts];
	m_pMapVerts = new CVector2[nNumVerts];

	assert(m_pVertices);
	assert(m_pMapVerts);

	for(int y = 0; y < m_nHeight + 1; y++)
	{
		for(int x = 0; x < m_nWidth + 1; x++)
		{
			float u, v;
 
			u = (float)x / (float)m_nWidth;
			v = (float)y / (float)m_nHeight;

			m_pVertices[nIndex].Set(u, v, -0.5f);
			m_pMapVerts[nIndex].Set(u, 1.0f - v);

			nIndex++;
		}
	}

	return true;
}

bool CFXDistortion::Free()
{
	m_texture.Free();
	m_fLastDistort = 0.0f;

	SAFE_DELETE_A(m_pVertices);
	SAFE_DELETE_A(m_pMapVerts);

	m_nWidth  = 0;
	m_nHeight = 0;

	return true;
}

bool CFXDistortion::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	// Render to texture
	m_texture.CopyFromFramebuffer(0, 0, 0, 0, m_texture.GetWidth(), m_texture.GetHeight());

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Get Params

	CVarFloat::CValueFloat valueAlphaRange;
	CVarFloat::CValueFloat valueDistortRange;
	CVarFloat::CValueFloat valueDistortValue;
	CVarFloat::CValueFloat valueFrequency;

	EvaluateVar("Alpha Range",   fEffectTime, &valueAlphaRange);
	EvaluateVar("Distort Range", fEffectTime, &valueDistortRange);
	EvaluateVar("Distort Value", fEffectTime, &valueDistortValue);
	EvaluateVar("Frequency",     fEffectTime, &valueFrequency);

	int nIndex, x, y;

	// Compute distortion

	float fPeriod = IS_ZERO(valueFrequency.GetValue()) ? FLOAT_MAX : 1.0f / valueFrequency.GetValue();

	srand(fEffectTime * 1000.0f);

	if(MYFABSF(fEffectTime - m_fLastDistort) > fPeriod)
	{
		m_fAlpha = 1.0f - ComputeRand(0.0f, valueAlphaRange.GetValue()) * valueDistortValue.GetValue();

		if(m_fAlpha < ALMOST_ZERO)
		{
			return true;
		}
		else if(m_fAlpha > 1.0f) m_fAlpha = 1.0f;

		m_fDistort     = ComputeRand(0.0f, valueDistortRange.GetValue()) * valueDistortValue.GetValue();
		m_fLastDistort = fEffectTime;

		nIndex = 0;

		float fXAmp = m_fDistort * 0.5f;
		float fYAmp = m_fDistort * 0.5f;

		for(y = 0; y < m_nHeight + 1; y++)
		{
			for(x = 0; x < m_nWidth + 1; x++)
			{
				float u, v;
 
				u = (float)x / m_nWidth  + ComputeRand(-fXAmp, fXAmp);
				v = (float)y / m_nHeight + ComputeRand(-fYAmp, fYAmp);
				m_pMapVerts[nIndex].Set(u, 1.0f - v);

				nIndex++;
			}
		}
	}

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_ZERO);

	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, m_fAlpha));

	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	m_texture.SetActive();

	glBegin(GL_QUADS);

	nIndex = 0;

	for(y = 0; y < m_nHeight; y++)
	{
		for(x = 0; x < m_nWidth; x++)
		{
			glTexCoord2fv(m_pMapVerts[nIndex               ].Data());
			glVertex3fv  (m_pVertices[nIndex               ].Data());

			glTexCoord2fv(m_pMapVerts[nIndex + m_nWidth + 1].Data());
			glVertex3fv  (m_pVertices[nIndex + m_nWidth + 1].Data());

			glTexCoord2fv(m_pMapVerts[nIndex + m_nWidth + 2].Data());
			glVertex3fv  (m_pVertices[nIndex + m_nWidth + 2].Data());

			glTexCoord2fv(m_pMapVerts[nIndex + 1           ].Data());
			glVertex3fv  (m_pVertices[nIndex + 1           ].Data());

			nIndex++;
		}

		nIndex++;
	}

	glEnd();

	UtilGL::States::Set(UtilGL::States::ZBUFFER,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);

	return true;
}