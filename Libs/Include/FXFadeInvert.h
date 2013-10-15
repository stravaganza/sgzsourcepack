#ifndef __FXFADEINVERT_H
#define __FXFADEINVERT_H


class CFXFadeInvert : public CEffect
{


public:


	CFXFadeInvert();
	~CFXFadeInvert();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Fades";   }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


};


#endif