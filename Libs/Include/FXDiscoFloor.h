#ifndef __FXDISCOFLOOR_H
#define __FXDISCOFLOOR_H


class CFXDiscoFloor : public CEffect
{


public:


	CFXDiscoFloor();
	~CFXDiscoFloor();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Scenes";  }

	int         GetSupportedCommandsCount	()      const { return 1;         }
	std::string GetCommandSyntax			(int i) const { return "#Camera"; }
	bool        ExecuteCommand				(const std::string& strCommand);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	std::string m_strCamera;
	CResource*  m_pResScene;

	std::vector<CObject*> m_vecTiles;

	int   m_nPeriod;
	float m_fAnimStart, m_fAnimEnd;

};


#endif
