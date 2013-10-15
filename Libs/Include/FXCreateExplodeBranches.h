#ifndef __FXCREATEEXPLODEBRANCHES_H
#define __FXCREATEEXPLODEBRANCHES_H


class CFXCreateExplodeBranches : public CEffect
{


public:


	CFXCreateExplodeBranches();
	~CFXCreateExplodeBranches();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME ;     }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource* m_pResTexture;

	struct SBranch
	{
		float fTStart,  fTEnd;            // Construction start/end (0-1)
		float fRadius1, fRadius2;         // Bottom, Top.
		float fExplosionIntensity;
		float fExplosionGravity;

		// Precalc

		CSpline*   pSpline;
		int        nPrecTotalSections;    // i sections
		CVector3** ppv3PrecVertices;      // i sections with j vertices
		CVector3** ppv3PrecNormals;
		CVector2** ppv2PrecMapping;
		CVector3*  pv3PrecSections;       // i section centers. (vertices are local to the center and with radius 1)
		CVector3*  pv3PrecSectionDirs;    // i section directions

		// Mesh

		bool      bComplete; // Avoid computing already tesselated branches
		int		  nRenderSections;

		CVector3* pv3Vertices;
		CVector3* pv3Normals;
		CVector2* pv2Mapping;
		int*	  pnIndices;
		int		  nIndices;
		int		  nSections;
		int		  nVertsPerSection;

		// Exploding Mesh

		int       nExplFaces;
		bool	  bExploded;

		CVector3* pv3ExplFaceCenters;
		CVector3* pv3ExplDirection;
		CVector3* pv3ExplWorldVertices;    // The ones that will be rendered
		CVector3* pv3ExplLocalVertices;    // Grouped by 4. Vertices stored in local face coords
		CVector3* pv3ExplNormals;          //      "
		CVector2* pv2ExplMapping;          //      "
	};

	typedef std::vector<SBranch*> VECBRANCHES;

	VECBRANCHES m_vecBranches;

	bool GenerateBranch(SBranch* pBranch, float fT); // t < 0 = invisible, 0 < t < 1 = creating, t > 1 = exploding
	bool RenderBranch  (SBranch* pBranch, float fT);
};


#endif