// ==[ File ]===================================================================================
//
//  -Name     : LightingVisitor.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLightingVisitor ìmplementation.
//
// =============================================================================================

/*

	14/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLightingVisitor
//  - prototype : CLightingVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
//
//  - Purpose   : CLightingVisitor's constructor. Initializes with the given
//                traversal mode.
//
// -----------------------------------------------------------------------------
CLightingVisitor::CLightingVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
{
	SetTraversalMode(eTraversalMode);
	m_v4GlobalAmbient.Set(0.1f, 0.1f, 0.1f, 1.0f);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLightingVisitor
//  - prototype : ~CLightingVisitor()
//
//  - Purpose   : CLightingVisitor's destructor
//
// -----------------------------------------------------------------------------
CLightingVisitor::~CLightingVisitor()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLightingVisitor
//  - prototype : bool Reset()
//
//  - Purpose   : Deletes visitor's accumulated data and disables lighting.
//
// -----------------------------------------------------------------------------
bool CLightingVisitor::Reset()
{
	assert(UtilGL::IsReady());

	UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);

	for(int i = 0; i < UtilGL::GetMaxLights(); i++)
	{
		UtilGL::States::Set(UtilGL::States::GetLight(i), UtilGL::States::DISABLED);
	}

	m_v4GlobalAmbient.Set(0.1f, 0.1f, 0.1f, 1.0f);

	m_vecLights.clear();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLightingVisitor
//  - prototype : bool VisitLight(CLight* pLight)
//
//  - Purpose   : Visits a light. Internally stores a pointer to the light
//                in a list. ApplyLighting() will configure lighting with the
//                data in that list.
//
// -----------------------------------------------------------------------------
bool CLightingVisitor::VisitLight(CLight* pLight)
{
	assert(pLight);
	m_vecLights.push_back(pLight);

	return Traverse(pLight);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLightingVisitor
//  - prototype : bool VisitScene(CScene* pScene)
//
//  - Purpose   : Visits a scene. Stores its ambient light value.
//
// -----------------------------------------------------------------------------
bool CLightingVisitor::VisitScene(CScene* pScene)
{
	assert(pScene);
	m_v4GlobalAmbient = pScene->GetAmbientLight();

	return Traverse(pScene);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLightingVisitor
//  - prototype : bool ApplyLighting()
//
//  - Purpose   : Processes all stored (through visiting) lights configuring
//                OpenGL lighting.
//
// -----------------------------------------------------------------------------
bool CLightingVisitor::ApplyLighting()
{
	int nError = 0;

	if(m_vecLights.empty())
	{
		// No lights, disable lighting

		if(!UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED))
		{
			nError++;
		}
	}
	else
	{
		// We have at least 1 light, enable lighting

		if(!UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::ENABLED))
		{
			nError++;
		}

		// Configure ambient lighting

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_v4GlobalAmbient.Data());
		//glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

		if(UtilGL::Extensions::GetAvailableExtensions()->EXT_separate_specular_color)
		{
			glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
		}

		// Enable + configure single lights

		int nLights = m_vecLights.size();

		if(nLights > UtilGL::GetMaxLights())
		{
			LOG.Write("\nWARNING - CLightingVisitor::ApplyLighting(): Number of lights (%u) exceeds %u", nLights, UtilGL::GetMaxLights());
			nLights = UtilGL::GetMaxLights();
		}

		for(int i = 0; i < nLights; i++)
		{
			CLight* pLight  = m_vecLights[i];
			GLenum  glLight = GL_LIGHT0 + i;

			assert(pLight);

			UtilGL::States::Set(UtilGL::States::GetLight(i), UtilGL::States::ENABLED);

			UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pLight->GetAbsoluteTM());

			glLightfv(glLight, GL_AMBIENT,  pLight->GetAmbient().Data());
			glLightfv(glLight, GL_DIFFUSE,  pLight->GetDiffuse().Data());
			glLightfv(glLight, GL_SPECULAR, pLight->GetSpecular().Data());

			CVector4 v4Pos(0.0f, 0.0f,  0.0f, 1.0f);
			CVector4 v4Dir(0.0f, 0.0f, -1.0f, 1.0f);

			// Directional?

			if(pLight->GetNodeSubType() == ID_LIGHT_DIRECTIONAL)
			{
				// v4Pos will have w = 0.0 and (x, y, z) representing a vector
				// pointing from origin to light at infinity.

				v4Pos.SetZ(1.0f);
				v4Pos.SetW(0.0f);
			}

			glLightfv(glLight, GL_POSITION, v4Pos.Data());

			// Spot?

			if(pLight->GetNodeSubType() == ID_LIGHT_SPOT)
			{
				glLightfv(glLight, GL_SPOT_DIRECTION, v4Dir.Data());
				glLightf (glLight, GL_SPOT_CUTOFF,    pLight->GetSpotCutoff());
				glLightf (glLight, GL_SPOT_EXPONENT,  pLight->GetSpotExponent());
			}

			// Attenuation
/*
			glLightf(glLight, GL_CONSTANT_ATTENUATION,  pLight->GetAttenConstant());
			glLightf(glLight, GL_LINEAR_ATTENUATION,    pLight->GetAttenLinear());
			glLightf(glLight, GL_QUADRATIC_ATTENUATION, pLight->GetAttenQuadratic());
*/		}

		// Disable the rest of the lights

		for(;i < UtilGL::GetMaxLights(); i++)
		{
			UtilGL::States::Set(UtilGL::States::GetLight(i), UtilGL::States::DISABLED);
		}
	}

	return true;
}
