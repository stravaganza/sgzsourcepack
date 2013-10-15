#ifndef __FXRTTRADIALHILIGHTS_H
#define __FXRTTRADIALHILIGHTS_H


class CFXRTTRadialHilights : public CEffect
{


public:


	CFXRTTRadialHilights();
	~CFXRTTRadialHilights();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;           }
	std::string	GetGroupName	() const { return "Render To Texture"; }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		DrawQuad		(UtilGL::Texturing::CTexture2D* pTexture,
								 float fCenterX, float fCenterY, float fRadius, float fAlpha);


private:


	CResource* m_pResTexture;

};


#endif