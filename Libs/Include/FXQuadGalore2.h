#ifndef __FXQUADGALORE2_H
#define __FXQUADGALORE2_H


class CFXQuadGalore2 : public CEffect
{


public:


	CFXQuadGalore2();
	~CFXQuadGalore2();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;

};


#endif