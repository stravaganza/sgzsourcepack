#ifndef __FXSTENCILSTATE_H
#define __FXSTENCILSTATE_H


class CFXStencilState : public CEffect
{


public:


	CFXStencilState();
	~CFXStencilState();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Stencil"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

};


#endif