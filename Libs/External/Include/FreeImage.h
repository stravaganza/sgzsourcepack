// ==========================================================
// FreeImage
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Hervé Drolon (drolon@iut.univ-lehavre.fr)
// - Alexander Dymerets (sashad@te.net.ua)
// - Alex Kwak
//
// This file is part of FreeImage
//
// Use at own risk!
// ==========================================================

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifndef WINDOWS
#include <windows.h>
#endif

// Parts that are enabled in the DLL ----------------------------------------

#define BMP_ENABLED
#define ICO_ENABLED
#define KOALA_ENABLED
#define JPEG_ENABLED
#define PCD_ENABLED
#define PCX_ENABLED
#define PNG_ENABLED
#define PNM_ENABLED			// PBM, PGM & PPM
#define RAS_ENABLED
#define TARGA_ENABLED
#define TIFF_ENABLED
#define FILETYPE_ENABLED
#define EFFECTS_ENABLED
#define CONVERSION_ENABLED

// File IO routines ---------------------------------------------------------

#ifndef FREEIMAGE_IO
#define FREEIMAGE_IO

#define fi_handle void*

typedef unsigned (*FI_ReadProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef unsigned (*FI_WriteProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef int (*FI_SeekProc) (fi_handle handle, long offset, int origin);
typedef long (*FI_TellProc) (fi_handle handle);

#pragma pack (push, 1)

struct FreeImageIO {
	FI_ReadProc read_proc;
	FI_WriteProc write_proc;
	FI_SeekProc seek_proc;
	FI_TellProc tell_proc;
};

#pragma pack(pop)

enum FREE_IMAGE_COLOR_TYPE {
	FIC_MINISWHITE 	= 0,		// min value is white
	FIC_MINISBLACK 	= 1,		// min value is black
	FIC_RGB			= 2,		// RGB color model
	FIC_PALETTE		= 3,		// color map indexed
};

enum FREE_IMAGE_FORMAT {
	FIF_UNKNOWN = 0,
	FIF_BMP,
	FIF_ICO,
	FIF_JPEG,
	FIF_KOALA,
	FIF_PBM,
	FIF_PCD,
	FIF_PCX,
	FIF_PGM,
	FIF_PNG,
	FIF_PPM,
	FIF_RAS,
	FIF_TARGA,
	FIF_TIFF,
};

enum BMPFlags {
	BMP_DEFAULT = 0,
};

enum ICOFlags {
	ICO_DEFAULT = 0,
	ICO_FIRST = 0,
	ICO_SECOND = 1,
	ICO_THIRD = 2,
};

enum KOALAFlags {
	KOALA_DEFAULT = 0,
};

enum JPEGFlags {
	JPEG_DEFAULT = 0,
	JPEG_FAST = 1,
	JPEG_ACCURATE = 2,
};

enum PCDFlags {
	PCD_DEFAULT = 0,
	PCD_BASE = 1,
	PCD_BASEDIV4 = 2,
	PCD_BASEDIV16 = 3,
};

enum PCXFlags {
	PCX_DEFAULT = 0,
};

enum PNGFlags {
	PNG_DEFAULT = 0,
};

enum PNMFlags {				//! PBM, PGM, PPM
	PNM_DEFAULT		= 0,
	PNM_SAVE_RAW	= 0,	//! If set the writer save in RAW format (i.e. P4, P5 or P6)
	PNM_SAVE_ASCII	= 1		//! If set the writer save in ASCII format (i.e. P1, P2 or P3)

};

enum RASFlags {
	RAS_DEFAULT   = 0,
};

enum TARGAFlags {
	TARGA_DEFAULT			= 0,
	TARGA_LOAD_RGB888		= 1, // If set the loader convert ARGB8888 -> RGB888.
	TARGA_LOAD_RGB555		= 2, // If set the loader doesn't convert 16bit RGB555 -> 24bit RGB888.
};

enum TIFFFlags {
	TIFF_DEFAULT = 0,
};

#endif // !FREEIMAGE_IO

// --------------------------------------------------------------------------

extern "C" {

// Version routines ---------------------------------------------------------

DLL_API const char *FI_GetVersion();
DLL_API const char *FI_GetCopyrightMessage();

// MMX routines  ------------------------------------------------------------

DLL_API void FI_EnableMMX();
DLL_API void FI_DisableMMX();

// Allocate/Unload routines  ------------------------------------------------

DLL_API BYTE *FI_Allocate(int width, int height, int bpp);
DLL_API void FI_Unload(void *dib);

// Image Load routines  -----------------------------------------------------

DLL_API void *FI_LoadBMP(const char *filename, BMPFlags flags);
DLL_API void *FI_LoadBMPFromHandle(FreeImageIO *io, fi_handle handle, BMPFlags flags);
DLL_API void *FI_LoadICO(const char *filename, ICOFlags flags);
DLL_API void *FI_LoadICOFromHandle(FreeImageIO *io, fi_handle handle, ICOFlags flags);
DLL_API void *FI_LoadJPEG(const char *filename, JPEGFlags flags);
DLL_API void *FI_LoadJPEGFromHandle(FreeImageIO *io, fi_handle handle, JPEGFlags flags);
DLL_API void *FI_LoadKOALA(const char *filename, KOALAFlags flags);
DLL_API void *FI_LoadKOALAFromHandle(FreeImageIO *io, fi_handle handle, KOALAFlags flags);
DLL_API void *FI_LoadPCD(const char *filename, PCDFlags flags);
DLL_API void *FI_LoadPCDFromHandle(FreeImageIO *io, fi_handle handle, PCDFlags flags);
DLL_API void *FI_LoadPCX(const char *filename, PCXFlags flags);
DLL_API void *FI_LoadPCXFromHandle(FreeImageIO *io, fi_handle handle, PCXFlags flags);
DLL_API void *FI_LoadPNM(const char *filename, PNMFlags flags);
DLL_API void *FI_LoadPNMFromHandle(FreeImageIO *io, fi_handle handle, PNMFlags flags);
DLL_API void *FI_LoadPNG(const char *filename, PNGFlags flags);
DLL_API void *FI_LoadPNGFromHandle(FreeImageIO *io, fi_handle handle, PNGFlags flags);
DLL_API void *FI_LoadRAS(const char *filename, RASFlags flags);
DLL_API void *FI_LoadRASFromHandle(FreeImageIO *io, fi_handle handle, RASFlags flags);
DLL_API void *FI_LoadTARGA(const char *filename, TARGAFlags flags);
DLL_API void *FI_LoadTARGAFromHandle(FreeImageIO *io, fi_handle handle, TARGAFlags flags);
DLL_API void *FI_LoadTIFF(const char *filename, TIFFFlags flags);
DLL_API void *FI_LoadTIFFFromHandle(FreeImageIO *io, fi_handle handle, TIFFFlags flags);

// Image Save routines ------------------------------------------------------

DLL_API bool FI_SaveBMP(void *dib, const char *filename, BMPFlags flags = BMP_DEFAULT);
DLL_API bool FI_SaveBMPToHandle(void *dib, FreeImageIO *io, fi_handle handle, BMPFlags flags = BMP_DEFAULT);
DLL_API bool FI_SavePNG(void *dib, const char *filename, PNGFlags flags = PNG_DEFAULT);
DLL_API bool FI_SavePNGToHandle(void *dib, FreeImageIO *io, fi_handle handle, PNGFlags flags = PNG_DEFAULT);
DLL_API bool FI_SavePNM(void *dib, const char *filename, PNMFlags flags = PNM_DEFAULT);
DLL_API bool FI_SavePNMToHandle(void *dib, FreeImageIO *io, fi_handle handle, PNMFlags flags = PNM_DEFAULT);
DLL_API bool FI_SaveTIFF(void *dib, const char *filename, TIFFFlags flags = TIFF_DEFAULT);
DLL_API bool FI_SaveTIFFToHandle(void *dib, FreeImageIO *io, fi_handle handle, TIFFFlags flags = TIFF_DEFAULT);

// Image info routines ------------------------------------------------------

DLL_API unsigned FI_GetColorsUsed(void *dib);
DLL_API BYTE * FI_GetBits(void *dib);
DLL_API unsigned FI_GetBPP(void *dib);
DLL_API unsigned FI_GetWidth(void *dib);
DLL_API unsigned FI_GetHeight(void *dib);
DLL_API BITMAPINFO * FI_GetInfo(void *dib);
DLL_API BITMAPINFOHEADER * FI_GetInfoHeader(void *dib);
DLL_API unsigned FI_GetLine(void *dib);
DLL_API RGBQUAD * FI_GetPalette(void *dib);
DLL_API unsigned FI_GetPitch(void *dib);
DLL_API unsigned FI_GetDIBSize(void *dib);
DLL_API FREE_IMAGE_COLOR_TYPE FI_GetColorType(void *dib);

// Filetype routines  -------------------------------------------------------

DLL_API FREE_IMAGE_FORMAT FI_GetFileType(const char *filename, int size);
DLL_API FREE_IMAGE_FORMAT FI_GetFileTypeFromHandle(FreeImageIO *io, fi_handle handle, int size);
DLL_API FREE_IMAGE_FORMAT FI_GetFileTypeFromExt(const char *ext);

// Image Effects ------------------------------------------------------------

DLL_API void FI_Blur(void *dib);
DLL_API void FI_Brighten(void *dib, double factor);
DLL_API void * FI_Clone(void *dib);
DLL_API void FI_Combine(void *dib1, void *dib2, int x, int y, unsigned int alpha);
DLL_API void * FI_AddAlphaMask(void *dib, void *mask);
DLL_API void FI_MaskCombine(void *pdst, int dstx, int dsty, unsigned int width, unsigned int height, void *psrc, int srcx, int srcy);
DLL_API void * FI_Crop(void *dib, int left, int top, int right, int bottom);
DLL_API void FI_Darken(void *dib, double factor);
DLL_API void FI_Flip(void *dib);
DLL_API void FI_Mirror(void *dib);
DLL_API void FI_Invert(void *dib);

// Conversion routines ------------------------------------------------------

DLL_API void FI_ConvertToGreyscale(void *dib);
DLL_API void * FI_ConvertTo8Bits(void *dib);
DLL_API void * FI_ConvertTo16Bits(void *dib, bool type555);
DLL_API void * FI_ConvertTo24Bits(void *dib);
DLL_API void * FI_ConvertTo32Bits(void *dib);
DLL_API void * FI_ColorQuantize(void *dib);

}
