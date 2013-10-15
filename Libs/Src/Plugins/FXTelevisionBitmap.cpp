#include "Plugins.h"
#include "FXTelevisionBitmap.h"


const std::string CFXTelevisionBitmap::CLASSNAME("Television Bitmap");

REGISTER_PLUGIN(CPluginFactory, CFXTelevisionBitmap)


CFXTelevisionBitmap::CFXTelevisionBitmap()
{
	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	m_pResTexture   = NULL;
	m_pTexture      = NULL;
	m_pucOwnTexture = NULL;
}

CFXTelevisionBitmap::~CFXTelevisionBitmap()
{
	Free();
}

bool CFXTelevisionBitmap::LoadData(CResourceList* pResourceList)
{
	assert(pResourceList);

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);
	m_pTexture = (UtilGL::Texturing::CTexture2D*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();

	SAFE_DELETE_A(m_pucOwnTexture);
	m_pucOwnTexture = new unsigned char[m_pTexture->GetWidth() * m_pTexture->GetHeight() * 4];

	return true;
}

bool CFXTelevisionBitmap::Free()
{
	m_pTexture = NULL;

	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
		return false;
	}

	SAFE_DELETE_A(m_pucOwnTexture);
	return true;
}

bool CFXTelevisionBitmap::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pTexture) return false;

	int nCount;

	// Do the television effect

	float fTime = fDemoTime;

	srand(fTime * 1000.0f);

	int nResX = m_pTexture->GetWidth();
	int nResY = m_pTexture->GetHeight();

	for(nCount = 0; nCount < nResX * nResY; nCount++)
	{
		int  nCount_4 = nCount * 4;
		char therand  = rand();
                therand |= (1 << 7) | (1 << 6); // start from 128, brighter.

		m_pucOwnTexture[nCount_4 + 0] = therand;
		m_pucOwnTexture[nCount_4 + 1] = therand;
		m_pucOwnTexture[nCount_4 + 2] = therand;
		m_pucOwnTexture[nCount_4 + 3] = 255;
	}

	// Tune lines

	float fHeight1 = 0.3f;
	float fHeight2 = 0.6f;
	int   nHeight1 = 0.3f * nResY;
	int   nHeight2 = 0.6f * nResY;
	int   nSpeed1  = 220;
	int   nSpeed2  = 2630;

	int   nYPos1   = nSpeed1 - ((int)(fTime * 1000.0f) % nSpeed1);
	int   nYPos2   = nSpeed2 - ((int)(fTime * 1000.0f) % nSpeed2);
	int   nY1_2    = (((float)nYPos1 / nSpeed1) * (1.0f + fHeight1)) * nResY;
	int   nY2_2    = (((float)nYPos2 / nSpeed2) * (1.0f + fHeight2)) * nResY;
	int   nY1_1    = nY1_2 - nHeight1;
	int   nY2_1    = nY2_2 - nHeight2;

	if(nY1_1 < 0) nY1_1 = 0; if(nY1_1 > nResY) nY1_1 = nResY;
	if(nY1_2 < 0) nY1_2 = 0; if(nY1_2 > nResY) nY1_2 = nResY;
	if(nY2_1 < 0) nY2_1 = 0; if(nY2_1 > nResY) nY2_1 = nResY;
	if(nY2_2 < 0) nY2_2 = 0; if(nY2_2 > nResY) nY2_2 = nResY;

	// 1

	int n1, n2;

	n1 = nResX * nY1_1;
	n2 = nResX * nY1_2;

	for(nCount = n1; nCount < n2; nCount++)
	{
		int  nCount_4 = nCount * 4;
		unsigned char ucValue = m_pucOwnTexture[nCount_4 + 0];

		if(ucValue < 60) ucValue = 0;
		else ucValue -= 60;

		m_pucOwnTexture[nCount_4 + 0] = ucValue;
		m_pucOwnTexture[nCount_4 + 1] = ucValue;
		m_pucOwnTexture[nCount_4 + 2] = ucValue;
	}

	n1 = nResX * nY2_1;
	n2 = nResX * nY2_2;

	for(nCount = n1; nCount < n2; nCount++)
	{
		int nCount_4 = nCount * 4;
		unsigned char ucValue = m_pucOwnTexture[nCount_4 + 0];

        if(ucValue < 60) ucValue = 0;
        else ucValue -= 60;

		m_pucOwnTexture[nCount_4 + 0] = ucValue;
		m_pucOwnTexture[nCount_4 + 1] = ucValue;
		m_pucOwnTexture[nCount_4 + 2] = ucValue;
	}

	return m_pTexture->CopyFromBuffer(0, 0, nResX, nResY, GL_RGBA, GL_UNSIGNED_BYTE, m_pucOwnTexture);
}