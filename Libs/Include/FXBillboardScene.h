#ifndef __FXBILLBOARDSCENE_H
#define __FXBILLBOARDSCENE_H


class CFXBillboardScene : public CEffect
{


public:


	CFXBillboardScene();
	~CFXBillboardScene();

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


	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
	CResource*  m_pResTexture;

};


#endif