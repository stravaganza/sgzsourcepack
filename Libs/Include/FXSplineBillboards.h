#ifndef __FXSPLINEBILLBOARDS_H
#define __FXSPLINEBILLBOARDS_H


class CFXSplineBillboards : public CEffect
{


public:


	CFXSplineBillboards();
	~CFXSplineBillboards();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CSplineCatmullRom m_spline;

	CVector3* m_pv3PrecalcPoints;
	CVector3* m_pv3BillBoards;

	CResource* m_pResTexture;

};


#endif