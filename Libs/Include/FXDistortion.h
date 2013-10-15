#ifndef __FXDISTORTION_H
#define __FXDISTORTION_H


class CFXDistortion : public CEffect
{


public:


	CFXDistortion();
	~CFXDistortion();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;     }
	std::string	GetGroupName	() const { return "Distortions"; }

	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);
	bool		LoadOnCreate    () const { return true;      } // LoadData() should be called on creation.

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	UtilGL::Texturing::CTexture2D m_texture;

	int   m_nWidth, m_nHeight;
	float m_fLastDistort;
	float m_fAlpha;
	float m_fDistort;

	CVector3* m_pVertices;
	CVector2* m_pMapVerts;
};


#endif
