#ifndef __FXSPLINERAYS_H
#define __FXSPLINERAYS_H


class CFXSplineRays : public CEffect
{


public:


	CFXSplineRays();
	~CFXSplineRays();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	struct SSpline
	{
		CSplineCatmullRom spline;
		CVector3* pv3BillBoards;
		float*    pfSizes;
		float     fTStart, fTEnd;
	};

	std::vector<SSpline*> m_vecSplines;

	CResource* m_pResTexture;

};


#endif