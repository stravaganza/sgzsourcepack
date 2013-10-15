// ==[ File ]===================================================================================
//
//  -Name     : Effect.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CEffect definition.
//
// =============================================================================================

/*

	03/11/2001  -  File created.


*/


#ifndef __EFFECT_H
#define __EFFECT_H


// ==[ Class definitions ]======================================================================


class CDemo;


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CEffect
//
//  - Purpose   : Base class to build demo effects.
//                Almost all functionality is here so that child effects
//                just need to implement loading/drawing methods.
//
// ---------------------------------------------------------------------------
class CEffect : public CPlugin
{


public:


	CEffect();
	virtual ~CEffect();

	static std::string	GETCLASSTYPE				();
	std::string			GetClassType				() const { return GETCLASSTYPE(); }

	// Implement in childs:

	virtual std::string GetGroupName				() const = 0;              // To agroup same effects for the editor (f.e. fades)

	virtual bool        LoadOnCreate                () const { return false; } // For the editor

	virtual bool		LoadData					(CResourceList* pResourceList) = 0;
	virtual bool		Free						()                             = 0;
	virtual bool		DoFrame						(CDemo* pDemo, float fEffectTime, float fDemoTime) = 0;

	virtual int         GetSupportedCommandsCount	()                            const { return 0;    }
	virtual std::string GetCommandSyntax			(int i)                       const { return "";   }
	virtual bool        ExecuteCommand				(const std::string& strCommand)     { return true; }

	virtual void        OnEnter						(CDemo* pDemo, float fEffectTime, float fDemoTime)	{ }
	virtual void        OnLeave						(CDemo* pDemo, float fEffectTime, float fDemoTime)	{ }
	virtual void        OnDemoFrameBegin			(CDemo* pDemo, float fEffectTime, float fDemoTime)	{ }
	virtual void        OnDemoFrameEnd				(CDemo* pDemo, float fEffectTime, float fDemoTime)	{ }

	// CEffect methods:

	std::string			GetFXName		() const;
	void				SetFXName		(const std::string& strName);

	CDemo*				GetDemo			()			  const { return m_pDemo;    }
	void				SetDemo			(CDemo* pDemo)      { m_pDemo  = pDemo;  }

	void                SetBegin        (float fBegin)      { m_fBegin = fBegin; }
	void                SetEnd          (float fEnd)        { m_fEnd   = fEnd;   }
	void                SetLayer        (int   nLayer)      { m_nLayer = nLayer; }
	float               GetBegin        ()            const { return m_fBegin;   }
	float               GetEnd          ()            const { return m_fEnd;     }
	int                 GetLayer        ()            const { return m_nLayer;   }
	bool				LeftScale		(float fNewBegin);
	bool				RightScale		(float fNewEnd);

	bool				Parse			(CAdvancedParser* pParser);
	bool				WriteASCII      (CWriterASCII*    pWriter);

	bool				DefineVar       (const std::string& strName, const std::string& strClassName, const std::string& strDescription = "");
	bool				EvaluateVar     (const std::string& strName, float fTime, CVar::CValue* pValueOut);
	CVar*				GetVar          (const std::string& strName);
	CVar*				GetVar          (int i);
	int                 GetVarCount     () const;

	bool                DefineResourceReference  (const std::string& strName, const std::string& strClassName);
	int                 GetResourceReferenceCount() const;
	int                 GetResourceIndex         (const std::string& strName);
	bool				GetResourceName          (int i, std::string* pstrName);
	bool				GetResourceClassName     (int i, std::string* pstrClassName);
	bool                GetResourceValue         (int i, std::string* pstrValue);
	bool                SetResourceValue         (int i, const std::string& strValue);

	void                AddCommand               (float fTime, const std::string& strCommand);
	int					GetCommandCount          ();
	bool				GetCommand				 (int i, float* pfTimeOut, std::string* pstrCommandOut);

	void                FXLoadError              (const char* szError, ...);
	void                FXRuntimeError           (const char* szError, ...);

	static bool			SortByTimePredicate		 (CEffect* pfx1, CEffect* pfx2);
	static bool			SortByLayerPredicate	 (CEffect* pfx1, CEffect* pfx2);

	static bool			GetShowErrors			 ()             { return s_ShowErrors;  }
	static void			SetShowErrors			 (bool bShow)   { s_ShowErrors = bShow; }


protected:


	CResource*			FindResource             (CResourceList* pResourceList,
												  const std::string& strName,
												  const std::string& strClassName);


private:


	static bool s_ShowErrors;

	CDemo* m_pDemo;

	float m_fBegin, m_fEnd;
	int   m_nLayer;

	struct SResourceRef
	{
		std::string strClass;
		std::string strValue;
	};

	struct SCommand
	{
		float       fTime;
		std::string strCommand;

		static bool Sort(const SCommand& cmd1, const SCommand& cmd2) { return cmd1.fTime < cmd2.fTime; }
	};

	typedef std::map<std::string, CVar*>        MAPVARS;
	typedef std::map<std::string, CVar*>        MAPGLOBALVARS;
	typedef std::map<std::string, SResourceRef> MAPRESOURCES;
	typedef std::vector<SCommand>               VECCOMMANDS;

	std::string  m_strName;
	MAPVARS      m_mapVars;
	MAPRESOURCES m_mapResources;
	VECCOMMANDS  m_vecCommands;

	// Additional transformations (if defined)

	CMatrix  m_mtxAddLocal;
	CMatrix  m_mtxAddWorld;
	CVector3 m_v3AddCamPos;
	CVector3 m_v3AddCamAngles;
};


#endif
