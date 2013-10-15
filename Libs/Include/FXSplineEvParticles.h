#ifndef __FXSPLINEEVPARTICLES_H
#define __FXSPLINEEVPARTICLES_H


#include "FXSplineEvaluator.h"


class CFXSplineEvParticles : public CFXSplineEvaluator
{


public:


	CFXSplineEvParticles();
	~CFXSplineEvParticles();

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

	float GetTimeEvaluation(float fTime);
	void OnSplineBegin   (int nSplineCount, int nSplineIndex);
	void OnSplineFinish  (int nSplineCount, int nSplineIndex);

	void OnSplinePoint   (const CVector3& v3PrevPoint, const CVector3& v3Point, const CVector3& v3NextPoint,
						  int   nPointIndex, int   nTotalPoints,
						  float fEvalTime,   float fPointEvalTime, float fPointSplineTime);

	void OnSplineEndPoint(const CVector3& v3Point, const CVector3& v3EscapeDir,
						  float fEvalTime, float fPointEvalTime, float fPointSplineTime);


private:


	float               m_fTime;
	CResource*          m_pResTexture;
	CParticleSystemStd* m_pPartSystem;

	typedef std::vector<CAnimRotSampled::TSample> VECROTSAMPLES;

	CSplineCatmullRom   m_posTrack;
	VECROTSAMPLES       m_rotTrack;
};


#endif