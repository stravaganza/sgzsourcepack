#ifndef __FXRADIALBLUR2_H
#define __FXRADIALBLUR2_H


class CFXRadialBlur2 : public CEffect
{


public:


	CFXRadialBlur2();
	~CFXRadialBlur2();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Blurs";   }

	bool		LoadOnCreate    () const { return true;      } // LoadData() should be called on creation.

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		DrawQuad		(float fCenterX, float fCenterY, float fRadius, float fAlpha);

private:


	UtilGL::Texturing::CTexture2D m_texture;

};


#endif