// ==[ File ]===================================================================================
//
//  -Name     : BuildNodes.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Scenegraph building.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


// ==[ Implementation ]=========================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool PreProcessNodes(INode* pMaxNode)
//
//  - Purpose   : Preprocess recursively the scene node tree structure to
//                buil the material list and also get number of nodes.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::PreProcessNodes(INode* pMaxNode)
{
	if(!pMaxNode)
	{
		return true;
	}

	if(pMaxNode->GetMtl())
	{
		AddMaterial(pMaxNode->GetMtl());
	}

	// Get the object state at the end of the pipeline.

	ObjectState os = pMaxNode->EvalWorldState(SECONDS_TO_TICKS(m_fStartTime));

	if(os.obj)
	{
		switch(os.obj->SuperClassID())
		{

		case GEOMOBJECT_CLASS_ID:

			if(IsValidGeomObject(pMaxNode))
			{
				if(!pMaxNode->IsHidden())
				{
					m_nNumGeomObjects++;
				}

				AddNodeBones(m_vecBones, pMaxNode);
			}
			else
			{
				if(os.obj->ClassID() == SUPRSPRAY_CLASS_ID)
				{
					if(!pMaxNode->IsHidden())
					{
						m_nNumParticleSystems++;
					}
				}
			}

			break;

		case CAMERA_CLASS_ID:

			m_nNumCameras++;

			break;

		case LIGHT_CLASS_ID:

			if(m_bUseLights)
			{
				m_nNumLights++;
			}

			break;

		case HELPER_CLASS_ID:

			m_nNumHelpers++;

			break;

		default: break;

		}
	}

	if(HasLensFlareUserDef(pMaxNode))
	{
		m_nNumLensFlares++;
	}

	// Recurse into child nodes
	
	for(int nIndex = 0; nIndex < pMaxNode->NumberOfChildren(); nIndex++)
	{
		INode* pChild = pMaxNode->GetChildNode(nIndex);
		PreProcessNodes(pChild);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool BuildSceneData()
//
//  - Purpose   : Builds our scene graph from current MAX scene data.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::BuildSceneData()
{
	m_nNumNodes = 0;
	m_nCurNode  = 0;

	// Preprocess scene. We get the total number of nodes and we also build our material list

	LOG.Write("\n\nPreprocessing nodes: ");

	m_nNumNodes = m_nNumGeomObjects = m_nNumCameras = m_nNumLights = m_nNumHelpers = m_nNumLensFlares = m_nNumParticleSystems = 0;

	PreProcessNodes(m_pMaxInterface->GetRootNode());

	m_nNumNodes = m_nNumGeomObjects + m_nNumCameras + m_nNumLights + m_nNumHelpers + m_nNumLensFlares + m_nNumParticleSystems;
	m_nCurNode  = 0;

	LOG.Write(" Ok. Got %u geomobjects, %u cameras, %u lights %u helpers %u lensflares and %u particle systems (%u total nodes).", m_nNumGeomObjects, m_nNumCameras, m_nNumLights, m_nNumHelpers, m_nNumLensFlares, m_nNumParticleSystems, m_nNumNodes);

	if(m_nNumNodes == 0)
	{
		CLogger::NotifyWindow("No scene data");
		m_pRootNode = NULL;
		return false;
	}

	// Build shaders

	if(!BuildShaders())
	{
		CLogger::NotifyWindow("Error building shaders");
		m_pRootNode = NULL;
		return false;
	}

	// Init a progress bar.

	m_pMaxInterface->ProgressStart("Building scene", TRUE, DummyProc, NULL);

	m_fDuration = m_fEndTime - m_fStartTime;

	m_nNumPosSamples    = (int)(m_fDuration * m_nNumPosSamplesPerSec);
	m_nNumSclSamples    = (int)(m_fDuration * m_nNumSclSamplesPerSec);
	m_nNumRotSamples    = (int)(m_fDuration * m_nNumRotSamplesPerSec);
	m_nNumCamSettings   = (int)(m_fDuration * m_nNumCamSettingsPerSec);
	m_nNumLightSettings = (int)(m_fDuration * m_nNumLightSettingsPerSec);

	// Build scene root node and build recursively the rest of the scene graph

	LOG.Write("\n\nBuilding nodes:\n");

	bool bCancelled = false;
	m_pRootNode     = BuildNode(m_pMaxInterface->GetRootNode(), &bCancelled);

	// Finish

	LOG.Write("\n");
	m_pMaxInterface->ProgressEnd();

	if(m_pRootNode == NULL)
	{
		// Cancel or error

		std::vector<CShaderStandard*>::iterator it;

		for(it = m_vecShaders.begin(); it < m_vecShaders.end(); ++it)
		{
			delete *it;
		}
	}

	return m_pRootNode != NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CNode* BuildNode(INode* pMaxNode, bool* pbCancelled)
//
//  - Purpose   : Recursive function to build our scene graph.
//                Root call should have (maxrootnode) as parameter.
//                Returns a pointer to the root scene node. pbCancelled returns
//                true if the return is NULL due to a user cancel instead of
//                an error.
//
// -----------------------------------------------------------------------------
CNode* CStravaganzaMaxTools::BuildNode(INode* pMaxNode, bool* pbCancelled)
{
	assert(pMaxNode);
	assert(pbCancelled);

	m_pMaxInterface->ProgressUpdate((int)((float)m_nCurNode / (float)m_nNumNodes * 100.0f), FALSE, pMaxNode->GetName());

	CNode* pNode = NULL;

	if(pMaxNode == m_pMaxInterface->GetRootNode())
	{
		// It's the root call so instance a CScene object representing our top
		// (scene) hierarchy node. The rest of the nodes will pend from this one.

		LOG.Write("\nScene Root");

		CScene* pScene = new CScene;

		pScene->SetNodeName("Scene Root");

		Interval validRange = m_pMaxInterface->GetAnimRange();
		Point3   ambient    = m_pMaxInterface->GetAmbient(SECONDS_TO_TICKS(m_fStartTime), validRange);

		pScene->SetAmbientLight(Point3ToVector3(ambient));
		pScene->SetFogType(ID_FOG_NONE);

		// Search for fog in the atmospheric effects list

		for(int i = 0; i < m_pMaxInterface->NumAtmospheric(); i++)
		{
			Atmospheric* pMaxAtmospheric = m_pMaxInterface->GetAtmospheric(i);

			if(pMaxAtmospheric)
			{
				if(pMaxAtmospheric->ClassID() == Class_ID(FOG_CLASS_ID, 0))
				{
					StdFog* pMaxFog = (StdFog*)pMaxAtmospheric;
					Color   fogCol  = pMaxFog->GetColor(SECONDS_TO_TICKS(m_fStartTime));

					pScene->SetFogColor(CVector4(fogCol.r, fogCol.g, fogCol.b, 1.0f));

					float fFogNear, fFogFar, fFogDensity;

					switch(pMaxFog->GetType())
					{

					case 0:

						fFogNear = pMaxFog->GetNear(SECONDS_TO_TICKS(m_fStartTime));
						fFogFar  = pMaxFog->GetFar (SECONDS_TO_TICKS(m_fStartTime));

						pScene->SetFogType(ID_FOG_LINEAR);
						pScene->SetFogNear(fFogNear);
						pScene->SetFogFar (fFogFar);

						break;

					case 1:

						fFogDensity = pMaxFog->GetDensity(SECONDS_TO_TICKS(m_fStartTime));

						pScene->SetFogType   (ID_FOG_EXP);
						pScene->SetFogDensity(fFogDensity);

						break;

					default:

						LOG.Write("\n  WARNING - Unknown fog.");
					}
				}
			}
		}

		pNode = pScene;
	}
	else
	{
		// Evaluate MAX node at the end of the pipeline and instance the right node
		// of our scenegraph depending on type.

		ObjectState os = pMaxNode->EvalWorldState(SECONDS_TO_TICKS(m_fStartTime));
		assert(os.obj);

		switch(os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID:

			if(IsValidGeomObject(pMaxNode))
			{
				if(!pMaxNode->IsHidden())
				{
					std::string strObjName = pMaxNode->GetName();

					bool bBiped = false;

					if(strObjName.length() > 2)
					{
						if((strObjName[0] == 'B' || strObjName[0] == 'b') &&
						   (strObjName[1] == 'I' || strObjName[1] == 'i') &&
						   (strObjName[2] == 'P' || strObjName[2] == 'p'))
						{
							// TODO: hack
							bBiped = true;
						}
					}

					if(!bBiped)
					{
						if(!(pNode = BuildObjectNode(pMaxNode)))
						{
							CLogger::NotifyWindow("Error building object %s", pMaxNode->GetName());
						}
					}
				}

				m_nCurNode++;
			}
			else
			{
				if(os.obj->ClassID() == SUPRSPRAY_CLASS_ID)
				{
					if(!pMaxNode->IsHidden())
					{
						if(!(pNode = BuildParticleSystemNode(pMaxNode)))
						{
							CLogger::NotifyWindow("Error building particle system %s", pMaxNode->GetName());
						}
					}
				}

				m_nCurNode++;
			}

			break;

		case CAMERA_CLASS_ID:

			if(!(pNode = BuildCameraNode(pMaxNode)))
			{
				CLogger::NotifyWindow("Error building camera %s", pMaxNode->GetName());
			}

			m_nCurNode++;
			break;

		case LIGHT_CLASS_ID:

			if(m_bUseLights)
			{
				if(!(pNode = BuildLightNode(pMaxNode)))
				{
					// CLogger::NotifyWindow("Error building light %s", pMaxNode->GetName());
				}

				m_nCurNode++;
			}

			break;

		case HELPER_CLASS_ID:

			if(!(pNode = BuildHelperNode(pMaxNode)))
			{
				CLogger::NotifyWindow("Error building helper %s", pMaxNode->GetName());
			}

			m_nCurNode++;
			break;

		default:

			LOG.Write("\nWARNING - CStravaganzaMaxTools::BuildNode(): %s has unknown node type. Ignoring.", pMaxNode->GetName());
			break;
		}
	}

	// Force instance if pNode hasn't been build. Create a dummy node.

	if(pNode == NULL)
	{
		CHelper* pHelper = new CHelper;
		assert(pHelper);
		pHelper->SetNodeName(pMaxNode->GetName());
		SetNodeUserDefString(pMaxNode, pHelper);
		LOG.Write("\nBuilding object node %s (Dummy node)", pHelper->GetNodeName().data());
		LOG.IncIndentation(2);
		BuildNodeAnimation(pMaxNode, pHelper);
		pNode = pHelper;
		LOG.DecIndentation(2);
	}

	// Lensflare support through user defined string properties

	if(HasLensFlareUserDef(pMaxNode))
	{
		CLensFlare* pLensFlare = BuildLensFlare(pNode->GetUserDefinedString());

		if(pLensFlare)
		{
			pLensFlare->SetNodeName(pNode->GetNodeName() + " (lensflare)");
			pNode->AttachChild(pLensFlare);
			m_nCurNode++;
		}
	}

	// Recursively build subhierarchy

	int nNumChilds = pMaxNode->NumberOfChildren();

	for(int nCount = 0; nCount < nNumChilds && !*pbCancelled; nCount++)
	{
		// Build+Add childs to current node

		LOG.IncIndentation(2);

		INode* pMaxChildNode = pMaxNode->GetChildNode(nCount);
		CNode* pChild        = BuildNode(pMaxChildNode, pbCancelled);

		LOG.DecIndentation(2);

		if(pChild)
		{
			pNode->AttachChild(pChild);
		}
	}

	// User cancelled?

	if(m_pMaxInterface->GetCancel())
	{
		*pbCancelled = true;
		SAFE_DELETE(pNode);
		return NULL;
	}

	return pNode;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : void SetNodeUserDefString(INode* pMaxNode, CNode* pNode) 
//
//  - Purpose   : Sets the user defined string of a node, getting it from
//                the user defined properties buffer of a MAX node.
//
// -----------------------------------------------------------------------------
void CStravaganzaMaxTools::SetNodeUserDefString(INode* pMaxNode, CNode* pNode)
{
	TSTR buffer;
	pMaxNode->GetUserPropBuffer(buffer);

	if(buffer.data())
	{
		pNode->SetUserDefinedString(buffer.data());
	}
	else
	{
		pNode->SetUserDefinedString("");
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool IsValidGeomObject(INode* pMaxNode)
//
//  - Purpose   : Checks if a given node is a valid geometric object. This
//                test has to be made because camera targets are also geometric
//                objects for MAX.
//                We also discard objects whose prefix is equal to the one
//                indicated in the export dialog box.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::IsValidGeomObject(INode* pMaxNode)
{
	if(pMaxNode == NULL)
	{
		return false;
	}

	ObjectState os = pMaxNode->EvalWorldState(SECONDS_TO_TICKS(m_fStartTime));

	if(!os.obj)
	{
		return false;
	}

	// Camera targets are also geometric objects for MAX.

	if(os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
	{
		return false;
	}

	if(os.obj->IsParticleSystem())
	{
		return false;
	}

/*
	std::string strObjName = pMaxNode->GetName();

	if(m_strSkipPrefix.length() > strObjName.length())
	{
		return false;
	}
	
	if(m_strSkipPrefix.compare(0, m_strSkipPrefix.length(), strObjName) == 0)
	{
		return false;
	}
*/
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CObject* BuildObjectNode(INode* pMaxObjectNode)
//
//  - Purpose   : Builds a CObject from a MAX geometric object node.
//
// -----------------------------------------------------------------------------
CObject* CStravaganzaMaxTools::BuildObjectNode(INode* pMaxObjectNode)
{
	assert(pMaxObjectNode);

	LOG.Write("\nBuilding object node %s", pMaxObjectNode->GetName());
	LOG.IncIndentation(2);

	TSTR buffer;
	pMaxObjectNode->GetUserPropBuffer(buffer);

	CObject* pObject;

	if(strstr(buffer.data(), "dynamic") || GetPhysiqueModifier(pMaxObjectNode))
	{
		LOG.Write(" (dynamic mesh)");
		pObject = new CObject(false);
	}
	else
	{
		LOG.Write(" (static mesh)");
		pObject = new CObject(true);
	}

	if(strstr(buffer.data(), "nolighting"))
	{
		LOG.Write(" (no lighting)");
		pObject->SetAffectLighting(false);
	}

	// Name and user defined string

	pObject->SetNodeName(pMaxObjectNode->GetName());
	SetNodeUserDefString(pMaxObjectNode, pObject);

	// Animation:

	if(!BuildNodeAnimation(pMaxObjectNode, pObject))
	{
		LOG.DecIndentation(2);
		delete pObject;
		return NULL;
	}

	// Shader

	if(pMaxObjectNode->GetMtl())
	{
		pObject->SetShaderName(pMaxObjectNode->GetMtl()->GetName().data());
		pObject->SetShader(GetShader(pMaxObjectNode->GetMtl()->GetName().data()));
	}

	// Mesh

	if(!BuildMesh(pMaxObjectNode, pObject))
	{
		LOG.DecIndentation(2);
		delete pObject;
		return NULL;
	}

	if(pObject->GetBoundingVolume())
	{
		pObject->GetBoundingVolume()->Build(pObject);
	}

	LOG.DecIndentation(2);

	return pObject;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CCamera* BuildCameraNode(INode* pMaxCameraNode)
//
//  - Purpose   : Builds a CCamera from a MAX camera node.
//
// -----------------------------------------------------------------------------
CCamera* CStravaganzaMaxTools::BuildCameraNode(INode* pMaxCameraNode)
{
	assert(pMaxCameraNode);

	LOG.Write("\nBuilding camera node %s", pMaxCameraNode->GetName());
	LOG.IncIndentation(2);

	CCamera* pCamera = new CCamera;

	// Name and user defined string

	pCamera->SetNodeName(pMaxCameraNode->GetName());
	SetNodeUserDefString(pMaxCameraNode, pCamera);

	// Build settings + animation.

	if(!BuildNodeAnimation(pMaxCameraNode, pCamera))
	{
		LOG.DecIndentation(2);
		delete pCamera;
		return NULL;
	}

	if(!BuildCameraSettings(pMaxCameraNode, pCamera))
	{
		LOG.DecIndentation(2);
		delete pCamera;		
		return NULL;
	}

	LOG.DecIndentation(2);

	return pCamera;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CLight* BuildLightNode(INode* pMaxLightNode)
//
//  - Purpose   : Builds a CLight from a MAX light node.
//
// -----------------------------------------------------------------------------
CLight* CStravaganzaMaxTools::BuildLightNode(INode* pMaxLightNode)
{
	assert(pMaxLightNode);

	LOG.Write("\nBuilding light node %s", pMaxLightNode->GetName());
	LOG.IncIndentation(2);

	CLight* pLight = new CLight;

	// Name and user defined string

	pLight->SetNodeName(pMaxLightNode->GetName());
	SetNodeUserDefString(pMaxLightNode, pLight);

	// Settings + animation.

	if(!BuildNodeAnimation(pMaxLightNode, pLight))
	{
		LOG.DecIndentation(2);
		delete pLight;
		return NULL;
	}

	if(!BuildLightSettings(pMaxLightNode, pLight))
	{
		LOG.DecIndentation(2);
		delete pLight;
		return NULL;
	}

	LOG.DecIndentation(2);

	return pLight;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CHelper* BuildHelperNode(INode* pMaxHelperNode)
//
//  - Purpose   : Builds a CHelper from a MAX helper node.
//
// -----------------------------------------------------------------------------
CHelper* CStravaganzaMaxTools::BuildHelperNode(INode* pMaxHelperNode)
{
	assert(pMaxHelperNode);

	LOG.Write("\nBuilding helper node %s", pMaxHelperNode->GetName());
	LOG.IncIndentation(2);

	CHelper* pHelper = new CHelper;

	// Name and user defined string

	pHelper->SetNodeName(pMaxHelperNode->GetName());
	SetNodeUserDefString(pMaxHelperNode, pHelper);
	
	// Animation.

	if(!BuildNodeAnimation(pMaxHelperNode, pHelper))
	{
		LOG.DecIndentation(2);
		delete pHelper;
		return NULL;
	}

	LOG.DecIndentation(2);

	return pHelper;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CParticleSystemStd* BuildParticleSystemNode(INode* pMaxSuperSprayNode)
//
//  - Purpose   : Builds a CHelper from a MAX helper node.
//
// -----------------------------------------------------------------------------
CParticleSystemStd* CStravaganzaMaxTools::BuildParticleSystemNode(INode* pMaxSuperSprayNode)
{
	assert(pMaxSuperSprayNode);

	LOG.Write("\nBuilding superspray node %s", pMaxSuperSprayNode->GetName());
	LOG.IncIndentation(2);

	CParticleSystemStd* pPSystem = new CParticleSystemStd;

	// Name and user defined string

	pPSystem->SetNodeName(pMaxSuperSprayNode->GetName());
	SetNodeUserDefString(pMaxSuperSprayNode, pPSystem);
	
	// Animation

	if(!BuildNodeAnimation(pMaxSuperSprayNode, pPSystem))
	{
		LOG.DecIndentation(2);
		delete pPSystem;
		return NULL;
	}

	// Convert

	TimeValue t = SECONDS_TO_TICKS(m_fStartTime);

	Object *pObject = pMaxSuperSprayNode->EvalWorldState(t).obj;
	SimpleParticle* pMaxSuperSpray;
	bool bDelete = false;

	// Is it superspray?
	if(pObject->CanConvertToType(SUPRSPRAY_CLASS_ID))
	{
		// Ok, it's superspray but we cast it to a SimpleParticle (Superspray isn't part of
		// the MAX SDK API, it's a plugin which its sourcecode in the samples folder).
		// SimpleParticle has the parameter array that stores parameters from some child
		// particle systems like this (SimpleParticle::pblock).
		pMaxSuperSpray = (SimpleParticle*)pObject->ConvertToType(t, SUPRSPRAY_CLASS_ID);

		if(pMaxSuperSpray != pObject)
		{
			bDelete = true;
		}
	}
	else
	{
		LOG.DecIndentation(2);
		delete pPSystem;
		return NULL;
	}

	// Build

	// Particle basic params

	float     fVal, fVal2;
	int       nVal;
	TimeValue timeVal, timeVal2;

	IParamArray* pBlock = pMaxSuperSpray->pblock;
	TimeValue    time   = SECONDS_TO_TICKS(m_fStartTime);

	std::string strTexture = "";
	CVector4    v4PartColor(1.0f, 1.0f, 1.0f, 1.0f);

	if(pPSystem->GetUserDefinedString() != "")
	{
		// By the user defined properties from the node, in MAX, we can add
		// some other properties that the superspray lacks.
		//      -Emitter radius
		//      -Texture (*)
		//      -Particle color
		//      -Affect lighting
		//      -Acceleration
		//
		// (*) I first got the texture through the material of the object
		//     indicated in the instanced geometry property. To simplify things
		//     (and avoid too much max's superspray code dependency, which doesn't
		//     belong to the MAX SDK but to the sample code) now I don't
		//     have access to the SuprSprayParticle interface and thus to the texture.
		//     (Though it could have been done if the node name was stored in the
		//     SimpleParticle::pblock parameter array instead of as a pointer in the
		//     SuprSprayParticle interface).

		CAdvancedParser parser;

		CInputMemory inputmem((char *)pPSystem->GetUserDefinedString().data(), pPSystem->GetUserDefinedString().length());
		parser.Init(&inputmem);

		if(parser.ReadKeyFloat("#radius", '=', &fVal, CParser::SEARCH_WHOLEDATA))
		{
			pPSystem->SetEmitRadius(fVal);
		}

		if(parser.ReadKeyString("#texture", '=', &strTexture, CParser::SEARCH_WHOLEDATA))
		{
			// In export mode, copy the texture to the texture folder if specified

			if(!m_bPreview)
			{
				std::string strDestPath = m_strTexturePath;

				if(strDestPath[strDestPath.length() - 1] != '\\')
				{
					strDestPath.append("\\", 1);
				}

				TSTR texture(strTexture.data()), path, filename;
				SplitPathFile(texture, &path, &filename);

				strDestPath = strDestPath + filename.data();

				if(m_bCopyTextures && m_strTexturePath != "")
				{
					if(!CopyFile(strTexture.data(), strDestPath.data(), FALSE))
					{
						CLogger::NotifyWindow("Unable to copy %s to %s", strTexture.data(), strDestPath.data());
					}
				}

				// In preview mode, strTexture = full path, in export only the filename.

				strTexture = filename.data();
			}
		}

		parser.ReadKeyVector4("#color", '=', &v4PartColor, CParser::SEARCH_WHOLEDATA);

		bool bMult;

		if(parser.ReadKeyBool("#blendmult", '=', &bMult, CParser::SEARCH_WHOLEDATA))
		{
			pPSystem->SetBlendMult(bMult);
		}

		CVector3 v3Acceleration;

		if(parser.ReadKeyVector3("#acceleration", '=', &v3Acceleration, CParser::SEARCH_WHOLEDATA))
		{
			pPSystem->SetPartWorldAccel(v3Acceleration);
		}

		bool bAffectLighting;

		if(parser.ReadKeyBool("#affectlighting", '=', &bAffectLighting, CParser::SEARCH_WHOLEDATA))
		{
			pPSystem->SetAffectLighting(bAffectLighting);
		}

		parser.Close();
		inputmem.Close();
	}

	pPSystem->SetPartTexture(strTexture);
	pPSystem->SetPartColor  (v4PartColor);

	pBlock->GetValue(PB_OFFAXIS,	time, fVal,  FOREVER);
	pBlock->GetValue(PB_AXISSPREAD,	time, fVal2, FOREVER); pPSystem->SetEmitOffAxis (fVal * RAD_TO_DEG, fVal2 * RAD_TO_DEG);
	pBlock->GetValue(PB_OFFPLANE,	time, fVal,  FOREVER);
	pBlock->GetValue(PB_PLANESPREAD,time, fVal2, FOREVER); pPSystem->SetEmitOffPlane(fVal * RAD_TO_DEG, fVal2 * RAD_TO_DEG);
	pBlock->GetValue(PB_SPEED,		time, fVal,  FOREVER);
	pBlock->GetValue(PB_SPEEDVAR,	time, fVal2, FOREVER); pPSystem->SetPartSpeed(fVal / TICKS_TO_SECONDS(GetTicksPerFrame()), fVal2 * 0.01f);

	// Particle generation

	pBlock->GetValue(PB_BIRTHMETHOD, time, nVal, FOREVER);

	if(!nVal)
	{
		pBlock->GetValue(PB_PBIRTHRATE, time, nVal, FOREVER);
		pPSystem->SetNumParticles(nVal / TICKS_TO_SECONDS(GetTicksPerFrame()), true);
	}
	else
	{
		pBlock->GetValue(PB_PTOTALNUMBER,time, nVal, FOREVER);
		pPSystem->SetNumParticles(nVal, false);
	}

	pBlock->GetValue(PB_EMITSTART,	time, timeVal,  FOREVER); pPSystem->SetEmitStart(TICKS_TO_SECONDS(timeVal));
	pBlock->GetValue(PB_EMITSTOP,	time, timeVal,  FOREVER); pPSystem->SetEmitStop (TICKS_TO_SECONDS(timeVal));
	pBlock->GetValue(PB_LIFE,		time, timeVal,  FOREVER);
	pBlock->GetValue(PB_LIFEVAR,	time, timeVal2, FOREVER); pPSystem->SetPartLife(TICKS_TO_SECONDS(timeVal), TICKS_TO_SECONDS(timeVal2) / TICKS_TO_SECONDS(timeVal));
	pBlock->GetValue(PB_SIZE,		time, fVal,     FOREVER);
	pBlock->GetValue(PB_SIZEVAR,	time, fVal2,    FOREVER); pPSystem->SetPartSize(fVal, fVal2 * 0.01f);
	pBlock->GetValue(PB_GROWTIME,	time, timeVal,  FOREVER); pPSystem->SetPartGrowEnd  (TICKS_TO_SECONDS(timeVal));
	pBlock->GetValue(PB_FADETIME,	time, timeVal,  FOREVER); pPSystem->SetPartFadeStart(TICKS_TO_SECONDS(timeVal)); pPSystem->SetPartFadeSize(true);
	pBlock->GetValue(PB_RNDSEED,	time, nVal,     FOREVER); pPSystem->SetSeed(nVal);

	// Particle type

	pBlock->GetValue(PB_PARTICLECLASS, time, nVal, FOREVER);
	pBlock->GetValue(PB_PARTICLETYPE,  time, nVal, FOREVER);

	// Particle bubble motion

	pBlock->GetValue(PB_BUBLAMP, time, fVal, FOREVER);

	if(fVal > ALMOST_ZERO)
	{
		pBlock->GetValue(PB_BUBLAMPVAR,	time, fVal2,   FOREVER); pPSystem->SetBubbleAmp   (fVal, fVal2 * 0.01);
		pBlock->GetValue(PB_BUBLPER,	time, timeVal, FOREVER);
		pBlock->GetValue(PB_BUBLPERVAR,	time, fVal2,   FOREVER); pPSystem->SetBubblePeriod(TICKS_TO_SECONDS(timeVal), fVal2 * 0.01);
		pBlock->GetValue(PB_BUBLPHAS,	time, fVal,    FOREVER);
		pBlock->GetValue(PB_BUBLPHASVAR,time, fVal2,   FOREVER); pPSystem->SetBubblePhase (fVal * RAD_TO_DEG, fVal2);
	}

	LOG.DecIndentation(2);

	if(bDelete)
	{
		delete pMaxSuperSpray;
	}

	return pPSystem;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool BuildMesh(INode* pMaxObjectNode, CObject* pObject)
//
//  - Purpose   : Builds the mesh of a given object.
//
// -----------------------------------------------------------------------------

#define MESHPREPROC_MAXDEEP 2000 // -1 means search whole list, else search within a limit

struct SSharedVertex
{
	int nIndex;
	int smGroup;
};

bool CStravaganzaMaxTools::BuildMesh(INode* pMaxObjectNode, CObject* pObject)
{
	assert(pMaxObjectNode);
	assert(pObject);

	int  nCount, nCount3, nFCount;
	int  nVtx1 = 0;
	int  nVtx2 = 1;
	int  nVtx3 = 2;
	bool bNeedDel;
	bool bNegScaling = TMNegParity(pMaxObjectNode->GetObjectTM(SECONDS_TO_TICKS(m_fStartTime)));

	ObjectState os = pMaxObjectNode->EvalWorldState(SECONDS_TO_TICKS(m_fStartTime));

	if(!os.obj || os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
	{
		return false;
	}

	if(bNegScaling) // Adjust indices when negative scaling present (to compute vertex normals properly).
	{
		nVtx1 = 2;
		nVtx2 = 1;
		nVtx3 = 0;
	}

	TriObject *pTriObject = GetTriObjectFromNode(pMaxObjectNode, SECONDS_TO_TICKS(m_fStartTime), &bNeedDel);
	
	if(!pTriObject)
	{
		return false;
	}

	Mesh *pMaxMesh = &pTriObject->GetMesh();

	if(pMaxMesh->getNumVerts() < 1)
	{
		if(bNeedDel)
		{
			delete pTriObject;
		}
		return false;
	}

	pMaxMesh->buildNormals();

	// If mesh has material, check which mesh data we have to export.

	CShaderStandard* pShader = NULL;

	if(pMaxObjectNode->GetMtl())
	{
		pShader = GetShader(pMaxObjectNode->GetMtl()->GetName().data());
	}

	bool bExportCVertChannel  = false;
	bool bExportDiffUVChannel = false;
	bool bExportBumpUVChannel = false;
	bool bExportMap2UVChannel = false;
	bool bHasDiff             = false;
	bool bHasBump             = false;
	bool bHasMap2             = false;

	bool bNeedsPreprocess     = false; // Texture vertices and geometric vertices are not the same number?

	unsigned uDiffUVChannel = -1;
	unsigned uBumpUVChannel = -1;
	unsigned uMap2UVChannel = -1;

	if(pMaxMesh->numCVerts > 0)
	{
		bExportCVertChannel = true;

		if(pMaxMesh->numCVerts != pMaxMesh->getNumVerts())
		{
			bNeedsPreprocess = true;
		}
	}

	if(pShader)
	{
		LOG.Write("\nProcessing shader");

		bHasDiff = pShader->HasLayer(CShaderStandard::LAYER_DIFF);
		bHasBump = pShader->HasLayer(CShaderStandard::LAYER_BUMP);
		bHasMap2 = pShader->HasLayer(CShaderStandard::LAYER_MAP2);

		CShaderStandard::SLayerInfo layerInfo;

		if(bHasDiff)
		{
			pShader->GetLayer(CShaderStandard::LAYER_DIFF, &layerInfo);
			bExportDiffUVChannel = layerInfo.eUVGen == CShaderStandard::UVGEN_EXPLICITMAPPING;
			uDiffUVChannel       = layerInfo.uMapChannel;

			if(bExportDiffUVChannel && pMaxMesh->getNumMapVerts(uDiffUVChannel) == 0)
			{
				CLogger::ErrorWindow("Object %s has material with diffuse, but no mapping values. Skipping", pMaxObjectNode->GetName());

				if(bNeedDel)
				{
					delete pTriObject;
				}

				return false;
			}

			if(bExportDiffUVChannel && (pMaxMesh->getNumVerts() != pMaxMesh->getNumMapVerts(uDiffUVChannel)))
			{
				bNeedsPreprocess = true;
			}
		}
		if(bHasBump)
		{
			pShader->GetLayer(CShaderStandard::LAYER_BUMP, &layerInfo);
			bExportBumpUVChannel = layerInfo.eUVGen == CShaderStandard::UVGEN_EXPLICITMAPPING;
			uBumpUVChannel       = layerInfo.uMapChannel;

			if(bExportBumpUVChannel && (pMaxMesh->getNumVerts() != pMaxMesh->getNumMapVerts(uBumpUVChannel)))
			{
				bNeedsPreprocess = true;
			}
			else if(bExportDiffUVChannel && (pMaxMesh->getNumMapVerts(uDiffUVChannel) != pMaxMesh->getNumMapVerts(uBumpUVChannel)))
			{
				bNeedsPreprocess = true;
			}

			if(bExportBumpUVChannel && pMaxMesh->getNumMapVerts(uBumpUVChannel) == 0)
			{
				CLogger::ErrorWindow("Object %s has material with bump, but no mapping values. Skipping", pMaxObjectNode->GetName());

				if(bNeedDel)
				{
					delete pTriObject;
				}

				return false;
			}
		}
		if(bHasMap2)
		{
			pShader->GetLayer(CShaderStandard::LAYER_MAP2, &layerInfo);
			bExportMap2UVChannel = layerInfo.eUVGen == CShaderStandard::UVGEN_EXPLICITMAPPING;
			uMap2UVChannel       = layerInfo.uMapChannel;

			if(bExportMap2UVChannel && (pMaxMesh->getNumVerts() != pMaxMesh->getNumMapVerts(uMap2UVChannel)))
			{
				bNeedsPreprocess = true;
			}
			else if(bExportDiffUVChannel&& (pMaxMesh->getNumMapVerts(uDiffUVChannel) != pMaxMesh->getNumMapVerts(uMap2UVChannel)))
			{
				bNeedsPreprocess = true;
			}

			if(bExportMap2UVChannel && pMaxMesh->getNumMapVerts(uMap2UVChannel) == 0)
			{
				CLogger::ErrorWindow("Object %s has material with map2 (lightmap/reflection), but no mapping values. Skipping", pMaxObjectNode->GetName());

				if(bNeedDel)
				{
					delete pTriObject;
				}

				return false;
			}
		}
	}

	// MESH CONSOLIDATION: Process smoothing groups and non shared vertex data

	typedef std::vector<SSharedVertex> SVERTEXVECTOR;

	std::vector<Face>          vecFaceList;
	std::vector<Point3>        vecVtxList;
	std::vector<int>		   vecVNormalRefs;
	std::vector<int>           vecVtxRemapList;      // A table with the topology change
	std::vector<SVERTEXVECTOR> vecNonSharedVertices; // For smoothing groups
	SVERTEXVECTOR::iterator it;

	std::vector<Color>  vecColList;
	std::vector<Point3> vecDiffUVList;
	std::vector<Point3> vecBumpUVList;
	std::vector<Point3> vecMap2UVList;

	// Build list of vertices that are shared through faces with a different smoothing group.
	// We must duplicate those vertices later, since they have a different normal for each face.

	LOG.Write("\nConsoliding mesh:\n  -In  : %u faces, %u verts, %u diffuse, %u bump, %u map2",
				pMaxMesh->getNumFaces(), pMaxMesh->getNumVerts(),
				pMaxMesh->getNumMapVerts(uDiffUVChannel),
				pMaxMesh->getNumMapVerts(uBumpUVChannel),
				pMaxMesh->getNumMapVerts(uMap2UVChannel));

	vecNonSharedVertices.resize(pMaxMesh->getNumVerts());

	for(nFCount = 0; nFCount < pMaxMesh->getNumFaces(); nFCount++)
	{
		Face face = pMaxMesh->faces[nFCount];

		int v[3];

		for(nCount3 = 0; nCount3 < 3; nCount3++)
		{
			bool bFound = false;
			int  nIndex;

			// Search for this smoothing group in our list

			for(it  = vecNonSharedVertices[face.v[nCount3]].begin();
				(it != vecNonSharedVertices[face.v[nCount3]].end()) && !bFound;
				++it)
			{
					if(it->smGroup == face.smGroup)
					{
						nIndex  = it->nIndex;
						bFound  = true;
					}
			}

			if(!bFound)
			{
				// Not found, new smoothing group so create and index.

				int nOldIndex = face.v[nCount3];

				vecVtxList.push_back(pMaxMesh->verts[nOldIndex]);
				vecVtxRemapList.push_back(nOldIndex);
				vecVNormalRefs.push_back(-1);  // Own vertex normal

				SSharedVertex sharedVertex;
				sharedVertex.nIndex  = vecVtxList.size() - 1;
				sharedVertex.smGroup = face.smGroup;
				vecNonSharedVertices[face.v[nCount3]].push_back(sharedVertex);

				v[nCount3] = sharedVertex.nIndex;

				// Add other data

				if(bExportCVertChannel)  vecColList.push_back   (pMaxMesh->vertCol [pMaxMesh->vcFace                  [nFCount].t[nCount3]]);
				if(bExportDiffUVChannel) vecDiffUVList.push_back(pMaxMesh->mapVerts(uDiffUVChannel)[pMaxMesh->mapFaces(uDiffUVChannel)[nFCount].t[nCount3]]);
				if(bExportBumpUVChannel) vecBumpUVList.push_back(pMaxMesh->mapVerts(uBumpUVChannel)[pMaxMesh->mapFaces(uBumpUVChannel)[nFCount].t[nCount3]]);
				if(bExportMap2UVChannel) vecMap2UVList.push_back(pMaxMesh->mapVerts(uMap2UVChannel)[pMaxMesh->mapFaces(uMap2UVChannel)[nFCount].t[nCount3]]);
			}
			else
			{
				// Vertex already stored with this smoothing
				//   -Index if the rest of vertex data is the same as the original mesh
				//   -Create+index otherwise

				bool bEqual = true;

				if(bExportCVertChannel)
				{
					if(!Point3Equal(vecColList[nIndex], pMaxMesh->vertCol[pMaxMesh->vcFace[nFCount].t[nCount3]]))
					{
						bEqual = false;
					}
				}

				if(bExportDiffUVChannel && bEqual)
				{
					if(!Point3Equal(vecDiffUVList[nIndex], pMaxMesh->mapVerts(uDiffUVChannel)[pMaxMesh->mapFaces(uDiffUVChannel)[nFCount].t[nCount3]]))
					{
						bEqual = false;
					}
				}

				if(bExportBumpUVChannel && bEqual)
				{
					if(!Point3Equal(vecBumpUVList[nIndex], pMaxMesh->mapVerts(uBumpUVChannel)[pMaxMesh->mapFaces(uBumpUVChannel)[nFCount].t[nCount3]]))
					{
						bEqual = false;
					}
				}

				if(bExportMap2UVChannel && bEqual)
				{
					if(!Point3Equal(vecMap2UVList[nIndex], pMaxMesh->mapVerts(uMap2UVChannel)[pMaxMesh->mapFaces(uMap2UVChannel)[nFCount].t[nCount3]]))
					{
						bEqual = false;
					}
				}

				if(bEqual)
				{
					v[nCount3] = nIndex;
				}
				else
				{
					// Vertex

					vecVtxList.push_back(pMaxMesh->verts[face.v[nCount3]]);
					vecVtxRemapList.push_back(face.v[nCount3]);

					vecVNormalRefs.push_back(nIndex); // Will get the vertex normal from here

					v[nCount3] = vecVtxList.size() - 1;

					// Rest

					if(bExportCVertChannel)  vecColList.push_back   (pMaxMesh->vertCol [pMaxMesh->vcFace                  [nFCount].t[nCount3]]);
					if(bExportDiffUVChannel) vecDiffUVList.push_back(pMaxMesh->mapVerts(uDiffUVChannel)[pMaxMesh->mapFaces(uDiffUVChannel)[nFCount].t[nCount3]]);
					if(bExportBumpUVChannel) vecBumpUVList.push_back(pMaxMesh->mapVerts(uBumpUVChannel)[pMaxMesh->mapFaces(uBumpUVChannel)[nFCount].t[nCount3]]);
					if(bExportMap2UVChannel) vecMap2UVList.push_back(pMaxMesh->mapVerts(uMap2UVChannel)[pMaxMesh->mapFaces(uMap2UVChannel)[nFCount].t[nCount3]]);
				}
			}
/*
			LOG.Write("\n%u %u(%f, %f, %f)->%u(%f, %f, %f)", nFCount,
																face.v[nCount3],
																pMaxMesh->verts[face.v[nCount3]].x,
																pMaxMesh->verts[face.v[nCount3]].y,
																pMaxMesh->verts[face.v[nCount3]].z,
																v[nCount3],
																vecVtxList[v[nCount3]].x,
																vecVtxList[v[nCount3]].y,
																vecVtxList[v[nCount3]].z);
*/																
		}

		Face newFace = face;
		newFace.v[0] = v[0];
		newFace.v[1] = v[1];
		newFace.v[2] = v[2];

		vecFaceList.push_back(newFace);
	}

	LOG.Write("\n  -Out : %u faces, %u verts, %u diffuse, %u bump, %u map2",
				vecFaceList.size(), vecVtxList.size(), vecDiffUVList.size(), vecBumpUVList.size(), vecMap2UVList.size());

	DWORD dColor = pMaxObjectNode->GetWireColor();
	pObject->SetWireColor(CVector4(GetRValue(dColor) / 255.0f, GetGValue(dColor) / 255.0f, GetBValue(dColor) / 255.0f, 1.0f));

	// PHYSIQUE

	int nBones = 0;

	std::vector<std::string>    vecBoneNames;
	std::vector<CBlendedVertex> vecBlendedVertices;

	if(GetPhysiqueModifier(pMaxObjectNode))
	{
		nBones = BuildPhysiqueData(pMaxObjectNode, pObject, vecBoneNames, vecBlendedVertices);

		if(nBones == 0)
		{
			LOG.Write("\nWARNING - Error building physique data");
		}
	}

	// COPY DATA /////////

	LOG.Write("\nCopying mesh data: ");

	// Vertex list:

	LOG.Write("vtx..");
	CVector3* pVertices = new CVector3[vecVtxList.size()];
	assert(pVertices);

	for(nCount = 0; nCount < vecVtxList.size(); nCount++)
	{
		pVertices[nCount] = Point3ToVector3(vecVtxList[nCount]);
	}

	pObject->SetVertices(vecVtxList.size(), pVertices);

	// Reference to vertex normals (duplicated vertex that lost topology information to average face normals)

	if(vecVNormalRefs.size() > 1)
	{
		LOG.Write("vnref..");

		int* pnVNormalRefs = new int[vecVNormalRefs.size()];
		assert(pnVNormalRefs);
		assert(vecVNormalRefs.size() == vecVtxList.size());

		for(nCount = 0; nCount < vecVNormalRefs.size(); nCount++)
		{
			pnVNormalRefs[nCount] = vecVNormalRefs[nCount];
		}

		pObject->SetVNormalRefs(pnVNormalRefs);
	}

	// Vertex color list:

	if(pMaxMesh->numCVerts == vecVtxList.size() && !bNeedsPreprocess)
	{
		LOG.Write("vcol..");

		CVector4* pVertexColors = new CVector4[pMaxMesh->numCVerts];
		assert(pVertexColors);

		for(nCount = 0; nCount < pMaxMesh->numCVerts; nCount++)
		{
			Color col = pMaxMesh->vertCol[nCount];
			pVertexColors[nCount].Set(col.r, col.g, col.b, 1.0f);
		}

		pObject->SetVertexColors(pVertexColors);
	}
	
	// Diffuse channel

	if(vecDiffUVList.size() > 0)
	{
		LOG.Write("diff..");

		CVector2* pv2DiffMapping = new CVector2[vecDiffUVList.size()];
		assert(pv2DiffMapping);

		for(nCount = 0; nCount < vecDiffUVList.size(); nCount++)
		{
			pv2DiffMapping[nCount].Set(vecDiffUVList[nCount].x, vecDiffUVList[nCount].y);
		}

		pObject->SetMapChannel(uDiffUVChannel, pv2DiffMapping);
	}

	// Bump map channel

	if(vecBumpUVList.size() > 0)
	{
		LOG.Write("bump..");

		CVector2* pBumpMapping = new CVector2[vecBumpUVList.size()];
		assert(pBumpMapping);

		for(nCount = 0; nCount < vecBumpUVList.size(); nCount++)
		{
			pBumpMapping[nCount].Set(vecBumpUVList[nCount].x, vecBumpUVList[nCount].y);
		}

		pObject->SetMapChannel(uBumpUVChannel, pBumpMapping);
	}

	// Map channel 2

	if(vecMap2UVList.size() > 0)
	{
		LOG.Write("map2..");

		CVector2* pv2Map2Mapping = new CVector2[vecMap2UVList.size()];
		assert(pv2Map2Mapping);

		for(nCount = 0; nCount < vecMap2UVList.size(); nCount++)
		{
			pv2Map2Mapping[nCount].Set(vecMap2UVList[nCount].x, vecMap2UVList[nCount].y);
		}

		pObject->SetMapChannel(uMap2UVChannel, pv2Map2Mapping);
	}

	// Edge flags

	LOG.Write("edgef..");

	GLboolean* pglbEdgeFlags = new GLboolean[vecFaceList.size() * 3];

	for(nCount = 0; nCount < vecFaceList.size(); nCount++)
	{
		int a = 0, b = 1, c = 2;

		if(bNegScaling)
		{
			b = 0; a = 1; c = 2;
		}

		pglbEdgeFlags[(nCount * 3) + 0] = vecFaceList[nCount].getEdgeVis(a) ? GL_TRUE : GL_FALSE;
		pglbEdgeFlags[(nCount * 3) + 1] = vecFaceList[nCount].getEdgeVis(b) ? GL_TRUE : GL_FALSE;
		pglbEdgeFlags[(nCount * 3) + 2] = vecFaceList[nCount].getEdgeVis(c) ? GL_TRUE : GL_FALSE;
	}

	pObject->SetEdgeFlags(pglbEdgeFlags);

	// Face list with corrected order if we have negative scaling:

	LOG.Write("faces..");

	unsigned* puFaces = new unsigned[vecFaceList.size() * 3];
	assert(puFaces);

	for(nCount = 0; nCount < vecFaceList.size(); nCount++)
	{
		Face face = vecFaceList[nCount];

		puFaces[(nCount * 3) + 0] = face.v[nVtx1]; // A index
		puFaces[(nCount * 3) + 1] = face.v[nVtx2]; // B index
		puFaces[(nCount * 3) + 2] = face.v[nVtx3]; // C index
	}

	pObject->SetFaceList(vecFaceList.size(), puFaces);

	// Bone data

	if(nBones > 0)
	{
		LOG.Write("bones..");

		// As we modified original topology in our mesh consolidation we need to remap
		// physique vertex data.

		if(vecBlendedVertices.size() != pMaxMesh->getNumVerts())
		{
			CLogger::NotifyWindow("Object %s vertex count doesn't match physique vertex count (%u vs %u).\nThis can be caused by some mesh modifier. Ignoring bone data.", pMaxObjectNode->GetName(), pMaxMesh->getNumVerts(), vecBlendedVertices.size());
		}
		else
		{
			std::vector<int>::iterator remapIt;

			CBlendedVertex* pBlendedVertices = new CBlendedVertex[vecVtxList.size()];
			assert(pBlendedVertices);

			nCount = 0;

			for(remapIt = vecVtxRemapList.begin(); remapIt != vecVtxRemapList.end(); ++remapIt)
			{
				pBlendedVertices[nCount] = vecBlendedVertices[*remapIt];
				nCount++;
			}

			pObject->SetBoneNames(vecBoneNames);
			pObject->SetBlendedVertices(pBlendedVertices);
		}
	}

	LOG.Write("computenormals..");

	// Compute face / vertex normals

	pObject->ComputeNormals(pObject->GetFaceNormals() == NULL, pObject->GetVertexNormals() == NULL);

	LOG.Write("OK");

	if(bNeedDel)
	{
		delete pTriObject;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : TriObject* GetTriObjectFromNode(INode* pMaxNode, TimeValue t, bool *pDeleteIt)
//
//  - Purpose   : Gets the TriObject for a given node if exists. It also returns
//                in pDeleteIt if it needs to be deleted after use.
//
// -----------------------------------------------------------------------------
TriObject* CStravaganzaMaxTools::GetTriObjectFromNode(INode* pMaxNode, TimeValue t, bool *pDeleteIt)
{
	assert(pDeleteIt);

	*pDeleteIt = false;

	Object *pObject = pMaxNode->EvalWorldState(t).obj;

	if(pObject->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{
		TriObject *pTriObject = (TriObject *)pObject->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));

		if(pObject != pTriObject)
		{
			*pDeleteIt = true;
		}

		return pTriObject;
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool HasLensFlareUserDef(INode* pMaxNode)
//
//  - Purpose   : Checks if a node has in its user defined properties a
//                lensflare definition.
//                We provide our lensflare support for MAX through editing
//                the node's user defined properties and writing the lensflare
//                description.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::HasLensFlareUserDef(INode* pMaxNode)
{
	assert(pMaxNode);

	TSTR buffer;
	pMaxNode->GetUserPropBuffer(buffer);

	if(buffer.data())
	{
		if(strstr(buffer.data(), "#lensflare"))
		{
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CLensFlare* BuildLensFlare(const std::string& strUserDef)
//
//  - Purpose   : Builds a lensflare node from a node's user defined properties.
//                The created lensflare should be assigned as child of the node
//                that has the definition (can be object, light, helper or whatever).
//                See HasLensFlareUserDef().
//
// -----------------------------------------------------------------------------
CLensFlare* CStravaganzaMaxTools::BuildLensFlare(const std::string& strUserDef)
{
	LOG.Write("\nBuilding lensflare node");
	LOG.IncIndentation(2);

	CLensFlare* pLensFlare = new CLensFlare;
	assert(pLensFlare);

	CParser      parser;
	CInputMemory inputMem;

	inputMem.Open((char *)strUserDef.data(), strUserDef.size());
	parser.Init(&inputMem);

	bool  bZTest        = true;
	float fFadeDuration = 0.08f;

	// Read parameters

	parser.ReadKeyBool ("#ztest",        '=', &bZTest,        CParser::SEARCH_WHOLEDATA);
	parser.ReadKeyFloat("#fadeduration", '=', &fFadeDuration, CParser::SEARCH_WHOLEDATA);

	pLensFlare->SetZTestFade(bZTest, fFadeDuration);
	parser.ResetPos();

	std::string strTexture;

	while(parser.ReadKeyString("#texture", '=', &strTexture, CParser::SEARCH_FROMCURPOS))
	{
		float fPos = 0.0f, fSize = 0.5f;

		int nReadPos = parser.GetInputInterface()->GetPosition();
		parser.ReadKeyFloat("#pos",  '=', &fPos,  CParser::SEARCH_FROMCURPOS);

		parser.GetInputInterface()->SetPosition(nReadPos, CInput::SEEKFROM_START);
		parser.ReadKeyFloat("#size", '=', &fSize, CParser::SEARCH_FROMCURPOS);

		// In export mode, copy the texture to the texture folder if specified

		if(!m_bPreview)
		{
			std::string strDestPath = m_strTexturePath;

			if(strDestPath[strDestPath.length() - 1] != '\\')
			{
				strDestPath.append("\\", 1);
			}

			TSTR texture(strTexture.data()), path, filename;
			SplitPathFile(texture, &path, &filename);

			strDestPath = strDestPath + filename.data();

			if(m_bCopyTextures && m_strTexturePath != "")
			{
				if(!CopyFile(strTexture.data(), strDestPath.data(), FALSE))
				{
					CLogger::NotifyWindow("Unable to copy %s to %s", strTexture.data(), strDestPath.data());
				}
			}

			// In preview mode, strTexture = full path, in export only the filename.

			strTexture = filename.data();
		}

		pLensFlare->AddComponent(strTexture, fPos, fSize);
	}

	LOG.DecIndentation(2);
	return pLensFlare;
}
