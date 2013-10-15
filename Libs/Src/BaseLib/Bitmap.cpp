// ==[ File ]===================================================================================
//
//  -Name     : Bitmap.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CBitmap implementation (see Bitmap.h).
//
//  -Note     : FreeImage library is used to handle all bitmap manipulation.
//              Get more info at http://www.6ixsoft.com 
//
// =============================================================================================

/*

	28/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"

#ifdef __BORLANDC__
  #include "FreeImage.h"
#endif


// ==[ Functions  ]=============================================================================


// We need to provide some of these in order to load bitmaps from virtually anywhere (using
// our CInput class).

// fread

unsigned int MyReadProc(void *pBuffer, unsigned int uSize, unsigned int uCount, fi_handle handle)
{
	return ((CInput *)handle)->ReadRawData(uSize, uCount, pBuffer);
}

// fwrite

unsigned int MyWriteProc(void *pBuffer, unsigned int uSize, unsigned int uCount, fi_handle handle)
{
	return ((COutput *)handle)->WriteRawData(uSize, uCount, pBuffer);
}

// fseek

int MySeekProc(fi_handle handle, long lOffset, int nOrigin)
{
	switch(nOrigin)
	{

	case SEEK_SET:

		return ((CInput *)handle)->SetPosition(lOffset, CInput::SEEKFROM_START);

	case SEEK_CUR:

		return ((CInput *)handle)->SetPosition(lOffset, CInput::SEEKFROM_CURPOS);

	case SEEK_END:

		return ((CInput *)handle)->SetPosition(lOffset, CInput::SEEKFROM_END);

	default : return 1; // != 0 means error.
	}

	return 0; // 0 means success
}

// ftell

long int MyTellProc(fi_handle handle)
{
	return ((CInput *)handle)->GetPosition();
}


// ==[ Class implementations ]==================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBitmap
//  - Prototype : bool Load(CInput* pData, const std::string& strExt)
//
//  - Purpose   : Load's an image into memory.
//                strExt tells the image format (f.e "bmp" or "jpg")
//
// -----------------------------------------------------------------------------
bool CBitmap::Load(CInput* pData, const std::string& strExt)
{
	assert(pData);
	assert(pData->Ready());

	if(m_pDib)
	{
		Free();
		m_pDib = NULL;
	}

	FreeImageIO io;

	io.read_proc  = MyReadProc;
	io.write_proc = MyWriteProc;
	io.seek_proc  = MySeekProc;
	io.tell_proc  = MyTellProc;

	// Get image format:

	/*

	Doesn't work with some formats (missing headers or headers at the
	end of the file):
	FREE_IMAGE_FORMAT fileType = FI_GetFileTypeFromHandle(&io, pData, 256);

	*/

	if(!stricmp(strExt.data(), "bmp" )) m_pDib = FI_LoadBMPFromHandle  (&io, pData, BMP_DEFAULT);   else
	if(!stricmp(strExt.data(), "jpg" )) m_pDib = FI_LoadJPEGFromHandle (&io, pData, JPEG_DEFAULT);  else
	if(!stricmp(strExt.data(), "jpeg")) m_pDib = FI_LoadJPEGFromHandle (&io, pData, JPEG_DEFAULT);  else
	if(!stricmp(strExt.data(), "png" )) m_pDib = FI_LoadPNGFromHandle  (&io, pData, PNG_DEFAULT);   else
	if(!stricmp(strExt.data(), "tga" )) m_pDib = FI_LoadTARGAFromHandle(&io, pData, TARGA_DEFAULT); else
	if(!stricmp(strExt.data(), "tif" )) m_pDib = FI_LoadTIFFFromHandle (&io, pData, TIFF_DEFAULT);  else
	if(!stricmp(strExt.data(), "tiff")) m_pDib = FI_LoadTIFFFromHandle (&io, pData, TIFF_DEFAULT);  else
	{
#ifdef _DEBUG
		CLogger::NotifyWindow("\nWARNING - CBitmap::Load():\nUnknown extension (%s)", strExt.data());
#endif
	}

	return m_pDib ? true : false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBitmap
//  - Prototype : bool AddAlphaChannel(CInput* pData, const std::string& strExt)
//
//  - Purpose   : Adds an alpha channel to an existent bitmap.
//
// -----------------------------------------------------------------------------
bool CBitmap::AddAlphaChannel(CInput* pData, const std::string& strExt)
{
	CBitmap alpha(pData, strExt);

	m_pDib = FI_AddAlphaMask(m_pDib, alpha.m_pDib);

	return m_pDib ? true : false;
}


bool     CBitmap::Free()          {                 if(m_pDib) FI_Unload(m_pDib); m_pDib = NULL; return true;    }
void*    CBitmap::GetPixels()     { assert(m_pDib); return FI_GetBits(m_pDib);                                   }
RGBQUAD* CBitmap::GetPalette()    { assert(m_pDib); return FI_GetPalette(m_pDib);                                }

int CBitmap::GetBpp()       const { assert(m_pDib); return FI_GetBPP(m_pDib);                                    }
int CBitmap::GetWidth()     const { assert(m_pDib); return FI_GetWidth(m_pDib);                                  }
int CBitmap::GetHeight()    const { assert(m_pDib); return FI_GetHeight(m_pDib);                                 }
int CBitmap::GetPitch()     const { assert(m_pDib); return FI_GetPitch(m_pDib);                                  }

bool CBitmap::ToGreyscale()       { assert(m_pDib); FI_ConvertToGreyscale(m_pDib);              return !!m_pDib; }
bool CBitmap::To8Bits()           { assert(m_pDib); m_pDib = FI_ConvertTo8Bits(m_pDib);         return !!m_pDib; }
bool CBitmap::To16Bits555()       { assert(m_pDib); m_pDib = FI_ConvertTo16Bits(m_pDib, true);  return !!m_pDib; }
bool CBitmap::To16Bits565()       { assert(m_pDib); m_pDib = FI_ConvertTo16Bits(m_pDib, false); return !!m_pDib; }
bool CBitmap::To24Bits()          { assert(m_pDib); m_pDib = FI_ConvertTo24Bits(m_pDib);        return !!m_pDib; }
bool CBitmap::To32Bits()          { assert(m_pDib); m_pDib = FI_ConvertTo32Bits(m_pDib);        return !!m_pDib; }

bool CBitmap::Flip()              { assert(m_pDib); FI_Flip(m_pDib);   return !!m_pDib;                          }
bool CBitmap::Mirror()            { assert(m_pDib); FI_Mirror(m_pDib); return !!m_pDib;                          }
bool CBitmap::Invert()            { assert(m_pDib); FI_Invert(m_pDib); return !!m_pDib;                          }
