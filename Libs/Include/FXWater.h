#ifndef __FXWATER_H
#define __FXWATER_H


class CFXWater : public CEffect
{


public:


	CFXWater();
	~CFXWater();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	bool TestCreateGrid(float fTime, bool bForce);
	void SmoothGrid    (int   nSteps);
	void ComputeNormals(bool  bNormalize);


	CResource*    m_pResTexture;
	CResource*    m_pResTextureEnv;

	//

	float         m_fTimeMesh1;
	float         m_fTimeMesh2;

	CPerlinNoise2D m_perlin1;
	CPerlinNoise2D m_perlin2;

	// Mesh

	int           m_nResX;
	int           m_nResY;

	CVector3*     m_pv3Vertices;
	CVector3*     m_pv3Normals;
	CVector3*     m_pv3FaceNormals;
	CVector2*     m_pv2Mapping;
	CVector2*     m_pv2Mapping2;

	unsigned int* m_pFaceList;

};


#endif