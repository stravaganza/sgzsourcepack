#ifndef __FXVIEWPORTPIXELS_H
#define __FXVIEWPORTPIXELS_H


class CFXViewportPixels : public CEffect
{


public:


	CFXViewportPixels();
	~CFXViewportPixels();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME ; }
	std::string	GetGroupName	() const { return "Viewport"; }

	void		OnDemoFrameEnd	(CDemo* pDemo, float fEffectTime, float fDemoTime) { pDemo->SetAspect(m_fAspect); }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	float m_fAspect;

};


#endif