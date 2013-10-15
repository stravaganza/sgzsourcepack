#ifndef __FXTRANSTOCUBEDBITMAP_H
#define __FXTRANSTOCUBEDBITMAP_H


class CFXTransToCubedBitmap : public CEffect
{


public:


	CFXTransToCubedBitmap();
	~CFXTransToCubedBitmap();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;     }
	std::string	GetGroupName	() const { return "Transitions"; }

	bool		LoadOnCreate    () const { return true;          } // LoadData() should be called on creation.
	void		OnDemoFrameBegin(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource*                     m_pResTexture;
	UtilGL::Texturing::CTexture2D* m_pTexture;
	UtilGL::Texturing::CTexture2D  m_textureRTT;
	CVector4					   m_v4FlatColor;

	struct SCube
	{
		CVector3 v1, v2, v3, v4;
		CVector3 v5, v6, v7, v8;
		CVector3 v3Center;
		float    fU, fU2, fV, fV2;
		float    fTStart;
		float    fTEnd;
		float    fAngle;
	};

	typedef std::vector<SCube> VECCUBES;

	VECCUBES m_vecCubes;

	bool DrawCube(const SCube& cube, float fT);

};


#endif