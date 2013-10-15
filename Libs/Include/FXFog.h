#ifndef __FXFOG_H
#define __FXFOG_H


class CFXFog : public CEffect
{


public:


	CFXFog();
	~CFXFog();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;     }
	std::string	GetGroupName	() const { return "Fog";         }

	void        OnLeave			(CDemo* pDemo, float fEffectTime, float fDemoTime);
	void		OnDemoFrameEnd	(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

};


#endif