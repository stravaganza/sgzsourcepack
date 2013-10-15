#include "Plugins.h"
#include "FXWater.h"


const std::string CFXWater::CLASSNAME("Water Effect");

REGISTER_PLUGIN(CPluginFactory, CFXWater)


#define MAXRES 2000


CFXWater::CFXWater()
{
	DefineVar("Position X",              CVarFloat::CLASSNAME);
	DefineVar("Position Y",              CVarFloat::CLASSNAME);
	DefineVar("Position Z",              CVarFloat::CLASSNAME);
	DefineVar("Resolution X",            CVarInt::CLASSNAME);
	DefineVar("Resolution Y",            CVarInt::CLASSNAME);
	DefineVar("Size X",                  CVarFloat::CLASSNAME);
	DefineVar("Size Y",                  CVarFloat::CLASSNAME);
	DefineVar("Perlin Start Frequency",  CVarFloat::CLASSNAME);
	DefineVar("Perlin Num Octaves",      CVarInt::CLASSNAME);
	DefineVar("Perlin Persistence",      CVarFloat::CLASSNAME);
	DefineVar("Tex Water Tile",          CVarFloat::CLASSNAME);
	DefineVar("Tex Env Tile",            CVarFloat::CLASSNAME);
	DefineVar("Tex Env Tile U",          CVarFloat::CLASSNAME);
	DefineVar("Tex Env Tile V",          CVarFloat::CLASSNAME);
	DefineVar("Perlin Persistence",      CVarFloat::CLASSNAME);
	DefineVar("Wave Frequency",          CVarFloat::CLASSNAME);
	DefineVar("Wave Amplitude",          CVarFloat::CLASSNAME);
	DefineVar("Wireframe Mode",          CVarCombo::CLASSNAME);
	DefineVar("Smooth Steps",            CVarInt::CLASSNAME);
	DefineVar("Keep Previous Camera",    CVarCombo::CLASSNAME);
	DefineVar("Normalize",				 CVarCombo::CLASSNAME);
	DefineVar("Env Normal Generation",   CVarCombo::CLASSNAME);
	DefineVar("Animation Type",			 CVarCombo::CLASSNAME);

	DefineResourceReference("Water Texture", CResourceTexture2D::CLASSNAME);
	DefineResourceReference("Env Texture",   CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarFloat*)GetVar("Position X"))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Position Y"))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Position Z"))->SetConstant(0.0f);
	((CVarInt*)GetVar("Resolution X" ))->SetConstant(30);
	((CVarInt*)GetVar("Resolution Y"))->SetConstant(30);
	((CVarFloat*)GetVar("Size X" ))->SetConstant(100.0f);
	((CVarFloat*)GetVar("Size Y" ))->SetConstant(100.0f);
	((CVarFloat*)GetVar("Perlin Start Frequency"))->SetConstant(1.0f);
	((CVarInt*)GetVar("Perlin Num Octaves"    ))->SetConstant(1);
	((CVarFloat*)GetVar("Perlin Persistence"    ))->SetConstant(0.5f);
	((CVarFloat*)GetVar("Tex Water Tile" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Tex Env Tile"   ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Tex Env Tile U" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Tex Env Tile V" ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Wave Frequency" ))->SetConstant(3.0f);
	((CVarFloat*)GetVar("Wave Amplitude" ))->SetConstant(0.1f);
	((CVarInt*)GetVar("Smooth Steps" ))->SetConstant(1);

	((CVarCombo*)GetVar("Wireframe Mode"))->AddOption("No");
	((CVarCombo*)GetVar("Wireframe Mode"))->AddOption("Yes");

	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("No");
	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("Yes");

	((CVarCombo*)GetVar("Normalize"))->AddOption("Yes");
	((CVarCombo*)GetVar("Normalize"))->AddOption("No");

	((CVarCombo*)GetVar("Env Normal Generation"))->AddOption("Reflection Map");
	((CVarCombo*)GetVar("Env Normal Generation"))->AddOption("Normal Map");
	((CVarCombo*)GetVar("Env Normal Generation"))->AddOption("Sphere Map");
	((CVarCombo*)GetVar("Env Normal Generation"))->AddOption("Explicit UV");

	((CVarCombo*)GetVar("Animation Type"))->AddOption("Perlin Morph");
	((CVarCombo*)GetVar("Animation Type"))->AddOption("Senoidal Mesh");

	DefineAddTransformVars(this);

	m_fTimeMesh1 = -1.0f;
	m_fTimeMesh2 = -1.0f;

	m_pResTexture    = NULL;
	m_pResTextureEnv = NULL;
	m_nResX          = 0;
	m_nResY          = 0;
	m_pv3Vertices    = NULL;
	m_pv3Normals     = NULL;
	m_pv3FaceNormals = NULL;
	m_pv2Mapping     = NULL;
	m_pv2Mapping2    = NULL;
	m_pFaceList      = NULL;
}

CFXWater::~CFXWater()
{
	Free();
}

bool CFXWater::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Water Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	if(!(m_pResTextureEnv = this->FindResource(pResourceList, "Env Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find env texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	TestCreateGrid(0.0f, true);

	return true;
}

bool CFXWater::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	if(m_pResTextureEnv)
	{
		m_pResTextureEnv->RemoveDependency(this);
		m_pResTextureEnv = NULL;
	}

	SAFE_DELETE_A(m_pv3Vertices);
	SAFE_DELETE_A(m_pv3Normals);
	SAFE_DELETE_A(m_pv3FaceNormals);
	SAFE_DELETE_A(m_pv2Mapping);
	SAFE_DELETE_A(m_pv2Mapping2);
	SAFE_DELETE_A(m_pFaceList);

	return true;
}

bool CFXWater::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResTexture)
	{
		FXRuntimeError("Texture resource not found");
		return false;
	}

	if(!m_pResTextureEnv)
	{
		FXRuntimeError("Texture Env resource not found");
		return false;
	}

	UtilGL::Texturing::CTexture2D* pTexture    = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTexture)->GetTexture2D());
	UtilGL::Texturing::CTexture2D* pTextureEnv = const_cast<UtilGL::Texturing::CTexture2D*>(((CResourceTexture2D*)m_pResTextureEnv)->GetTexture2D());

	if(!pTexture)
	{
		FXRuntimeError("WARNING: Texture not available, effect will draw flat quad");
	}

	if(!pTextureEnv)
	{
		FXRuntimeError("WARNING: Texture Env not available, effect will draw flat quad");
	}

	CVarFloat::CValueFloat valuePosX;
	CVarFloat::CValueFloat valuePosY;
	CVarFloat::CValueFloat valuePosZ;
	CVarInt::CValueInt     valueResX;
	CVarInt::CValueInt     valueResY;
	CVarFloat::CValueFloat valueSizeX;
	CVarFloat::CValueFloat valueSizeY;
	CVarFloat::CValueFloat valuePerlinStartFrequency;
	CVarInt::CValueInt     valuePerlinNumOctaves;
	CVarFloat::CValueFloat valuePerlinPersistence;
	CVarFloat::CValueFloat valueTexWaterTile;
	CVarFloat::CValueFloat valueTexEnvTile;
	CVarFloat::CValueFloat valueTexEnvTileU;
	CVarFloat::CValueFloat valueTexEnvTileV;
	CVarFloat::CValueFloat valueWaveFreq;
	CVarFloat::CValueFloat valueWaveAmplitude;
	CVarInt::CValueInt     valueSmoothSteps;
	CVarCombo::CValueCombo valueWireframe;
	CVarCombo::CValueCombo valueKeepCamera;
	CVarCombo::CValueCombo valueNormalize;
	CVarCombo::CValueCombo valueTexGen;
	CVarCombo::CValueCombo valueAnimationType;

	EvaluateVar("Position X",              fEffectTime, &valuePosX);
	EvaluateVar("Position Y",              fEffectTime, &valuePosY);
	EvaluateVar("Position Z",              fEffectTime, &valuePosZ);
	EvaluateVar("Resolution X",            fEffectTime, &valueResX);
	EvaluateVar("Resolution Y",            fEffectTime, &valueResY);
	EvaluateVar("Size X",                  fEffectTime, &valueSizeX);
	EvaluateVar("Size Y",                  fEffectTime, &valueSizeY);
	EvaluateVar("Perlin Start Frequency",  fEffectTime, &valuePerlinStartFrequency); 
	EvaluateVar("Perlin Num Octaves",      fEffectTime, &valuePerlinNumOctaves);
	EvaluateVar("Perlin Persistence",      fEffectTime, &valuePerlinPersistence);
	EvaluateVar("Tex Water Tile",          fEffectTime, &valueTexWaterTile);
	EvaluateVar("Tex Env Tile",            fEffectTime, &valueTexEnvTile);
	EvaluateVar("Tex Env Tile U",          fEffectTime, &valueTexEnvTileU);
	EvaluateVar("Tex Env Tile V",          fEffectTime, &valueTexEnvTileV);
	EvaluateVar("Wave Frequency",          fEffectTime, &valueWaveFreq);
	EvaluateVar("Wave Amplitude",          fEffectTime, &valueWaveAmplitude);
	EvaluateVar("Wireframe Mode",          fEffectTime, &valueWireframe);
	EvaluateVar("Smooth Steps",            fEffectTime, &valueSmoothSteps);
	EvaluateVar("Keep Previous Camera",    fEffectTime, &valueKeepCamera);
	EvaluateVar("Normalize",			   fEffectTime, &valueNormalize);
	EvaluateVar("Env Normal Generation",   fEffectTime, &valueTexGen);
	EvaluateVar("Animation Type",		   fEffectTime, &valueAnimationType);

	bool bPerlin = valueAnimationType.GetValue() == "Perlin Morph";

	bool bKeepCamera  = valueKeepCamera.GetValue() == "Yes";
	bool bWire        = valueWireframe.GetValue() == "Yes";
	int  nSmoothSteps = valueSmoothSteps.GetValue();
	bool bNormalize   = valueNormalize.GetValue() == "Yes";

	if(nSmoothSteps > 20)
	{
		nSmoothSteps = 20;
	}

	// Let's see if we have to compute new perlin noises

	float fWaveFrequency = valueWaveFreq.GetValue();

	if(fWaveFrequency < 0.01f) fWaveFrequency = 0.01f;

	float fWavePeriod = 1.0f / fWaveFrequency;

	int   nLoop       = bPerlin ? fEffectTime / fWavePeriod : 1;
	float fTimeMesh1  = fWavePeriod * nLoop;
	float fTimeMesh2  = fTimeMesh1 + fWavePeriod;

	if(!ARE_EQUAL(m_fTimeMesh1, fTimeMesh1))
	{
		m_perlin1.Init(	nLoop,
						valuePerlinNumOctaves.GetValue(),
						valuePerlinStartFrequency.GetValue(),
						valuePerlinPersistence.GetValue());

		m_fTimeMesh1 = fTimeMesh1;
	}

	if(!ARE_EQUAL(m_fTimeMesh2, fTimeMesh2))
	{
		m_perlin2.Init(	nLoop + 1,
						valuePerlinNumOctaves.GetValue(),
						valuePerlinStartFrequency.GetValue(),
						valuePerlinPersistence.GetValue());

		m_fTimeMesh2 = fTimeMesh2;
	}

	// OK, create mesh if necessary

	TestCreateGrid(fEffectTime, false);

	// Morph between the two meshes

	float fTimeMorph = (fEffectTime - fTimeMesh1) / (fTimeMesh2 - fTimeMesh1);
	float fU         = 0.0f;
	float fV         = 1.0f;
	float fIncU      = (1.0f / (m_nResX - 1)) * valueTexWaterTile.GetValue();
	float fIncV      = (1.0f / (m_nResY - 1)) * -valueTexWaterTile.GetValue();
	float fXPos;
	float fYPos      = valuePosY.GetValue();
	float fZPos      = valuePosZ.GetValue() - (valueSizeY.GetValue() * 0.5f);
	float fIncX      = valueSizeX.GetValue() / (float)(m_nResX - 1);
	float fIncZ      = valueSizeY.GetValue() / (float)(m_nResY - 1);
	int   nCount     = 0;

	int x, y;

	for(y = 0; y < m_nResY; y++)
	{
		fXPos = valuePosX.GetValue() - (valueSizeX.GetValue() * 0.5f);
		fU    = 0.0f;

		for(x = 0; x < m_nResX; x++)
		{
			float fVal1  = m_perlin1.Evaluate(x, y);
			float fVal2  = m_perlin2.Evaluate(x, y);
			float fMorph;

			if(bPerlin)
			{
				fMorph = fVal1 * (1.0f - fTimeMorph) + fVal2 * fTimeMorph;
			}
			else
			{
				float fT = ((m_perlin1.Evaluate(x, y) + fEffectTime) * fWaveFrequency) + m_perlin2.Evaluate(x, y);
				fMorph   = sinf(fT * TWOPI);
			}

			float fY = fYPos + fMorph * valueWaveAmplitude.GetValue();

			m_pv3Vertices[nCount].Set(fXPos, fY, fZPos);
			m_pv2Mapping [nCount].Set(fU, fV);
			m_pv2Mapping2[nCount].Set(m_pv3Normals[nCount].X(), m_pv3Normals[nCount].Y());

			fXPos += fIncX;
			fU    += fIncU;
			nCount++;
		}

		fV    += fIncV;
		fZPos += fIncZ;
	}

	// Smooth + Compute normals

	SmoothGrid(nSmoothSteps);
	ComputeNormals(bNormalize);

	// Transformations

	CMatrix  mtxCam;
	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	mtxCam.RotateX(v3AddCamAngles.X());
	mtxCam.RotateY(v3AddCamAngles.Y());
	mtxCam.RotateZ(v3AddCamAngles.Z());
	mtxCam.SetPosition(v3AddCamPos);

	CMatrix matCombined = mtxAddWorld * mtxAddLocal;
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, matCombined);

	if(!bKeepCamera)
	{
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW, mtxCam.Inverse());

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
		glMatrixMode(GL_PROJECTION);
		gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);
	}
	else mtxCam = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW).Inverse();

	// Renderstates

	if(bWire)
	{
		UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_LINE);
	}

	UtilGL::States::Set(UtilGL::States::LIGHTING,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D,   UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,    UtilGL::States::DISABLED);

	CVector4 v4Color(1.0f, 1.0f, 1.0f, 1.0f);
	UtilGL::States::SetColor(v4Color);

	// Setup both texture units

	UtilGL::Texturing::SetCurrentUnit(0);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	pTexture->SetActive();
	pTexture->SetTiling(true);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	UtilGL::Texturing::SetCurrentUnit(1);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	pTextureEnv->SetActive();
	pTextureEnv->SetTiling(true);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

	if(valueTexGen.GetValue() == "Reflection Map")
	{
		UtilGL::States::Set(UtilGL::States::UGENFUNC, UtilGL::States::TEXGEN_REFLECTIONMAP);
		UtilGL::States::Set(UtilGL::States::VGENFUNC, UtilGL::States::TEXGEN_REFLECTIONMAP);
	}
	else if(valueTexGen.GetValue() == "Normal Map")
	{
		UtilGL::States::Set(UtilGL::States::UGENFUNC, UtilGL::States::TEXGEN_NORMALMAP);
		UtilGL::States::Set(UtilGL::States::VGENFUNC, UtilGL::States::TEXGEN_NORMALMAP);
	}
	else if(valueTexGen.GetValue() == "Sphere Map")
	{
		UtilGL::States::Set(UtilGL::States::UGENFUNC, UtilGL::States::TEXGEN_SPHEREMAP);
		UtilGL::States::Set(UtilGL::States::VGENFUNC, UtilGL::States::TEXGEN_SPHEREMAP);
	}

	CMatrix texEnvMatrix;
	texEnvMatrix.BuildScaling(valueTexEnvTile.GetValue() * valueTexEnvTileU.GetValue(),
							  valueTexEnvTile.GetValue() * valueTexEnvTileV.GetValue(), 1.0f);

	if(valueTexGen.GetValue() == "Explicit UV")
	{
		nCount = 0;

		for(nCount = 0; nCount < m_nResX * m_nResY; nCount++)
		{
			CPlane plane(m_pv3Vertices[nCount], m_pv3Normals[nCount]);
			CRay   rayView(mtxCam.Position(), CVector3(mtxCam.Position(), m_pv3Vertices[nCount]));
			CRay   rayReflexion;
			rayView.Reflexion(plane, &rayReflexion);
			CVector3 v3Map(rayReflexion.Direction().X(), rayReflexion.Direction().Y(), 1.0f);
			v3Map.Normalize();
			v3Map = v3Map * valueWaveAmplitude.GetValue();
			m_pv2Mapping2[nCount].Set(m_pv2Mapping[nCount].X() + v3Map.X(), m_pv2Mapping[nCount].Y() + v3Map.Y());
		}

		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, m_pv2Mapping2);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::GENU, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::GENV, UtilGL::States::ENABLED);

		//texEnvMatrix = mtxCam * texEnvMatrix;
	}

	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_TEXTURE, texEnvMatrix);

	UtilGL::Texturing::SetCurrentUnit(0);

	// Draw

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer  (3, GL_FLOAT, 0, m_pv3Vertices);
	glNormalPointer  (   GL_FLOAT, 0, m_pv3Normals);
	glTexCoordPointer(2, GL_FLOAT, 0, m_pv2Mapping);

	glDrawRangeElements(GL_QUADS, 0, m_nResX * m_nResY - 1, (m_nResX - 1) * (m_nResY - 1) * 4, GL_UNSIGNED_INT, m_pFaceList);

 	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	UtilGL::Texturing::SetCurrentUnit(1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::GENU, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::GENV, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::GENQ, UtilGL::States::DISABLED);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	UtilGL::Texturing::SetCurrentUnit(0);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::ENABLED);

	if(bWire)
	{
		glBegin(GL_LINES);

		glColor3f(0.0f, 1.0f, 0.0f);

		for(nCount = 0; nCount < m_nResX * m_nResY; nCount++)
		{
			glVertex3fv(m_pv3Vertices[nCount].Data());
			glVertex3fv((m_pv3Vertices[nCount] + m_pv3Normals[nCount]).Data());
		}

		glEnd();
		UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_FILL);
	}

	return true;
}


bool CFXWater::TestCreateGrid(float fTime, bool bForce)
{
	CVarInt::CValueInt valueResX;
	CVarInt::CValueInt valueResY;

	EvaluateVar("Resolution X", fTime, &valueResX);
	EvaluateVar("Resolution Y", fTime, &valueResY);

	int nResX = valueResX.GetValue();
	int nResY = valueResY.GetValue();

	if(nResX > MAXRES) nResX = MAXRES; if(nResX < 2) nResX = 2;
	if(nResY > MAXRES) nResY = MAXRES; if(nResY < 2) nResY = 2;

	if(bForce == false)
	{
		if(m_nResX == nResX && m_nResY == nResY) return true;
	}

	m_nResX = nResX;
	m_nResY = nResY;

	SAFE_DELETE_A(m_pv3Vertices);
	SAFE_DELETE_A(m_pv3Normals);
	SAFE_DELETE_A(m_pv3FaceNormals);
	SAFE_DELETE_A(m_pv2Mapping);
	SAFE_DELETE_A(m_pv2Mapping2);
	SAFE_DELETE_A(m_pFaceList);

	m_pv3Vertices    = new CVector3[m_nResX * m_nResY];
	m_pv3Normals     = new CVector3[m_nResX * m_nResY];
	m_pv2Mapping     = new CVector2[m_nResX * m_nResY];
	m_pv2Mapping2    = new CVector2[m_nResX * m_nResY];
	m_pv3FaceNormals = new CVector3[(m_nResX - 1) * (m_nResY - 1)];
	m_pFaceList      = new unsigned int[(m_nResX - 1) * (m_nResY - 1) * 4];

	assert(m_pv3Vertices);
	assert(m_pv3Normals);
	assert(m_pv3FaceNormals);
	assert(m_pv2Mapping);
	assert(m_pv2Mapping2);
	assert(m_pFaceList);

	int nCount = 0;

	for(int y = 0; y < m_nResY - 1; y++)
	{
		for(int x = 0; x < m_nResX - 1; x++)
		{
			m_pFaceList[nCount + 0] = y * m_nResX + x;
			m_pFaceList[nCount + 1] = (y + 1) * m_nResX + x;
			m_pFaceList[nCount + 2] = (y + 1) * m_nResX + x + 1;
			m_pFaceList[nCount + 3] = y * m_nResX + x + 1;

			nCount += 4;
		}
	}

	return true;
}

void CFXWater::SmoothGrid(int nSteps)
{
	int i, x, y;

	for(i = 0; i < nSteps; i++)
	{
		int nCount = 0;

		for(y = 0; y < m_nResY; y++)
		{
			for(x = 0; x < m_nResX; x++)
			{
				float fYSmoothed     = 0.0f;
				float fContributions = 0.0f;

				// Upper row

				if(y > 0)
				{
					fYSmoothed += m_pv3Vertices[nCount - m_nResX].Y();
					fContributions += 1.0f;

					if(x > 0)
					{
						fYSmoothed += m_pv3Vertices[nCount - m_nResX - 1].Y();
						fContributions += 1.0f;
					}

					if(x < (m_nResX - 1))
					{
						fYSmoothed += m_pv3Vertices[nCount - m_nResX + 1].Y();
						fContributions += 1.0f;
					}
				}

				// Mid row

				if(x > 0)
				{
					fYSmoothed += m_pv3Vertices[nCount - 1].Y();
					fContributions += 1.0f;
				}

				if(x < (m_nResX - 1))
				{
					fYSmoothed += m_pv3Vertices[nCount + 1].Y();
					fContributions += 1.0f;
				}

				// Bottom row

				if(y < (m_nResY - 1))
				{
					fYSmoothed += m_pv3Vertices[nCount + m_nResX].Y();
					fContributions += 1.0f;

					if(x > 0)
					{
						fYSmoothed += m_pv3Vertices[nCount + m_nResX - 1].Y();
						fContributions += 1.0f;
					}

					if(x < (m_nResX - 1))
					{
						fYSmoothed += m_pv3Vertices[nCount + m_nResX + 1].Y();
						fContributions += 1.0f;
					}
				}

				m_pv3Vertices[nCount].SetY(	(m_pv3Vertices[nCount].Y()    * 0.5f) +
											(fYSmoothed / fContributions) * 0.5f);
				nCount++;
			}
		}
	}
}

void CFXWater::ComputeNormals(bool bNormalize)
{
	int nNumFaces = (m_nResX - 1) * (m_nResY - 1);
	int nNumVerts = m_nResX * m_nResY;
	int nCount, x, y;

	// Face normals

	CVector3 v1, v2;
	nCount = 0;

	for(y = 0; y < (m_nResY - 1); y++)
	{
		for(x = 0; x < (m_nResX - 1); x++)
		{
			int nIndex = (m_nResX * y) + x;

			v1.Build(m_pv3Vertices[nIndex], m_pv3Vertices[nIndex + 1]);
			v2.Build(m_pv3Vertices[nIndex], m_pv3Vertices[nIndex + m_nResX]);

			m_pv3FaceNormals[nCount] = v1 ^ v2;
			//m_pv3FaceNormals[nCount].Normalize();

			nCount++;
		}
	}

	// Compute vertex normals

	for(nCount = 0; nCount < nNumVerts; nCount++)
	{
		m_pv3Normals[nCount].Set(0.0f, 0.0f, 0.0f);
	}

	// Compute

	for(nCount = 0; nCount < nNumFaces; nCount++)
	{
		int nIndex = nCount * 4;

		m_pv3Normals[m_pFaceList[nIndex + 0]] = m_pv3Normals[m_pFaceList[nIndex + 0]] + m_pv3FaceNormals[nCount];
		m_pv3Normals[m_pFaceList[nIndex + 1]] = m_pv3Normals[m_pFaceList[nIndex + 1]] + m_pv3FaceNormals[nCount];
		m_pv3Normals[m_pFaceList[nIndex + 2]] = m_pv3Normals[m_pFaceList[nIndex + 2]] + m_pv3FaceNormals[nCount];
		m_pv3Normals[m_pFaceList[nIndex + 3]] = m_pv3Normals[m_pFaceList[nIndex + 3]] + m_pv3FaceNormals[nCount];
	}

	// Normalize

	if(bNormalize)
	{
		for(nCount = 0; nCount < nNumVerts; nCount++)
		{
			m_pv3Normals[nCount].Normalize();
		}
	}
}