#include "Plugins.h"
#include "FXQuadGalore.h"


const std::string CFXQuadGalore::CLASSNAME("Quad Galore");

REGISTER_PLUGIN(CPluginFactory, CFXQuadGalore)


CFXQuadGalore::CFXQuadGalore()
{
	DefineVar("Num Rows",         CVarInt::CLASSNAME);
	DefineVar("Num Columns",      CVarInt::CLASSNAME);
	DefineVar("Num Depth Layers", CVarInt::CLASSNAME);
	DefineVar("Total Width",      CVarFloat::CLASSNAME);
	DefineVar("Total Height",     CVarFloat::CLASSNAME);
	DefineVar("Total Depth",      CVarFloat::CLASSNAME);
	DefineVar("Quad Width",       CVarFloat::CLASSNAME);
	DefineVar("Quad Height",      CVarFloat::CLASSNAME);
	DefineVar("Alpha",            CVarFloat::CLASSNAME);
	DefineVar("Blend Mode",       CVarCombo::CLASSNAME);
	DefineVar("Filtering",        CVarCombo::CLASSNAME);
	DefineVar("Tile U",           CVarFloat::CLASSNAME);
	DefineVar("Tile V",           CVarFloat::CLASSNAME);

	DefineVar("Keep Previous Camera", CVarCombo::CLASSNAME);

	DefineResourceReference("Texture", CResourceTexture2D::CLASSNAME);

	// Set default values (mainly to have default values for the editor)

	((CVarInt*)  GetVar("Num Rows"         ))->SetConstant(20);
	((CVarInt*)  GetVar("Num Columns"      ))->SetConstant(20);
	((CVarInt*)  GetVar("Num Depth Layers" ))->SetConstant(20);
	((CVarFloat*)GetVar("Total Width"      ))->SetConstant(30.0f);
	((CVarFloat*)GetVar("Total Height"     ))->SetConstant(30.0f);
	((CVarFloat*)GetVar("Total Depth"      ))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Quad Width"       ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Quad Height"      ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Alpha"            ))->SetConstant(0.3f);
	((CVarFloat*)GetVar("Tile U"           ))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Tile V"           ))->SetConstant(1.0f);

	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Add"); // default
	((CVarCombo*)GetVar("Blend Mode"))->AddOption("Mult");

	((CVarCombo*)GetVar("Filtering"))->AddOption("Yes"); // default
	((CVarCombo*)GetVar("Filtering"))->AddOption("No");

	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("No");  // default
	((CVarCombo*)GetVar("Keep Previous Camera"))->AddOption("Yes");

	DefineAddTransformVars(this);

	m_pResTexture = NULL;
}

CFXQuadGalore::~CFXQuadGalore()
{
	Free();
}

bool CFXQuadGalore::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResTexture = this->FindResource(pResourceList, "Texture", CResourceTexture2D::CLASSNAME)))
	{
		FXLoadError("Can't find texture resource");
		return false;
	}

	m_pResTexture->NotifyDependency(this);

	return true;
}

bool CFXQuadGalore::Free()
{
	if(m_pResTexture)
	{
		m_pResTexture->RemoveDependency(this);
		m_pResTexture = NULL;
	}

	return true;
}

bool CFXQuadGalore::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
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

	CVarInt::CValueInt     valueNumRows;
	CVarInt::CValueInt     valueNumColumns;
	CVarInt::CValueInt     valueNumDepthLayers;
	CVarFloat::CValueFloat valueTotalWidth;
	CVarFloat::CValueFloat valueTotalHeight;
	CVarFloat::CValueFloat valueTotalDepth;
	CVarFloat::CValueFloat valueQuadWidth;
	CVarFloat::CValueFloat valueQuadHeight;
	CVarFloat::CValueFloat valueAlpha;
	CVarCombo::CValueCombo valueBlendMode;
	CVarCombo::CValueCombo valueFiltering;
	CVarCombo::CValueCombo valueKeepPreviousCamera;
	CVarFloat::CValueFloat valueTileU;
	CVarFloat::CValueFloat valueTileV;

	EvaluateVar("Num Rows",         fEffectTime, &valueNumRows);
	EvaluateVar("Num Columns",      fEffectTime, &valueNumColumns);
	EvaluateVar("Num Depth Layers", fEffectTime, &valueNumDepthLayers);
	EvaluateVar("Total Width",      fEffectTime, &valueTotalWidth);
	EvaluateVar("Total Height",     fEffectTime, &valueTotalHeight);
	EvaluateVar("Total Depth",      fEffectTime, &valueTotalDepth);
	EvaluateVar("Quad Width",       fEffectTime, &valueQuadWidth);
	EvaluateVar("Quad Height",      fEffectTime, &valueQuadHeight);
	EvaluateVar("Alpha",            fEffectTime, &valueAlpha);
	EvaluateVar("Blend Mode",       fEffectTime, &valueBlendMode);
	EvaluateVar("Filtering",        fEffectTime, &valueFiltering);
	EvaluateVar("Keep Previous Camera", fEffectTime, &valueKeepPreviousCamera);
	EvaluateVar("Tile U",               fEffectTime, &valueTileU);
	EvaluateVar("Tile V",               fEffectTime, &valueTileV);

	// Change filter mode

	GLint minFilter, magFilter;

	if(pTexture)
	{
		pTexture->SetActive();
		pTexture->SetTiling(true);

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

	float fAlpha = valueAlpha.GetValue() > 1.0f ? 1.0f : valueAlpha.GetValue();

	if(fAlpha < ALMOST_ZERO)
	{
		return true;
	}

	CVector4 v4Color(1.0f, 1.0f, 1.0f, fAlpha);

	int nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	     if(valueBlendMode.GetValue() == "Add")  nDstBlend = UtilGL::States::BLEND_ONE;
	else if(valueBlendMode.GetValue() == "Mult") nDstBlend = UtilGL::States::BLEND_INVSRCALPHA;

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);

	if(ARE_EQUAL(fAlpha, 1.0f) && valueBlendMode.GetValue() == "Mult" && !pTexture->HasAlpha())
	{
		UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_ONE);
		UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_ZERO);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_SRCALPHA);
		UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);
	}

	CMatrix  mtxCam;
	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	// RENDERING

	float fHalfWidth  = valueQuadWidth.GetValue()  * 0.5f;
	float fHalfHeight = valueQuadHeight.GetValue() * 0.5f;

	CVector3 v1(-fHalfWidth,  fHalfHeight, 0.0f);
	CVector3 v2(-fHalfWidth, -fHalfHeight, 0.0f);
	CVector3 v3( fHalfWidth, -fHalfHeight, 0.0f);
	CVector3 v4( fHalfWidth,  fHalfHeight, 0.0f);

	v1 = mtxAddLocal * v1;
	v2 = mtxAddLocal * v2;
	v3 = mtxAddLocal * v3;
	v4 = mtxAddLocal * v4;

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::DISABLED);

	UtilGL::States::SetColor(v4Color);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);

	// Transforming

	mtxCam.RotateX(v3AddCamAngles.X());
	mtxCam.RotateY(v3AddCamAngles.Y());
	mtxCam.RotateZ(v3AddCamAngles.Z());
	mtxCam.SetPosition(v3AddCamPos);

	if(valueKeepPreviousCamera.GetValue() == "No")
	{
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_VIEW, mtxCam.Inverse());

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
		glMatrixMode(GL_PROJECTION);
		gluPerspective(60.0f, pDemo->GetAspect(), 1.0f, 1000.0f);
	}

	CMatrix mtxTexturing;
	mtxTexturing.BuildScaling(valueTileU.GetValue(), valueTileV.GetValue(), 1.0f);

	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD,   mtxAddWorld);
	UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_TEXTURE, mtxTexturing);

	CVector3 v3Vector;

	for(int k = 0; k < valueNumDepthLayers.GetValue(); k++)
	{
		v3Vector.SetX(ComputeLERP(	-valueTotalDepth.GetValue() * 0.5f,
									+valueTotalDepth.GetValue() * 0.5f,
									(float)k/valueNumDepthLayers.GetValue()));

		for(int j = 0; j < valueNumRows.GetValue(); j++)
		{
			v3Vector.SetY(ComputeLERP(	-valueTotalHeight.GetValue() * 0.5f,
										+valueTotalHeight.GetValue() * 0.5f,
										(float)j/valueNumRows.GetValue()));

			for(int i = 0; i < valueNumColumns.GetValue(); i++)			
			{
				v3Vector.SetZ(ComputeLERP(	-valueTotalWidth.GetValue() * 0.5f,
											+valueTotalWidth.GetValue() * 0.5f,
											(float)i/valueNumColumns.GetValue()));

				if(pTexture)
				{
					glBegin(GL_QUADS);

					glTexCoord2f(0.0f, 1.0f);
					glVertex3fv((v1 + v3Vector).Data());

					glTexCoord2f(0.0f, 0.0f);
					glVertex3fv((v2 + v3Vector).Data());

					glTexCoord2f(1.0f, 0.0f);
					glVertex3fv((v3 + v3Vector).Data());

					glTexCoord2f(1.0f, 1.0f);
					glVertex3fv((v4 + v3Vector).Data());

					glEnd();
				}
				else
				{
					glBegin(GL_QUADS);

					glVertex3fv((v1 + v3Vector).Data());
					glVertex3fv((v2 + v3Vector).Data());
					glVertex3fv((v3 + v3Vector).Data());
					glVertex3fv((v4 + v3Vector).Data());

					glEnd();
				}

			}
		}
	}

	// Restore filtering

	if(pTexture)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	}

	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BFCULLING, UtilGL::States::ENABLED);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);

	return true;
}
