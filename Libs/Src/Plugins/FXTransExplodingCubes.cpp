#include "Plugins.h"
#include "FXTransExplodingCubes.h"


const std::string CFXTransExplodingCubes::CLASSNAME("Exploding Cubes");

REGISTER_PLUGIN(CPluginFactory, CFXTransExplodingCubes)


CFXTransExplodingCubes::CFXTransExplodingCubes()
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
	DefineVar("Start Speed X",    CVarFloat::CLASSNAME);
	DefineVar("Start Speed Y",    CVarFloat::CLASSNAME);
	DefineVar("Start Speed Z",    CVarFloat::CLASSNAME);
	DefineVar("Accel X",          CVarFloat::CLASSNAME);
	DefineVar("Accel Y",          CVarFloat::CLASSNAME);
	DefineVar("Accel Z",          CVarFloat::CLASSNAME);
	DefineVar("Assimetry",        CVarFloat::CLASSNAME);
	DefineVar("Max Rotation",     CVarFloat::CLASSNAME);
	DefineVar("Variation",        CVarFloat::CLASSNAME);

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
	((CVarFloat*)GetVar("Start Speed X" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Start Speed X" ))->LockConstant(true);
	((CVarFloat*)GetVar("Start Speed Y" ))->SetConstant(-0.4f);
	((CVarFloat*)GetVar("Start Speed Y" ))->LockConstant(true);
	((CVarFloat*)GetVar("Start Speed Z" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Start Speed Z" ))->LockConstant(true);
	((CVarFloat*)GetVar("Accel X"       ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Accel X"       ))->LockConstant(true);
	((CVarFloat*)GetVar("Accel Y"       ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Accel Y"       ))->LockConstant(true);
	((CVarFloat*)GetVar("Accel Z"       ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Accel Z"       ))->LockConstant(true);
	((CVarFloat*)GetVar("Assimetry"     ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Assimetry"     ))->LockConstant(true);
	((CVarFloat*)GetVar("Max Rotation"  ))->SetConstant(2000.0f);
	((CVarFloat*)GetVar("Max Rotation"  ))->LockConstant(true);
	((CVarFloat*)GetVar("Variation"     ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Variation"     ))->LockConstant(true);

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	GetVar("X Cubes"       )->ReloadFXOnChange(this);
	GetVar("Y Cubes"       )->ReloadFXOnChange(this);
	GetVar("Z Depth"       )->ReloadFXOnChange(this);
	GetVar("Seed"          )->ReloadFXOnChange(this);
	GetVar("Start Speed X" )->ReloadFXOnChange(this);
	GetVar("Start Speed Y" )->ReloadFXOnChange(this);
	GetVar("Start Speed Z" )->ReloadFXOnChange(this);
	GetVar("Accel X"       )->ReloadFXOnChange(this);
	GetVar("Accel Y"       )->ReloadFXOnChange(this);
	GetVar("Accel Z"       )->ReloadFXOnChange(this);
	GetVar("Assimetry"     )->ReloadFXOnChange(this);
	GetVar("Max Rotation"  )->ReloadFXOnChange(this);
	GetVar("Variation"     )->ReloadFXOnChange(this);
}

CFXTransExplodingCubes::~CFXTransExplodingCubes()
{
	Free();
}

void CFXTransExplodingCubes::OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	// At frame start (before all effects are drawn), set viewport matching texture dimensions.

	pDemo->SetViewport(0, 0, m_textureRTT.GetWidth(), m_textureRTT.GetHeight(), pDemo->ScissorActive());
}

void CFXTransExplodingCubes::OnDemoFrameEnd(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	UtilGL::States::Set(UtilGL::States::STENCILBUFFER, UtilGL::States::DISABLED);
}

bool CFXTransExplodingCubes::LoadData(CResourceList* pResourceList)
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

	// Create cubes

	CVarInt::CValueInt     valueXCubes;
	CVarInt::CValueInt     valueYCubes;
	CVarInt::CValueInt     valueSeed;
	CVarFloat::CValueFloat valueAngleMultiple;
	CVarFloat::CValueFloat valueDepth;
	CVarFloat::CValueFloat valueAngleStep;

	CVarFloat::CValueFloat valueAccelX;
	CVarFloat::CValueFloat valueAccelY;
	CVarFloat::CValueFloat valueAccelZ;
	CVarFloat::CValueFloat valueSpeedX;
	CVarFloat::CValueFloat valueSpeedY;
	CVarFloat::CValueFloat valueSpeedZ;
	CVarFloat::CValueFloat valueAssimetry;
	CVarFloat::CValueFloat valueMaxRotation;
	CVarFloat::CValueFloat valueVariation;

	EvaluateVar("X Cubes",        0.0f, &valueXCubes);
	EvaluateVar("Y Cubes",        0.0f, &valueYCubes);
	EvaluateVar("Seed",		      0.0f, &valueSeed);
	EvaluateVar("Z Depth",        0.0f, &valueDepth);
	EvaluateVar("Angle Step",     0.0f, &valueAngleStep);
	EvaluateVar("Accel X",        0.0f, &valueAccelX);
	EvaluateVar("Accel Y",        0.0f, &valueAccelY);
	EvaluateVar("Accel Z",        0.0f, &valueAccelZ);
	EvaluateVar("Start Speed X",  0.0f, &valueSpeedX);
	EvaluateVar("Start Speed Y",  0.0f, &valueSpeedY);
	EvaluateVar("Start Speed Z",  0.0f, &valueSpeedZ);
	EvaluateVar("Assimetry",      0.0f, &valueAssimetry);
	EvaluateVar("Max Rotation",   0.0f, &valueMaxRotation);
	EvaluateVar("Variation",      0.0f, &valueVariation);

	CVector3 v3Accel(valueAccelX.GetValue(), valueAccelY.GetValue(), valueAccelZ.GetValue());
	CVector3 v3Speed(valueSpeedX.GetValue(), valueSpeedY.GetValue(), valueSpeedZ.GetValue());

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

			cube.v3Accel    = v3Accel * ComputeRandWithVariation(1.0f, valueVariation.GetValue()) * ((botRight.Y() - upLeft.Y()) * xRes);
			cube.v3Speed    = v3Speed * ComputeRandWithVariation(1.0f, valueVariation.GetValue()) * ((botRight.Y() - upLeft.Y()) * xRes);

			cube.fStartTime = (MYFABSF(x - (xRes / 2.0f)) / (float)(xRes / 2.0f)) * valueAssimetry.GetValue();
			cube.fRotation  = ComputeRand(0.0f, valueMaxRotation.GetValue());

			UtilGL::Transforming::NormViewportToLocal(&cube.v3Center);

			cube.fTStart = 0.0f;
			cube.fTEnd   = 1.0f;

			m_vecCubes.push_back(cube);
		}
	}

	return true;
}

bool CFXTransExplodingCubes::Free()
{
	m_vecCubes.clear();
	return true;
}

bool CFXTransExplodingCubes::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

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
	UtilGL::States::Set (UtilGL::States::STENCILBUFFER, UtilGL::States::ENABLED);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp  (GL_REPLACE, GL_REPLACE, GL_REPLACE);

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
		float fTime = valueTime.GetValue() - it->fStartTime;
		if(fTime < 0.0f) fTime = 0.0f;
		DrawCube(*it, fTime);
	}

	glStencilFunc(GL_NOTEQUAL, 1, 1);
	glStencilOp  (GL_KEEP, GL_KEEP, GL_KEEP);

	// Restore

	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);

	return true;
}

bool CFXTransExplodingCubes::DrawCube(const SCube& cube, float fT)
{
	float fAngle = cube.fRotation * fT;
	
	CVector3 v3Axis(1.0f, 0.0f, 0.0f);
	CVector3 v3Center = cube.v3Center + (cube.v3Speed * fT) + (cube.v3Accel * fT * fT * 0.5f);

	CMatrix mtxRot;
	mtxRot.BuildRotationZ(fAngle);
	mtxRot.Transform(&v3Axis, true);

	CVector2 mapping[4] = { CVector2(cube.fU,  cube.fV),
							CVector2(cube.fU,  cube.fV2),
							CVector2(cube.fU2, cube.fV2),
							CVector2(cube.fU2, cube.fV) };

	CMatrix worldMtx;
	worldMtx.BuildRotation(fAngle, 1.0f, 0.0f, 0.0f);
	worldMtx.Translate    (v3Center.X(), v3Center.Y(), v3Center.Z());

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

	glTexCoord2fv(mapping[0].Data()); glVertex3fv(cube.v8.Data());
	glTexCoord2fv(mapping[1].Data()); glVertex3fv(cube.v7.Data());
	glTexCoord2fv(mapping[2].Data()); glVertex3fv(cube.v6.Data());
	glTexCoord2fv(mapping[3].Data()); glVertex3fv(cube.v5.Data());

	glEnd();

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
/*
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
*/
	return true;
}