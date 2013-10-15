#ifndef __FXSCENEDOF_H
#define __FXSCENEDOF_H


class CFXSceneDOF : public CEffect
{


public:


	CFXSceneDOF();
	~CFXSceneDOF();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Scenes";  }

	void		OnDemoFrameBegin			(CDemo* pDemo, float fEffectTime, float fDemoTime);

	int         GetSupportedCommandsCount	()      const { return 1;         }
	std::string GetCommandSyntax			(int i) const { return "#Camera"; }
	bool        ExecuteCommand				(const std::string& strCommand);

	bool		LoadData			(CResourceList* pResourceList);
	bool		Free				();
	bool		DoFrame				(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;

	UtilGL::Texturing::CTexture2D m_texDOF;
	UtilGL::Texturing::CTexture2D m_texBlur;

	CResource* m_pResDOFTexture;
	CResource* m_pResBlurTexture;

	UtilGL::Texturing::CTexture2D* m_pTexDOF;
	UtilGL::Texturing::CTexture2D* m_pTexBlur;

	GLuint m_gluiDOFVertexProgram;

};


#endif