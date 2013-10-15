#include "Plugins.h"
#include "FXMosaic.h"


const std::string CFXMosaic::CLASSNAME("Mosaic");

REGISTER_PLUGIN(CPluginFactory, CFXMosaic)


CFXMosaic::CFXMosaic()
{
	DefineVar("X Position",            CVarFloat::CLASSNAME);
	DefineVar("Y Position",            CVarFloat::CLASSNAME);
	DefineVar("Width",                 CVarFloat::CLASSNAME);
	DefineVar("Height",                CVarFloat::CLASSNAME);
	DefineVar("Scale",                 CVarFloat::CLASSNAME);
	DefineVar("Alpha",                 CVarFloat::CLASSNAME);
	DefineVar("Angle",                 CVarFloat::CLASSNAME);
	DefineVar("Blend Mode",            CVarCombo::CLASSNAME);
	DefineVar("Filtering",             CVarCombo::CLASSNAME);
	DefineVar("Tiles X",               CVarInt::CLASSNAME);
	DefineVar("Tiles Y",               CVarInt::CLASSNAME);
	DefineVar("Tile Source Width",     CVarInt::CLASSNAME);
	DefineVar("Tile Source Height",    CVarInt::CLASSNAME);
	DefineVar("Seed",                  CVarInt::CLASSNAME);
	DefineVar("Change Frequency",      CVarFloat::CLASSNAME);
	DefineVar("Change Freq Variation", CVarFloat::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("X Position"           ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Y Position"           ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Width"                ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Height"               ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Scale"                ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Alpha"                ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Angle"                ))->SetConstant(0.0f);
	((CVarInt*  )GetVar("Tiles X"              ))->SetConstant(10);
	((CVarInt*  )GetVar("Tiles X"              ))->LockConstant(true);
	((CVarInt*  )GetVar("Tiles Y"              ))->SetConstant(8);
	((CVarInt*  )GetVar("Tiles Y"              ))->LockConstant(true);
	((CVarInt*  )GetVar("Tile Source Width"    ))->SetConstant(64);
	((CVarInt*  )GetVar("Tile Source Width"    ))->LockConstant(true);
	((CVarInt*  )GetVar("Tile Source Height"   ))->SetConstant(64);
	((CVarInt*  )GetVar("Tile Source Height"   ))->LockConstant(true);
	((CVarInt*  )GetVar("Seed"                 ))->SetConstant(0);
	((CVarInt*  )GetVar("Seed"                 ))->LockConstant(true);
	((CVarFloat*)GetVar("Change Frequency"     ))->SetConstant(5.0f);
	((CVarFloat*)GetVar("Change Freq Variation"))->SetConstant(0.0f);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add");
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Color Mult");

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	GetVar("Tiles X")->ReloadFXOnChange(this);
	GetVar("Tiles Y")->ReloadFXOnChange(this);
	GetVar("Seed"   )->ReloadFXOnChange(this);

	m_pResTexture = NULL;
	m_fLastUpdate = 0.0f;
}

CFXMosaic::~CFXMosaic()
{
	Free();
}

bool CFXMosaic::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	UtilGL::Texturing::CTexture2D* pTexture = (UtilGL::Texturing::CTexture2D*)((CResourceTexture2D*)m_pResTexture)->GetTexture2D();

	CVarInt::CValueInt     valueTilesX;
	CVarInt::CValueInt     valueTilesY;
	CVarInt::CValueInt     valueTileSourceWidth;
	CVarInt::CValueInt     valueTileSourceHeight;
	CVarInt::CValueInt     valueSeed;

	EvaluateVar("Tiles X",               0.0f, &valueTilesX);
	EvaluateVar("Tiles Y",               0.0f, &valueTilesY);
	EvaluateVar("Tile Source Width",     0.0f, &valueTileSourceWidth);
	EvaluateVar("Tile Source Height",    0.0f, &valueTileSourceHeight);
	EvaluateVar("Seed",                  0.0f, &valueSeed);

	int i, j;

	int nTilesX    = valueTilesX.GetValue() < 1 ? 1 : valueTilesX.GetValue();
	int nTilesY    = valueTilesY.GetValue() < 1 ? 1 : valueTilesY.GetValue();
	int nImgTilesX = pTexture && valueTileSourceWidth.GetValue()  > 0 ? pTexture->GetWidth()  / valueTileSourceWidth.GetValue()  : 1;
	int nImgTilesY = pTexture && valueTileSourceHeight.GetValue() > 0 ? pTexture->GetHeight() / valueTileSourceHeight.GetValue() : 1;

	srand(valueSeed.GetValue());

	for(i = 0; i < nTilesX; i++)
	{
		for(j = 0; j < nTilesY; j++)
		{
			STile tile;

			tile.nX        = i;
			tile.nY        = j;
			tile.nImgTileX = rand() % nImgTilesX;
			tile.nImgTileY = rand() % nImgTilesY;

			m_vecTiles.push_back(tile);
		}
	}

	m_fLastUpdate = 0.0f;

	return true;
}

bool CFXMosaic::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	m_vecTiles.clear();
	return true;
}

bool CFXMosaic::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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
		FXRuntimeError("WARNING: Texture not available");
		return false;
	}

	if(m_vecTiles.size() < 1)
	{
		FXRuntimeError("WARNING: No tiles available");
		return false;
	}

	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valueWidth;
	CVarFloat::CValueFloat valueHeight;
	CVarFloat::CValueFloat valueScale;
	CVarFloat::CValueFloat valueAlpha;
	CVarFloat::CValueFloat valueAngle;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueFiltering;
	CVarInt::CValueInt     valueTilesX;
	CVarInt::CValueInt     valueTilesY;
	CVarInt::CValueInt     valueTileSourceWidth;
	CVarInt::CValueInt     valueTileSourceHeight;
	CVarFloat::CValueFloat valueChangeFrequency;
	CVarFloat::CValueFloat valueChangeFreqVariation;

	EvaluateVar("X Position",            fEffectTime, &valuePosX);
	EvaluateVar("Y Position",            fEffectTime, &valuePosY);
	EvaluateVar("Width",                 fEffectTime, &valueWidth);
	EvaluateVar("Height",                fEffectTime, &valueHeight);
	EvaluateVar("Scale",                 fEffectTime, &valueScale);
	EvaluateVar("Alpha",                 fEffectTime, &valueAlpha);
	EvaluateVar("Angle",                 fEffectTime, &valueAngle);
	EvaluateVar("Blend Mode",            fEffectTime, &valueBlendMode);
	EvaluateVar("Filtering",             fEffectTime, &valueFiltering);
	EvaluateVar("Tiles X",               fEffectTime, &valueTilesX);
	EvaluateVar("Tiles Y",               fEffectTime, &valueTilesY);
	EvaluateVar("Tile Source Width",     fEffectTime, &valueTileSourceWidth);
	EvaluateVar("Tile Source Height",    fEffectTime, &valueTileSourceHeight);
	EvaluateVar("Change Frequency",      fEffectTime, &valueChangeFrequency);
	EvaluateVar("Change Freq Variation", fEffectTime, &valueChangeFreqVariation);

	// Change filter mode

	GLint minFilter, magFilter;
	GLfloat fMaxAnisotropy;

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	pTexture->SetActive();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

	if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	}

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

	CVarFloat* pVarAngle = reinterpret_cast<CVarFloat*>(GetVar("Angle"));
	assert(pVarAngle);

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);
	UtilGL::States::SetColor(v4Color);

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
	else if(valueBlendMode.GetValue() == "Color Mult")
	{
		nSrcBlend = UtilGL::States::BLEND_DSTCOLOR;
		nDstBlend = UtilGL::States::BLEND_ZERO;
	}

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !pTexture->HasAlpha())
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

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, pTexture->GetWidth(), pTexture->GetHeight(), 0, 0.0f, 10.0f);


	// Update ***********


	int nImgTilesX = pTexture && valueTileSourceWidth.GetValue()  > 0 ? pTexture->GetWidth()  / valueTileSourceWidth.GetValue()  : 1;
	int nImgTilesY = pTexture && valueTileSourceHeight.GetValue() > 0 ? pTexture->GetHeight() / valueTileSourceHeight.GetValue() : 1;

	float fChangePeriod = IS_ZERO(valueChangeFrequency.GetValue()) ? FLOAT_MAX : 1.0f / valueChangeFrequency.GetValue();

	if(fChangePeriod < 0.0001f) fChangePeriod = 0.0001f;

	while(MYFABSF(m_fLastUpdate - fEffectTime) > fChangePeriod)
	{
		int nRandTile = rand() % m_vecTiles.size();

		m_vecTiles[nRandTile].nImgTileX = rand() % nImgTilesX;
		m_vecTiles[nRandTile].nImgTileY = rand() % nImgTilesY;

		if(fEffectTime < m_fLastUpdate)
		{
			m_fLastUpdate -= fChangePeriod;
		}
		else
		{
			m_fLastUpdate += fChangePeriod;
		}
	}


	// Render ***********


	VECTILES::iterator it;

	float fUTileWidth  = (float)valueTileSourceWidth.GetValue()  / (float)pTexture->GetWidth();
	float fVTileHeight = (float)valueTileSourceHeight.GetValue() / (float)pTexture->GetHeight();

	int nTilesX = valueTilesX.GetValue() < 1 ? 1 : valueTilesX.GetValue();
	int nTilesY = valueTilesY.GetValue() < 1 ? 1 : valueTilesY.GetValue();

	for(it = m_vecTiles.begin(); it < m_vecTiles.end(); ++it)
	{
		STile tile = *it;

		float fTileWidth  = pTexture->GetWidth()  / (float)nTilesX;
		float fTileHeight = pTexture->GetHeight() / (float)nTilesY;
		float fPosX       = ((valuePosX.GetValue() - 0.5f) * pTexture->GetWidth())  + (fTileWidth  * tile.nX);
		float fPosY       = ((valuePosY.GetValue() - 0.5f) * pTexture->GetHeight()) + (fTileHeight * tile.nY);
		float fScaleX     = valueWidth.GetValue()  * valueScale.GetValue();
		float fScaleY     = valueHeight.GetValue() * valueScale.GetValue();
		float fU          = tile.nImgTileX * fUTileWidth;
		float fV          = 1.0f - (tile.nImgTileY * fVTileHeight);
		float fU2         = fU + fUTileWidth;
		float fV2         = fV - fVTileHeight;

		CVector3 v3Center(pTexture->GetWidth() * 0.5f, pTexture->GetHeight() * 0.5f, -5.0f);

		CMatrix worldMtx;
		worldMtx.SetIdentity();

		worldMtx.Translate(fPosX, fPosY, 0.0f);
		worldMtx.Translate(-v3Center.X(), -v3Center.Y(), 0.0f);
		worldMtx.Scale    (fScaleX, fScaleY, 1.0f);
		worldMtx.RotateZ  (fAngle);
		worldMtx.Translate(+v3Center.X(), +v3Center.Y(), 0.0f);

		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, worldMtx);

		glBegin(GL_QUADS);

		glTexCoord2f(fU, fV);
		glVertex3f(0.0f, 0.0f, -5.0f);

		glTexCoord2f(fU, fV2);
		glVertex3f(0.0f, fTileHeight, -5.0f);

		glTexCoord2f(fU2, fV2);
		glVertex3f(fTileWidth, fTileHeight, -5.0f);

		glTexCoord2f(fU2, fV);
		glVertex3f(fTileWidth, 0.0f, -5.0f);

		glEnd();
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);

	// Restore filtering

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);

	if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fMaxAnisotropy);
	}

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);

	return true;
}
