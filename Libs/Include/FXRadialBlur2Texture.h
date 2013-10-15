#ifndef __FXRADIALBLUR2TEXTURE_H
#define __FXRADIALBLUR2TEXTURE_H


class CFXRadialBlur2Texture : public CEffect
{


public:


	CFXRadialBlur2Texture();
	~CFXRadialBlur2Texture();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Blurs";   }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		DrawQuad		(float fCenterX, float fCenterY, float fRadius, float fAlpha);


private:


	CResource* m_pResTexture;
	UtilGL::Texturing::CTexture2D* m_pTexture;

};


#endif