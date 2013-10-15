#ifndef __FXQUADGALORE_H
#define __FXQUADGALORE_H


class CFXQuadGalore : public CEffect
{


public:


	CFXQuadGalore();
	~CFXQuadGalore();

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