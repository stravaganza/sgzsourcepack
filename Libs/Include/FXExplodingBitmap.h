#ifndef __FXEXPLODINGBITMAP_H
#define __FXEXPLODINGBITMAP_H


class CFXExplodingBitmap : public CEffect
{


public:


	CFXExplodingBitmap();
	~CFXExplodingBitmap();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Bitmaps"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;
	CResource* m_pResTexturePart;

	int       m_nNumParticles;
	CVector3* m_pv3Positions;
	CVector3* m_pv3PositionsExpl;
	CVector3* m_pv3Directions;
	CVector4* m_pv4Colors;
	float*    m_pfSizes;

};


#endif