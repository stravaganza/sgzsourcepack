#ifndef __FXRADIALBLUR_H
#define __FXRADIALBLUR_H


class CFXRadialBlur : public CEffect
{


public:


	CFXRadialBlur();
	~CFXRadialBlur();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Blurs";   }

	bool		LoadOnCreate    () const { return true;      } // LoadData() should be called on creation.

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	UtilGL::Texturing::CTexture2D m_texture;

};


#endif