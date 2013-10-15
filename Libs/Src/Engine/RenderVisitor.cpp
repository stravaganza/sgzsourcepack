// ==[ File ]===================================================================================
//
//  -Name     : RenderVisitor.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CRenderVisitor ìmplementation.
//
// =============================================================================================

/*

	17/02/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : CRenderVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
//
//  - Purpose   : CRenderVisitor's constructor. Initializes with the given
//                traversal mode.
//
// -----------------------------------------------------------------------------
CRenderVisitor::CRenderVisitor(CNodeVisitor::ETraversalMode eTraversalMode)
{
	SetTraversalMode(eTraversalMode);
	m_eRenderMode = MODE_FULLDETAIL;

	m_pScene      = NULL;

	m_nDrawnObjs  = 0;
	m_nTotalObjs  = 0;
	m_nDrawnPolys = 0;
	m_nTotalPolys = 0;
	m_nDrawnVerts = 0;
	m_nTotalVerts = 0;
	m_nDrawnParts = 0;
	m_nTotalParts = 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : ~CRenderVisitor()
//
//  - Purpose   : CRenderVisitor's destructor
//
// -----------------------------------------------------------------------------
CRenderVisitor::~CRenderVisitor()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool Reset()
//
//  - Purpose   : Deletes visitor's accumulated data.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::Reset()
{
	m_vecRenderables.clear();

	m_nDrawnObjs  = 0;
	m_nTotalObjs  = 0;
	m_nDrawnPolys = 0;
	m_nTotalPolys = 0;
	m_nDrawnVerts = 0;
	m_nTotalVerts = 0;
	m_nDrawnParts = 0;
	m_nTotalParts = 0;

	m_pScene = NULL;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool VisitScene(CScene* pScene)
//
//  - Purpose   : Visits a scene. Stores it in the pointer member.
//                Later, when rendering, we we'll check for fog presence.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::VisitScene(CScene* pScene)
{
	assert(pScene);

	m_pScene = pScene;

	return Traverse(pScene);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool VisitObject(CObject* pObject)
//
//  - Purpose   : Visits an object. Stores it in the draw list.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::VisitObject(CObject* pObject)
{
	assert(pObject);

	m_vecRenderables.push_back(pObject);

	return Traverse(pObject);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool VisitLensFlare(CLensFlare* pLensFlare)
//
//  - Purpose   : Visits a lensflare. Stores it the draw list.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::VisitLensFlare(CLensFlare* pLensFlare)
{
	assert(pLensFlare);

	m_vecRenderables.push_back(pLensFlare);

	return Traverse(pLensFlare);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool VisitParticleSystem(CParticleSystem* pParticleSystem)
//
//  - Purpose   : Visits a particle system. Stores it the draw list.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::VisitParticleSystem(CParticleSystem* pParticleSystem)
{
	assert(pParticleSystem);

	m_vecRenderables.push_back(pParticleSystem);

	return Traverse(pParticleSystem);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool Render(CCamera* pCamera)
//
//  - Purpose   : Renders accumulated objects (first opaque, then translucent).
//                Opaque objects are sorted first by shader, then from front
//                to back. Translucent objects are sorted back to front.
//                pCamera points to the camera that shall be used as point of
//                view (and internally also used for frustum culling).
//
//  - Notes     : Render modes different than MODE_FULLDETAIL are coded for
//                debugging purposes, so they haven't been optimized.
//                Objects without shader also use this render, so avoid using
//                them.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::Render(CCamera *pCamera)
{
	unsigned uError = 0;

	std::vector<CRenderable*>::iterator it;

	int nLightState     = UtilGL::States::Get(UtilGL::States::LIGHTING);
	int nPolygonMode    = UtilGL::States::Get(UtilGL::States::POLYGONMODE);
	int nTexturingState = UtilGL::States::Get(UtilGL::States::TEXTURE2D);

	CObject*         pObject;
	CShader*         pShader;
	CLensFlare*      pLensFlare;
	CParticleSystem* pPartSystem;

	// Check for fog

	if(m_pScene)
	{
		glFogfv(GL_FOG_COLOR, m_pScene->GetFogColor().Data());

		float fViewDepth = 1000.0f;

		if(pCamera)
		{
			fViewDepth = pCamera->Far() - pCamera->Near();
		}

		switch(m_pScene->GetFogType())
		{
			case ID_FOG_LINEAR:

				UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::ENABLED);
				glFogi(GL_FOG_MODE,  GL_LINEAR);

				glFogf(GL_FOG_START, fViewDepth * m_pScene->GetFogNear());
				glFogf(GL_FOG_END,   fViewDepth * m_pScene->GetFogFar());

				break;

			case ID_FOG_EXP:

				UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::ENABLED);
				glFogi(GL_FOG_MODE,    GL_EXP);
				glFogf(GL_FOG_DENSITY, m_pScene->GetFogDensity());

				break;

			case ID_FOG_EXP2:

				UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::ENABLED);
				glFogi(GL_FOG_MODE, GL_EXP2);
				glFogf(GL_FOG_DENSITY, m_pScene->GetFogDensity());

				break;

			default:

				UtilGL::States::Set(UtilGL::States::FOG, UtilGL::States::DISABLED);
				break;
		}
	}

	// Compute view positiosn

	CMatrix mtxView = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW);

	for(it = m_vecRenderables.begin(); it != m_vecRenderables.end(); ++it)
	{
		(*it)->ComputeViewPos(mtxView);
	}

	// Render depending on requested mode:

	switch(m_eRenderMode)
	{

	case MODE_POINTS:
	case MODE_WIREFRAME:
	case MODE_SOLID:

		// Sort renderables. Shouldn't be neccesary if we hadn't lensflares.

		std::sort(m_vecRenderables.begin(), m_vecRenderables.end(), CRenderable::SortPredicate);

		// Set the chosen polygon mode:

		switch(m_eRenderMode)
		{
		case MODE_POINTS:    UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_POINT); break;
		case MODE_WIREFRAME: UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_LINE);  break;
		case MODE_SOLID:     UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_FILL);  break;
		}

		if(m_eRenderMode != MODE_WIREFRAME)
		{
			glEnableClientState(GL_VERTEX_ARRAY);

			if(nLightState == UtilGL::States::ENABLED)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
			}
		}

		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);

		// Lock some states:

		UtilGL::States::Lock(UtilGL::States::POLYGONMODE, true);
		UtilGL::States::Lock(UtilGL::States::TEXTURE2D,   true);

		// Iterate and render

		for(it = m_vecRenderables.begin(); it != m_vecRenderables.end(); ++it)
		{
			(*it)->ExecutePreRenderCallback();

			switch((*it)->GetNodeType())
			{

			case ID_NODE_OBJECT:

				// Object render

				pObject = (CObject*)(*it);

				if(IS_ZERO(pObject->GetAlpha()))
				{
					pObject->ExecutePostRenderCallback();
					continue;
				}

				UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pObject->GetAbsoluteTM());

				if(nLightState == UtilGL::States::ENABLED)
				{
					UtilGL::States::SetMaterialAmbient (GL_FRONT_AND_BACK, CVector4(0.00f, 0.00f, 0.00f, 0.00f));
					UtilGL::States::SetMaterialDiffuse (GL_FRONT_AND_BACK, pObject->GetWireColor());
					UtilGL::States::SetMaterialSpecular(GL_FRONT_AND_BACK, CVector4(0.05f, 0.05f, 0.05f, 0.00f));
					glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10);
				}
				else
				{
					UtilGL::States::SetColor(pObject->GetWireColor());
				}

				m_nTotalObjs++;
				m_nTotalPolys += pObject->GetNumFaces();
				m_nTotalVerts += pObject->GetNumVertices();

				if(pObject->TestVisibility(pCamera))
				{
					if(m_eRenderMode == MODE_WIREFRAME)
					{
						WireRender(pObject, nLightState == UtilGL::States::ENABLED);
					}
					else
					{
						DefaultRender(pObject, nLightState == UtilGL::States::ENABLED);
					}

					m_nDrawnObjs++;
					m_nDrawnPolys += pObject->GetNumFaces();
					m_nDrawnVerts += pObject->GetNumVertices();
				}

				break;

			case ID_NODE_LENSFLARE:

				// Lensflare render

				pLensFlare = (CLensFlare*)(*it);

				m_nTotalPolys += pLensFlare->GetNumComponents();
				m_nTotalVerts += pLensFlare->GetNumComponents() * 4;

				if(!pLensFlare->Render())
				{
					uError++;
				}
				else
				{
					if(pLensFlare->IsVisible())
					{
						m_nDrawnPolys += pLensFlare->GetNumComponents();
						m_nDrawnVerts += pLensFlare->GetNumComponents() * 4;
					}
				}

				break;

			case ID_NODE_PARTICLESYSTEM:

				// Particle system render

				pPartSystem = (CParticleSystem*)(*it);

				m_nTotalPolys += pPartSystem->GetNumParticles();
				m_nTotalVerts += pPartSystem->GetNumParticles() * 4;
				m_nTotalParts += pPartSystem->GetNumParticles();

				UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pPartSystem->GetAbsoluteTM());

				if(!pPartSystem->Render())
				{
					uError++;
				}
				else
				{
					m_nDrawnPolys += pPartSystem->GetNumParticles();
					m_nDrawnVerts += pPartSystem->GetNumParticles() * 4;
					m_nDrawnParts += pPartSystem->GetNumParticles();
				}

				break;

			default:

				// Should not happen
				assert(!"CRenderVisitor::Render(): Unknown node.");
				break;
			}

			(*it)->ExecutePostRenderCallback();
		}

		// Unlock states:

		UtilGL::States::Lock(UtilGL::States::POLYGONMODE, false);
		UtilGL::States::Lock(UtilGL::States::TEXTURE2D,   false);

		if(m_eRenderMode != MODE_WIREFRAME)
		{
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		break;

	case MODE_FULLDETAIL:

		// Sort renderables:

		std::sort(m_vecRenderables.begin(), m_vecRenderables.end(), CRenderable::SortPredicate);

		for(it = m_vecRenderables.begin(); it != m_vecRenderables.end(); ++it)
		{
			UtilGL::States::Set(UtilGL::States::LIGHTING, nLightState);

			(*it)->ExecutePreRenderCallback();

			switch((*it)->GetNodeType())
			{

			case ID_NODE_OBJECT:

				// Object render

				pObject = (CObject*)(*it);
				pShader = const_cast<CShader*>(pObject->GetShader());

				if(IS_ZERO(pObject->GetAlpha()))
				{
					pObject->ExecutePostRenderCallback();
					continue;
				}

				m_nTotalObjs++;

				// TODO: draw objects sharing the same shader reusing the pShader->PrepareStates() call

				if(pShader)
				{
					m_nTotalPolys += pObject->GetNumFaces()    * pShader->GetNumPasses();
					m_nTotalVerts += pObject->GetNumVertices() * pShader->GetNumPasses();

					if(pObject->TestVisibility(pCamera))
					{
						pShader->Begin();

						assert(pObject->GetNumDisplayLists() == pShader->GetNumPasses());

						UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pObject->GetAbsoluteTM());

						for(int nPass = 0; nPass < pShader->GetNumPasses(); nPass++)
						{
							pShader->PrepareStates(nPass);          // Shader specific states
							pShader->PrepareObject(nPass, pObject); // Object specific states

							// If the pass draws dinamic geometry, the returned display list
							// will be -1 and we'll have to call glDrawElements(). If not, then
							// we have a static geometry pass and we can use glCallList.

							int nDisplayList = pObject->GetDisplayList(nPass);

							if(nDisplayList == -1 || pObject->GetNodeSubType() == ID_OBJECT_DYNAMIC)
							{
								glDrawElements(GL_TRIANGLES, pObject->GetNumFaces() * 3, GL_UNSIGNED_INT, pObject->GetFaceList());
							}
							else
							{
								glCallList(nDisplayList);
							}

							m_nDrawnPolys += pObject->GetNumFaces();
							m_nDrawnVerts += pObject->GetNumVertices();
						}

						pShader->End();
						m_nDrawnObjs++;
					}
				}
				else
				{
					m_nTotalPolys += pObject->GetNumFaces();
					m_nTotalVerts += pObject->GetNumVertices();

					if(pObject->TestVisibility(pCamera))
					{
						glEnableClientState(GL_VERTEX_ARRAY);
						if(nLightState == UtilGL::States::ENABLED) glEnableClientState(GL_NORMAL_ARRAY);

						if(nLightState == UtilGL::States::ENABLED)
						{
							UtilGL::States::SetMaterialAmbient (GL_FRONT_AND_BACK, CVector4(0.00f, 0.00f, 0.00f, 0.00f));
							UtilGL::States::SetMaterialDiffuse (GL_FRONT_AND_BACK, pObject->GetWireColor());
							UtilGL::States::SetMaterialSpecular(GL_FRONT_AND_BACK, CVector4(0.05f, 0.05f, 0.05f, 0.00f));
							glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10);
						}
						else
						{
							UtilGL::States::SetColor(pObject->GetWireColor());
						}

						UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pObject->GetAbsoluteTM());
						DefaultRender(pObject, nLightState == UtilGL::States::ENABLED);

						glDisableClientState(GL_VERTEX_ARRAY);
						glDisableClientState(GL_NORMAL_ARRAY);

						m_nDrawnObjs++;
						m_nDrawnPolys += pObject->GetNumFaces();
						m_nDrawnVerts += pObject->GetNumVertices();
					}
				}

				break;

			case ID_NODE_LENSFLARE:

				// Lensflare render

				pLensFlare = (CLensFlare*)(*it);

				m_nTotalPolys += pLensFlare->GetNumComponents();
				m_nTotalVerts += pLensFlare->GetNumComponents() * 4;

				if(!pLensFlare->Render())
				{
					uError++;
				}
				else
				{
					if(pLensFlare->IsVisible())
					{
						m_nDrawnPolys += pLensFlare->GetNumComponents();
						m_nDrawnVerts += pLensFlare->GetNumComponents() * 4;
					}
				}

				break;

			case ID_NODE_PARTICLESYSTEM:

				// Particle system render

				pPartSystem = (CParticleSystem*)(*it);

				m_nTotalPolys += pPartSystem->GetNumParticles();
				m_nTotalVerts += pPartSystem->GetNumParticles() * 4;
				m_nTotalParts += pPartSystem->GetNumParticles();

				UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_WORLD, pPartSystem->GetAbsoluteTM());

				if(!pPartSystem->Render())
				{
					uError++;
				}
				else
				{
					m_nDrawnPolys += pPartSystem->GetNumParticles();
					m_nDrawnVerts += pPartSystem->GetNumParticles() * 4;
					m_nDrawnParts += pPartSystem->GetNumParticles();
				}

				break;

			default:

				// Should not happen
				assert(!"CRenderVisitor::Render(): Unknown node.");
				break;
			}

			(*it)->ExecutePostRenderCallback();
		}

		break;

	default:

		// Should not happen
		assert(!"CRenderVisitor::Render(): Unknown render mode.");
		break;
	}

	UtilGL::States::Set(UtilGL::States::POLYGONMODE, nPolygonMode);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D,   nTexturingState);
	UtilGL::States::Set(UtilGL::States::BLENDING,    UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::FOG,         UtilGL::States::DISABLED);

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool DefaultRender(CObject* pObject, bool bLightingEnabled)
//
//  - Purpose   : Renders an object in default mode.
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::DefaultRender(CObject* pObject, bool bLightingEnabled)
{
	glVertexPointer(3, GL_FLOAT, 0, pObject->GetVertices());

	if(bLightingEnabled)
	{
		glNormalPointer(GL_FLOAT, 0, pObject->GetVertexNormals());
	}

	glDrawRangeElements(GL_TRIANGLES, 0, pObject->GetNumVertices() - 1, pObject->GetNumFaces() * 3, GL_UNSIGNED_INT, pObject->GetFaceList());

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderVisitor
//  - prototype : bool WireRender(CObject* pObject, bool bLightingEnabled)
//
//  - Purpose   : Renders an object in wireframe with no shader, this is an
//				  unoptimized routine only for debug purposes!
//
// -----------------------------------------------------------------------------
bool CRenderVisitor::WireRender(CObject* pObject, bool bLightingEnabled)
{
	glBegin(GL_TRIANGLES);

	unsigned int* puFaces = pObject->GetFaceList();

	for(int uCount = 0; uCount < pObject->GetNumFaces(); uCount++)
	{
		unsigned uCount_3 = uCount * 3;

		if(bLightingEnabled)        glNormal3fv(pObject->GetVertexNormals()[puFaces[uCount_3 + 0]].Data());
		if(pObject->GetEdgeFlags()) glEdgeFlag (pObject->GetEdgeFlags    ()[uCount_3 + 0]);

		glVertex3fv(pObject->GetVertices()[puFaces[uCount_3 + 0]].Data());

		if(bLightingEnabled)        glNormal3fv(pObject->GetVertexNormals()[puFaces[uCount_3 + 1]].Data());
		if(pObject->GetEdgeFlags()) glEdgeFlag (pObject->GetEdgeFlags    ()[uCount_3 + 1]);

		glVertex3fv(pObject->GetVertices()[puFaces[uCount_3 + 1]].Data());

		if(bLightingEnabled)        glNormal3fv(pObject->GetVertexNormals()[puFaces[uCount_3 + 2]].Data());
		if(pObject->GetEdgeFlags()) glEdgeFlag (pObject->GetEdgeFlags    ()[uCount_3 + 2]);

		glVertex3fv(pObject->GetVertices()[puFaces[uCount_3 + 2]].Data());
		
	}

	glEnd();

	return true;
}