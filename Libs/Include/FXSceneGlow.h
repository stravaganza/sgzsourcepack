#ifndef __FXSCENEGLOW_H
#define __FXSCENEGLOW_H


class CFXSceneGlow : public CEffect
{


public:


	CFXSceneGlow();
	~CFXSceneGlow();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Scenes";  }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	int         GetSupportedCommandsCount	()      const { return 1;         }
	std::string GetCommandSyntax			(int i) const { return "#Camera"; }
	bool        ExecuteCommand				(const std::string& strCommand);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	bool		DrawScene		(CDemo* pDemo, float fEffectTime, float fDemoTime);
	bool		DrawGlow		(CDemo* pDemo, float fEffectTime, float fDemoTime);

	float       m_fTime;
	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
	CResource*  m_pResTexture;

	CMatrix		m_mtxWorld, m_mtxView, m_mtxProjection;

};


#endif