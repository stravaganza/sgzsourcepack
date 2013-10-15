#ifndef __FXSCENECONSTRUCTION_H
#define __FXSCENECONSTRUCTION_H


class CFXSceneConstruction : public CEffect
{


public:


	CFXSceneConstruction();
	~CFXSceneConstruction();

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

	static void	PreRender       (CRenderable* pRenderable, void* pData);
	static void	PostRender      (CRenderable* pRenderable, void* pData);


private:


	struct SOffset
	{
		CMatrix     originalTM;
		CVector3    v3Start;
		float	    fOriginalAlpha;
		float	    fAlpha;
		bool		bHasQuat;
		CQuaternion quat;
		float	    fStartTime;
		float       fDuration;
		int		    nGroup;

		float       fTime;

		static bool SortNegX(SOffset* p1, SOffset* p2) { return p1->v3Start.X() > p2->v3Start.X(); }
		static bool SortPosX(SOffset* p1, SOffset* p2) { return p1->v3Start.X() < p2->v3Start.X(); }
		static bool SortNegY(SOffset* p1, SOffset* p2) { return p1->v3Start.Y() > p2->v3Start.Y(); }
		static bool SortPosY(SOffset* p1, SOffset* p2) { return p1->v3Start.Y() < p2->v3Start.Y(); }
		static bool SortNegZ(SOffset* p1, SOffset* p2) { return p1->v3Start.Z() > p2->v3Start.Z(); }
		static bool SortPosZ(SOffset* p1, SOffset* p2) { return p1->v3Start.Z() < p2->v3Start.Z(); }
	};

	typedef std::map<CRenderable*, SOffset> MAPOFFSETS;
	MAPOFFSETS  m_mapOffsets;

	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
};


#endif