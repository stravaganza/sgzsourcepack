#ifndef __FXCREATIONRAYS_H
#define __FXCREATIONRAYS_H


class CFXCreationRays : public CEffect
{


public:


	CFXCreationRays();
	~CFXCreationRays();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	struct SRay
	{
		CVector3 v3Start;
		CVector3 v3End;
		CVector2 v2Map1;
		CVector2 v2Map2;
		float    fSize;
		float    fStart;    // seconds
		float    fDuration; //   "

		static bool SortPredicate(const SRay& pRay1, const SRay& pRay2)
		{
			return pRay1.fStart < pRay2.fStart;
		}
	};

	typedef std::vector<SRay> VECRAYS;

	VECRAYS    m_vecRays;
	CResource* m_pResTexture;

};


#endif