// ==[ File ]===================================================================================
//
//  -Name     : ShaderStandard.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CShaderStandard implementation.
//
// =============================================================================================

/*

	07/01/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


using namespace UtilGL::Texturing;

static const bUseCombiners = true;

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : CShaderStandard()
//
//  - Purpose   : CShaderStandard's default constructor.
//
// -----------------------------------------------------------------------------
CShaderStandard::CShaderStandard()
{
	for(int i = 0; i < 3; i++)
	{
		m_ppTextures[i] = NULL;
		m_pbTexMtxIsIdentity[i] = true;
	}

	SetDefaults();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : CShaderStandard()
//
//  - Purpose   : CShaderStandard's destructor
//
// -----------------------------------------------------------------------------
CShaderStandard::~CShaderStandard()
{
	FreeResources();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool WriteASCII(CWriterASCII* pWriter) const
//
//  - Purpose   : Saves in text all shader's properties.
//                The opposite (parsing) can be done with ParseShader()
//
// -----------------------------------------------------------------------------
bool CShaderStandard::WriteASCII(CWriterASCII* pWriter) const
{
	assert(pWriter);
	assert(pWriter->Ready());

	// Shader properties

	pWriter->Write("\n#Shader #Name=\"%s\" #Type=Standard", GetName().data());
	pWriter->Write("\n{");
	pWriter->Write("\n    #ambient        = (%f, %f, %f, %f)", m_v4Ambient.X(),  m_v4Ambient.Y(),  m_v4Ambient.Z(),  m_v4Ambient.W());
	pWriter->Write("\n    #diffuse        = (%f, %f, %f, %f)", m_v4Diffuse.X(),  m_v4Diffuse.Y(),  m_v4Diffuse.Z(),  m_v4Diffuse.W());
	pWriter->Write("\n    #specular       = (%f, %f, %f, %f)", m_v4Specular.X(), m_v4Specular.Y(), m_v4Specular.Z(), m_v4Specular.W());
	pWriter->Write("\n    #shininess      = %f", m_fShininess);
	pWriter->Write("\n    #twosided       = %s", m_bTwoSided ? "true" : "false");
	pWriter->Write("\n    #faceted        = %s", m_bFaceted  ? "true" : "false");
	pWriter->Write("\n    #postwire       = %s", m_bPostWire ? "true" : "false");
	pWriter->Write("\n    #wirethickness  = %f", m_fWireLineThickness);

	// Blending

	std::string strBlendSrcFactor = "GL_ONE", strBlendDstFactor = "GL_ZERO";

	if     (m_nBlendSrcFactor == UtilGL::States::BLEND_ZERO)             strBlendSrcFactor = "GL_ZERO";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_ONE)              strBlendSrcFactor = "GL_ONE";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_DSTCOLOR)         strBlendSrcFactor = "GL_DST_COLOR";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_INVDSTCOLOR)      strBlendSrcFactor = "GL_ONE_MINUS_DST_COLOR";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_SRCALPHA)         strBlendSrcFactor = "GL_SRC_ALPHA";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_INVSRCALPHA)      strBlendSrcFactor = "GL_ONE_MINUS_SRC_ALPHA";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_DSTALPHA)         strBlendSrcFactor = "GL_DST_ALPHA";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_INVDSTALPHA)	     strBlendSrcFactor = "GL_ONE_MINUS_DST_ALPHA";
	else if(m_nBlendSrcFactor == UtilGL::States::BLEND_SRCALPHASATURATE) strBlendSrcFactor = "GL_SRC_ALPHA_SATURATE";

	if     (m_nBlendDstFactor == UtilGL::States::BLEND_ZERO)             strBlendDstFactor = "GL_ZERO";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_ONE)              strBlendDstFactor = "GL_ONE";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_DSTCOLOR)         strBlendDstFactor = "GL_DST_COLOR";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_INVDSTCOLOR)      strBlendDstFactor = "GL_ONE_MINUS_DST_COLOR";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_SRCALPHA)         strBlendDstFactor = "GL_SRC_ALPHA";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_INVSRCALPHA)      strBlendDstFactor = "GL_ONE_MINUS_SRC_ALPHA";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_DSTALPHA)         strBlendDstFactor = "GL_DST_ALPHA";
	else if(m_nBlendDstFactor == UtilGL::States::BLEND_INVDSTALPHA)      strBlendDstFactor = "GL_ONE_MINUS_DST_ALPHA";

	pWriter->Write("\n    #blendsrcfactor = %s", strBlendSrcFactor.data());
	pWriter->Write("\n    #blenddstfactor = %s", strBlendDstFactor.data());

	// Layers

	for(int i = 0; i < 3; i++)
	{
		if(!m_pbLayerPresent[i]) continue;

		std::string strLayer, strTexEnv = "MODULATE", strUVGen="ENVMAPPING", strTexType="UNKNOWN";

		// Get layer type

		if(i == 0) strLayer = "#layerdiffuse"; else
		if(i == 1) strLayer = "#layerbump";    else
		if(i == 2) strLayer = "#layermap2";

		// Get texture type

		switch(m_pLayerInfo[i].texInfo.eTextureType)
		{
		case CTexture::TEXTURE2D:      strTexType = "2D";      break;
		case CTexture::TEXTURE2DANIM:  strTexType = "2DANIM";  break;
		case CTexture::TEXTURECUBEMAP: strTexType = "CUBEMAP"; break;
		}

		// Texture info:

		pWriter->Write("\n\n    %s", strLayer.data());
		pWriter->Write("\n    {");

		pWriter->Write("\n         #texture(%s)", strTexType.data());
		pWriter->Write("\n         {");

		if(m_pLayerInfo[i].texInfo.eTextureType == CTexture::TEXTURE2DANIM)
		{
			// Loop only for animated textures

			pWriter->Write("\n             #loop=%s\n", m_pLayerInfo[i].texInfo.bLoop ? "TRUE" : "FALSE");
		}

		// Texture bitmap(s)		

		std::vector<SBitmapInfo>::const_iterator bitmapIt;

		for(bitmapIt = m_pLayerInfo[i].texInfo.m_vecBitmaps.begin(); bitmapIt != m_pLayerInfo[i].texInfo.m_vecBitmaps.end(); ++bitmapIt)
		{
			pWriter->Write("\n             ");

			if(	m_pLayerInfo[i].texInfo.eTextureType == CTexture::TEXTURE2DANIM ||
				m_pLayerInfo[i].texInfo.eTextureType == CTexture::TEXTURECUBEMAP)
			{
				pWriter->Write("{ ");
			}

			// File + tiling

			std::string strTile = "FALSE";

			if(bitmapIt->bTile)
			{
				strTile = "TRUE";
			}

			pWriter->Write("#file=\"%s\" #tile=%s", bitmapIt->strFile.data(), strTile.data());

			// Filtering (if bForceFiltering == true)

			if(bitmapIt->bForceFiltering)
			{
				std::string strFilter = "";

				switch(bitmapIt->eFilter)
				{
				case FILTER_POINT:     strFilter = "POINT";     break;
				case FILTER_LINEAR:    strFilter = "LINEAR";    break;
				case FILTER_BILINEAR:  strFilter = "BILINEAR";  break;
				case FILTER_TRILINEAR: strFilter = "TRILINEAR"; break;
				}

				if(strFilter != "")
				{
					pWriter->Write(" #filter=%s", strFilter.data());
				}
			}

			// Duration (only if texturetype = 2DANIM

			if(m_pLayerInfo[i].texInfo.eTextureType == CTexture::TEXTURE2DANIM)
			{
				pWriter->Write(" #duration=%f", bitmapIt->fSeconds);
			}

			if(	m_pLayerInfo[i].texInfo.eTextureType == CTexture::TEXTURE2DANIM ||
				m_pLayerInfo[i].texInfo.eTextureType == CTexture::TEXTURECUBEMAP)
			{
				pWriter->Write(" }");
			}
		}

		pWriter->Write("\n         }\n");

		// Texture parameters

		if(m_pLayerInfo[i].eTexEnv == TEXENV_MODULATE)  strTexEnv = "MODULATE"; else
		if(m_pLayerInfo[i].eTexEnv == TEXENV_ADD)       strTexEnv = "ADD";

		if(m_pLayerInfo[i].eUVGen  == UVGEN_EXPLICITMAPPING) strUVGen  = "EXPLICIT"; else
		if(m_pLayerInfo[i].eUVGen  == UVGEN_ENVMAPPING)      strUVGen  = "ENVMAPPING";

		pWriter->Write("\n         #texenv          = %s", strTexEnv.data());
		pWriter->Write("\n         #uvgen           = %s", strUVGen.data());

		if(m_pLayerInfo[i].eUVGen == UVGEN_EXPLICITMAPPING)
		{
			pWriter->Write("\n         #mapchannel      = %u", m_pLayerInfo[i].uMapChannel);
		}

		pWriter->Write("\n         #uvwscrolloffset = (%f, %f, %f)", m_pLayerInfo[i].v3ScrollOffset.X(),   m_pLayerInfo[i].v3ScrollOffset.Y(),   m_pLayerInfo[i].v3ScrollOffset.Z());
		pWriter->Write("\n         #uvwrotateoffset = (%f, %f, %f)", m_pLayerInfo[i].v3RotationOffset.X(), m_pLayerInfo[i].v3RotationOffset.Y(), m_pLayerInfo[i].v3RotationOffset.Z());
		pWriter->Write("\n         #uvwscrollspeed  = (%f, %f, %f)", m_pLayerInfo[i].v3ScrollSpeed.X(),    m_pLayerInfo[i].v3ScrollSpeed.Y(),    m_pLayerInfo[i].v3ScrollSpeed.Z());
		pWriter->Write("\n         #uvwrotatespeed  = (%f, %f, %f)", m_pLayerInfo[i].v3RotationSpeed.X(),  m_pLayerInfo[i].v3RotationSpeed.Y(),  m_pLayerInfo[i].v3RotationSpeed.Z());

		pWriter->Write("\n    }");
	}

	pWriter->Write("\n}\n");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool CompileObject(CObject* pObject) const
//
//  - Purpose   : Compiles a geometric object. This is completely necessary
//                for the object if it's going to be rendered using the shader.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::CompileObject(CObject* pObject) const
{
	LOG.Write("\nCompiling object %s (shader standard)", pObject->GetNodeName().c_str());

	assert(pObject);
	assert(pObject->GetVertices());
	assert(pObject->GetVertexNormals());
	assert(pObject->GetFaceList());

	pObject->ClearDisplayLists();

	// We create a display list for each pass except for passes which involve bump
	// mapping. These passes use dynamic data arrays (primary+secondary color values storing
	// L and H vectors computed each frame), so -1 is stored in the object as display
	// list. The -1 value tells the renderer to use glDrawElements instead of glCallList for
	// a given pass.
	//
	//
	//            No|-> 1 Pass: 1 display list with no mapping coordinates data.
	//              |
    //              |
	//              |
	//              |                             No|-> 1 Pass: 1 display list (single texturing)
	//  layerdiff? -|                               |
	//              |             No|-> layermap2? -|
	//              |               |               |
	//              |               |            Yes|-> 1 Pass: 1 display list (multitexturing)
    //              |               |
	//           Yes|-> layerbump? -|
	//                              |
	//                              |             No|-> Combiners?
	//                              |               |      - No:  3 Passes: No display lists
	//                           Yes|-> layermap2? -|      - Yes: 1 Pass:   No display lists
	//                                              |
	//                                           Yes|-> Combiners?
	//                                                     - No:  4 Passes: 1 display list
	//                                                     - Yes: 2 Passes: 1 display list
	//
	// Notes:
    //
	// If the shader has the postwire property set to true, an additional pass using a display
	// list is made to render the object in wireframe.
	//
	// Bumpmapping is performed in 1 pass using register combiners (diffuse+specular) and
	// in three passes using texenv (the first two to achive specular lighting and the third
	// to do the diffuse lighting)
	//
	// Display lists are created ONLY if the object has static mesh, otherwise no display lists
	// are build and everything is drawn using glDrawElements().
    //
	// TO DO:
	//
	//   Reduce the number of passes been made taking advantage of cards with > 2 texture units.
	//

	int i;

	unsigned uDiffChannel = m_pLayerInfo[LAYER_DIFF].uMapChannel;
	unsigned uBumpChannel = m_pLayerInfo[LAYER_BUMP].uMapChannel;
	unsigned uMap2Channel = m_pLayerInfo[LAYER_MAP2].uMapChannel;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);

	for(i = 0; i < UtilGL::Texturing::GetNumUnits(); i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	GLuint nDisplayList;

	if(m_pbLayerPresent[LAYER_DIFF])
	{
		if(m_pbLayerPresent[LAYER_BUMP])
		{
			pObject->ComputeTextureSpaceMatrices();

			if(bUseCombiners && UtilGL::Extensions::GetAvailableExtensions()->NV_register_combiners)
			{
				// 1 pass for the bump + diffuse (no list)
				pObject->AddDisplayList(-1);
			}
			else
			{
				// 3 passes for the bump + diffuse (no lists)
				pObject->AddDisplayList(-1);
				pObject->AddDisplayList(-1);
				pObject->AddDisplayList(-1);
			}

			if(m_pbLayerPresent[LAYER_MAP2])
			{
				if(pObject->HasStaticMesh())
				{
					// 1 Additional pass for the map2 (display list)

					nDisplayList = glGenLists(1);
					glNewList(nDisplayList, GL_COMPILE);

					glVertexPointer  (3, GL_FLOAT, 0, pObject->GetVertices());
					glNormalPointer  (   GL_FLOAT, 0, pObject->GetVertexNormals());

					if(m_pLayerInfo[LAYER_MAP2].eUVGen == UVGEN_EXPLICITMAPPING)
					{
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uMap2Channel));
					}

					glDrawElements(GL_TRIANGLES, pObject->GetNumFaces() * 3, GL_UNSIGNED_INT, pObject->GetFaceList());

					glEndList();
					pObject->AddDisplayList(nDisplayList);
				}
				else
				{
					pObject->AddDisplayList(-1);
				}
			}
		}
		else
		{
			if(pObject->HasStaticMesh())
			{
				// 1 display list with single or multitexturing depending on map2 presence

				bool bUseDiffMap = m_pLayerInfo[LAYER_DIFF].eUVGen == UVGEN_EXPLICITMAPPING;
				bool bUseVtxCols = pObject->GetVertexColors() != NULL;
				bool bUseMap2Map = false;

				if(m_pbLayerPresent[LAYER_MAP2])
				{
					if(m_pLayerInfo[LAYER_MAP2].eUVGen == UVGEN_EXPLICITMAPPING)
					{
						bUseMap2Map = true;
					}
				}

				nDisplayList = glGenLists(1);
				glNewList(nDisplayList, GL_COMPILE);

				if(!m_bFaceted)
				{
					glVertexPointer  (3, GL_FLOAT, 0, pObject->GetVertices());
					glNormalPointer  (   GL_FLOAT, 0, pObject->GetVertexNormals());

					if(bUseDiffMap)
					{
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uDiffChannel));
					}

					if(bUseVtxCols)
					{
						glColorPointer(4, GL_FLOAT, 0, pObject->GetVertexColors());
					}

					if(bUseMap2Map)
					{
						// Add a second texcoord pointer

						glClientActiveTextureARB(GL_TEXTURE1_ARB);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uMap2Channel));
						glClientActiveTextureARB(GL_TEXTURE0_ARB);
					}

					glDrawElements(GL_TRIANGLES, pObject->GetNumFaces() * 3, GL_UNSIGNED_INT, pObject->GetFaceList());
				}
				else
				{
					glBegin(GL_TRIANGLES);

					for(i = 0; i < pObject->GetNumFaces(); i++)
					{
						glNormal3fv(pObject->GetFaceNormals()[i].Data());

						if(bUseDiffMap) glTexCoord2fv(pObject->GetMapChannel(uDiffChannel)[pObject->GetFaceList()[i * 3 + 0]].Data());
						if(bUseMap2Map) glMultiTexCoord2fv(GL_TEXTURE1_ARB, pObject->GetMapChannel(uMap2Channel)[pObject->GetFaceList()[i * 3 + 0]].Data());
						if(bUseVtxCols) glColor4fv(pObject->GetVertexColors()[pObject->GetFaceList()[i * 3 + 0]].Data());

						glVertex3fv(pObject->GetVertices()[pObject->GetFaceList()[i * 3 + 0]].Data());

						if(bUseDiffMap) glTexCoord2fv(pObject->GetMapChannel(uDiffChannel)[pObject->GetFaceList()[i * 3 + 1]].Data());
						if(bUseMap2Map) glMultiTexCoord2fv(GL_TEXTURE1_ARB, pObject->GetMapChannel(uMap2Channel)[pObject->GetFaceList()[i * 3 + 1]].Data());
						if(bUseVtxCols) glColor4fv(pObject->GetVertexColors()[pObject->GetFaceList()[i * 3 + 1]].Data());

						glVertex3fv(pObject->GetVertices()[pObject->GetFaceList()[i * 3 + 1]].Data());

						if(bUseDiffMap) glTexCoord2fv(pObject->GetMapChannel(uDiffChannel)[pObject->GetFaceList()[i * 3 + 2]].Data());
						if(bUseMap2Map) glMultiTexCoord2fv(GL_TEXTURE1_ARB, pObject->GetMapChannel(uMap2Channel)[pObject->GetFaceList()[i * 3 + 2]].Data());
						if(bUseVtxCols) glColor4fv(pObject->GetVertexColors()[pObject->GetFaceList()[i * 3 + 2]].Data());

						glVertex3fv(pObject->GetVertices()[pObject->GetFaceList()[i * 3 + 2]].Data());
					}

					glEnd();
				}

				glEndList();
				pObject->AddDisplayList(nDisplayList);
			}
			else
			{
				pObject->AddDisplayList(-1);
			}
		}
	}
	else
	{
		// Diffuse without map

		if(pObject->HasStaticMesh())
		{
			// 1 display list with no mapping data

			nDisplayList = glGenLists(1);
			glNewList(nDisplayList, GL_COMPILE);

			if(!m_bFaceted)
			{
				glVertexPointer(3, GL_FLOAT, 0, pObject->GetVertices());
				glNormalPointer(   GL_FLOAT, 0, pObject->GetVertexNormals());

				if(pObject->GetVertexColors())
				{
					glColorPointer(4, GL_FLOAT, 0, pObject->GetVertexColors());
				}

				glDrawElements(GL_TRIANGLES, pObject->GetNumFaces() * 3, GL_UNSIGNED_INT, pObject->GetFaceList());
			}
			else
			{
				glBegin(GL_TRIANGLES);

				for(i = 0; i < pObject->GetNumFaces(); i++)
				{
					glNormal3fv(pObject->GetFaceNormals()[i].Data());

					if(pObject->GetVertexColors()) glColor4fv(pObject->GetVertexColors()[pObject->GetFaceList()[i * 3 + 0]].Data());
					glVertex3fv(pObject->GetVertices()[pObject->GetFaceList()[i * 3 + 0]].Data());

					if(pObject->GetVertexColors()) glColor4fv(pObject->GetVertexColors()[pObject->GetFaceList()[i * 3 + 1]].Data());
					glVertex3fv(pObject->GetVertices()[pObject->GetFaceList()[i * 3 + 1]].Data());

					if(pObject->GetVertexColors()) glColor4fv(pObject->GetVertexColors()[pObject->GetFaceList()[i * 3 + 2]].Data());
					glVertex3fv(pObject->GetVertices()[pObject->GetFaceList()[i * 3 + 2]].Data());
				}

				glEnd();
			}

			glEndList();
			pObject->AddDisplayList(nDisplayList);
		}
		else
		{
			pObject->AddDisplayList(-1);
		}
	}

	// Additional list for post wire effect

	if(m_bPostWire)
	{
		if(pObject->HasStaticMesh())
		{
			assert(pObject->GetEdgeFlags());
			assert(pObject->GetExpVertices());

			glDisableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_EDGE_FLAG_ARRAY);

			nDisplayList = glGenLists(1);
			glNewList(nDisplayList, GL_COMPILE);

			glVertexPointer  (3, GL_FLOAT, 0, pObject->GetExpVertices());
			glEdgeFlagPointer(             0, pObject->GetEdgeFlags());
			glDrawArrays(GL_TRIANGLES, 0, pObject->GetNumFaces() * 3);

			glEndList();
			pObject->AddDisplayList(nDisplayList);
		}
		else
		{
			pObject->AddDisplayList(-1);
		}
	}

	// Disable client states before returning

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);

	glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);

	for(i = 0; i < UtilGL::Texturing::GetNumUnits(); i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool LoadResources(	const std::string& strDatfile   = "",
//                                      const std::string& strTexFolder = "",
//										UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL)
//
//  - Purpose   : Loads all the shader resources (textures etc...).
//
// -----------------------------------------------------------------------------
bool CShaderStandard::LoadResources(const std::string& strDatfile,
									const std::string& strTexFolder,
									UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	assert(strDatfile.data());
	assert(strTexFolder.data());

	int nError = 0;
	STexLoadOptions defaultTexOptions;

	if(pTexOptions == NULL)
	{
		pTexOptions = &defaultTexOptions;
	}

	UtilGL::Texturing::ETextureFilter eFilter = pTexOptions->eFilter;

	std::string strTexPath = "";

	if(strTexFolder[strTexFolder.size() - 1] != '\\' && strTexFolder != "")
	{
		strTexPath = strTexFolder + "\\";
	}
	else
	{
		strTexPath = strTexFolder;
	}

	// Iterate over layers

	for(int i = 0; i < 3; i++)
	{
		if(!m_pbLayerPresent[i])
		{
			continue;
		}

		STextureInfo* pTexInfo   = &m_pLayerInfo[i].texInfo;
		std::string   strTexFile;
		CTexture*     pTexture = NULL;
		CInput*       pInput;
		int           nSide;

		std::vector<SBitmapInfo>::iterator bitmapIt;

		// Load texture

		switch(pTexInfo->eTextureType)
		{

		case CTexture::TEXTURE2D: ///////////////////////////

			assert(pTexInfo->m_vecBitmaps.size() == 1);

			bitmapIt   = pTexInfo->m_vecBitmaps.begin();
			strTexFile = strTexPath + bitmapIt->strFile;
			pInput     = LoadFileFromDat(strTexFile, false, strDatfile);

			if(pInput == NULL)
			{
				CLogger::ErrorWindow("Unable to open %s", strTexFile.data());
				nError++;
				continue;
			}

			pTexture = new CTexture2D;

			pTexOptions->eFilter = bitmapIt->bForceFiltering ? bitmapIt->eFilter : eFilter;

			if(!((CTexture2D*)pTexture)->Load(	pInput,
												strTexFile,
												GetFileExt(strTexFile),
												bitmapIt->bTile,
												pTexOptions))
			{
				CLogger::ErrorWindow("Error loading texture %s", strTexFile.data());
				nError++;
				delete pTexture;
				delete pInput;
				continue;
			}

			delete pInput;
			break;


		case CTexture::TEXTURE2DANIM: ///////////////////////////


			assert(pTexInfo->m_vecBitmaps.size() > 0);

			pTexture = new CTexture2DAnim;

			// Load all frames

			for(bitmapIt = pTexInfo->m_vecBitmaps.begin(); bitmapIt != pTexInfo->m_vecBitmaps.end(); ++bitmapIt)
			{
				strTexFile = strTexPath + bitmapIt->strFile;
				pInput     = LoadFileFromDat(strTexFile, false, strDatfile);

				if(pInput == NULL)
				{
					CLogger::ErrorWindow("Unable to open %s", strTexFile.data());
					nError++;
					continue;
				}

				pTexOptions->eFilter = bitmapIt->bForceFiltering ? bitmapIt->eFilter : eFilter;

				if(!((CTexture2DAnim*)pTexture)->AddFrame(	pInput,
															strTexFile,
															GetFileExt(strTexFile),
															bitmapIt->fSeconds,
															bitmapIt->bTile,
															pTexOptions))
				{
					CLogger::ErrorWindow("Error loading texture %s", strTexFile.data());
					nError++;
					delete pInput;
					continue;
				}

				delete pInput;
			}

			break;


		case CTexture::TEXTURECUBEMAP: //////////////////////////////////////////////////


			assert(pTexInfo->m_vecBitmaps.size() == 6);

			pTexture = new CTextureCubemap;

			// Load all sides

			for(nSide = 0, bitmapIt = pTexInfo->m_vecBitmaps.begin(); bitmapIt != pTexInfo->m_vecBitmaps.end(); ++bitmapIt, nSide++)
			{
				strTexFile = strTexPath + bitmapIt->strFile;
				pInput     = LoadFileFromDat(strTexFile, false, strDatfile);

				if(pInput == NULL)
				{
					CLogger::ErrorWindow("Unable to open %s", strTexFile.data());
					nError++;
					continue;
				}

				CTextureCubemap::ECubeSide eSide;

				switch(nSide)
				{
				case 0: eSide = CTextureCubemap::SIDE_POSX; break;
				case 1: eSide = CTextureCubemap::SIDE_NEGX; break;
				case 2: eSide = CTextureCubemap::SIDE_POSY; break;
				case 3: eSide = CTextureCubemap::SIDE_NEGY; break;
				case 4: eSide = CTextureCubemap::SIDE_POSZ; break;
				case 5: eSide = CTextureCubemap::SIDE_NEGZ; break;
				}

				pTexOptions->eFilter = bitmapIt->bForceFiltering ? bitmapIt->eFilter : eFilter;

				if(!((CTextureCubemap*)pTexture)->LoadSide(	pInput,
															strTexFile,
															GetFileExt(strTexFile),
															eSide,
															bitmapIt->bTile,
															pTexOptions))
				{
					CLogger::ErrorWindow("Error loading texture %s", strTexFile.data());
					nError++;
					delete pInput;
					continue;
				}

				delete pInput;
			}

			break;

		default:

			break;
		}

		m_ppTextures[i] = pTexture;
	}

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool FreeResources()
//
//  - Purpose   : Frees all the shader resources.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::FreeResources()
{
	for(int i = 0; i < 3; i++)
	{
		m_pbLayerPresent[i] = false;

		if(m_ppTextures[i])
		{
			delete m_ppTextures[i];
			m_ppTextures[i] = NULL;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : int GetNumPasses() const
//
//  - Purpose   : Gets the number of passes needed to render an object using the
//                shader. The returned number is (graphics) hardware dependant.
//                See CShaderStandard::CompileObject() for more details.
//
// -----------------------------------------------------------------------------
int CShaderStandard::GetNumPasses() const
{
	int nNumPasses = 0;

	if(m_pbLayerPresent[LAYER_DIFF])
	{
		if(m_pbLayerPresent[LAYER_BUMP])
		{
			if(bUseCombiners && UtilGL::Extensions::GetAvailableExtensions()->NV_register_combiners)
			{
				// Bump
				nNumPasses = 1;
			}
			else
			{
				// Bump
				nNumPasses = 3;
			}

			if(m_pbLayerPresent[LAYER_MAP2])
			{
				// Map2 (Additional to bump)
				nNumPasses++;
			}
		}
		else
		{
			// Diffuse+Map2
			nNumPasses = 1;
		}
	}
	else
	{
		nNumPasses = 1;
	}

	// Additional list for post wire effect

	if(m_bPostWire)
	{
		nNumPasses++;
	}

	return nNumPasses;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool SetEvalTime(float fTime)
//
//  - Purpose   : Evaluates the shader with the given time.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::SetEvalTime(float fTime)
{
	unsigned uError = 0;

	for(int i = 0; i < 3; i++)
	{
		// Evaluate layer texture

		if(m_ppTextures[i])
		{
			if(!m_ppTextures[i]->SetEvalTime(fTime))
			{
				uError++;
			}
		}

		m_pbTexMtxIsIdentity[i] = true;
		m_pTexMatrices[i].SetIdentity();

		if(m_pbLayerPresent[i])
		{
			// Evaluate layer texture matrix

			CVector3 v3Rotation(m_pLayerInfo[i].v3RotationOffset);
			CVector3 v3Scroll  (m_pLayerInfo[i].v3ScrollOffset);

			if(m_pLayerInfo[i].v3RotationOffset != CVector3::NULLVECTOR)
			{
				m_pbTexMtxIsIdentity[i] = false;
			}

			if(m_pLayerInfo[i].v3ScrollOffset != CVector3::NULLVECTOR)
			{
				m_pbTexMtxIsIdentity[i] = false;
			}

			if(m_pLayerInfo[i].v3RotationSpeed != CVector3::NULLVECTOR)
			{
				m_pbTexMtxIsIdentity[i] = false;
				v3Rotation = v3Rotation + (m_pLayerInfo[i].v3RotationSpeed) * fTime;
			}

			if(m_pLayerInfo[i].v3ScrollSpeed != CVector3::NULLVECTOR)
			{
				m_pbTexMtxIsIdentity[i] = false;
				v3Scroll = v3Scroll + (m_pLayerInfo[i].v3ScrollSpeed) * fTime;
			}

			if(!m_pbTexMtxIsIdentity[i])
			{
				m_pTexMatrices[i].RotateX(v3Rotation.X());
				m_pTexMatrices[i].RotateY(v3Rotation.Y());
				m_pTexMatrices[i].RotateZ(v3Rotation.Z());
				m_pTexMatrices[i].Translate(v3Scroll.X(), v3Scroll.Y(), v3Scroll.Z());
			}
		}
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool Begin()
//
//  - Purpose   : Prepares the shader before rendering the passes of an object.
//                Should be called before the first pass.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::Begin()
{
	// Polygonmode, twosided & shademode

	UtilGL::States::Set(UtilGL::States::POLYGONMODE, UtilGL::States::POLYGONMODE_FILL);
	UtilGL::States::Set(UtilGL::States::BFCULLING,   m_bTwoSided ? UtilGL::States::DISABLED       : UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SHADEMODE,   m_bFaceted  ? UtilGL::States::SHADEMODE_FLAT : UtilGL::States::SHADEMODE_GOURAUD);

	// Blending OFF

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);

	// Get light0 and camera position in world coordinates for bumping

	if(m_pbLayerPresent[LAYER_BUMP])
	{
		float pfPos[4];
		glGetLightfv(GL_LIGHT0, GL_POSITION, pfPos);
		m_v3Light0Pos.Set(pfPos[0], pfPos[1], pfPos[2]);

		CMatrix mtxEye = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW).Inverse(); 

		m_v3EyePos    = mtxEye.Position();
		m_v3Light0Pos = mtxEye * m_v3Light0Pos; // eye -> world
	}

	// Enable polygonoffset

	glEnable(GL_POLYGON_OFFSET_FILL);

	// Hold some data

	m_nLightState = UtilGL::States::Get(UtilGL::States::LIGHTING);

	if(m_ppTextures[LAYER_DIFF]) m_peTexEnvs[LAYER_DIFF] = m_pLayerInfo[LAYER_DIFF].eTexEnv;
	if(m_ppTextures[LAYER_BUMP]) m_peTexEnvs[LAYER_BUMP] = m_pLayerInfo[LAYER_BUMP].eTexEnv;
	if(m_ppTextures[LAYER_MAP2]) m_peTexEnvs[LAYER_MAP2] = m_pLayerInfo[LAYER_MAP2].eTexEnv;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool End()
//
//  - Purpose   : Closes the pair of Begin(). Should be called after rendering
//                the last pass of an object using this shader.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::End()
{
	int i;

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);

	glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);

	for(i = 0; i < UtilGL::Texturing::GetNumUnits(); i++)
	{
		UtilGL::Texturing::SetCurrentUnit(i);
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
		UtilGL::Texturing::SafeDisableCubeMapping();
		UtilGL::States::Set(UtilGL::States::GENU, UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::GENV, UtilGL::States::DISABLED);

		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);

		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	UtilGL::Texturing::SetCurrentUnit(0);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_COLOR_MATERIAL);

	if(m_pbLayerPresent[LAYER_BUMP] && bUseCombiners)
	{
		if(UtilGL::Extensions::GetAvailableExtensions()->NV_register_combiners)
		{
			glDisable(GL_REGISTER_COMBINERS_NV);
		}
	}

	// Pop modified texenvs

	for(i = 0; i < 3; i++)
	{
		if(m_ppTextures[i])
		{
			m_ppTextures[i]->SetActive();

			switch(m_pLayerInfo[i].eTexEnv)
			{
			case TEXENV_MODULATE: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); break;
			case TEXENV_ADD:      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);      break;
			}
		}
	}

	UtilGL::States::Set(UtilGL::States::BFCULLING,    UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::POLYGONMODE,  UtilGL::States::POLYGONMODE_FILL);
	UtilGL::States::Set(UtilGL::States::SHADEMODE,    UtilGL::States::SHADEMODE_GOURAUD);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool PrepareStates(int nPass)
//
//  - Purpose   : Sets the renderstate values to the necessary ones in order to
//                render the given pass of any object using this shader.
//                See CShaderStandard::CompileObject for more details.
//
//  - Note      : Zero based index.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::PrepareStates(int nPass)
{
	// Do the setup for this pass

	bool bReadyForWire = false;

	if(m_pbLayerPresent[LAYER_DIFF])
	{
		if(m_pbLayerPresent[LAYER_BUMP])
		{
			if(bUseCombiners && UtilGL::Extensions::GetAvailableExtensions()->NV_register_combiners)
			{
				if(nPass == 0)
				{
					// Diffuse + bump (combiners)

					UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);
					SetupTexturing(nPass, LAYER_BUMP, 1); // Bump texmap -> Texture unit 0
					SetupTexturing(nPass, LAYER_DIFF, 0); // Diff texmap -> Texture unit 1
					EnableCombinersDot3();
				}
				else if(nPass == 1 && m_pbLayerPresent[LAYER_MAP2])
				{
					// Add map2

					glDisable(GL_REGISTER_COMBINERS_NV);
					UtilGL::Texturing::SetCurrentUnit(1);
					UtilGL::States::Set(UtilGL::States::LIGHTING,  m_nLightState);
					UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
					SetupTexturing(nPass, LAYER_MAP2, 0); // Map2 texmap -> Texture unit 0
				}
				else
				{
					glDisable(GL_REGISTER_COMBINERS_NV);
					bReadyForWire = true;
				}
			}
			else
			{
				// Diffuse + bump (texenv) + map2

				if(nPass == 0 || nPass == 1)
				{
					// Specular passes

					UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);
					SetupTexturing(nPass, LAYER_BUMP, 0); // Bump texmap -> Texture unit 0
					SetupTexturing(nPass, LAYER_BUMP, 1); // Bump texmap -> Texture unit 1
					EnableTexEnvDot3(nPass, false);
				}
				else if(nPass == 2)
				{
					// Diffuse pass

					UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);
					SetupTexturing(nPass, LAYER_BUMP, 0); // Bump texmap -> Texture unit 0						
					SetupTexturing(nPass, LAYER_DIFF, 1); // Diff texmap -> Texture unit 1
					EnableTexEnvDot3(nPass, true);
				}
				else if(nPass == 3 && m_pbLayerPresent[LAYER_MAP2])
				{
					UtilGL::States::Set(UtilGL::States::LIGHTING, m_nLightState);
					UtilGL::Texturing::SetCurrentUnit(1);
					UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
					SetupTexturing(nPass, LAYER_MAP2, 0); // Map2 texmap -> Texture unit 0
				}
				else
				{
					bReadyForWire = true;
				}
			}
		}
		else
		{
			UtilGL::States::Set(UtilGL::States::LIGHTING, m_nLightState);

			if(m_pbLayerPresent[LAYER_MAP2])
			{
				// Diffuse + map2

				if(nPass == 0)
				{
					SetupTexturing(nPass, LAYER_DIFF, 0); // Diffuse texmap -> Texture unit 0
					SetupTexturing(nPass, LAYER_MAP2, 1); // Map2    texmap -> Texture unit 1
				}
				else
				{
					bReadyForWire = true;
				}

			}
			else
			{
				// Only diffuse map

				if(nPass == 0)
				{
					UtilGL::Texturing::SetCurrentUnit(1);
					UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
					SetupTexturing(nPass, LAYER_DIFF, 0);
				}
				else
				{
					bReadyForWire = true;
				}
			}
		}
	}
	else
	{
		// No texture layers

		UtilGL::States::Set(UtilGL::States::LIGHTING, m_nLightState);

		if(nPass == 0)
		{
			UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
		}
		else
		{
			bReadyForWire = true;
		}
	}

	int nNonPostWirePasses = m_bPostWire ? GetNumPasses() - 1 : GetNumPasses();

	// Needed in a multipass scheme:

	glPolygonOffset(1.0f, nNonPostWirePasses - nPass);

	// Post wire

	if(m_bPostWire && bReadyForWire)
	{
		for(int i = 0; i < UtilGL::Texturing::GetNumUnits(); i++)
		{
			UtilGL::Texturing::SetCurrentUnit(i);
			UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
		}

		UtilGL::Texturing::SetCurrentUnit(0);
		UtilGL::States::Set(UtilGL::States::LIGHTING,     UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::POLYGONMODE,  UtilGL::States::POLYGONMODE_LINE);
		glLineWidth(m_fWireLineThickness);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool PrepareObject(int nPass, CObject* pObject)
//
//  - Purpose   : Prepares an object for its given render pass.
//
//  - Note      : Zero based index.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::PrepareObject(int nPass, CObject* pObject)
{
	assert(nPass >= 0 && nPass < pObject->GetNumDisplayLists());

	if(pObject->AffectsLighting() == false)
	{
		UtilGL::States::Set(UtilGL::States::LIGHTING, UtilGL::States::DISABLED);
	}

	// Material properties:

	CVector4 v4Ambient  = m_v4Ambient;  v4Ambient.SetW (0.0f);
	CVector4 v4Diffuse  = m_v4Diffuse;  v4Diffuse.SetW (m_v4Diffuse.W() * pObject->GetAlpha());
	CVector4 v4Specular = m_v4Specular; v4Specular.SetW(0.0f);

	glDisable(GL_COLOR_MATERIAL);

	if(UtilGL::States::Get(UtilGL::States::LIGHTING) == UtilGL::States::ENABLED)
	{
		UtilGL::States::SetMaterialAmbient (GL_FRONT_AND_BACK, v4Ambient);
		UtilGL::States::SetMaterialDiffuse (GL_FRONT_AND_BACK, v4Diffuse);
		UtilGL::States::SetMaterialSpecular(GL_FRONT_AND_BACK, v4Specular);
		glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m_fShininess);

		// Setup color material if we have per vertex color. We can't do this when
		// we have bumping since bumping uses opengl color arrays.

		if(pObject->GetVertexColors() && !m_pbLayerPresent[LAYER_BUMP])
		{
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		}
	}
	else
	{
		UtilGL::States::SetColor(v4Diffuse);
	}

	// Blending

	if(nPass == 0)
	{
		if(!IsOpaque())
		{
			UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
			UtilGL::States::Set(UtilGL::States::SRCBLEND, m_nBlendSrcFactor);
			UtilGL::States::Set(UtilGL::States::DSTBLEND, m_nBlendDstFactor);

			if(m_nBlendSrcFactor == UtilGL::States::BLEND_ONE && m_nBlendDstFactor == UtilGL::States::BLEND_ONE)
			{
				// Additive blending, disable depth write

				UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::DISABLED);
			}
		}
		else
		{
			// Shader has no blending function, check if we need blending anyways due to object's alpha

			if(pObject->GetAlpha() < (1.0f - ALMOST_ZERO))
			{
				UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
				UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_SRCALPHA);
				UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_INVSRCALPHA);
			}
		}
	}
	else
	{
		// Blending for multipass has already been setup on PrepareStates().
		// Enable depth writing if it was disabled on the first pass to render blending.

		UtilGL::States::Set(UtilGL::States::ZBUFFERWRITE, UtilGL::States::ENABLED);
	}

	// Initial array setup

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);

	glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);

	for(int i = 0; i < UtilGL::Texturing::GetNumUnits(); i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glVertexPointer(3, GL_FLOAT, 0, pObject->GetVertices());
	glNormalPointer(   GL_FLOAT, 0, pObject->GetVertexNormals());

	unsigned uDiffChannel = m_pLayerInfo[LAYER_DIFF].uMapChannel;
	unsigned uBumpChannel = m_pLayerInfo[LAYER_BUMP].uMapChannel;
	unsigned uMap2Channel = m_pLayerInfo[LAYER_MAP2].uMapChannel;

	// Do the array setup based on the pass number

	bool bReadyForWire = false;

	if(m_pbLayerPresent[LAYER_DIFF])
	{
		if(m_pbLayerPresent[LAYER_BUMP])
		{
			// Compute bump vectors

			if(nPass == 0)
			{
				if(pObject->HasStaticMesh() == false)
				{
					pObject->ComputeTextureSpaceMatrices();
				}

				pObject->ComputeBumpVectors(m_v3EyePos, m_v3Light0Pos);
			}

			// 2 renderpaths:

			if(bUseCombiners && UtilGL::Extensions::GetAvailableExtensions()->NV_register_combiners)
			{
				// Diffuse + bump (combiners)

				if(nPass == 0)
				{
					// Bump pass

					// Color arrays:

					glEnableClientState(GL_COLOR_ARRAY);
					glEnableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
					glColorPointer            (4, GL_UNSIGNED_BYTE, 0, pObject->GetBumpLVectors());
					glSecondaryColorPointerEXT(4, GL_UNSIGNED_BYTE, 0, pObject->GetBumpHVectors());

					// Texcoord arrays:

					glClientActiveTextureARB(GL_TEXTURE1_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uBumpChannel));

					glClientActiveTextureARB(GL_TEXTURE0_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uDiffChannel));
				}
				else if(nPass == 1 && m_pbLayerPresent[LAYER_MAP2])
				{
					if(pObject->HasStaticMesh())
					{
						// Display list
					}
					else
					{
						if(m_pLayerInfo[LAYER_MAP2].eUVGen == UVGEN_EXPLICITMAPPING)
						{
							glClientActiveTextureARB(GL_TEXTURE0_ARB);
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uMap2Channel));
						}
					}
				}
				else
				{
					bReadyForWire = true;
				}
			}
			else
			{
				// Diffuse + bump (texenv) + map2

				if(nPass == 0 || nPass == 1)
				{
					// Specular passes:

					// Color array:

					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(4, GL_UNSIGNED_BYTE, 0, pObject->GetBumpHVectors());

					// Texcoord arrays:

					glClientActiveTextureARB(GL_TEXTURE1_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uBumpChannel));

					glClientActiveTextureARB(GL_TEXTURE0_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uBumpChannel));
				}
				else if(nPass == 2)
				{
					// Diffuse pass:

					// Color array

					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer     (4, GL_UNSIGNED_BYTE, 0, pObject->GetBumpLVectors());

					// Texcoord arrays:

					glClientActiveTextureARB(GL_TEXTURE1_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uDiffChannel));

					glClientActiveTextureARB(GL_TEXTURE0_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uBumpChannel));
				}
				else if(nPass == 3 && m_pbLayerPresent[LAYER_MAP2])
				{
					if(pObject->HasStaticMesh())
					{
						// Display list
					}
					else
					{
						if(m_pLayerInfo[LAYER_MAP2].eUVGen == UVGEN_EXPLICITMAPPING)
						{
							glClientActiveTextureARB(GL_TEXTURE0_ARB);
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uMap2Channel));
						}
					}
				}
				else
				{
					bReadyForWire = true;
				}
			}
		}
		else
		{
			if(m_pbLayerPresent[LAYER_MAP2])
			{
				// Diffuse + map2

				if(nPass == 0)
				{
					if(pObject->HasStaticMesh())
					{
						// Display list
					}
					else
					{
						if(m_pLayerInfo[LAYER_MAP2].eUVGen == UVGEN_EXPLICITMAPPING)
						{
							glClientActiveTextureARB(GL_TEXTURE1_ARB);
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uMap2Channel));
						}

						if(m_pLayerInfo[LAYER_DIFF].eUVGen == UVGEN_EXPLICITMAPPING)
						{
							glClientActiveTextureARB(GL_TEXTURE0_ARB);
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uDiffChannel));
						}
					}
				}
				else
				{
					bReadyForWire = true;
				}
			}
			else
			{
				// Only diffuse map

				if(nPass == 0)
				{
					if(pObject->HasStaticMesh())
					{
						// Display list
					}
					else
					{
						if(m_pLayerInfo[LAYER_DIFF].eUVGen == UVGEN_EXPLICITMAPPING)
						{
							glClientActiveTextureARB(GL_TEXTURE0_ARB);
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glTexCoordPointer(2, GL_FLOAT, 0, pObject->GetMapChannel(uDiffChannel));
						}
					}
				}
				else
				{
					bReadyForWire = true;
				}
			}
		}
	}
	else
	{
		// No texture layers: No arrays, display list

		if(nPass == 0)
		{
			if(pObject->HasStaticMesh())
			{
				// Display list
			}
			else
			{
				// Vertex+normal arrays (already done)
			}
		}
		else if(nPass > 0)
		{
			bReadyForWire = true;
		}
	}

	// Post wire

	if(m_bPostWire && bReadyForWire)
	{
		CVector4 v4Col = pObject->GetWireColor();
		v4Col.SetW(v4Col.W() * pObject->GetAlpha() * m_v4Diffuse.W());

		UtilGL::States::SetColor(v4Col);

		if(v4Col.W() < (1.0f - ALMOST_ZERO))
		{
			UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
			UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_SRCALPHA);
			UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_INVSRCALPHA);
		}
		else
		{
			UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool SetupTexturing(int nPass, ELayerType eLayerType,
//                                    unsigned uTextureUnit)
//
//  - Purpose   : Setups the states of a texturing unit.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::SetupTexturing(int nPass, ELayerType eLayerType, unsigned uTextureUnit)
{
	SLayerInfo* pLayerInfo = &m_pLayerInfo[eLayerType];
	CTexture*   pTexture   = m_ppTextures[eLayerType];

	assert(pLayerInfo);
	assert(pTexture);

	UtilGL::Texturing::SetCurrentUnit(uTextureUnit);

	switch(pLayerInfo->texInfo.eTextureType)
	{
		case CTexture::TEXTURE2D:
		case CTexture::TEXTURE2DANIM:

			UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
			break;

		case CTexture::TEXTURECUBEMAP:

			UtilGL::Texturing::SafeEnableCubeMapping();
			break;		
	}

	pTexture->SetActive();

	CMatrix texMatrix = m_pTexMatrices[eLayerType];

	// Texture coords generation

	if(pLayerInfo->eUVGen == UVGEN_ENVMAPPING)
	{
		UtilGL::States::Set(UtilGL::States::GENU, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::GENV, UtilGL::States::ENABLED);

		if(pLayerInfo->texInfo.eTextureType == UtilGL::Texturing::CTexture::TEXTURECUBEMAP)
		{
			UtilGL::States::Set(UtilGL::States::GENR,     UtilGL::States::ENABLED);
			UtilGL::States::Set(UtilGL::States::UGENFUNC, UtilGL::States::TEXGEN_REFLECTIONMAP);
			UtilGL::States::Set(UtilGL::States::VGENFUNC, UtilGL::States::TEXGEN_REFLECTIONMAP);
			UtilGL::States::Set(UtilGL::States::RGENFUNC, UtilGL::States::TEXGEN_REFLECTIONMAP);

			CMatrix inverse = (	UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW) * 
								UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_WORLD)).Inverse();

			inverse.RemovePosition();
//			inverse.RemoveScale();

//			CMatrix inverse = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW).Inverse();

			texMatrix = texMatrix * inverse;

			m_pbTexMtxIsIdentity[eLayerType] = false;
		}
		else
		{
			UtilGL::States::Set(UtilGL::States::UGENFUNC, UtilGL::States::TEXGEN_SPHEREMAP);
			UtilGL::States::Set(UtilGL::States::VGENFUNC, UtilGL::States::TEXGEN_SPHEREMAP);
		}
	}
	else if(pLayerInfo->eUVGen == UVGEN_EXPLICITMAPPING)
	{
		UtilGL::States::Set(UtilGL::States::GENU, UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::GENV, UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::GENR, UtilGL::States::DISABLED);
	}

	// Texture env

	if(nPass == 0)
	{
		switch(pLayerInfo->eTexEnv)
		{
		case TEXENV_MODULATE: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); break;
		case TEXENV_ADD:      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);      break;
		}
	}
	else
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);

		switch(pLayerInfo->eTexEnv)
		{
		case TEXENV_MODULATE:

			UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_DSTCOLOR);
			UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_ZERO);

			break;

		case TEXENV_ADD:

			UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_ONE);
			UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_ONE);

			break;
		}
	}

	// Texture matrix

	if(m_pbTexMtxIsIdentity[eLayerType])
	{
		UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_TEXTURE);
	}
	else
	{
		UtilGL::Transforming::SetMatrix(UtilGL::Transforming::MATRIX_TEXTURE, texMatrix);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool EnableCombinersDot3()
//
//  - Purpose   : Setups the register combiners of nvidia cards to perform
//                DOT3 bump mapping.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::EnableCombinersDot3()
{
	glEnable(GL_REGISTER_COMBINERS_NV);

	glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, m_v4Diffuse.Data());
	glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, m_v4Specular.Data());

/*
	We can't use the nvidia nvparse lib together with the OpenGL extensions lib

	nvparse(
		"!!RC1.0                                      \n"
		"{                                            \n"
		"	rgb {                                     \n"
		"		spare0 = expand(col0) . expand(tex1); \n"  // L * N'
		"		spare1 = expand(col1) . expand(tex1); \n"  // H * N'
		"	}                                         \n"
		"   alpha {                                   \n"
		"       discard = expand(col0.b);             \n"
		"       discard = expand(col0.b);             \n"
		"       spare0  = sum();                      \n"
		"       scale_by_four();                      \n"  // spare0.a = 8 * L.z
		"   }                                         \n"
		"}                                            \n"
		"{                                            \n"
		"	rgb {                                     \n"
		"       spare0 = tex0 * unsigned(spare0);     \n"  // Texd * (L * N')
		"		spare1 = unsigned(spare1) *           \n"
		"				 unsigned(spare1);            \n"  // (H * N')^2
		"	}                                         \n"
		"}                                            \n"
		"final_product = spare1 * spare1;             \n"  // (H * N')^4
		"out.rgb = final_product * unsigned(spare0.a) + spare0;\n"  // Texd * (L * N') + ((H * N')^4 * min(8 * max(0, L.z), 1))
		"out.a   = const0.a; \n"                                    // matdiffuse.a
	);
*/

	glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);

	/*** Combiner 0 ***/

	// input RGB

	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV,   GL_EXPAND_NORMAL_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB,       GL_EXPAND_NORMAL_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SECONDARY_COLOR_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB,       GL_EXPAND_NORMAL_NV, GL_RGB);

	// output RGB   :  spare0 = L dot N'  spare1 = H dot N'

	glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB,	GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_TRUE, GL_FALSE);

	/*** Combiner 1 ***/

	// input RGB

	glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV,    GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV,    GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE1_NV,    GL_UNSIGNED_IDENTITY_NV, GL_RGB);

	// input alpha

	glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV,   GL_EXPAND_NORMAL_NV,   GL_BLUE);
	glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO,               GL_UNSIGNED_INVERT_NV, GL_ALPHA);
	glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV,   GL_EXPAND_NORMAL_NV,   GL_BLUE);
	glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO,               GL_UNSIGNED_INVERT_NV, GL_ALPHA);

	// output RGB   :  spare0 = texdiffuse * spare0, spare1 = spare1 * spare1

	glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB,	GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

	// output alpha :  spare0.a = 4 * (L.z + L.z) = L.z * 8

	glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_SCALE_BY_FOUR_NV, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

	/*** Final combiner ***/

	// A = unsigned(spare0.a) : min(8 * max(0, L.z), 1)) (self shadowing factor)
	// B = spare1*spare1      : (H * N')^4               (specular)
	// C = 0
	// D = spare0             : (L * N') * texdiffuse :   (diffuse)
	// E = spare1             \
	// F = spare1             / Goes to B multiplied
	// G = const0.a

	// B=E*F, result=AB+(1-A)C+D

	glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_SPARE0_NV,          GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
	glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_E_TIMES_F_NV,       GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO,               GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_SPARE0_NV,          GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glFinalCombinerInputNV(GL_VARIABLE_E_NV, GL_SPARE1_NV,          GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glFinalCombinerInputNV(GL_VARIABLE_F_NV, GL_SPARE1_NV,          GL_UNSIGNED_IDENTITY_NV, GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool EnableTexEnvDot3(int nPass, bool bDiffuse)
//
//  - Purpose   : Setups the texenv parameters to perform DOT3 bump mapping.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::EnableTexEnvDot3(int nPass, bool bDiffuse)
{
	if(bDiffuse == false)
	{
		// Specular lighting: Primary color Dot3 Texture (H dot N')

		UtilGL::Texturing::SetCurrentUnit(0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_PRIMARY_COLOR_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

		// Raise the power to 2 (H dot L)^2

		UtilGL::Texturing::SetCurrentUnit(1);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

		if(nPass > 0)
		{
			// Pass 0 is the first specular pass, the second performs another
			// specular pass and the two should be multiplied together to
			// get the (H dot L)^4 result:

			UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
			UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_DSTCOLOR);
			UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_ZERO);
		}
	}
	else
	{
		// Diffuse lighting: Primary color Dot3 Texture (L dot N)

		UtilGL::Texturing::SetCurrentUnit(0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_PRIMARY_COLOR_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

		// Modulate by diffuse texture (bitmap)

		UtilGL::Texturing::SetCurrentUnit(1);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// Multi pass diffuse (add diffuse to specular in the framebuffer)

		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND, UtilGL::States::BLEND_ONE);
		UtilGL::States::Set(UtilGL::States::DSTBLEND, UtilGL::States::BLEND_ONE);
	}

	UtilGL::Texturing::SetCurrentUnit(0);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool IsOpaque() const
//
//  - Purpose   : Returns true if the shader doesn't need blending for an
//                opaque object when rendering, otherwise false.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::IsOpaque() const
{
	if(	(m_nBlendSrcFactor == UtilGL::States::BLEND_ONE  && m_nBlendDstFactor == UtilGL::States::BLEND_ZERO) ||
		(m_nBlendSrcFactor == UtilGL::States::BLEND_ZERO && m_nBlendDstFactor == UtilGL::States::BLEND_ONE))
	{
		return true;
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : int GetBumpChannel() const
//
//  - Purpose   : Gets the index of the bump mapping channel for all objects
//                who have this shader.
//
// -----------------------------------------------------------------------------
int CShaderStandard::GetBumpChannel() const
{
	if(!m_pbLayerPresent[LAYER_BUMP])
	{
		return -1;
	}

	if(m_pLayerInfo[LAYER_BUMP].eUVGen != UVGEN_EXPLICITMAPPING)
	{
		return -1;
	}

	return m_pLayerInfo[LAYER_BUMP].uMapChannel;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool bool SetDefaults()
//
//  - Purpose   : Sets the default shader settings.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::SetDefaults()
{
	m_v4Ambient.Set (0.2f, 0.2f, 0.2f, 1.0f);
	m_v4Diffuse.Set (0.7f, 0.7f, 0.7f, 1.0f);
	m_v4Specular.Set(0.0f, 0.0f, 0.0f, 1.0f);

	m_fShininess         = 60.0f; 
	m_bTwoSided          = false;
	m_bFaceted           = false;
	m_bPostWire          = false;
	m_fWireLineThickness = 1.0f;
	m_nBlendSrcFactor    = UtilGL::States::BLEND_ONE;
	m_nBlendDstFactor    = UtilGL::States::BLEND_ZERO;

	return FreeResources();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool HasLayer(ELayerType eLayerType) const
//
//  - Purpose   : Returns true if the given layer's present.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::HasLayer(ELayerType eLayerType) const
{
	return m_pbLayerPresent[eLayerType];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool GetLayer(ELayerType eLayerType, SLayerInfo* pLayerInfo) const
//
//  - Purpose   : Puts in pLayerInfo the information of the given layer.
//                Remains unchanged if the return value is false (meaning nonexistant).
//
// -----------------------------------------------------------------------------
bool CShaderStandard::GetLayer(ELayerType eLayerType, SLayerInfo* pLayerInfo) const
{
	if(!m_pbLayerPresent[eLayerType])
	{
		return false;
	}

	assert(pLayerInfo);

	*pLayerInfo = m_pLayerInfo[eLayerType];

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool SetLayer(ELayerType eLayerType, const SLayerInfo& layerInfo)
//
//  - Purpose   : Sets one of the shader's layer properties.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::SetLayer(ELayerType eLayerType, const SLayerInfo& layerInfo)
{
	m_pLayerInfo    [eLayerType] = layerInfo;
	m_pbLayerPresent[eLayerType] = true;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool EraseLayer(ELayerType eLayerType)
//
//  - Purpose   : Erases one of the shader's layers, and frees its resources.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::EraseLayer(ELayerType eLayerType)
{
	m_pbLayerPresent[eLayerType] = false;

	if(m_ppTextures[eLayerType])
	{
		delete m_ppTextures[eLayerType];
		m_ppTextures[eLayerType] = NULL;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : UtilGL::Texturing::CTexture* GetTexture(ELayerType eLayerType)
//
//  - Purpose   : Returns the given texture (NULL if it doesn't exist).
//
// -----------------------------------------------------------------------------
UtilGL::Texturing::CTexture* CShaderStandard::GetTexture(ELayerType eLayerType)
{
	return m_ppTextures[eLayerType];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool SetTexture(ELayerType eLayerType,
//                                UtilGL::Texturing::CTexture* pTexture,
//                                bool bDeletePrevious);
//
//  - Purpose   : Sets the texture in the given channel and deletes optionally
//                the existing one.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::SetTexture(	ELayerType eLayerType,
									UtilGL::Texturing::CTexture* pTexture,
									bool bDeletePrevious)
{

	if(bDeletePrevious && m_ppTextures[eLayerType] != pTexture)
	{
		SAFE_DELETE(m_ppTextures[eLayerType]);
	}

	m_ppTextures    [eLayerType] = pTexture;
	m_pbLayerPresent[eLayerType] = pTexture ? true : false;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool ParseShader(CInput* pInput)
//
//  - Purpose   : Gets all shader's properties parsing a text.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::ParseShader(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	SetDefaults();

	CAdvancedParser shaderParser;

	if(!shaderParser.Init(pInput))
	{
		return false;
	}

	unsigned uError = 0;

	// Shader properties

	std::string strBlendSrcFactor, strBlendDstFactor;

	shaderParser.ReadKeyVector4("#ambient",        '=', &m_v4Ambient,          CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyVector4("#diffuse",        '=', &m_v4Diffuse,          CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyVector4("#specular",       '=', &m_v4Specular,         CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyFloat  ("#shininess",      '=', &m_fShininess,         CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyBool   ("#twosided",       '=', &m_bTwoSided,          CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyBool   ("#faceted",        '=', &m_bFaceted,           CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyBool   ("#postwire",       '=', &m_bPostWire,          CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyFloat  ("#wirethickness",  '=', &m_fWireLineThickness, CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyRaw    ("#blendsrcfactor", '=', &strBlendSrcFactor,    CAdvancedParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyRaw    ("#blenddstfactor", '=', &strBlendDstFactor,    CAdvancedParser::SEARCH_WHOLEDATA);

	if(m_v4Specular.X() > 0.5f && m_v4Specular.Y() > 0.5f && m_v4Specular.Z() > 0.5f && m_fShininess < 1.3f)
	{
		// FIX:  Backwards compatibility with the previous specular ilumination model.
		//	 	 Now we have separate specular color added to the final fragment.
		// TODO: Think about a better method :)

		m_v4Specular.Set(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if     (strBlendSrcFactor == "GL_ZERO")                m_nBlendSrcFactor = UtilGL::States::BLEND_ZERO;
	else if(strBlendSrcFactor == "GL_ONE")                 m_nBlendSrcFactor = UtilGL::States::BLEND_ONE;
	else if(strBlendSrcFactor == "GL_DST_COLOR")           m_nBlendSrcFactor = UtilGL::States::BLEND_DSTCOLOR;
	else if(strBlendSrcFactor == "GL_ONE_MINUS_DST_COLOR") m_nBlendSrcFactor = UtilGL::States::BLEND_INVDSTCOLOR;
	else if(strBlendSrcFactor == "GL_SRC_ALPHA")           m_nBlendSrcFactor = UtilGL::States::BLEND_SRCALPHA;
	else if(strBlendSrcFactor == "GL_ONE_MINUS_SRC_ALPHA") m_nBlendSrcFactor = UtilGL::States::BLEND_INVSRCALPHA;
	else if(strBlendSrcFactor == "GL_DST_ALPHA")           m_nBlendSrcFactor = UtilGL::States::BLEND_DSTALPHA;
	else if(strBlendSrcFactor == "GL_ONE_MINUS_DST_ALPHA") m_nBlendSrcFactor = UtilGL::States::BLEND_INVDSTALPHA;
	else if(strBlendSrcFactor == "GL_SRC_ALPHA_SATURATE")  m_nBlendSrcFactor = UtilGL::States::BLEND_SRCALPHASATURATE;
	else
	{
		LOG.Write("\nWARNING - CShaderStandard::ParseShader(), unknown blendsrcfactor (%s)", strBlendSrcFactor.data());
	}

	if     (strBlendDstFactor == "GL_ZERO")                m_nBlendDstFactor = UtilGL::States::BLEND_ZERO;
	else if(strBlendDstFactor == "GL_ONE")                 m_nBlendDstFactor = UtilGL::States::BLEND_ONE;
	else if(strBlendDstFactor == "GL_DST_COLOR")           m_nBlendDstFactor = UtilGL::States::BLEND_DSTCOLOR;
	else if(strBlendDstFactor == "GL_ONE_MINUS_DST_COLOR") m_nBlendDstFactor = UtilGL::States::BLEND_INVDSTCOLOR;
	else if(strBlendDstFactor == "GL_SRC_ALPHA")           m_nBlendDstFactor = UtilGL::States::BLEND_SRCALPHA;
	else if(strBlendDstFactor == "GL_ONE_MINUS_SRC_ALPHA") m_nBlendDstFactor = UtilGL::States::BLEND_INVSRCALPHA;
	else if(strBlendDstFactor == "GL_DST_ALPHA")           m_nBlendDstFactor = UtilGL::States::BLEND_DSTALPHA;
	else if(strBlendDstFactor == "GL_ONE_MINUS_DST_ALPHA") m_nBlendDstFactor = UtilGL::States::BLEND_INVDSTALPHA;
	else
	{
		LOG.Write("\nWARNING - CShaderStandard::ParseShader(), unknown blenddstfactor (%s)", strBlendDstFactor.data());
	}

	// Shader layers

	std::string strLayer;

	// Diffuse

	if(shaderParser.Find("#layerdiffuse", CParser::SEARCH_WHOLEDATA) != -1)
	{
		if(shaderParser.ReadStringBetweenChars('{', '}', &strLayer, CParser::SEARCH_FROMCURPOS, true))
		{
			if(!ParseLayer(LAYER_DIFF, strLayer))
			{
				LOG.Write("\nERROR - CShaderStandard::ParseShader(), error parsing #layerdiffuse");
				uError++;
			}
		}
		else
		{
			LOG.Write("\nERROR - CShaderStandard::ParseShader(), error reading #layerdiffuse field");
			uError++;
		}
	}

	// Bump

	if(shaderParser.Find("#layerbump", CParser::SEARCH_WHOLEDATA) != -1)
	{
		strLayer.erase();

		if(shaderParser.ReadStringBetweenChars('{', '}', &strLayer, CParser::SEARCH_FROMCURPOS, true))
		{
			if(!ParseLayer(LAYER_BUMP, strLayer))
			{
				LOG.Write("\nWARNING - CShaderStandard::ParseShader(), error parsing #layerbump");
				uError++;
			}
		}
		else
		{
			LOG.Write("\nWARNING - CShaderStandard::ParseShader(), error reading #layerbump field");
			uError++;
		}
	}

	// Map2

	if(shaderParser.Find("#layermap2", CParser::SEARCH_WHOLEDATA) != -1)
	{
		strLayer.erase();

		if(shaderParser.ReadStringBetweenChars('{', '}', &strLayer, CParser::SEARCH_FROMCURPOS, true))
		{
			if(!ParseLayer(LAYER_MAP2, strLayer))
			{
				LOG.Write("\nWARNING - CShaderStandard::ParseShader(), error parsing #layermap2");
				uError++;
			}
		}
		else
		{
			LOG.Write("\nWARNING - CShaderStandard::ParseShader(), error reading #layermap2 field");
			uError++;
		}
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CShaderStandard
//  - prototype : bool ParseLayer(ELayerType eLayerType, const std::string& strLayer)
//
//  - Purpose   : Parses a layer field.
//
// -----------------------------------------------------------------------------
bool CShaderStandard::ParseLayer(ELayerType eLayerType, const std::string& strLayer)
{
	m_pbLayerPresent[eLayerType] = false;

	SLayerInfo* pLayerInfo = &m_pLayerInfo[eLayerType];

	CAdvancedParser shaderParser;
	CInputMemory    inputString;

	inputString.Open(const_cast<char *>(strLayer.data()), strLayer.length());

	if(!shaderParser.Init(&inputString))
	{
		return false;
	}

	// Set default values before parsing

	pLayerInfo->eTexEnv     = TEXENV_MODULATE;
	pLayerInfo->eUVGen      = UVGEN_ENVMAPPING; // To avoid indexing a nonexistant mapping channel
	pLayerInfo->uMapChannel = 0;
	pLayerInfo->v3ScrollOffset.Set  (0.0f, 0.0f, 0.0f);
	pLayerInfo->v3RotationOffset.Set(0.0f, 0.0f, 0.0f);
	pLayerInfo->v3ScrollSpeed.Set   (0.0f, 0.0f, 0.0f);
	pLayerInfo->v3RotationSpeed.Set (0.0f, 0.0f, 0.0f);

	// Parse
	
	unsigned uError = 0;

	std::string strUVGen = "ENVMAPPING", strTexEnv = "MODULATE";

	shaderParser.ReadKeyRaw("#texenv", '=', &strTexEnv, CParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyRaw("#uvgen",  '=', &strUVGen,  CParser::SEARCH_WHOLEDATA);

	shaderParser.ReadKeyVector3("#uvwscrolloffset", '=', &pLayerInfo->v3ScrollOffset,   CParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyVector3("#uvwrotateoffset", '=', &pLayerInfo->v3RotationOffset, CParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyVector3("#uvwscrollspeed",  '=', &pLayerInfo->v3ScrollSpeed,    CParser::SEARCH_WHOLEDATA);
	shaderParser.ReadKeyVector3("#uvwrotatespeed",  '=', &pLayerInfo->v3RotationSpeed,  CParser::SEARCH_WHOLEDATA);

	if(uError)
	{
		return false;
	}

	// Texenv

	if(strTexEnv == "MODULATE")
	{
		pLayerInfo->eTexEnv = TEXENV_MODULATE;
	}
	else if(strTexEnv == "ADD")
	{
		pLayerInfo->eTexEnv = TEXENV_ADD;
	}
	else
	{
		LOG.Write("\nWARNING - CShaderStandard::ParseLayer(), invalid texenv (%s)", strTexEnv.data());
		pLayerInfo->eTexEnv = TEXENV_MODULATE;
	}

	// UVGen and mapping channel

	if(strUVGen == "EXPLICIT")
	{
		pLayerInfo->eUVGen = UVGEN_EXPLICITMAPPING;

		if(!shaderParser.ReadKeyInt("#mapchannel", '=', (int *)(&pLayerInfo->uMapChannel), CParser::SEARCH_WHOLEDATA))
		{
			LOG.Write("\nWARNING - CShaderStandard::ParseLayer(), error parsing mapchannel");
		}
	}
	else if(strUVGen == "ENVMAPPING")
	{
		pLayerInfo->eUVGen = UVGEN_ENVMAPPING;
	}
	else
	{
		LOG.Write("\nWARNING - CShaderStandard::ParseLayer(), invalid uvgen (%s)", strUVGen.data());
		pLayerInfo->eUVGen = UVGEN_ENVMAPPING;
	}

	// Texture

	if(!shaderParser.ReadTextureInfo("#texture", &pLayerInfo->texInfo, CParser::SEARCH_WHOLEDATA))
	{
		uError++;
	}

	if(uError)
	{
		return false;
	}

	m_pbLayerPresent[eLayerType] = true;
	return true;
}
