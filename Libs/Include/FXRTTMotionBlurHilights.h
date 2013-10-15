#ifndef __FXRTTMOTIONBLURHILIGHTS_H
#define __FXRTTMOTIONBLURHILIGHTS_H


class CFXRTTMotionBlurHilights : public CEffect
{


public:


	CFXRTTMotionBlurHilights();
	~CFXRTTMotionBlurHilights();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;           }
	std::string	GetGroupName	() const { return "Render To Texture"; }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;
	UtilGL::Texturing::CTexture2D m_auxTexture;

};


#endif