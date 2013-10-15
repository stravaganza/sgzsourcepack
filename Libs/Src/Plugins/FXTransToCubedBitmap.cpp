#include "Plugins.h"
#include "FXTransToCubedBitmap.h"


const std::string CFXTransToCubedBitmap::CLASSNAME("Cubes to bitmap");

REGISTER_PLUGIN(CPluginFactory, CFXTransToCubedBitmap)


CFXTransToCubedBitmap::CFXTransToCubedBitmap()
{
	DefineVar("X Cubes",          CVarInt::CLASSNAME);
	DefineVar("Y Cubes",          CVarInt::CLASSNAME);
	DefineVar("Z Depth",          CVarFloat::CLASSNAME);
	DefineVar("Seed",			  CVarInt::CLASSNAME);
	DefineVar("Time (0-1)",       CVarFloat::CLASSNAME);
	DefineVar("Tex Scale X",      CVarFloat::CLASSNAME);
	DefineVar("Tex Scale Y",      CVarFloat::CLASSNAME);
	DefineVar("Filtering",        CVarCombo::CLASSNAME);
	DefineVar("RenderTex Width" , CVarInt::CLASSNAME);
	DefineVar("RenderTex Height", CVarInt::CLASSNAME);
	DefineVar("Flat Color",       CVarColor::CLASSNAME);
	DefineVar("Angle Step",       CVarFloat::CLASSNAME);


	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarInt*)GetVar("X Cubes"         ))->LockConstant(true);
	((CVarInt*)GetVar("X Cubes"         ))->SetConstant(10);
	((CVarInt*)GetVar("Y Cubes"         ))->LockConstant(true);
	((CVarInt*)GetVar("Y Cubes"         ))->SetConstant(10);
	((CVarFloat*)GetVar("Z Depth"       ))->LockConstant(true);
	((CVarFloat*)GetVar("Z Depth"       ))->SetConstant(0.01f);
	((CVarInt*)GetVar("Seed"            ))->LockConstant(true);
	((CVarInt*)GetVar("Seed"            ))->SetConstant(0);
	((CVarFloat*)GetVar("Time (0-1)"    ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Tex Scale X"   ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Tex Scale Y"   ))->SetConstant(1.0f);
	((CVarInt*)GetVar("RenderTex Width" ))->LockConstant(true);
	((CVarInt*)GetVar("RenderTex Width" ))->SetConstant(512);
	((CVarInt*)GetVar("RenderTex Height"))->LockConstant(true);
	((CVarInt*)GetVar("RenderTex Height"))->SetConstant(512);
	((CVarColor*)GetVar("Flat Color"    ))->SetConstant(CVector3(0.5f, 0.5f, 0.5f));
	((CVarFloat*)GetVar("Angle Step"    ))->SetConstant(90.0f);
	((CVarFloat*)GetVar("Angle Step"    ))->LockConstant(true);

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	GetVar("X Cubes"       )->ReloadFXOnChange(this);
	GetVar("Y Cubes"       )->ReloadFXOnChange(this);
	GetVar("Z Depth"       )->ReloadFXOnChange(this);
	GetVar("Seed"          )->ReloadFXOnChange(this);
	GetVar("Angle Step"    )->ReloadFXOnChange(this);

	m_pResTexture = NULL;
	m_pTexture    = NULL;
}

CFXTransToCubedBitmap::~CFXTransToCubedBitmap()
{
	Free();
}

void CFXTransToCubedBitmap::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_textureRTT.GetWidth(), m_textureRTT.GetHeight(), pDemo->ScissorActive());
}

bool CFXTransToCubedBitmap::LoadData(CResourceList* pResourceList)
{
	// Create RTT Texture

	CVarInt::CValueInt valueInt;

	EvaluateVar("RenderTex Width",  0.0f, &valueInt);
	if(valueInt.GetValue() < 1) valueInt.SetValue(1);
	int nWidthPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	EvaluateVar("RenderTex Height", 0.0f, &valueInt);
	if(valueInt.GetValue() < 1) valueInt.SetValue(1);
	int nHeightPwr = MYROUND(log10f(valueInt.GetValue()) / log10f(2));

	int nWidth  = pow(2, nWidthPwr);
	int nHeight = pow(2, nHeightPwr);

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();
	texOptions.eFilter = UtilGL::Texturing::FILTER_LINEAR;

	m_textureRTT.LoadFlat(nWidth, nHeight, CVector4(0.0f, 0.0f, 0.0f, 1.0f), false, false, &texOptions);

	// Get texture

	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);
	m_pTexture = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());

	// Create cubes

	CVarInt::CValueInt     valueXCubes;
	CVarInt::CValueInt     valueYCubes;
	CVarInt::CValueInt     valueSeed;
	CVarFloat::CValueFloat valueAngleMultiple;
	CVarFloat::CValueFloat valueDepth;
	CVarFloat::CValueFloat valueAngleStep;

	EvaluateVar("X Cubes",        0.0f, &valueXCubes);
	EvaluateVar("Y Cubes",        0.0f, &valueYCubes);
	EvaluateVar("Seed",		      0.0f, &valueSeed);
	EvaluateVar("Z Depth",        0.0f, &valueDepth);
	EvaluateVar("Angle Step",     0.0f, &valueAngleStep);

	int xRes = valueXCubes.GetValue() > 1 ? valueXCubes.GetValue() : 1;
	int yRes = valueYCubes.GetValue() > 1 ? valueYCubes.GetValue() : 1;

	float fSizeX = 1.0f / xRes;
	float fSizeY = 1.0f / yRes;

	srand(valueSeed.GetValue());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.33f, 1.0f, 100.0f);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

	for(int y = 0; y < yRes; y++)
	{
		for(int x = 0; x < xRes; x++)
		{
			SCube cube;

			cube.fU  = x * fSizeX;
			cube.fV  = 1.0f - (y * fSizeY);
			cube.fU2 = x * fSizeX + fSizeX;
			cube.fV2 = 1.0f - (y * fSizeY + fSizeY);

			CVector3 upLeft  (x * fSizeX,          y * fSizeY,          0.5f);
			CVector3 botRight(x * fSizeX + fSizeX, y * fSizeY + fSizeY, 0.5f);

			UtilGL::Transforming::NormViewportToLocal(&upLeft);
			UtilGL::Transforming::NormViewportToLocal(&botRight);

			float fHalfX = (botRight.X() - upLeft.X()) * 0.5f;
			float fHalfY = (botRight.Y() - upLeft.Y()) * 0.5f;
			float fHalfZ = valueDepth.GetValue() * 0.5f;

			cube.v1.Set(-fHalfX, -fHalfY, +fHalfZ);
			cube.v2.Set(-fHalfX, +fHalfY, +fHalfZ);
			cube.v3.Set(+fHalfX, +fHalfY, +fHalfZ);
			cube.v4.Set(+fHalfX, -fHalfY, +fHalfZ);
			cube.v5.Set(-fHalfX, -fHalfY, -fHalfZ);
			cube.v6.Set(-fHalfX, +fHalfY, -fHalfZ);
			cube.v7.Set(+fHalfX, +fHalfY, -fHalfZ);
			cube.v8.Set(+fHalfX, -fHalfY, -fHalfZ);

			cube.v3Center.Set(	(x * fSizeX) + (fSizeX * 0.5f),
								(y * fSizeY) + (fSizeY * 0.5f),
								0.5f);

			UtilGL::Transforming::NormViewportToLocal(&cube.v3Center);

			cube.fTStart = 0.0f;
			cube.fTEnd   = 1.0f;

			// Build random angle

			float fAngle = valueAngleStep.GetValue() * (rand() % 1000);
			fAngle       = fmod(fAngle, 360.0f);

			cube.fAngle  = fAngle;

			m_vecCubes.push_back(cube);
		}
	}

	return true;
}

bool CFXTransToCubedBitmap::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	m_pTexture = NULL;

	m_vecCubes.clear();
	return true;
}

bool CFXTransToCubedBitmap::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pTexture)
	{
		FXRuntimeError("WARNING: Texture not available -> flat draw");
	}

	CVarFloat::CValueFloat valueTime;
	CVarFloat::CValueFloat valueScaleX;
	CVarFloat::CValueFloat valueScaleY;
	CVarCombo::CValueCombo valueFiltering;
	CVarColor::CValueColor valueColor;

	EvaluateVar("Time (0-1)",  fEffectTime, &valueTime);
	EvaluateVar("Tex Scale X", fEffectTime, &valueScaleX);
	EvaluateVar("Tex Scale Y", fEffectTime, &valueScaleY);
	EvaluateVar("Filtering",   fEffectTime, &valueFiltering);
	EvaluateVar("Flat Color",  fEffectTime, &valueColor);

	m_v4FlatColor = valueColor.GetValue();

	float fTime = valueTime.GetValue();

	if(fTime < 0.0f) fTime = 0.0f;
	if(fTime > 1.0f) fTime = 1.0f;

	// Change filter mode

	GLint minFilter, magFilter;

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	m_textureRTT.SetActive();

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

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Framebuffer to texture
	m_textureRTT.CopyFromFramebuffer(0, 0, 0, 0, m_textureRTT.GetWidth(), m_textureRTT.GetHeight());

	// Restore demo viewport
	pDemo->SetDemoViewport();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// States

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.33f, 1.0f, 100.0f);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);

	CMatrix mtxTex;
	mtxTex.BuildScaling(valueScaleX.GetValue(), valueScaleY.GetValue(), 1.0f);

	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_TEXTURE, mtxTex);

	// Render:

	VECCUBES::iterator it;

	for(it = m_vecCubes.begin(); it != m_vecCubes.end(); ++it)
	{
		float fT = (it->fTStart - fTime) / (it->fTEnd - it->fTStart);
		DrawCube(*it, fT);
	}

	// Restore

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);

	return true;
}

bool CFXTransToCubedBitmap::DrawCube(const SCube& cube, float fT)
{
	float fAngle = 180.0f * fT;
	
	CVector3 v3Axis(1.0f, 0.0f, 0.0f);

	CMatrix mtxRot;
	mtxRot.BuildRotationZ(cube.fAngle);
	mtxRot.Transform(&v3Axis, true);

	int nVal = MYROUND(cube.fAngle / 45.0f) % 4;

	CVector2 mapping[4] = { CVector2(cube.fU,  cube.fV),
							CVector2(cube.fU,  cube.fV2),
							CVector2(cube.fU2, cube.fV2),
							CVector2(cube.fU2, cube.fV) };

	int nMap1 = 2, nMap2 = 3, nMap3 = 1, nMap4 = 0;

	switch(nVal)
	{
	case 0: nMap1 = 2; nMap2 = 3; nMap3 = 0; nMap4 = 1; break;
	case 1: nMap1 = 1; nMap2 = 2; nMap3 = 3; nMap4 = 0; break;
	case 2: nMap1 = 0; nMap2 = 1; nMap3 = 2; nMap4 = 3; break;
	case 3: nMap1 = 3; nMap2 = 0; nMap3 = 1; nMap4 = 2; break;
	}

	CMatrix worldMtx;
	worldMtx.BuildRotation(fAngle, v3Axis.X(), v3Axis.Y(), v3Axis.Z());
	worldMtx.Translate    (cube.v3Center.X(), cube.v3Center.Y(), cube.v3Center.Z());

	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, worldMtx);

	// front  back
	// 1   4  5  8
	// 2   3  6  7

	m_textureRTT.SetActive();

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));

	glBegin(GL_QUADS);

	glTexCoord2fv(mapping[0].Data()); glVertex3fv(cube.v1.Data());
	glTexCoord2fv(mapping[1].Data()); glVertex3fv(cube.v2.Data());
	glTexCoord2fv(mapping[2].Data()); glVertex3fv(cube.v3.Data());
	glTexCoord2fv(mapping[3].Data()); glVertex3fv(cube.v4.Data());

	glEnd();

	if(m_pTexture)
	{
		m_pTexture->SetActive();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBegin(GL_QUADS);
		glTexCoord2fv(mapping[nMap1].Data()); glVertex3fv(cube.v8.Data());
		glTexCoord2fv(mapping[nMap2].Data()); glVertex3fv(cube.v7.Data());
		glTexCoord2fv(mapping[nMap3].Data()); glVertex3fv(cube.v6.Data());
		glTexCoord2fv(mapping[nMap4].Data()); glVertex3fv(cube.v5.Data());
		glEnd();
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
		glBegin(GL_QUADS);
		glVertex3fv(cube.v8.Data());
		glVertex3fv(cube.v7.Data());
		glVertex3fv(cube.v6.Data());
		glVertex3fv(cube.v5.Data());
		glEnd();
	}

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::SetColor(m_v4FlatColor);

	glBegin(GL_QUADS);

	// left
	glVertex3fv(cube.v5.Data());
	glVertex3fv(cube.v6.Data());
	glVertex3fv(cube.v2.Data());
	glVertex3fv(cube.v1.Data());

	//right
	glVertex3fv(cube.v4.Data());
	glVertex3fv(cube.v3.Data());
	glVertex3fv(cube.v7.Data());
	glVertex3fv(cube.v8.Data());

	//bottom
	glVertex3fv(cube.v7.Data());
	glVertex3fv(cube.v3.Data());
	glVertex3fv(cube.v2.Data());
	glVertex3fv(cube.v6.Data());

	//top
	glVertex3fv(cube.v5.Data());
	glVertex3fv(cube.v1.Data());
	glVertex3fv(cube.v4.Data());
	glVertex3fv(cube.v8.Data());

	glEnd();

	glLineWidth(1.0f);
	UtilGL::States::SetColor(CVector4(0.0f, 0.0f, 0.0f, 0.0f));
	UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_LINE);

	glBegin(GL_QUADS);

	// left
	glVertex3fv(cube.v5.Data());
	glVertex3fv(cube.v6.Data());
	glVertex3fv(cube.v2.Data());
	glVertex3fv(cube.v1.Data());

	//right
	glVertex3fv(cube.v4.Data());
	glVertex3fv(cube.v3.Data());
	glVertex3fv(cube.v7.Data());
	glVertex3fv(cube.v8.Data());

	//bottom
	glVertex3fv(cube.v7.Data());
	glVertex3fv(cube.v3.Data());
	glVertex3fv(cube.v2.Data());
	glVertex3fv(cube.v6.Data());

	//top
	glVertex3fv(cube.v5.Data());
	glVertex3fv(cube.v1.Data());
	glVertex3fv(cube.v4.Data());
	glVertex3fv(cube.v8.Data());

	glEnd();

	UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_FILL);

	return true;
}