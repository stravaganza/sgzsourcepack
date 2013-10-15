#ifndef __FXFADE_H
#define __FXFADE_H


class CFXFade : public CEffect
{


public:


	CFXFade();
	~CFXFade();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Fades";   }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

};


#endif