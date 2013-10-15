// OpenGL texture managing

// ==[ File ]===================================================================================
//
//  -Name     : Texturing.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Texturing namespace implementation.
//
// =============================================================================================

/*

	13/01/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

extern SContextData* _pContextData;

namespace Texturing
{


// ==[ Functions  ]=============================================================================


// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : void Reset()
//
//  - Purpose   : Resets the render context texturing data.
//
// -----------------------------------------------------------------------------
void Reset()
{
	_pContextData->_mapTexHandle.clear();
	_pContextData->_mapTexRefCount.clear();
	_pContextData->_nTexActiveUnit = 0;
	_pContextData->_nTexUsedMem    = 0;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : int GetNumUnits()
//
//  - Purpose   : Gets the number of texture units of current installed videocard.
//
// -----------------------------------------------------------------------------
int GetNumUnits()
{
	assert(UtilGL::IsReady());

	int nNumUnits;

	if(UtilGL::Extensions::GetAvailableExtensions()->ARB_multitexture)
	{
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &nNumUnits);
		return nNumUnits;
	}

	return 1;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : int GetCurrentUnit()
//
//  - Purpose   : Gets the current active texture unit.
//
// -----------------------------------------------------------------------------
int GetCurrentUnit()
{
	assert(UtilGL::IsReady());

	return _pContextData->_nTexActiveUnit;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool SetCurrentUnit()
//
//  - Purpose   : Sets the current active texture unit.
//
// -----------------------------------------------------------------------------
bool SetCurrentUnit(int nUnit)
{
	assert(UtilGL::IsReady());

	if(nUnit < 0 || nUnit >= GetNumUnits())
	{
		return false;
	}

	if(!UtilGL::Extensions::GetAvailableExtensions()->ARB_multitexture)
	{
		return false; // No multitexturing
	}

	glActiveTextureARB(GL_TEXTURE0_ARB + nUnit);
	_pContextData->_nTexActiveUnit = nUnit;
	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : int GetMaxTextureWidth()
//
//  - Purpose   : Gets the maximum width of the texture the videocard is able to load.
//
// -----------------------------------------------------------------------------
int GetMaxTextureWidth()
{
	assert(UtilGL::IsReady());

	int nMaxWidth;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &nMaxWidth);
	return nMaxWidth;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : int GetMaxTextureHeight()
//
//  - Purpose   : Gets the maximum height of the texture the videocard is able to load.
//
// -----------------------------------------------------------------------------
int GetMaxTextureHeight()
{
	assert(UtilGL::IsReady());

	int nMaxHeight;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &nMaxHeight);
	return nMaxHeight;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool GetNewHandle(const std::string& strTexName, int* pnHandleOut)
//
//  - Purpose   : Creates a new opengl texture handle.
//                If the name already exists in our list of loaded textures, we
//                only increment an internal counter and return false.
//                Otherwise, if the texture is not loaded, we really get a new
//                opengl handle and add the name+handle to our list, the function
//                will return true.
//                To properly save our list, the name should identify uniquely
//                the texture (f.e. using the filename).
//                That would prevent loading the same texture over and over again.
//                A returned handle value of zero means error.
// -----------------------------------------------------------------------------
bool GetNewHandle(const std::string& strTexName, int* pnHandleOut)
{
	assert(UtilGL::IsReady());

	std::map<std::string, int>::iterator it;
	std::map<int, int>        ::iterator it2;

	// Check if already loaded:

	if((it = _pContextData->_mapTexHandle.find(strTexName)) == _pContextData->_mapTexHandle.end())
	{
		// Not loaded, create new handle:

		unsigned int nNewHandle;
		glGenTextures(1, &nNewHandle);

		if(nNewHandle == 0)
		{
			return true;
		}

		// We've got the handle, now we need to update our lists:

		_pContextData->_mapTexHandle.insert(std::map<std::string, int>::value_type(strTexName, nNewHandle));
		_pContextData->_mapTexRefCount.insert(std::map<int, int>::value_type(nNewHandle, 1));

		*pnHandleOut = nNewHandle;
		return true;
	}

	// Already loaded, return its handle and increment the reference:

	it2 = _pContextData->_mapTexRefCount.find(it->second);
	assert(it2 != _pContextData->_mapTexRefCount.end());
	it2->second++;

	*pnHandleOut = it->second;

	return false;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool DeleteHandle(int nGLTexHandle)
//
//  - Purpose   : Deletes an existing texture handle.
//                In our case, that will mean checking if the counter relative
//                to the handle is > 1. If it is, we decrement the counter. If
//                it is 1, we need to delete the texture from memory.
//
// -----------------------------------------------------------------------------
bool DeleteHandle(int nGLTexHandle)
{
	assert(UtilGL::IsReady());
	std::map<std::string, int>::iterator it1;
	std::map<int, int>::iterator         it2;

	// Check if valid:

	it2 = _pContextData->_mapTexRefCount.find(nGLTexHandle);

	if(it2 == _pContextData->_mapTexRefCount.end())
	{
		return false;
	}

	// Has it been loaded more than once? -> decrement reference count.

	if(it2->second > 1)
	{
		it2->second--;
	}
	else
	{
		// It has been loaded only once, delete texture from memory:

		LOG.Write("\nDeleting texture object %d", nGLTexHandle);

		_pContextData->_mapTexRefCount.erase(it2);

		for(it1 = _pContextData->_mapTexHandle.begin(); it1 != _pContextData->_mapTexHandle.end(); ++it1)
		{
			if(it1->second == nGLTexHandle)
			{
				_pContextData->_mapTexHandle.erase(it1);
				break;
			}
		}

		glDeleteTextures(1, reinterpret_cast<unsigned int *>(&nGLTexHandle));
		LOG.Write(" OK.");
	}

	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool UnloadAllTextures()
//
//  - Purpose   : Disables cube mapping only if extension is supported.
//
// -----------------------------------------------------------------------------
bool UnloadAllTextures()
{
	assert(UtilGL::IsReady());

	std::map<std::string, int>::iterator it;

	for(it = _pContextData->_mapTexHandle.begin(); it != _pContextData->_mapTexHandle.end(); ++it)
	{
		LOG.Write("\nDeleting texture object %d", it->second);
		glDeleteTextures(1, reinterpret_cast<unsigned int *>(&(it->second)));
	}

	_pContextData->_mapTexHandle.clear();
	_pContextData->_mapTexRefCount.clear();

	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool SafeEnableCubeMapping()
//
//  - Purpose   : Enables cubemapping only if extension is available, preventing
//                the program from crashing.
//
// -----------------------------------------------------------------------------
bool SafeEnableCubeMapping()
{
	static bool bCheckedForCubeMap = false;
	static bool bCubeMapSupported  = false;

	if(!bCheckedForCubeMap)
	{
		if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_cube_map)
		{
			bCubeMapSupported  = true;
		}
		else
		{
			bCubeMapSupported  = false;
		}

		bCheckedForCubeMap = true;
	}

	if(bCubeMapSupported)
	{
		UtilGL::States::Set(UtilGL::States::TEXTURECUBEMAP, UtilGL::States::ENABLED);
		return true;
	}

	return false;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool SafeDisableCubeMapping()
//
//  - Purpose   : Disables cubemapping only if extension is available, preventing
//                the program from crashing.
//
// -----------------------------------------------------------------------------
bool SafeDisableCubeMapping()
{
	static bool bCheckedForCubeMap = false;
	static bool bCubeMapSupported  = false;

	if(!bCheckedForCubeMap)
	{
		if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_cube_map)
		{
			bCubeMapSupported  = true;
		}
		else
		{
			bCubeMapSupported  = false;
		}

		bCheckedForCubeMap = true;
	}

	if(bCubeMapSupported)
	{
		UtilGL::States::Set(UtilGL::States::GENR,			UtilGL::States::DISABLED);
		UtilGL::States::Set(UtilGL::States::TEXTURECUBEMAP, UtilGL::States::DISABLED);
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : int GetUsedMem()
//
//  - Purpose   : Returns the number of bytes of memory used for textures.
//
// -----------------------------------------------------------------------------
int CTexture::GetUsedMem()
{
	return _pContextData->_nTexUsedMem;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : CTexture2D()
//
//  - Purpose   : Default CTexture2D constructor.
//
// -----------------------------------------------------------------------------
CTexture2D::CTexture2D()
{
	m_nGLHandle = 0;
	m_nWidth    = 0;
	m_nHeight   = 0;
	m_bLoadFlat = false;
	m_bAlpha    = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : ~CTexture2D()
//
//  - Purpose   : CTexture2D destructor.
//
// -----------------------------------------------------------------------------
CTexture2D::~CTexture2D()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool Load(CInput* pInput, const std::string& strTexName,
//                          const std::string& strExt, bool bTile,
//                          STexLoadOptions* pOptions)
//
//  - Purpose   : Loads a texture into OpenGL memory.
//                pInput is the data source (can be file/memory/whatever).
//                See UtilGL::Texturing::GetNewHandle() for strTexName info.
//                strExt is the bitmap extension (f.e. "bmp" or "jpg")
//                bTile tells if the texture will be tiled or not.
//                See STexLoadOptions for the rest of parameters.
//                If pOptions is NULL, default options will be used.
//                
// -----------------------------------------------------------------------------
bool CTexture2D::Load(CInput* pInput, const std::string& strTexName, const std::string& strExt, bool bTile, STexLoadOptions* pOptions)
{
	assert(UtilGL::IsReady());
	assert(pInput);
	assert(pInput->Ready());
	assert(strTexName.data());

	if(m_nGLHandle)
	{
		this->Free();
	}

	STexLoadOptions defaultTexOptions;

	if(pOptions == NULL)
	{
		pOptions = &defaultTexOptions;
	}

	assert(pOptions->uLoadBpp == 8 || pOptions->uLoadBpp == 16 || pOptions->uLoadBpp == 32);

	LOG.Write("\nLoading texture. Name = %s:", strTexName.data());

	// Get OpenGL texture handle:

	int  nGLHandle;

	if(!GetNewHandle(strTexName, &nGLHandle))
	{
		LOG.Write("\nSkipping, already loaded.");
		m_nGLHandle = nGLHandle;
		return true;
	}

	LOG.Write("\n  OpenGL texture handle %u.", nGLHandle);

	if(nGLHandle == 0)
	{
		LOG.Write("ERROR - CTexture2D::Load():\nUnable to generate new opengl texture handle.");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, nGLHandle);

	// Load bitmap

	CBitmap bitmap;

	LOG.Write("\n  Loading bitmap:");

	if(!bitmap.Load(pInput, strExt))
	{
		LOG.Write("ERROR - CTexture2D::Load():\nError loading texture %s", strTexName.data());
		DeleteHandle(nGLHandle);
		return false;
	}

	LOG.Write(" OK, %ux%ux%ubpp, pitch=%u", bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetBpp(), bitmap.GetPitch());

	GLenum nGLInternalFormat, nGLPixelFormat, nGLPixelType;

	// Src format:

	m_bAlpha = false;

	switch(bitmap.GetBpp())
	{
	case 8:

		nGLPixelFormat = GL_LUMINANCE;
		nGLPixelType   = GL_UNSIGNED_BYTE;
		break;

	case 16:

		nGLPixelFormat = GL_BGR_EXT;
		nGLPixelType   = GL_UNSIGNED_SHORT_5_6_5;
		break;

	case 24:

		nGLPixelFormat = GL_BGR_EXT;
		nGLPixelType   = GL_UNSIGNED_BYTE;
		break;

	case 32:

		nGLPixelFormat    = GL_BGRA_EXT;
		nGLPixelType      = GL_UNSIGNED_BYTE;
		m_bAlpha          = true;
		break;

	default:

		assert(!"CTexture2D::Load():\nInvalid bitmap bpp while loading texture bitmap");
	}

	// Dst format:

	bool bCompressed = false;

	switch(pOptions->uLoadBpp)
	{

	case 8:

		if(!m_bAlpha) nGLInternalFormat = GL_LUMINANCE8;
		else          nGLInternalFormat = GL_ALPHA8;

		if(pOptions->bCompress && UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
		{
			if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_LUMINANCE_ARB;
			else          nGLInternalFormat = GL_COMPRESSED_ALPHA_ARB;

			bCompressed       = true;
		}

		break;

	case 16:

		if(!m_bAlpha) nGLInternalFormat = GL_RGB5;
		else          nGLInternalFormat = GL_RGBA4;

		if(pOptions->bCompress)
		{
			if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_compression_s3tc)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

				bCompressed       = true;
			}
			else if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_ARB;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_ARB;

				bCompressed       = true;
			}
		}

		break;

	case 32:

		if(!m_bAlpha) nGLInternalFormat = GL_RGB8;
		else          nGLInternalFormat = GL_RGBA8;

		if(pOptions->bCompress)
		{
			if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_compression_s3tc)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

				bCompressed       = true;
			}
			else if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_ARB;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_ARB;

				bCompressed       = true;
			}
		}

		break;
	}

	// Reescale if neccesary:

	int nMaxWidth  = GetMaxTextureWidth();
	int nMaxHeight = GetMaxTextureHeight();
	int nWidth     = bitmap.GetWidth();
	int nHeight    = bitmap.GetHeight();

	void* pPixels     = NULL;
	int*  pRescPixels = NULL;

	if(bitmap.GetWidth() > nMaxWidth || bitmap.GetHeight() > nMaxHeight)
	{
		if(nWidth  > nMaxWidth)  nWidth  = nMaxWidth;
		if(nHeight > nMaxHeight) nHeight = nMaxHeight;

		LOG.Write("\n  Resizing to %ux%u due to device limits.", nWidth, nHeight);

		pRescPixels = new int[nWidth * nHeight];

		if(pRescPixels == NULL)
		{
			LOG.Write("ERROR - CTexture2D::Load():\nOut of system memory while loading texture %s", strTexName.data());
			bitmap.Free();
			DeleteHandle(nGLHandle);
			return false;
		}

		gluScaleImage(nGLPixelFormat, bitmap.GetWidth(), bitmap.GetHeight(), nGLPixelType, bitmap.GetPixels(), nWidth, nHeight, nGLPixelType, pRescPixels);
		pPixels = pRescPixels;
	}
	else
	{
		pPixels = bitmap.GetPixels();
	}

	bool bUseMipmaps = false;
	int  nLevels     = 1;

	if(!(pOptions->eFilter == FILTER_POINT || pOptions->eFilter == FILTER_LINEAR))
	{
		// math.h doesn't have a function to compute the logarithm in base 2, so
		// we'll use the formula log2(x) = log10(x) / log10(2)
		bUseMipmaps  = true;
		int nMaxSide = nWidth > nHeight ? nWidth : nHeight;
		nLevels      = (log10f(nMaxSide) / log10f(2.0f)) + 1;
	}

	// Set parameters:

	LOG.Write("\n  Setting up parameters.");

	if(bTile)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else if(UtilGL::Extensions::GetAvailableExtensions()->OpenGL12)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Upload and set filter

	LOG.Write("\n  Uploading and setting up.");

	if(bUseMipmaps)
	{
		// Build mipmaps and upload them all. Nice glu func, it even rescales if texture size
		// is invalid (with a side not power of two).

		LOG.Write(" Computing %u mipmap levels.", nLevels);

		gluBuild2DMipmaps(GL_TEXTURE_2D,
			              nGLInternalFormat,
				          nWidth,
					      nHeight,
						  nGLPixelFormat,
	                      nGLPixelType,
		                  pPixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		switch(pOptions->eFilter)
		{
			case FILTER_BILINEAR:    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
			case FILTER_TRILINEAR:   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  break;
		}
	}
	else
	{
		LOG.Write(" No mipmaps.", nLevels);

		glTexImage2D(GL_TEXTURE_2D, 0, nGLInternalFormat, nWidth, nHeight, 0,
					 nGLPixelFormat, nGLPixelType, pPixels);

		switch(pOptions->eFilter)
		{
		case FILTER_POINT:

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		case FILTER_LINEAR:
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	// Set max anisotropy value if anisotropic filtering was requested.

	if(pOptions->bAnisotropic && UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
	{
		float fMaxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fMaxAnisotropy);
	}

	// Finalize

	bitmap.Free();

	SAFE_DELETE_A(pRescPixels);

	m_nGLHandle = nGLHandle;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &m_nWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_nHeight);
	m_bLoadFlat = false;

	// Compute video memory used

	unsigned uRawSize = m_nWidth * m_nHeight * (pOptions->uLoadBpp / 8);

	if(bUseMipmaps)
	{
		uRawSize = uRawSize + uRawSize / 3;
	}

	if(bCompressed)
	{
		unsigned uTotalCompressedSize = 0; // Sum of all levels

		for(int i = 0; i < nLevels; i++)
		{
			GLint compressed, compressedsize;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_ARB, &compressed);

			if(compressed == GL_TRUE)
			{
				glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &compressedsize);
				uTotalCompressedSize += compressedsize;
			}
			else
			{
				LOG.Write("\n  Failed to compress level %u.", i);
				int nLevelWidth, nLevelHeight;
				glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH,  &nLevelWidth);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_HEIGHT, &nLevelHeight);
				uTotalCompressedSize += nLevelWidth * nLevelHeight * (pOptions->uLoadBpp / 8);
			}
		}

		_pContextData->_nTexUsedMem += uTotalCompressedSize;
		LOG.Write("\n  Using texture compression (downsized to %u%%)", (uTotalCompressedSize * 100) / uRawSize);
	}
	else
	{
		LOG.Write("\n  No compression.");
		_pContextData->_nTexUsedMem += uRawSize;
	}

	LOG.Write("\nTexture loaded succesfully.");

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool LoadFlat(int nWidth, int nHeight,
//                              const CVector4& color4, bool bTile, bool bUseAlpha,
//                              STexLoadOptions* pOptions)
//
//  - Purpose   : Loads a flat texture into memory.
//                This is usually used to create the dummy to render later to.
//                
// -----------------------------------------------------------------------------
bool CTexture2D::LoadFlat(int   nWidth, int nHeight,
						  const CVector4& color4, bool  bTile, bool bUseAlpha,
						  STexLoadOptions* pOptions)
{
	assert(UtilGL::IsReady());
	assert(nWidth > 0);
	assert(nHeight > 0);

	STexLoadOptions defaultTexOptions;

	if(pOptions == NULL)
	{
		pOptions = &defaultTexOptions;
	}

	assert(pOptions->uLoadBpp == 16 || pOptions->uLoadBpp == 32);

	LOG.Write("\nCreating empty %dx%dx%dbpp texture:", nWidth, nHeight, pOptions->uLoadBpp);

	// Check size first.

	int nMaxWidth  = GetMaxTextureWidth();
	int nMaxHeight = GetMaxTextureHeight();

	if(nWidth > nMaxWidth)
	{
		LOG.Write("\n  Card doesn't support %u width textures. Setting width to %u.", nWidth, nMaxWidth);
		nWidth = nMaxWidth;
	}

	if(nHeight > nMaxHeight)
	{
		LOG.Write("\n  Card doesn't support %u height textures. Setting height to %u.", nHeight, nMaxHeight);
		nHeight = nMaxHeight;
	}

	// Create temp buffer:

	int nSize = nWidth * nHeight * (pOptions->uLoadBpp / 8);
	unsigned int* puPixels = new unsigned int[nSize / sizeof(int)];

	if(puPixels == NULL)
	{
		LOG.Write("ERROR - CTexture2D::LoadFlat():\nOut of system memory while loading flat texture");
		return NULL;
	}

	int nRed   = (int)(color4.X() * 255);
	int nGreen = (int)(color4.Y() * 255);
	int nBlue  = (int)(color4.Z() * 255);
	int nAlpha = (int)(color4.W() * 255);

	GLenum nGLInternalFormat;

	bool bCompressed = false;

	switch(pOptions->uLoadBpp)
	{

	case 8:

		if(pOptions->bCompress && UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
		{
			if(!bUseAlpha) nGLInternalFormat = GL_COMPRESSED_LUMINANCE_ARB;
			else           nGLInternalFormat = GL_COMPRESSED_ALPHA_ARB;

			bCompressed = true;
		}
		else
		{
			if(!bUseAlpha) nGLInternalFormat = GL_LUMINANCE8;
			else           nGLInternalFormat = GL_ALPHA8;
		}

		break;

	case 16:

		if(!bUseAlpha) nGLInternalFormat = GL_RGB5;
		else           nGLInternalFormat = GL_RGBA4;

		if(pOptions->bCompress)
		{
			if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_compression_s3tc)
			{
				if(!bUseAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				else           nGLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

				bCompressed       = true;
			}
			else if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
			{
				if(!bUseAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_ARB;
				else           nGLInternalFormat = GL_COMPRESSED_RGBA_ARB;

				bCompressed       = true;
			}
		}

		break;

	case 32:

		if(!bUseAlpha) nGLInternalFormat = GL_RGB8;
		else           nGLInternalFormat = GL_RGBA8;

		if(pOptions->bCompress)
		{
			if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_compression_s3tc)
			{
				if(!bUseAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				else           nGLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

				bCompressed       = true;
			}
			else if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
			{
				if(!bUseAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_ARB;
				else           nGLInternalFormat = GL_COMPRESSED_RGBA_ARB;

				bCompressed       = true;
			}
		}
	}

	// pack pixels in 8888 RGBA format.

	unsigned int uColor = (nRed << 24) | (nGreen << 16) | (nBlue << 8) | nAlpha;

	for(int nCount = 0; nCount < nWidth * nHeight; nCount++)
	{
		puPixels[nCount] = uColor;
	}

	// Generate new opengl texture handle.

	int nGLHandle;
	glGenTextures(1, (unsigned int *)&nGLHandle);

	if(nGLHandle == 0)
	{
		LOG.Write("ERROR - CTexture2D::LoadFlat():\nUnable to generate new opengl texture handle.");
                return false;
	}

	glBindTexture(GL_TEXTURE_2D, nGLHandle);

	// Params

	LOG.Write("\n Setting up parameters.");

	switch(pOptions->eFilter)
	{
	case FILTER_POINT:

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_POINT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_POINT);

	case FILTER_LINEAR:
	case FILTER_BILINEAR:
	case FILTER_TRILINEAR:

		// No mipmapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if(pOptions->bAnisotropic && UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
	{
		float fMaxAnisotropy;
		glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, fMaxAnisotropy);
	}

	// Upload

	LOG.Write("\n  Uploading.");

	glTexImage2D(GL_TEXTURE_2D, 0, nGLInternalFormat, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, puPixels);

	SAFE_DELETE_A(puPixels);

	m_nGLHandle  = nGLHandle;
	m_nWidth     = nWidth;
	m_nHeight    = nHeight;
	m_bLoadFlat  = true;

	SetTiling(bTile);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Compute size

	unsigned uRawSize = nWidth * nHeight * (pOptions->uLoadBpp / 8);

	if(bCompressed)
	{
		GLint compressed;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_ARB, &compressed);

		if(compressed == GL_TRUE)
		{
			GLint compressedsize;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &compressedsize);
			_pContextData->_nTexUsedMem += compressedsize;
		}
		else
		{
			_pContextData->_nTexUsedMem += uRawSize;
		}
	}
	else
	{
		_pContextData->_nTexUsedMem += uRawSize;
	}

	_pContextData->_mapTexRefCount.insert(std::map<int, int>::value_type(nGLHandle, 1));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool CopyFromBuffer(int    nTexOffsetX,    int   nTexOffsetY,
//                                    int    nWidth,         int   nHeight,
//                                    GLenum nGLPixelFormat, GLenum nGLPixelType,
//									  void* pPixels)
//
//  - Purpose   : Copies pixels from a buffer into the texture.
//                Coordinates are specified in pixels.
//
// -----------------------------------------------------------------------------
bool CTexture2D::CopyFromBuffer(int    nTexOffsetX,    int   nTexOffsetY,
								int    nWidth,         int   nHeight,
								GLenum nGLPixelFormat, GLenum nGLPixelType,
								void*  pPixels)
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	SetActive();
	glTexSubImage2D(GL_TEXTURE_2D, 0, nTexOffsetX, nTexOffsetY, nWidth, nHeight, nGLPixelFormat, nGLPixelType, pPixels);

	if(nGLPixelType == GL_RGBA)
	{
		m_bAlpha = true;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool CopyFromFramebuffer(int nFBOffsetX,  int nFBOffsetY,
//                                         int nWidth,      int nHeight)
//
//  - Purpose   : Copies pixels from the framebuffer into the texture.
//                Coordinates are specified in pixels and are relative to the
//                window upper left corner.
//
// -----------------------------------------------------------------------------
bool CTexture2D::CopyFromFramebuffer(	int nFBOffsetX,  int nFBOffsetY,
										int nTexOffsetX, int nTexOffsetY,
										int nWidth,      int nHeight)
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	HWND hWnd = CWindowGL::GetActiveWindow()->GetHWND();
	RECT rect;

	GetWindowRect(hWnd, &rect);

	SetActive();
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, nTexOffsetX, nTexOffsetY, nFBOffsetX, (rect.bottom - rect.top) - (nFBOffsetY + nHeight), nWidth, nHeight);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool SetActive()
//
//  - Purpose   : Sets current texture as the active for rendering.
//
// -----------------------------------------------------------------------------
bool CTexture2D::SetActive()
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	SafeDisableCubeMapping();

	glBindTexture(GL_TEXTURE_2D, m_nGLHandle);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool SetTiling(bool bTrue)
//
//  - Purpose   : Activates tiling (true) or edge clamping (false) for the
//                texture.
//
// -----------------------------------------------------------------------------
bool CTexture2D::SetTiling(bool bTrue)
{
	SetActive();

	if(bTrue)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else if(UtilGL::Extensions::GetAvailableExtensions()->OpenGL12)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool Free()
//
//  - Purpose   : See DeleteHandle()
//
// -----------------------------------------------------------------------------
bool CTexture2D::Free()
{
	if(m_nGLHandle == 0)
	{
		return true;
	}

	assert(UtilGL::IsReady());

	bool bResult = DeleteHandle(m_nGLHandle);

	m_nGLHandle = 0;
	m_nWidth    = 0;
	m_nHeight   = 0;
	m_bLoadFlat = false;

	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool HasAlpha()
//
//  - Purpose   : Return true if texture has alpha channel.
//
// -----------------------------------------------------------------------------
bool CTexture2D::HasAlpha()
{
	assert(UtilGL::IsReady());

	return m_bAlpha;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool SetPriority(float fPriority)
//
//  - Purpose   : Sets a texture's priority. Values range from 0.0 (minimum
//                priority) to 1.0 (maximum priority).
//                When short of free videomemory, textures with higher priority
//                will stay in videomemory while low priority textures will
//                have less chances to stay and would be stored in AGP/sys memory.
//
// -----------------------------------------------------------------------------
bool CTexture2D::SetPriority(float fPriority)
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	unsigned int nHandle = m_nGLHandle;

	glPrioritizeTextures(1, &nHandle, &fPriority);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool IncReference()
//
//  - Purpose   : If a texture pointer is assigned to another, you should call
//                this method right after
//
// -----------------------------------------------------------------------------
bool CTexture2D::IncReference()
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	std::map<int, int>::iterator it;

	it = _pContextData->_mapTexRefCount.find(m_nGLHandle);

	if(it == _pContextData->_mapTexRefCount.end())
	{
		return false;
	}

	it->second++;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2D
//  - prototype : bool DecReference()
//
//  - Purpose   : If a texture pointer that has called IncReference() is going
//                to be deleted, it should call this method.
//
// -----------------------------------------------------------------------------
bool CTexture2D::DecReference()
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	return DeleteHandle(m_nGLHandle);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : CTexture2DAnim()
//
//  - Purpose   : CTexture2DAnim's default constructor
//
// -----------------------------------------------------------------------------
CTexture2DAnim::CTexture2DAnim()
{
	m_pActiveTexture = NULL;
	m_bLoop = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : ~CTexture2DAnim()
//
//  - Purpose   : CTexture2DAnim's destructor
//
// -----------------------------------------------------------------------------
CTexture2DAnim::~CTexture2DAnim()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool CTexture2DAnim::AddFrame(CInput* pInput, const std::string& strTexName,
//                                              const std::string& strExt,
//												float fSeconds, bool bTile,
//												STexLoadOptions* pOptions)
//
//  - Purpose   : Adds a new frame to the animation. Frames must be added
//                in the same secuential order they will be displayed.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::AddFrame(CInput* pInput, const std::string& strTexName,
							  const std::string& strExt,
							  float fSeconds, bool bTile,
							  STexLoadOptions* pOptions)
{
	assert(UtilGL::IsReady());
	assert(pInput);
	assert(pInput->Ready());
	assert(strTexName.data());

	STexture2DFrame* pTextureFrame = new STexture2DFrame;

	pTextureFrame->fDuration = fSeconds;

	if(pTextureFrame->texture.Load(pInput, strTexName, strExt, bTile, pOptions) == false)
	{
		delete pTextureFrame;
		return false;
	}

	m_fAnimDuration += fSeconds;
	m_vecFrames.push_back(pTextureFrame);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : CTexture2D* GetCurFrame()
//
//  - Purpose   : Gets the current texture (frame) of the animation.
//                We use an internal timer to choose the frame we have to bind.
//
// -----------------------------------------------------------------------------
CTexture2D* CTexture2DAnim::GetCurFrame()
{
	return m_pActiveTexture;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool SetLoop(bool bLoop)
//
//  - Purpose   : Indicates whether to loop animation or not.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool SetEvalTime(float fTime)
//
//  - Purpose   : Evaluates the animation so that an internal pointer will
//                point to the textureframe of that time.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::SetEvalTime(float fTime)
{
	assert(UtilGL::IsReady());

	if(m_vecFrames.empty())
	{
		return false;
	}

	if(!m_bLoop && (fTime > m_fAnimDuration))
	{
		// No loop and time over duration, set last frame as active

		m_pActiveTexture = &m_vecFrames[m_vecFrames.size() - 1]->texture;
		return true;
	}
	else
	{
		ComputeLoop(0.0f, m_fAnimDuration, &fTime);
	}

	std::vector<STexture2DFrame*>::iterator it;
	float fTimeFrameStart = 0.0f;

	for(it = m_vecFrames.begin(); it < m_vecFrames.end(); ++it)
	{
		// Check for fTimeFrameStart <= fTime < (fTimeFrameStart + Frame.Duration)

		if(fTime >= fTimeFrameStart)
		{
			if(fTime < (fTimeFrameStart + (*it)->fDuration))
			{
				m_pActiveTexture = &((*it)->texture);
				return true;
			}
		}

		fTimeFrameStart += (*it)->fDuration;
	}

	LOG.Write("\nWARNING - CTexture2DAnim::SetEvalTime(): didn't find the texture");
	m_pActiveTexture = NULL;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool SetActive()
//
//  - Purpose   : Binds the current evaluated texture (CTexture2DAnim::SetEvalTime())
//                as the active when rendering.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::SetActive()
{
	assert(m_pActiveTexture);
	return m_pActiveTexture->SetActive();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool Free()
//
//  - Purpose   : Frees textures from memory if their reference count == 1.
//                See UtilGL::Texturing::DeleteHandle().
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::Free()
{
	assert(UtilGL::IsReady());

	std::vector<STexture2DFrame*>::iterator it;

	for(it = m_vecFrames.begin(); it < m_vecFrames.end(); ++it)
	{
		delete *it;
	}

	m_vecFrames.clear();
	m_bLoop          = false;
	m_fAnimDuration  = 0.0f;
	m_pActiveTexture = NULL;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool HasAlpha()
//
//  - Purpose   : Returns true if the current frame has an alpha channel.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::HasAlpha()
{
	assert(GetCurFrame());

	return GetCurFrame()->HasAlpha();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool SetPriority(float fPriority)
//
//  - Purpose   : Sets a given priority to all animation textures.
//                See CTexture2D::SetPriority() for more info.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::SetPriority(float fPriority)
{
	assert(UtilGL::IsReady());

	std::vector<STexture2DFrame*>::iterator it;

	for(it = m_vecFrames.begin(); it < m_vecFrames.end(); ++it)
	{
		(*it)->texture.SetPriority(fPriority);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool IncReference()
//
//  - Purpose   : If a texture pointer is assigned to another, you should call
//                this method right after
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::IncReference()
{
	assert(UtilGL::IsReady());
	std::vector<STexture2DFrame*>::iterator it;

	int nError = 0;

	for(it = m_vecFrames.begin(); it < m_vecFrames.end(); ++it)
	{
		if(!(*it)->texture.IncReference()) nError++;
	}

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTexture2DAnim
//  - prototype : bool DecReference()
//
//  - Purpose   : If a texture pointer that has called IncReference() is going
//                to be deleted, it should call this method.
//
// -----------------------------------------------------------------------------
bool CTexture2DAnim::DecReference()
{
	assert(UtilGL::IsReady());
	std::vector<STexture2DFrame*>::iterator it;

	int nError = 0;

	for(it = m_vecFrames.begin(); it < m_vecFrames.end(); ++it)
	{
		if(!(*it)->texture.DecReference()) nError++;
	}

	return nError == 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : CTextureCubemap()
//
//  - Purpose   : CTextureCubemap's default constructor.
//
// -----------------------------------------------------------------------------
CTextureCubemap::CTextureCubemap()
{
	m_nGLHandle = 0;
	m_bAlpha    = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : ~CTextureCubemap()
//
//  - Purpose   : CTextureCubemap's destructor.
//
// -----------------------------------------------------------------------------
CTextureCubemap::~CTextureCubemap()
{
	Free();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : bool LoadSide(CInput* pInput, const std::string& strTexName,
//                              const std::string& strExt, ECubeSide eSide, bool bTile,
//                              STexLoadOptions* pOptions)
//
//  - Purpose   : Loads a texture into a side of the cubemap
//                We won't use GetNewTextureHandle() since the way cubemapping
//                works (using 6 textures to map) doesn't fit with our
//                texture handle generation caché.
//
// -----------------------------------------------------------------------------
bool CTextureCubemap::LoadSide(CInput* pInput, const std::string& strTexName, const std::string& strExt,
							   ECubeSide eSide, bool bTile,
							   STexLoadOptions* pOptions)
{
	assert(UtilGL::IsReady());
	assert(pInput);
	assert(pInput->Ready());
	assert(strTexName.data());

	STexLoadOptions defaultTexOptions;

	if(pOptions == NULL)
	{
		pOptions = &defaultTexOptions;
	}

	assert(pOptions->uLoadBpp == 8 || pOptions->uLoadBpp == 16 || pOptions->uLoadBpp == 32);

	LOG.Write("\nLoading cubemap side texture. Texture = %s, Side = %u", strTexName.data(), eSide);

	CBitmap bitmap;

	LOG.Write("\n  Loading bitmap:");

	if(!bitmap.Load(pInput, strExt))
	{
		LOG.Write("ERROR - CTextureCubemap::LoadSide():\nError loading texture %s", strTexName.data());
		return false;
	}

	LOG.Write(" OK, %ux%ux%ubpp, pitch=%u", bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetBpp(), bitmap.GetPitch());

	GLenum nGLInternalFormat, nGLPixelFormat, nGLPixelType;

	// Src format:

	switch(bitmap.GetBpp())
	{
	case 8:

		nGLPixelFormat = GL_LUMINANCE;
		nGLPixelType   = GL_UNSIGNED_BYTE;
		break;

	case 16:

		nGLPixelFormat = GL_BGR;
		nGLPixelType   = GL_UNSIGNED_SHORT_5_6_5;
		break;

	case 24:

		nGLPixelFormat = GL_BGR;
		nGLPixelType   = GL_UNSIGNED_BYTE;
		break;

	case 32:

		nGLPixelFormat = GL_BGRA;
		nGLPixelType   = GL_UNSIGNED_BYTE;
		m_bAlpha       = true;
		break;

	default:

		assert(!"CTextureCubemap::LoadSide():\nInvalid bitmap bpp while loading texture bitmap");
	}

	// Dst format:

	bool bCompressed = false;

	switch(pOptions->uLoadBpp)
	{

	case 8:

		if(!m_bAlpha) nGLInternalFormat = GL_LUMINANCE8;
		else          nGLInternalFormat = GL_ALPHA8;

		if(pOptions->bCompress && UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
		{
			if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_LUMINANCE_ARB;
			else          nGLInternalFormat = GL_COMPRESSED_ALPHA_ARB;

			bCompressed       = true;
		}

		break;

	case 16:

		if(!m_bAlpha) nGLInternalFormat = GL_RGB5;
		else          nGLInternalFormat = GL_RGBA4;

		if(pOptions->bCompress)
		{
			if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_compression_s3tc)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

				bCompressed       = true;
			}
			else if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_ARB;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_ARB;

				bCompressed       = true;
			}
		}

		break;

	case 32:

		if(!m_bAlpha) nGLInternalFormat = GL_RGB8;
		else          nGLInternalFormat = GL_RGBA8;

		if(pOptions->bCompress)
		{
			if(UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_compression_s3tc)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

				bCompressed       = true;
			}
			else if(UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_compression)
			{
				if(!m_bAlpha) nGLInternalFormat = GL_COMPRESSED_RGB_ARB;
				else          nGLInternalFormat = GL_COMPRESSED_RGBA_ARB;

				bCompressed       = true;
			}
		}

		break;
	}

	// Reescale if neccesary:

	int nMaxWidth;
	int nMaxHeight;

	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &nMaxWidth);
	nMaxHeight = nMaxWidth;

	int nWidth  = bitmap.GetWidth();
	int nHeight = bitmap.GetHeight();

	void* pPixels     = NULL;
	int*  pRescPixels = NULL;

	if(bitmap.GetWidth() > nMaxWidth || bitmap.GetHeight() > nMaxHeight)
	{
		if(nWidth  > nMaxWidth)  nWidth  = nMaxWidth;
		if(nHeight > nMaxHeight) nHeight = nMaxHeight;

		LOG.Write("\n  Resizing to %ux%u due to device limits.", nWidth, nHeight);

		pRescPixels = new int[nWidth * nHeight];

		if(pRescPixels == NULL)
		{
			LOG.Write("ERROR - CTextureCubemap::LoadSide():\nOut of system memory while loading texture %s", strTexName.data());
			bitmap.Free();
			return false;
		}

		gluScaleImage(nGLPixelFormat, bitmap.GetWidth(), bitmap.GetHeight(), nGLPixelType, bitmap.GetPixels(), nWidth, nHeight, nGLPixelType, pRescPixels);
		pPixels = pRescPixels;
	}
	else
	{
		pPixels = bitmap.GetPixels();
	}

	// Get OpenGL texture handle:

	bool bUseMipmaps = false;
	int  nLevels     = 1;

	if(!m_nGLHandle) // First cube side load.
	{
		glGenTextures(1, reinterpret_cast<unsigned int*>(&m_nGLHandle));

		LOG.Write("\n  Got OpenGL texture handle %u.", m_nGLHandle);

		if(m_nGLHandle == 0)
		{
			LOG.Write("ERROR - CTextureCubemap::LoadSide():\nUnable to generate new opengl texture handle.");
			SAFE_DELETE_A(pRescPixels)
			bitmap.Free();
			return false;
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_nGLHandle);

	if(!(pOptions->eFilter == FILTER_POINT || pOptions->eFilter == FILTER_LINEAR))
	{
		// math.h doesn't have a function to compute the logarithm in base 2, so
		// we'll use the formula log2(x) = log10(x) / log10(2)
		bUseMipmaps  = true;
		int nMaxSide = nWidth > nHeight ? nWidth : nHeight;
		nLevels      = (log10f(nMaxSide) / log10f(2.0)) + 1;
	}

	// Set parameters:

	LOG.Write("\n  Setting up parameters.");

	if(bTile)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if(UtilGL::Extensions::GetAvailableExtensions()->OpenGL12)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP);
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Upload and set filter

	LOG.Write("\n  Uploading.");

	if(bUseMipmaps)
	{
		// Build mipmaps and upload them all. Nice glu func, it even rescales if texture size
		// is invalid (with a side not power of two).

		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + eSide,
			              nGLInternalFormat,
				          nWidth,
					      nHeight,
						  nGLPixelFormat,
	                      nGLPixelType,
		                  pPixels);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		switch(pOptions->eFilter)
		{
			case FILTER_BILINEAR:    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
			case FILTER_TRILINEAR:   glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  break;
		}
	}
	else
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + eSide, 0, nGLInternalFormat, nWidth, nHeight, 0,
					 nGLPixelFormat, nGLPixelType, pPixels);

		switch(pOptions->eFilter)
		{
		case FILTER_POINT:

			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		case FILTER_LINEAR:
			
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	// Set max anisotropy value if anisotropic filtering was requested.
/*
	if(pOptions->bAnisotropic && UtilGL::Extensions::GetAvailableExtensions()->EXT_texture_filter_anisotropic)
	{
		float fMaxAnisotropy;
		glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropy);
		glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, fMaxAnisotropy);
	}
*/
	// Finalize

	m_bAlpha = m_bAlpha || bitmap.GetBpp() == 32;
	bitmap.Free();
	SAFE_DELETE_A(pRescPixels);

	// Compute video memory used

	glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_ARB, 0, GL_TEXTURE_WIDTH,  &nWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_ARB, 0, GL_TEXTURE_HEIGHT, &nHeight);

	unsigned uRawSize = nWidth * nHeight * (pOptions->uLoadBpp / 8);

	if(bUseMipmaps)
	{
		uRawSize = uRawSize + uRawSize / 3;
	}

	if(bCompressed)
	{
		unsigned uTotalCompressedSize = 0; // Sum of all levels

		for(int i = 0; i < nLevels; i++)
		{
			GLint compressed, compressedsize;
			glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_ARB, i, GL_TEXTURE_COMPRESSED_ARB, &compressed);

			if(compressed == GL_TRUE)
			{
				glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_ARB, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &compressedsize);
				uTotalCompressedSize += compressedsize;
			}
			else
			{
				LOG.Write("\n  Failed to compress level %u.", i);
				int nLevelWidth, nLevelHeight;
				glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_ARB, i, GL_TEXTURE_WIDTH,  &nLevelWidth);
				glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_ARB, i, GL_TEXTURE_HEIGHT, &nLevelHeight);
				uTotalCompressedSize += nLevelWidth * nLevelHeight * (pOptions->uLoadBpp / 8);
			}
		}

		_pContextData->_nTexUsedMem += uTotalCompressedSize;
		LOG.Write("\n  Using texture compression (downsized to %u%%)", (uTotalCompressedSize * 100) / uRawSize);
	}
	else
	{
		LOG.Write("\n  No compression.");
		_pContextData->_nTexUsedMem += uRawSize;
	}

	_pContextData->_mapTexRefCount.insert(std::map<int, int>::value_type(m_nGLHandle, 1));

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : bool SetActive()
//
//  - Purpose   : Sets the current cubemap as active for texturing.
//                Note that it won't set automatic texture coords generation.
//
// -----------------------------------------------------------------------------
bool CTextureCubemap::SetActive()
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_nGLHandle);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : bool Free()
//
//  - Purpose   : Frees the current texture from memory.
//
// -----------------------------------------------------------------------------
bool CTextureCubemap::Free()
{
	if(m_nGLHandle == 0)
	{
		return true;
	}

	assert(UtilGL::IsReady());

	bool bResult = DeleteHandle(m_nGLHandle);

	m_nGLHandle = 0;
	m_bAlpha    = false;

	return bResult;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : bool SetPriority(float fPriority)
//
//  - Purpose   : Sets the texture priority. See CTexture2D::SetPriority() for
//                more details.
//
// -----------------------------------------------------------------------------
bool CTextureCubemap::SetPriority(float fPriority)
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	glPrioritizeTextures(1, reinterpret_cast<unsigned int *>(&m_nGLHandle), &fPriority);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : bool IncReference()
//
//  - Purpose   : If a texture pointer is assigned to another, you should call
//                this method right after
//
// -----------------------------------------------------------------------------
bool CTextureCubemap::IncReference()
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	std::map<int, int>::iterator it;

	it = _pContextData->_mapTexRefCount.find(m_nGLHandle);

	if(it == _pContextData->_mapTexRefCount.end())
	{
		return false;
	}

	it->second++;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTextureCubemap
//  - prototype : bool DecReference()
//
//  - Purpose   : If a texture pointer that has called IncReference() is going
//                to be deleted, it should call this method.
//
// -----------------------------------------------------------------------------
bool CTextureCubemap::DecReference()
{
	assert(UtilGL::IsReady());
	assert(m_nGLHandle > 0);

	return DeleteHandle(m_nGLHandle);
}

} // namespace Texturing

} // namespace UtilGL