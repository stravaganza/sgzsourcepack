#ifndef __FXCLEARFRAMEBUFFER_H
#define __FXCLEARFRAMEBUFFER_H


class CFXClearFramebuffer : public CEffect
{


public:


	CFXClearFramebuffer();
	~CFXClearFramebuffer();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;     }
	std::string	GetGroupName	() const { return "Framebuffer"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

};


#endif