#ifndef __FXRINGS_H
#define __FXRINGS_H


class CFXRings : public CEffect
{


public:


	CFXRings();
	~CFXRings();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	struct SRing
	{
		enum EType
		{
			HORIZONTAL = 0,
			VERTICAL
		};

		EType eType;
		float fRadius;
		float fWidth;
		float fAlpha;
		float fDistToCenter;
		float fRotSpeed;
		float fRotAngleStart;
		float fOscillationAmplitude;
		float fOscillationFrequency;
		float fOscillationOffset;
	};

	typedef std::vector<SRing> VECRINGS;

	VECRINGS m_vecRings;

	CResource* m_pResHorTexture;
	CResource* m_pResVertTexture;

	CVector3*  m_pv3Circle;
	int        m_nSubdivisions;

	CVector3*  m_pv3MeshVertices;
	CVector2*  m_pv2MeshMapping;
	int*       m_pnMeshFaceList;
};


#endif