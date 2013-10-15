#ifndef __FXSPLINEEVSTENCILSPRITE_H
#define __FXSPLINEEVSTENCILSPRITE_H


#include "FXSplineEvaluator.h"


class CFXSplineEvStencilSprite : public CFXSplineEvaluator
{


public:


	CFXSplineEvStencilSprite();
	~CFXSplineEvStencilSprite();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;        }
	std::string	GetGroupName	() const { return "Spline Writers"; }

	void        OnDemoFrameEnd	(CDemo* pDemo, float fEffectTime, float fDemoTime);

	// From CFXSplineEvaluator

	bool OnLoadData	(CResourceList* pResourceList);
	bool OnFree		();

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
						  float fEvalTime, float fPointEvalTime, float fPointSplineTime);


private:


	CResource* m_pResTexture;
	CVector3   m_verts[4];

	bool m_bEvaluateWholeSpline;
};


#endif