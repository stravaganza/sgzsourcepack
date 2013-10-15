#ifndef __FXDISTORTEDBITMAP_H
#define __FXDISTORTEDBITMAP_H


class CFXDistortedBitmap : public CEffect
{


public:


	CFXDistortedBitmap();
	~CFXDistortedBitmap();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Bitmaps"; }

	bool		LoadOnCreate    () const { return true;      } // LoadData() should be called on creation.

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;

	int   m_nWidth, m_nHeight;
	float m_fLastDistort;
	float m_fAlpha;
	float m_fDistort;

	CVector3* m_pVertices;
	CVector2* m_pMapVerts;
};


#endif
