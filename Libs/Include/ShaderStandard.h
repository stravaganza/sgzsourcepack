// ==[ File ]===================================================================================
//
//  -Name     : ShaderStandard.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CShaderStandard definition.
//
// =============================================================================================

/*

	07/01/2003  -  File created.


*/


#ifndef __SHADERSTANDARD_H
#define __SHADERSTANDARD_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CShaderStandard
//
//  - Purpose   : Standard shader.
//
// ---------------------------------------------------------------------------
class CShaderStandard : public CShader
{


public:


	// Enumerations & Structures /////////////////////////////////////////////


	// Featured layers
	enum ELayerType
	{
		LAYER_DIFF = 0, // Diffuse texture
		LAYER_BUMP,     // Bump texture
		LAYER_MAP2      // Additional texture (usually lightmap or envmap)
	};

	// Texture functions
	enum ETexEnv
	{
		TEXENV_MODULATE,
		TEXENV_ADD
	};

	// Layer texture coordinates generation
	enum EUVGen
	{
		UVGEN_EXPLICITMAPPING,
		UVGEN_ENVMAPPING
	};

	// Texturing

	struct SBitmapInfo
	{
		SBitmapInfo()  { strFile = ""; bTile = true; fSeconds = 0; bForceFiltering = false; }
		~SBitmapInfo() { }

		std::string strFile;

		bool  bTile;
		float fSeconds;        // Only for animated textures
		bool  bForceFiltering; // If true use filter below, otherwise use default.

		UtilGL::Texturing::ETextureFilter eFilter;
	};

	struct STextureInfo
	{
		STextureInfo()  { eTextureType = UtilGL::Texturing::CTexture::TEXTURE2D; bLoop = false; }
		~STextureInfo() { }

		UtilGL::Texturing::CTexture::ETextureType eTextureType; // Check !m_vecBitmaps.empty() first
		bool bLoop;                                             // only for animated textures

		std::vector<SBitmapInfo> m_vecBitmaps;
	};

	// Layer Information structure

	struct SLayerInfo
	{
		SLayerInfo()  { eTexEnv = TEXENV_MODULATE; eUVGen = UVGEN_ENVMAPPING; uMapChannel = 0; }
		~SLayerInfo() { }

		STextureInfo texInfo;         // Texture info.

		ETexEnv     eTexEnv;          // Texture operation over primary color
		EUVGen      eUVGen;           // Mapping coordinates autogeneration.
		unsigned    uMapChannel;      // If explicit mapping specified, here goes the mapping channel index used.
		CVector3    v3ScrollSpeed;    // Units/second of mapping coords scroll
		CVector3    v3ScrollOffset;   // Units of initial mapping scroll.
		CVector3    v3RotationSpeed;  // Angles/second of mapping coords rotation
		CVector3    v3RotationOffset; // Angles of initial mapping rotation.
	};


	// Methods ///////////////////////////////////////////////////////////////////


	CShaderStandard();
	~CShaderStandard();

	// From CShader:

	bool WriteASCII		(CWriterASCII* pWriter) const;

	bool CompileObject	(CObject* pObject) const;

	bool LoadResources	(const std::string& strDatfile   = "",
						 const std::string& strTexFolder = "",
						 UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL);
	bool FreeResources	();

	bool SetEvalTime	(float fTime);
	bool Begin			();
	bool End			();

	int  GetNumPasses	() const;
	bool PrepareStates	(int nPass);
	bool PrepareObject	(int nPass, CObject* pObject);

	bool IsOpaque		() const;
	int  GetBumpChannel	() const;

	// Non virtual

	CVector4 GetAmbient           ()                      const { return m_v4Ambient;                        }
	CVector4 GetDiffuse           ()                      const { return m_v4Diffuse;                        }
	CVector4 GetSpecular          ()                      const { return m_v4Specular;                       }
	float    GetShininess         ()                      const { return m_fShininess;                       }
	bool     GetTwoSided          ()                      const { return m_bTwoSided;                        }
	bool     GetFaceted           ()                      const { return m_bFaceted;                         }
	bool     GetPostWire          ()                      const { return m_bPostWire;                        }
	float    GetWireLineThickness ()                      const { return m_fWireLineThickness;               }
	int      GetBlendSrcFactor    ()                      const { return m_nBlendSrcFactor;                  }
	int      GetBlendDstFactor    ()                      const { return m_nBlendDstFactor;                  }

	void     SetAmbient           (const  CVector4& v4Ambient)  { m_v4Ambient          = v4Ambient;          }
	void     SetDiffuse           (const  CVector4& v4Diffuse)  { m_v4Diffuse          = v4Diffuse;          }
	void     SetSpecular          (const  CVector4& v4Specular) { m_v4Specular         = v4Specular;         }
	void     SetShininess         (float  fShininess)           { m_fShininess         = fShininess;         }
	void     SetTwoSided          (bool   bTwoSided)            { m_bTwoSided          = bTwoSided;          }
	void     SetFaceted           (bool   bFaceted)             { m_bFaceted           = bFaceted;           }
	void     SetPostWire          (bool   bPostWire)            { m_bPostWire          = bPostWire;          }
	void     SetWireLineThickness (float  fWireLineThickness)   { m_fWireLineThickness = fWireLineThickness; }
	void     SetBlendSrcFactor    (int    nBlendSrcFactor)      { m_nBlendSrcFactor    = nBlendSrcFactor;    }
	void     SetBlendDstFactor    (int    nBlendDstFactor)      { m_nBlendDstFactor    = nBlendDstFactor;    }

	bool SetDefaults();

	bool HasLayer  (ELayerType eLayerType) const;
	bool GetLayer  (ELayerType eLayerType, SLayerInfo* pLayerInfo) const;
	bool SetLayer  (ELayerType eLayerType, const SLayerInfo& layerInfo);
	bool EraseLayer(ELayerType eLayerType);

	UtilGL::Texturing::CTexture* GetTexture(ELayerType eLayerType);
	bool SetTexture(ELayerType eLayerType, UtilGL::Texturing::CTexture* pTexture, bool bDeletePrevious);


protected:


	bool ParseShader(CInput* pInput);
	bool ParseLayer(ELayerType eLayerType, const std::string& strLayer);


private:


	bool SetupTexturing(int nPass, ELayerType eLayerType, unsigned uTextureUnit);
	bool EnableCombinersDot3();
	bool EnableTexEnvDot3(int nPass, bool bDiffuse);


	CVector4 m_v4Ambient;
	CVector4 m_v4Diffuse;
	CVector4 m_v4Specular;
	float    m_fShininess; 
	bool     m_bTwoSided;
	bool     m_bFaceted;
	bool     m_bPostWire;
	float    m_fWireLineThickness;
	int      m_nBlendSrcFactor;
	int      m_nBlendDstFactor;

	bool                         m_pbLayerPresent[3];
	SLayerInfo                   m_pLayerInfo[3];
	UtilGL::Texturing::CTexture* m_ppTextures[3];
	CMatrix                      m_pTexMatrices[3];
	bool                         m_pbTexMtxIsIdentity[3];

	// Aux vars:

	int      m_nLightState;
	CVector3 m_v3Light0Pos, m_v3EyePos;
	ETexEnv  m_peTexEnvs[3];

};


#endif