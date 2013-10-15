#include "Plugins.h"
#include "FXDistortedBitmap.h"


const std::string CFXDistortedBitmap::CLASSNAME("Distorted Bitmap");

REGISTER_PLUGIN(CPluginFactory, CFXDistortedBitmap)


CFXDistortedBitmap::CFXDistortedBitmap()
{
	DefineVar("Alpha",		CVarFloat::CLASSNAME);
	DefineVar("X Position", CVarFloat::CLASSNAME);
	DefineVar("Y Position", CVarFloat::CLASSNAME);
	DefineVar("Width",      CVarFloat::CLASSNAME);
	DefineVar("Height",     CVarFloat::CLASSNAME);
	DefineVar("Scale",      CVarFloat::CLASSNAME);
	DefineVar("Angle",      CVarFloat::CLASSNAME);
	DefineVar("Blend Mode", CVarCombo::CLASSNAME);
	DefineVar("Filtering",  CVarCombo::CLASSNAME);

	DefineVar("Mesh X Res",    CVarInt::CLASSNAME);
	DefineVar("Mesh Y Res",    CVarInt::CLASSNAME);
	DefineVar("Alpha Range",   CVarFloat::CLASSNAME);
	DefineVar("Distort Range", CVarFloat::CLASSNAME);
	DefineVar("Distort Value", CVarFloat::CLASSNAME);
	DefineVar("Frequency",     CVarFloat::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Alpha"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("X Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Position"))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Width" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Height"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Scale" ))->SetConstant(1.0f);
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");
	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	((CVarInt*)GetVar("Mesh X Res"    ))->SetConstant(20);
	((CVarInt*)GetVar("Mesh Y Res"    ))->SetConstant(15);
	((CVarInt*)GetVar("Mesh X Res"    ))->LockConstant(true);
	((CVarInt*)GetVar("Mesh Y Res"    ))->LockConstant(true);
	((CVarFloat*)GetVar("Alpha Range"   ))->SetConstant(0.4f);
	((CVarFloat*)GetVar("Distort Range" ))->SetConstant(0.2f);
	((CVarFloat*)GetVar("Distort Value" ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Frequency"     ))->SetConstant(10.0f);

	GetVar("Mesh X Res")->ReloadFXOnChange(this);
	GetVar("Mesh Y Res")->ReloadFXOnChange(this);

	m_fLastDistort = 0.0f;
	m_fAlpha       = 1.0f;
	m_fDistort     = 0.0f;

	m_nWidth  = 0;
	m_nHeight = 0;

	m_pVertices = NULL;
	m_pMapVerts = NULL;

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);
	m_pResTexture = NULL;
}

CFXDistortedBitmap::~CFXDistortedBitmap()
{
	Free();
}

bool CFXDistortedBitmap::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	// Create mesh

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

			m_pVertices[nIndex].Set(u - 0.5f, v - 0.5f, -0.5f);
			m_pMapVerts[nIndex].Set(u, 1.0f - v);

			nIndex++;
		}
	}

	return true;
}

bool CFXDistortedBitmap::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	m_fLastDistort = 0.0f;

	SAFE_DELETE_A(m_pVertices);
	SAFE_DELETE_A(m_pMapVerts);

	m_nWidth  = 0;
	m_nHeight = 0;

	return true;
}

bool CFXDistortedBitmap::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

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
	else
	{
		pTexture->SetActive();
	}

	// Get Params

	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueScale;
	CVarFloat::CValueFloat valueAngle;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueFiltering;

	CVarFloat::CValueFloat valueAlphaRange;
	CVarFloat::CValueFloat valueDistortRange;
	CVarFloat::CValueFloat valueDistortValue;
	CVarFloat::CValueFloat valueFrequency;

	EvaluateVar("Alpha",	  fEffectTime, &valueAlpha);
	EvaluateVar("X Position", fEffectTime, &valuePosX);
	EvaluateVar("Y Position", fEffectTime, &valuePosY);
	EvaluateVar("Width",      fEffectTime, &valueWidth);
	EvaluateVar("Height",     fEffectTime, &valueHeight);
	EvaluateVar("Scale",      fEffectTime, &valueScale);
	EvaluateVar("Angle",      fEffectTime, &valueAngle);
	EvaluateVar("Blend Mode", fEffectTime, &valueBlendMode);
	EvaluateVar("Filtering",  fEffectTime, &valueFiltering);

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
		m_fAlpha  = 1.0f - ComputeRand(0.0f, valueAlphaRange.GetValue()) * valueDistortValue.GetValue();
		m_fAlpha *= valueAlpha.GetValue();

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

	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, m_fAlpha));

	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	// Change filter mode

	GLint minFilter, magFilter;

	if(pTexture)
	{
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

	CVarFloat* pVarAngle = reinterpret_cast<CVarFloat*>(GetVar("Angle"));
	assert(pVarAngle);

	float fAngle = valueAngle.GetValue();

	if(IS_ZERO(fAngle) && !pVarAngle->IsConstant())
	{
		// Avoid aspect ratio problem (see UtilGL::Rendering::DrawRect())
		fAngle = fAngle < 0.0f ? fAngle - (ALMOST_ZERO * 2.0f) : fAngle + (ALMOST_ZERO * 2.0f);
	}

	int nSrcBlend = UtilGL::States::BLEND_SRCALPHA;
	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	if(ARE_EQUAL(m_fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !pTexture->HasAlpha())
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

	// DRAW

	float fX = valuePosX.GetValue();
	float fY = valuePosY.GetValue();

	glBegin(GL_QUADS);

	nIndex = 0;

	for(y = 0; y < m_nHeight; y++)
	{
		for(x = 0; x < m_nWidth; x++)
		{
			CVector3 v1, v2, v3, v4;

			v1 = m_pVertices[nIndex];
			v2 = m_pVertices[nIndex + m_nWidth + 1];
			v3 = m_pVertices[nIndex + m_nWidth + 2];
			v4 = m_pVertices[nIndex + 1];

			CMatrix mtxTransform;

			mtxTransform.Scale( valueWidth.GetValue()  * valueScale.GetValue(),
								valueHeight.GetValue() * valueScale.GetValue(),
								1.0f);

			if(!IS_ZERO(fAngle))
			{
				float fAspect  = pDemo->GetAspect();
				mtxTransform.RotateZ(fAngle);
				mtxTransform.Scale(1.0f, fAspect, 1.0f);
			}

			float fCenterX = valuePosX.GetValue();
			float fCenterY = valuePosY.GetValue();
			mtxTransform.Translate(fCenterX, fCenterY, 0.0f);

			mtxTransform.Transform(&v1);
			mtxTransform.Transform(&v2);
			mtxTransform.Transform(&v3);
			mtxTransform.Transform(&v4);

			glTexCoord2fv(m_pMapVerts[nIndex].Data());
			glVertex3fv  (v1.Data());

			glTexCoord2fv(m_pMapVerts[nIndex + m_nWidth + 1].Data());
			glVertex3fv  (v2.Data());

			glTexCoord2fv(m_pMapVerts[nIndex + m_nWidth + 2].Data());
			glVertex3fv  (v3.Data());

			glTexCoord2fv(m_pMapVerts[nIndex + 1].Data());
			glVertex3fv  (v4.Data());

			nIndex++;
		}

		nIndex++;
	}

	glEnd();

	// Restore filtering

	if(pTexture)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	}

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);

	return true;
}