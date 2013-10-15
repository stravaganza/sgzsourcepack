// ==[ File ]===================================================================================
//
//  -Name     : Texturing.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Texturing namespace. OpenGL texture managing.
//
// =============================================================================================

/*

	28/01/2002  -  File created.


*/


#ifndef __TEXTURING_H
#define __TEXTURING_H


// ==[ Functions  ]=============================================================================


namespace UtilGL
{

namespace Texturing
{


enum ETextureFilter
{
	FILTER_POINT,
	FILTER_LINEAR,
	FILTER_BILINEAR,
	FILTER_TRILINEAR
};


void Reset();

int  GetNumUnits();
int  GetCurrentUnit();
bool SetCurrentUnit(int nUnit);

int  GetMaxTextureWidth();
int  GetMaxTextureHeight();

int  GetNewHandle(const std::string& strTexName);
bool DeleteHandle(int glTexHandle);

bool UnloadAllTextures();

bool SafeEnableCubeMapping();
bool SafeDisableCubeMapping();


// --[ Structure ]-----------------------------------------------------------
//
//  - Name      : STexLoadOptions
//
//  - Purpose   : Structure to hold texture load options.
//
// ---------------------------------------------------------------------------
struct STexLoadOptions
{
	STexLoadOptions()  { SetDefaults(); }
	~STexLoadOptions() {                }

	void SetDefaults()
	{
		uLoadBpp     = 32;
		eFilter      = FILTER_TRILINEAR;
		bAnisotropic = true;
		bCompress    = false;
	}

	unsigned       uLoadBpp;     // Specifies the bits per pixel to load the texture to.
	ETextureFilter eFilter;      // Specifies the filtering that OpenGL will use for the texture.
	bool           bAnisotropic; // Specifies if the texture needs anisotropic filtering.
	bool           bCompress;    // Specifies if the texture should be compressed in video memory.
};

// --[ Class ]----------------------------------------------------------------
//
//  - Name      : CTexture
//
//  - Purpose   : Base class used to generate child texture classes, each one
//                implementing a different texture type.
//                Common methods are setting it as opengl's current texture,
//                freeing it and setting its priority (0.0f -> 1.0f).
//
// ---------------------------------------------------------------------------
class CTexture
{


public:


	enum ETextureType
	{
		TEXTURE1D = 0,
		TEXTURE2D,
		TEXTURE2DANIM,
		TEXTURE3D,
		TEXTURECUBEMAP
	};

	CTexture()  { }
	~CTexture() { }

	virtual ETextureType Type() const = 0;

	virtual bool SetEvalTime(float fTime) { return true; }
	virtual bool SetActive()                        = 0;
	virtual bool Free()                             = 0;
	virtual bool HasAlpha()                         = 0;
	virtual bool SetPriority(float fPriority)       = 0;
	virtual bool IncReference()                     = 0;
	virtual bool DecReference()                     = 0;

	static int  GetUsedMem();

};

// --[ Class ]----------------------------------------------------------------
//
//  - Name      : CTexture2D
//
//  - Purpose   : Simple 2D texture class.
//
// ---------------------------------------------------------------------------
class CTexture2D : public CTexture
{


public:


	CTexture2D();
	~CTexture2D();

	bool Load(	CInput* pInput, const std::string& strTexName, const std::string& strExt,
				bool bTile, STexLoadOptions* pOptions);

	bool LoadFlat(	int nWidth, int nHeight, const CVector4& color4, bool bTile, bool bUseAlpha,
					STexLoadOptions* pOptions);

	int  GetWidth()  const { return m_nWidth;  }
	int  GetHeight() const { return m_nHeight; }

	bool CopyFromBuffer(int nTexOffsetX, int nTexOffsetY, int nWidth, int nHeight, GLenum nGLPixelFormat, GLenum nGLPixelType, void *pPixels);
	bool CopyFromFramebuffer(int nFBOffsetX,  int nFBOffsetY, int nTexOffsetX, int nTexOffsetY, int nWidth, int nHeight);

	// From CTexture:

	ETextureType Type() const { return TEXTURE2D; }

	bool SetActive();
	bool SetTiling(bool bTrue);
	bool Free();
	bool HasAlpha();
	bool SetPriority(float fPriority);
	bool IncReference();
	bool DecReference();


private:


	int  m_nGLHandle;
	int  m_nWidth, m_nHeight;
	bool m_bLoadFlat;
	bool m_bAlpha;

};

// --[ Class ]----------------------------------------------------------------
//
//  - Name      : CTexture2DAnim
//
//  - Purpose   : Multi-framed 2D texture class.
//
// ---------------------------------------------------------------------------
class CTexture2DAnim : public CTexture
{


public:


	CTexture2DAnim();
	~CTexture2DAnim();

	bool AddFrame(	CInput* pInput, const std::string& strTexName, const std::string& strExt,
					float fSeconds, bool bTile, STexLoadOptions* pOptions);
	bool SetLoop(bool bLoop);

	CTexture2D* GetCurFrame();

	// From CTexture:

	ETextureType Type() const { return TEXTURE2DANIM; }

	bool SetEvalTime(float fTime);
	bool SetActive();
	bool SetTiling(bool bTrue);
	bool Free();
	bool HasAlpha();
	bool SetPriority(float fPriority);
	bool IncReference();
	bool DecReference();


private:


	struct STexture2DFrame
	{
		CTexture2D texture;
		float      fDuration;
	};

	std::vector<STexture2DFrame*> m_vecFrames;

	bool  m_bLoop;
	float m_fAnimDuration;
	int   m_nRefCount;

	CTexture2D* m_pActiveTexture;
};

// --[ Class ]----------------------------------------------------------------
//
//  - Name      : CTextureCubemap
//
//  - Purpose   : Cubic mapping texture class.
//                The texture itself consists of 6 sub-textures, each one
//                for a cube's face.
//
// ---------------------------------------------------------------------------
class CTextureCubemap : public CTexture
{


public:


	enum ECubeSide
	{
		SIDE_POSX = 0,
		SIDE_NEGX,
		SIDE_POSY,
		SIDE_NEGY,
		SIDE_POSZ,
		SIDE_NEGZ
	};


	CTextureCubemap();
	~CTextureCubemap();

	bool LoadSide(	CInput* pInput, const std::string& strTexName, const std::string& strExt,
					ECubeSide nSide, bool bTile, STexLoadOptions* pOptions);

	// From CTexture:

	ETextureType Type()         const { return TEXTURECUBEMAP; }

	bool SetActive();
	bool SetTiling(bool bTrue);
	bool Free();
	bool HasAlpha()                   { return m_bAlpha;       }
	bool SetPriority(float fPriority);
	bool IncReference();
	bool DecReference();


private:


	bool m_bAlpha;
	int  m_nGLHandle;

};

} // namespace Texturing

} // namespace UtilGL


#endif

