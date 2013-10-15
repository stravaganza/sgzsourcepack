#ifndef __FXRECURSIVEBLURTEXTURE_H
#define __FXRECURSIVEBLURTEXTURE_H


class CFXRecursiveBlurTexture : public CEffect
{


public:


	CFXRecursiveBlurTexture();
	~CFXRecursiveBlurTexture();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Blurs";   }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;
	UtilGL::Texturing::CTexture2D* m_pTexture;

};


#endif
