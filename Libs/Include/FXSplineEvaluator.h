#ifndef __FXSPLINEEVALUATOR_H
#define __FXSPLINEEVALUATOR_H


// Standard base effect


class CFXSplineEvaluator : public CEffect
{


public:


	CFXSplineEvaluator();
	virtual ~CFXSplineEvaluator();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;        }
	std::string	GetGroupName	() const { return "Spline Writers"; }

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

	// Can be changed in childs:

	virtual bool OnLoadData		(CResourceList* pResourceList)	{ return true; }
	virtual bool OnFree			()								{ return true; }

	virtual UtilGL::Texturing::CTexture* GetTexture()			{ return NULL; }

	virtual bool  EvaluateWholeSpline()					        { return true; }
	virtual float GetTimeEvaluation(float fTime);
	virtual void  OnSplineFrameBegin(CDemo* pDemo,
									float fEffectTime,
									float fDemoTime)            { }
	virtual void OnSplineFrameFinish()							{ }

	virtual void OnSplineBegin (int nSplineCount, int nSplineIndex);
	virtual void OnSplineFinish(int nSplineCount, int nSplineIndex);

	// fEvalTime        = (0 - 1) end evaluation time for this frame
	// fPointEvalTime   = (0 - 1) current point evaluation time in the multiple spline structure
	// fPointSplineTime = (0 - 1) current point evaluation time in the current spline
	//
	// nPointIndex / (nTotalPoints - 1) should be equal to fPointSplineTime

	virtual void OnSplinePoint   (const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
								  int   nPointIndex, int   nTotalPoints,
								  float fEvalTime,   float fPointEvalTime, float fPointSplineTime);

	virtual void OnSplineEndPoint(const CVector3& v3Point, const CVector3& v3EscapeDir,
								  float fEvalTime, float fPointEvalTime, float fPointSplineTime) { }


protected:


	CResource* m_pResSpline;

};


#endif