#include "Plugins.h"
#include "FXTelevision.h"


const std::string CFXTelevision::CLASSNAME("Television");

REGISTER_PLUGIN(CPluginFactory, CFXTelevision)


CFXTelevision::CFXTelevision()
{
	DefineVar("X Resolution", CVarInt::CLASSNAME);
	DefineVar("Y Resolution", CVarInt::CLASSNAME);
	DefineVar("Alpha",        CVarFloat::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("X Resolution"))->SetConstant(512);
	((CVarFloat*)GetVar("Y Resolution"))->SetConstant(256);
	((CVarFloat*)GetVar("X Resolution"))->LockConstant(true);
	((CVarFloat*)GetVar("Y Resolution"))->LockConstant(true);
	((CVarFloat*)GetVar("Alpha" ))->SetConstant(1.0f);

	GetVar("X Resolution")->ReloadFXOnChange(this);
	GetVar("Y Resolution")->ReloadFXOnChange(this);

	m_nResX = m_nResY = 0;
	m_pucOwnTexture = NULL;
}

CFXTelevision::~CFXTelevision()
{
	Free();
}

bool CFXTelevision::LoadData(CResourceList* pResourceList)
{
	// Force width and height to be powers of two. log2(x) = log10(x) / log10(2)

	CVarInt::CValueInt valueInt;

	EvaluateVar("X Resolution",  0.0f, &valueInt);
	if(valueInt.GetValue() < 1) valueInt.SetValue(1);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	EvaluateVar("Y Resolution", 0.0f, &valueInt);
	if(valueInt.GetValue() < 1) valueInt.SetValue(1);
	int nHeightPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	m_nResX = pow(2, nWidthPwr);
	m_nResY = pow(2, nHeightPwr);

	// Create texture

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();
	texOptions.eFilter = UtilGL::Texturing::FILTER_POINT;

	m_texture.LoadFlat(m_nResX, m_nResY, CVector4(0.0f, 0.0f, 0.0f, 1.0f), false, false, &texOptions);

	SAFE_DELETE_A(m_pucOwnTexture);
	m_pucOwnTexture = new unsigned char[m_nResX * m_nResY * 4];

	return true;
}

bool CFXTelevision::Free()
{
	m_texture.Free();
	m_nResX = m_nResY = 0;
	SAFE_DELETE_A(m_pucOwnTexture);
	return true;
}

bool CFXTelevision::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	int nCount;

	// Do the television effect

	float fTime = fDemoTime;

	srand(fTime * 1000.0f);

	for(nCount = 0; nCount < m_nResX * m_nResY; nCount++)
	{
		int  nCount_4 = nCount * 4;
		char therand  = rand();

		m_pucOwnTexture[nCount_4 + 0] = therand;
		m_pucOwnTexture[nCount_4 + 1] = therand;
		m_pucOwnTexture[nCount_4 + 2] = therand;
		m_pucOwnTexture[nCount_4 + 3] = 255;
	}

	// Tune lines


	float fHeight1 = 0.3f;
	float fHeight2 = 0.6f;
	int   nHeight1 = 0.3f * m_nResY;
	int   nHeight2 = 0.6f * m_nResY;
	int   nSpeed1  = 220;
	int   nSpeed2  = 2630;

	int   nYPos1   = nSpeed1 - ((int)(fTime * 1000.0f) % nSpeed1);
	int   nYPos2   = nSpeed2 - ((int)(fTime * 1000.0f) % nSpeed2);
	int   nY1_2    = (((float)nYPos1 / nSpeed1) * (1.0f + fHeight1)) * m_nResY;
	int   nY2_2    = (((float)nYPos2 / nSpeed2) * (1.0f + fHeight2)) * m_nResY;
	int   nY1_1    = nY1_2 - nHeight1;
	int   nY2_1    = nY2_2 - nHeight2;

	if(nY1_1 < 0) nY1_1 = 0; if(nY1_1 > m_nResY) nY1_1 = m_nResY;
	if(nY1_2 < 0) nY1_2 = 0; if(nY1_2 > m_nResY) nY1_2 = m_nResY;
	if(nY2_1 < 0) nY2_1 = 0; if(nY2_1 > m_nResY) nY2_1 = m_nResY;
	if(nY2_2 < 0) nY2_2 = 0; if(nY2_2 > m_nResY) nY2_2 = m_nResY;
	
	// 1

	int n1, n2;

	n1 = m_nResX * nY1_1;
	n2 = m_nResX * nY1_2;

	for(nCount = n1; nCount < n2; nCount++)
	{
		int nCount_4 = nCount * 4;

		m_pucOwnTexture[nCount_4 + 0] >>= 2;
		m_pucOwnTexture[nCount_4 + 1] >>= 2;
		m_pucOwnTexture[nCount_4 + 2] >>= 2;
	}

	n1 = m_nResX * nY2_1;
	n2 = m_nResX * nY2_2;

	for(nCount = n1; nCount < n2; nCount++)
	{
		int nCount_4 = nCount * 4;

		m_pucOwnTexture[nCount_4 + 0] >>= 1;
		m_pucOwnTexture[nCount_4 + 1] >>= 1;
		m_pucOwnTexture[nCount_4 + 2] >>= 1;
	}

	m_texture.CopyFromBuffer(0, 0, m_nResX, m_nResY, GL_RGBA, GL_UNSIGNED_BYTE, m_pucOwnTexture);

	// Draw everything

	CVarFloat::CValueFloat valueAlpha;
	EvaluateVar("Alpha", fEffectTime, &valueAlpha);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);

	return UtilGL::Rendering::DrawCenteredQuad(	&m_texture,
												0.5f, 0.5f, 1.0f, 1.0f,
												0.0f, v4Color,
												UtilGL::States::BLEND_SRCALPHA, UtilGL::States::BLEND_INVSRCALPHA);
}