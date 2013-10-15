#ifndef __FXRENDERTOTEXTURE_H
#define __FXRENDERTOTEXTURE_H


class CFXRenderToTexture : public CEffect
{


public:


	CFXRenderToTexture();
	~CFXRenderToTexture();

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

};


#endif