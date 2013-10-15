#ifndef __FXSHINYSCENE_H
#define __FXSHINYSCENE_H


class CFXShinyScene : public CEffect
{


public:


	CFXShinyScene();
	~CFXShinyScene();

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

	// CFXShinyScene

	// The object post-render callback that will do the shiny effect

	static void	ShinyPostRender (CRenderable* pRenderable, void* pData);


private:


	struct SPostRenderData
	{
		CLight*   pLight;
		float     fAngleThreshold;
		float     fBillboardSize;

		CVector3* pBLBPositions;
		CVector4* pBLBColors;
		float*    pfBLBSizes;

		UtilGL::Texturing::CTexture2D* pTexture;

	}m_postRenderData;

	CEnumVisitor m_enumVisitor;

	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
	CResource*  m_pResTexture;
};


#endif