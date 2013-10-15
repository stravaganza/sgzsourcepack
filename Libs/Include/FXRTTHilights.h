#ifndef __FXRTTHILIGHTS_H
#define __FXRTTHILIGHTS_H


class CFXRTTHilights : public CEffect
{


public:


	CFXRTTHilights();
	~CFXRTTHilights();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;           }
	std::string	GetGroupName	() const { return "Render To Texture"; }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;

};


#endif