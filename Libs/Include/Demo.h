// ==[ File ]===================================================================================
//
//  -Name     : Demo.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CDemo definition.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __DEMO_H
#define __DEMO_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CDemo
//
//  - Purpose   : Base class to build demos.
//
// ---------------------------------------------------------------------------
class CDemo : public CWindowGL
{


public:


	CDemo();
	virtual ~CDemo();

	// Public interface

	bool            LoadScript			(const std::string& strFile, const std::string& strDatfile = "");
	bool            SaveScript			(const std::string& strFile);
	bool            FreeScript			();

	bool            LoadData			(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions);
	bool            FreeData			();

	bool			SetDefaultCFG		();

	float           GetStart            () const;
	float			GetLength			() const;
	void			SetStart			(float fSeconds);
	void			SetLength			(float fSeconds);
	void			SetLoop				(bool  bLoop);
	bool			GetLoop				() const;

	void            SetSoundtrack		(const std::string& strFile);
	std::string     GetSoundtrack       () const;
	const CSound*	GetSoundIface		() const;
	bool            ReloadSoundtrack    ();

	void            ConfigDemoViewport	(const float pfViewport[4], bool bScissor);
	float*			GetDemoViewport		();
	void            SetDemoViewport		();
	bool            ScissorActive       () const;

	void			ConfigDemoWindow    (int nWidth, int nHeight, bool bFullscreen);
	int				GetDemoWindowWidth  () const;
	int				GetDemoWindowHeight () const;
	bool			IsDemoWindowFS		() const;

	bool            AddResource			(CResource* pResource);
	int             GetResourceCount    () const;
	CResource*      GetResource         (int i);
	CResource*      GetResource			(const std::string& strName);
	bool            RemoveResource		(const std::string& strName, bool bAlsoFree);
	CResourceList*  GetResourceList     ();

	bool            AddEffect			(CEffect* pEffect);
	int             GetEffectCount      () const;
	CEffect*        GetEffect           (int i);
	CEffect*        GetEffect			(const std::string& strName);
	bool            RemoveEffect		(const std::string& strName, bool bAlsoFree);

	int				GetHiddenLayerCount () const;
	int				GetHiddenLayer		(int i) const;
	bool			IsLayerShown		(int nLayer) const;
	void			ShowLayer			(int nLayer, bool bShow);
	void			ShowAllLayers		();

	bool			Prepare				();
	bool			Run					(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions);
	bool			DoFrame				(float fTime);
	void			RequestExit			();
	bool			ExitRequested		() const;

	// Redefine in child classes:

	virtual bool	CheckRequirements();


protected:


	virtual LRESULT WndProc       (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	virtual void    OnLoadStart   ();
	virtual void    OnLoadFinish  ();
	virtual void    OnLoading     (float fPercentage);

	virtual void    OnPlayStart   ();
	virtual void    OnPlayFinish  ();

	virtual void    OnFrameStart  (float fDemoTime, int nNumFrame);
	virtual void    OnFrameFinish (float fDemoTime, int nNumFrame);

	virtual void    OnEffectStart (CEffect* pEffect);
	virtual void    OnEffectFinish(CEffect* pEffect);

private:


	CSound*                 m_pMusic;
	CResourceList			m_resourceList;
	std::string				m_strMusicFile;
	std::vector<CEffect *>	m_vecEffectList;
	std::vector<CEffect *>  m_vecLastFrameEffectList;
	std::vector<int>		m_vecHiddenLayers;

	float    m_pfViewport[4];  // (centerx, centery, width, height) all normalized (0 - 1)
	bool     m_bScissor;
	int      m_nWinWidth;
	int      m_nWinHeight;
	bool     m_bWinFullscreen;
	float    m_fLastTime;
	float    m_fStartTime;
	float    m_fLength;
	bool	 m_bLoop;
	bool     m_bExit;
	int      m_nNumFrame;
};


#endif
