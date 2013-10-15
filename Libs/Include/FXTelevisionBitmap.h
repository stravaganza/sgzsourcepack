#ifndef __FXTELEVISIONBITMAP_H
#define __FXTELEVISIONBITMAP_H


class CFXTelevisionBitmap : public CEffect
{


public:


	CFXTelevisionBitmap();
	~CFXTelevisionBitmap();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;
	UtilGL::Texturing::CTexture2D* m_pTexture;
	unsigned char* m_pucOwnTexture;

};


#endif