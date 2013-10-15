// ==[ File ]===================================================================================
//
//  -Name     : LensFlare.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLensFlare definition.
//
// =============================================================================================

/*

	04/04/2003  -  File created.

*/


#ifndef __LENSFLARE_H
#define __LENSFLARE_H


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CLensFlare
//
//  - Purpose   : LensFlare node
//
// ---------------------------------------------------------------------------
class CLensFlare : public CRenderable
{


public:


	CLensFlare();
	~CLensFlare();

	void AddComponent     ( const std::string& strTexture, float fPos, float fSize);
	bool LoadResources    (	const std::string& strDatfile   = "",
							const std::string& strTexFolder = "",
							UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL);
	void SetZTestFade     ( bool bOn, float fFadeDuration) { m_bZTestFade = bOn; m_fFadeDuration = fFadeDuration; }
	int  GetNumComponents () const                         { return m_vecComponents.size(); }
	bool IsVisible        () const                         { return !IS_ZERO(m_fIntensity); }
	bool SetEvalTime      (float fTime);
	bool Render           ();

	// From CNode:

	int  GetNodeType      ()                         const { return ID_NODE_LENSFLARE;                  }
	int  GetNodeSubType   ()                         const { return ID_LENSFLARE_STANDARD;              }
	bool AcceptVisitor    (CNodeVisitor* pNodeVisitor)     { return pNodeVisitor->VisitLensFlare(this); }
	bool FreeNodeData     ();
	bool ReadNodeData     (CInput*  pInput);
	bool WriteNodeData    (COutput* pOutput) const;

	// From CRenderable:

	bool           IsPostRender() const { return true;  }
	bool           IsOpaque()     const { return false; }
	const CShader* GetShader()    const { return NULL;  } // No shader use


private:


	struct SComponent // A flare component
	{
		float fPos;   // 0 < pos  < 1
		float fSize;  // 0 < size < 1 (size = 1 means whole screen, can be greater though)
		std::string strTexture;

		SComponent& operator = (const SComponent& component)
		{
			fPos  = component.fPos;
			fSize = component.fSize;
			strTexture = component.strTexture;
			return *this;
		}
	};

	std::vector<SComponent>                     m_vecComponents; 
	std::vector<UtilGL::Texturing::CTexture2D*> m_vecTextures;

	float m_fIntensity;
	float m_fLastInsideIntensity;
	bool  m_bZTestFade;
	float m_fFadeDuration;

	CTimer m_intensTimer;
};


#endif