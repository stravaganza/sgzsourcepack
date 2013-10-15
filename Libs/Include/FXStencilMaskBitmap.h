#ifndef __FXSTENCILMASKBITMAP_H
#define __FXSTENCILMASKBITMAP_H


class CFXStencilMaskBitmap : public CEffect
{


public:


	CFXStencilMaskBitmap();
	~CFXStencilMaskBitmap();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Stencil"; }

	void        OnDemoFrameEnd	(CDemo* pDemo, float fEffectTime, float fDemoTime);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	CResource*     m_pResTexture;
	CBitmap*       m_pBitmap;
	unsigned char* m_pucBuffer;

};


#endif