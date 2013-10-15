#ifndef __FXMOTIONBLURTEXTURE_H
#define __FXMOTIONBLURTEXTURE_H


class CFXMotionBlurTexture: public CEffect
{


public:


	CFXMotionBlurTexture();
	~CFXMotionBlurTexture();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Blurs";   }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	UtilGL::Texturing::CTexture2D* m_pTexture;
	CResource*					   m_pResTexture;

};


#endif