#ifndef __FXSPLINEEVTEXTURED_H
#define __FXSPLINEEVTEXTURED_H


#include "FXSplineEvaluator.h"


class CFXSplineEvTextured : public CFXSplineEvaluator
{


public:


	CFXSplineEvTextured();
	~CFXSplineEvTextured();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;        }
	std::string	GetGroupName	() const { return "Spline Writers"; }

	bool OnLoadData	(CResourceList* pResourceList);
	bool OnFree		();

	// From CFXSplineEvaluator

	UtilGL::Texturing::CTexture* GetTexture();

	bool EvaluateWholeSpline();
	void OnSplineFrameBegin(CDemo* pDemo,
							float fEffectTime,
							float fDemoTime);
	void OnSplineFrameFinish();

	void OnSplineBegin   (int nSplineCount, int nSplineIndex);
	void OnSplineFinish  (int nSplineCount, int nSplineIndex);

	void OnSplinePoint   (const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
						  int   nPointIndex, int   nTotalPoints,
						  float fEvalTime,   float fPointEvalTime, float fPointSplineTime);

	void OnSplineEndPoint(const CVector3& v3Point, const CVector3& v3EscapeDir,
						  float fEvalTime, float fPointEvalTime, float fPointSplineTime) { }


private:


	float      m_fThickness, m_fTexOffset;
	CResource* m_pResTexture;

};


#endif