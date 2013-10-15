#ifndef __FXCROSSFADE_H
#define __FXCROSSFADE_H


class CFXCrossFade : public CEffect
{


public:


	CFXCrossFade();
	~CFXCrossFade();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Fades";   }

	void		OnDemoFrameEnd	(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	float m_fOldAlphaScale;

};


#endif