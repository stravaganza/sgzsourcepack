#ifndef __FXMOSAIC_H
#define __FXMOSAIC_H


class CFXMosaic : public CEffect
{


public:


	CFXMosaic();
	~CFXMosaic();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;  }
	std::string	GetGroupName	() const { return "Bitmaps";  }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	struct STile
	{
		int   nX;
		int   nY;
		int   nImgTileX;
		int   nImgTileY;
	};

	typedef std::vector<STile> VECTILES;

	VECTILES m_vecTiles;

	CResource* m_pResTexture;

	float m_fLastUpdate;

};


#endif