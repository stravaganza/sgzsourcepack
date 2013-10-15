// ==[ File ]===================================================================================
//
//  -Name     : BuildShaders.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Shader related.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"
#include <acubic.cpp>
#include <shaders.h>

enum EMaxMapState
{
	MAXMAPSTATE_NONE     = 0,
	MAXMAPSTATE_DISABLED = 1,
	MAXMAPSTATE_ENABLED  = 2
};


// ==[ Implementation ]=========================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : unsigned MaterialCount() const
//
//  - Purpose   : Returns the number of materials in our list.
//
// -----------------------------------------------------------------------------
unsigned CStravaganzaMaxTools::MaterialCount() const
{
	return m_vecMaterials.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : void AddMaterial(Mtl* pMaxMtl)
//
//  - Purpose   : Adds a material to the list. A check is performed so that
//                only non repeated elements are inserted.
//
// -----------------------------------------------------------------------------
void CStravaganzaMaxTools::AddMaterial(Mtl* pMaxMtl)
{
	if(!pMaxMtl) return;

	std::vector<Mtl*>::iterator it;

	for(it = m_vecMaterials.begin(); it != m_vecMaterials.end(); ++it)
	{
		if(*it == pMaxMtl) return;
	}

	m_vecMaterials.push_back(pMaxMtl);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : int GetMaterialIndex(Mtl* pMaxMtl) const
//
//  - Purpose   : Returns the position in the list of the given material.
//                -1 is returned if not found.
//
// -----------------------------------------------------------------------------
int CStravaganzaMaxTools::GetMaterialIndex(Mtl* pMaxMtl) const
{
	std::vector<Mtl*>::const_iterator it;
	int nPos = 0;

	for(it = m_vecMaterials.begin(); it != m_vecMaterials.end(); ++it, nPos++)
	{
		if(*it == pMaxMtl) return nPos;
	}

	return -1;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : Mtl* GetMaterial(unsigned uIndex) const
//
//  - Purpose   : Returns the material given an index. If the index is invalid
//                GetMaterial() returns NULL.
//
// -----------------------------------------------------------------------------
Mtl* CStravaganzaMaxTools::GetMaterial(unsigned uIndex) const
{
	if(uIndex < 0 || uIndex >= m_vecMaterials.size())
	{
		return NULL;
	}

	return m_vecMaterials[uIndex];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : CShaderStandard* GetShader(const std::string& strName)
//
//  - Purpose   : Returns the shader from the list whose name matches the
//                parameter. If not found returns NULL.
//
//  - Note      : Of course BuildShaders() should have been called.
//
// -----------------------------------------------------------------------------
CShaderStandard* CStravaganzaMaxTools::GetShader(const std::string& strName)
{
	std::vector<CShaderStandard*>::const_iterator it;

	for(it = m_vecShaders.begin(); it != m_vecShaders.end(); ++it)
	{
		if((*it)->GetName() == strName)
		{
			return *it;
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool BuildShaders()
//
//  - Purpose   : Builds the shader list from MAX's materials.
//                Preview mode requires texture files to be stored with full
//                path in order to load them. When we export, we only store the
//                filename. Another thing is that in the export mode, we copy
//                all textures into the path specified by the user if that
//                option is checked.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::BuildShaders()
{
	std::vector<Mtl*>::iterator it;

	assert(m_vecShaders.empty());

	if(!m_bPreview && m_bCopyTextures && m_strTexturePath == "")
	{
		CLogger::NotifyWindow("Textures won't be copied\nSpecify a valid output texture path first");
	}

	LOG.Write("\n\n-Building shaders: ");

	for(it = m_vecMaterials.begin(); it != m_vecMaterials.end(); ++it)
	{
		Mtl* pMaxMaterial = *it;
		assert(pMaxMaterial);

		LOG.Write("\n    %s", pMaxMaterial->GetName().data());
		CShaderStandard* pShaderStd = new CShaderStandard;
		pShaderStd->SetName(pMaxMaterial->GetName().data());

		// Properties

		StdMat2 *pMaxStandardMtl = NULL;
		StdMat2 *pMaxBakedMtl    = NULL;

		float fAlpha;

		if(pMaxMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
		{
			pMaxStandardMtl = (StdMat2 *)pMaxMaterial;
		}
		else if(pMaxMaterial->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID, 0))
		{
			pMaxStandardMtl = (StdMat2 *)pMaxMaterial->GetSubMtl(0);
			pMaxBakedMtl    = (StdMat2 *)pMaxMaterial->GetSubMtl(1);
		}

		if(pMaxStandardMtl)
		{
			// Standard material

			fAlpha = pMaxStandardMtl->GetOpacity(0);

			Shader* pMaxShader = pMaxStandardMtl->GetShader();

			CVector4 v4Specular = ColorToVector4(pMaxStandardMtl->GetSpecular(0), 0.0f) * pMaxShader->GetSpecularLevel(0, 0);

			pShaderStd->SetAmbient  (ColorToVector4(pMaxStandardMtl->GetAmbient(0),  0.0f));
			pShaderStd->SetDiffuse  (ColorToVector4(pMaxStandardMtl->GetDiffuse(0),  fAlpha));
			pShaderStd->SetSpecular (v4Specular);
			pShaderStd->SetShininess(pMaxShader->GetGlossiness(0, 0) * 128.0f);

			if(pMaxStandardMtl->GetTwoSided() == TRUE)
			{
				pShaderStd->SetTwoSided(true);
			}

			// Need to cast to StdMat2 in order to get access to IsFaceted().
			// ¿Is StdMat2 always the interface for standard materials?
			if(((StdMat2*)pMaxStandardMtl)->IsFaceted())
			{
				pShaderStd->SetFaceted(true);
			}

			if(pMaxStandardMtl->GetWire() == TRUE)
			{
				pShaderStd->SetPostWire(true);
				pShaderStd->SetWireLineThickness(pMaxStandardMtl->GetWireSize(0));
			}
		}
		else
		{
			// Material != Standard

			fAlpha = 1.0f; // pMaxMaterial->GetXParency();

			pShaderStd->SetAmbient  (ColorToVector4(pMaxMaterial->GetAmbient(),  0.0f));
			pShaderStd->SetDiffuse  (ColorToVector4(pMaxMaterial->GetDiffuse(),  fAlpha));
			pShaderStd->SetSpecular (CVector4(0.0f, 0.0f, 0.0f, 0.0f));
			pShaderStd->SetShininess(0.0f);
		}

		// Layers

		if(!pMaxStandardMtl)
		{
			m_vecShaders.push_back(pShaderStd);
			continue;
		}

		bool bDiffuseMap32Bits = false;
		StdMat2 *pStandardMtl;

		for(int i = 0; i < 3; i++)
		{
			int nMap;

			pStandardMtl = pMaxStandardMtl;

			// 0 = diffuse, 1 == bump, 2 = lightmap (self illumination slot) or envmap (reflection slot)

			if(i == 0)
			{
				nMap = ID_DI;
			}
			else if(i == 1)
			{
				nMap = ID_BU;

				// If its a baked material, get the bump map from there

				if(pMaxBakedMtl)
				{
					pStandardMtl = pMaxBakedMtl;
				}
			}
			else if(i == 2)
			{
				bool bBaked = false;

				// If its a baked material, get the map2 (lightmap) from there

				if(pMaxBakedMtl)
				{
					if(pMaxBakedMtl->GetMapState(ID_SI) == MAXMAPSTATE_ENABLED)
					{
						bBaked       = true;
						nMap         = ID_SI;
						pStandardMtl = pMaxBakedMtl;
					}
				}

				if(!bBaked)
				{
					if(pStandardMtl->GetMapState(ID_SI) == MAXMAPSTATE_ENABLED)
					{
						nMap = ID_SI;
					}
					else
					{
						nMap = ID_RL;
					}
				}
			}

			// Check validity

			if(pStandardMtl->GetMapState(nMap) != MAXMAPSTATE_ENABLED)
			{
				if(i == 0)
				{
					LOG.Write("\n        No diffuse. Skipping.");
					break;
				}

				continue;
			}

			Texmap* pMaxTexmap = pStandardMtl->GetSubTexmap(nMap);

			if(!pMaxTexmap)
			{
				if(i == 0)
				{
					LOG.Write("\n        No diffuse. Skipping.");
					break;
				}

				continue;
			}

			// Get texmaps

			std::vector<std::string> vecTextures, vecPaths;

			CShaderStandard::SLayerInfo  layerInfo;
			CShaderStandard::SBitmapInfo bitmapInfo;

			if(pMaxTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
			{
				BitmapTex* pMaxBitmapTex = (BitmapTex*)pMaxTexmap;
				Bitmap*    pMaxBitmap    = pMaxBitmapTex->GetBitmap(SECONDS_TO_TICKS(m_fStartTime));
				StdUVGen*  pMaxUVGen     = pMaxBitmapTex->GetUVGen();

				if(!pMaxBitmap)
				{
					if(i == 0)
					{
						LOG.Write("\n        Invalid diffuse. Skipping.");
						break;
					}
					continue;
				}

				assert(pMaxUVGen);

				BitmapInfo bi = pMaxBitmap->Storage()->bi;

				// bi.Name() returns the full path
				// bi.Filename() returns just the filename

				vecTextures.push_back(bi.Filename());
				vecPaths.   push_back(bi.Name());

				LOG.Write("\n        Bitmap %s", vecTextures[0].data());

				// Check if diffuse texture has alpha channel

				if(i == 0)
				{
					CBitmap    bitmap;
					CInputFile bitmapFile;

					if(!bitmapFile.Open(bi.Name(), false))
					{
						CLogger::NotifyWindow("WARNING - CStravaganzaMaxTools::BuildShaders():\nUnable to load file %s", bi.Name());
					}
					else
					{
						if(!bitmap.Load(&bitmapFile, GetFileExt(bi.Name())))
						{
							CLogger::NotifyWindow("WARNING - CStravaganzaMaxTools::BuildShaders():\nUnable to load bitmap %s", bi.Name());
						}
						else
						{
							if(bitmap.GetBpp() == 32)
							{
								bDiffuseMap32Bits = true;
								LOG.Write(" (with alpha channel)");
							}
							bitmap.Free();
						}
						bitmapFile.Close();
					}
				}

				// Ok, copy properties

				layerInfo.texInfo.bLoop        = false;
				layerInfo.texInfo.eTextureType = UtilGL::Texturing::CTexture::TEXTURE2D;

				bitmapInfo.strFile         = m_bPreview ? bi.Name() : bi.Filename();
				bitmapInfo.bTile           = ((pMaxUVGen->GetTextureTiling() & (U_WRAP | V_WRAP)) == (U_WRAP | V_WRAP)) ? true : false;
				bitmapInfo.fSeconds        = 0.0f;
				bitmapInfo.bForceFiltering = false;
				bitmapInfo.eFilter         = UtilGL::Texturing::FILTER_TRILINEAR; // won't be used (forcefiltering = false)
				
				layerInfo.texInfo.m_vecBitmaps.push_back(bitmapInfo);

				layerInfo.eTexEnv          = nMap == ID_RL ? CShaderStandard::TEXENV_ADD : CShaderStandard::TEXENV_MODULATE;
				layerInfo.eUVGen           = pMaxUVGen->GetCoordMapping(0) == UVMAP_SPHERE_ENV ? CShaderStandard::UVGEN_ENVMAPPING : CShaderStandard::UVGEN_EXPLICITMAPPING;
				layerInfo.uMapChannel      = pMaxUVGen->GetMapChannel();
				layerInfo.v3ScrollSpeed    = CVector3(0.0f, 0.0f, 0.0f);
				layerInfo.v3RotationSpeed  = CVector3(0.0f, 0.0f, 0.0f);
				layerInfo.v3ScrollOffset   = CVector3(pMaxUVGen->GetUOffs(0), pMaxUVGen->GetVOffs(0), 0.0f);
				layerInfo.v3RotationOffset = CVector3(pMaxUVGen->GetUAng(0),  pMaxUVGen->GetVAng(0),  pMaxUVGen->GetWAng(0));
			}
			else if(pMaxTexmap->ClassID() == Class_ID(ACUBIC_CLASS_ID, 0))
			{
				ACubic*       pMaxCubic  = (ACubic*)pMaxTexmap;
				IParamBlock2* pBlock     = pMaxCubic->pblock;
				Interval      validRange = m_pMaxInterface->GetAnimRange();

				for(int nFace = 0; nFace < 6; nFace++)
				{
					int nMaxFace;

					switch(nFace)
					{
					case 0: nMaxFace = 3; break;
					case 1: nMaxFace = 2; break;
					case 2: nMaxFace = 1; break;
					case 3: nMaxFace = 0; break;
					case 4: nMaxFace = 5; break;
					case 5: nMaxFace = 4; break;
					}

					TCHAR *name;
					pBlock->GetValue(acubic_bitmap_names, TICKS_TO_SECONDS(m_fStartTime), name, validRange, nMaxFace);

					vecPaths.push_back(name);

					CStr path, file, ext;
					SplitFilename(CStr(name), &path, &file, &ext);

					std::string strFile = std::string(file.data()) + ext.data();

					vecTextures.push_back(strFile);

					bitmapInfo.strFile         = m_bPreview ? name : strFile;
					bitmapInfo.bTile           = false;
					bitmapInfo.fSeconds        = 0.0f;
					bitmapInfo.bForceFiltering = false;
					bitmapInfo.eFilter         = UtilGL::Texturing::FILTER_TRILINEAR;
					
					layerInfo.texInfo.m_vecBitmaps.push_back(bitmapInfo);
				}

				layerInfo.texInfo.bLoop        = false;
				layerInfo.texInfo.eTextureType = UtilGL::Texturing::CTexture::TEXTURECUBEMAP;

				layerInfo.eTexEnv          = nMap == ID_RL ? CShaderStandard::TEXENV_ADD : CShaderStandard::TEXENV_MODULATE;
				layerInfo.eUVGen           = CShaderStandard::UVGEN_ENVMAPPING;
				layerInfo.uMapChannel      = 0;
				layerInfo.v3ScrollSpeed    = CVector3(0.0f, 0.0f, 0.0f);
				layerInfo.v3RotationSpeed  = CVector3(0.0f, 0.0f, 0.0f);
				layerInfo.v3ScrollOffset   = CVector3(0.0f, 0.0f, 0.0f);
				layerInfo.v3RotationOffset = CVector3(0.0f, 0.0f, 0.0f);
			}
			else
			{
				if(i == 0)
				{
					LOG.Write("\n        No diffuse. Skipping.");
					break;
				}
				continue;
			}

			if(!m_bPreview && m_bCopyTextures && m_strTexturePath != "")
			{
				for(int nTex = 0; nTex != vecTextures.size(); nTex++)
				{
					// Copy textures into the specified folder

					std::string strDestPath = m_strTexturePath;

					if(strDestPath[strDestPath.length() - 1] != '\\')
					{
						strDestPath.append("\\", 1);
					}

					strDestPath.append(vecTextures[nTex]);

					if(!CopyFile(vecPaths[nTex].data(), strDestPath.data(), FALSE))
					{
						CLogger::NotifyWindow("Unable to copy %s to\n%s", vecPaths[i], strDestPath.data());
					}
				}
			}

			if(layerInfo.eUVGen == CShaderStandard::UVGEN_ENVMAPPING && i == 1)
			{
				CLogger::NotifyWindow("%s : Bump with spheremapping not supported", pShaderStd->GetName().data());
			}
			else
			{
				// Add layer

				switch(i)
				{
				case 0: pShaderStd->SetLayer(CShaderStandard::LAYER_DIFF, layerInfo); break;
				case 1: pShaderStd->SetLayer(CShaderStandard::LAYER_BUMP, layerInfo); break;
				case 2: pShaderStd->SetLayer(CShaderStandard::LAYER_MAP2, layerInfo); break;
				}
			}
		}

		// ¿Do we need blending?

		if(ARE_EQUAL(fAlpha, 1.0f) && !bDiffuseMap32Bits)
		{
			pShaderStd->SetBlendSrcFactor(UtilGL::States::BLEND_ONE);
			pShaderStd->SetBlendDstFactor(UtilGL::States::BLEND_ZERO);
		}
		else
		{
			pShaderStd->SetBlendSrcFactor(UtilGL::States::BLEND_SRCALPHA);
			pShaderStd->SetBlendDstFactor(UtilGL::States::BLEND_INVSRCALPHA);
		}

		// Add shader

		m_vecShaders.push_back(pShaderStd);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool ExportShaders()
//
//  - Purpose   : Builds an ascii file with all shader properties.
//                The file path and name are build from the scene file, the only
//                difference is the extension (.sha)
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::ExportShaders()
{
	CWriterASCII writer;
	std::string  strShaderFile;
	int nFinalPathPos = m_strFile.length() - 1;

	while(m_strFile[nFinalPathPos] != '.' && nFinalPathPos > 0)
	{
		nFinalPathPos--;
	}

	if(!writer.InitFile(m_strFile.substr(0, nFinalPathPos) + ".sha"))
	{
		return false;
	}

	std::vector<CShaderStandard*>::iterator it;

	for(it = m_vecShaders.begin(); it < m_vecShaders.end(); ++it)
	{
		if(*it == NULL)
		{
			continue;
		}

		(*it)->WriteASCII(&writer);
	}

	writer.Close();
	return true;
}
