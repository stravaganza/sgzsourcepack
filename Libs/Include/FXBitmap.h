#ifndef __FXBITMAP_H
#define __FXBITMAP_H


class CFXBitmap : public CEffect
{


public:


	CFXBitmap();
	~CFXBitmap();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Bitmaps"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;

};


#endif