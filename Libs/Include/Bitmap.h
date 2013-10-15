// ==[ File ]===================================================================================
//
//  -Name     : Bitmap.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CBitmap definition.
//
// =============================================================================================

/*

	28/09/2001  -  File created.


*/


#ifndef __BITMAP_H
#define __BITMAP_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CBitmap
//
//  - Purpose   : Provides image loading and manipulation methods.
//
// ---------------------------------------------------------------------------
class CBitmap
{


public:


	CBitmap()							{ m_pDib = NULL;		}
	CBitmap(CInput* pData,
			const std::string& strExt)	{ Load(pData, strExt);	}
	~CBitmap()							{ Free();				}

	bool Load(CInput* pData, const std::string& strExt);
	bool Free();

	void*    GetPixels();
	RGBQUAD* GetPalette();

	int GetBpp()    const;
	int GetWidth()  const;
	int GetHeight() const;
	int GetPitch()  const;

	bool ToGreyscale();
	bool To8Bits();
	bool To16Bits555();
	bool To16Bits565();
	bool To24Bits();
	bool To32Bits();

	bool AddAlphaChannel(CInput* pData, const std::string& strExt);

	bool Flip();
	bool Mirror();
	bool Invert();


private:


	void* m_pDib;

};


#endif