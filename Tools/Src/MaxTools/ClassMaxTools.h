// ==[ File ]===================================================================================
//
//  -Name     : ClassMaxTools.h
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CStravaganzaMaxTools definition
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


#ifndef CLASSMAXTOOLS_H
#define CLASSMAXTOOLS_H


#include <Engine.h>


// ==[ Classes ]================================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CStravaganzaMaxTools
//
//  - Purpose   : Our main interface, The tool class.
//                Must be child of MAXSDK's UtilityObj since it's a utility
//                plugin.
//
// ---------------------------------------------------------------------------
class CStravaganzaMaxTools : public UtilityObj
{


public:


	CStravaganzaMaxTools();
	~CStravaganzaMaxTools() { DeleteThis(); }

	void DeleteThis() { }

	// From UtilityObj

	void BeginEditParams(Interface *ip, IUtil *iu);
	void EndEditParams  (Interface *ip, IUtil *iu);

	// Window procedure

	static BOOL CALLBACK ToolDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	// Main methods

	void ReadConfig(bool bLoadDefaults = false);
	void SaveConfig();

	bool BuildSceneData();
	
	bool PrepareExportPreview(bool bPreview);
	bool FinishExportPreview();

	bool Export();
	bool Preview();

	enum EAnimType
	{
		ANIMATION_SAMPLING = 0,
		ANIMATION_MAXKEYS
	};


	// Some access to private members for CWinPreview


	bool     GetFullscreen    () const { return m_bFullscreen;     }
	float    GetViewportHeight() const { return m_fViewportHeight; }
	int      GetPreviewResX   () const { return m_nPreviewResX;    }
	int      GetPreviewResY   () const { return m_nPreviewResY;    }
	CCamera* GetCamera        () const { return m_pCamera;         }


private:


	bool        PreProcessNodes      (INode* pMaxRootNode);

	CNode*      BuildNode            (INode* pMaxNode, bool* pbCancelled);
	CObject*    BuildObjectNode      (INode* pMaxObjectNode);
	CCamera*    BuildCameraNode      (INode* pMaxCameraNode);
	CLight*     BuildLightNode       (INode* pMaxLightNode);
	CHelper*    BuildHelperNode      (INode* pMaxHelperNode);

	CParticleSystemStd* BuildParticleSystemNode(INode* pMaxSuperSprayNode);

	bool        BuildMesh            (INode* pMaxObjectNode, CObject* pObject);

	bool        BuildNodeAnimation   (INode* pMaxNode, CNode* pAnimNode);
	CAnimPos*   BuildAnimPos         (INode* pMaxNode);
	CAnimScl*   BuildAnimScl         (INode* pMaxNode);
	CAnimRot*   BuildAnimRot         (INode* pMaxNode);
	bool        BuildCameraSettings  (INode* pMaxCameraNode, CCamera* pCamera);
	bool        BuildLightSettings   (INode* pMaxLightNode,  CLight*  pLight);

	bool        BuildShaders         ();
	bool        ExportShaders        ();

	CMatrix     GetTM                (INode* pMaxNode, float fTime, bool bIncludeParent, bool bRemoveNonUniform);
	Matrix3		GetMaxTM             (INode* pMaxNode, float fTime, bool bIncludeParent, bool bRemoveNonUniform);
	Matrix3		UniformMatrix		 (Matrix3 maxMatrix);

	CVector3    Point3ToVector3      (Point3& p);
	CQuaternion QuatToQuaternion     (Quat&   q);
	CVector4    ColorToVector4       (Color&  col, float fAlpha = 1.0f);

	// Physique

	Modifier*	GetPhysiqueModifier  (INode* pMaxNode);
	int			BuildPhysiqueData	 (INode* pMaxNode, CObject* pObject, std::vector<std::string> &vecBoneNames, std::vector<CBlendedVertex> &vecBlendedVertices);
	bool		AddNodeBones		 (std::vector<INode*> &vecMaxBones, INode *pMaxNode);
	bool		IsBoneInList		 (std::vector<INode*> &vecMaxBones, INode *pBone);
	int			GetBoneIndex		 (std::vector<INode*> &vecMaxBones, INode *pBone);

	// Misc

	void        SetNodeUserDefString (INode* pMaxNode, CNode* pNode);

	bool        TMNegParity          (const Matrix3 &mat);
	bool        Point3Equal          (const Point3& p1, const Point3& p2);
	bool        QuatEqual            (const Quat&   q1, const Quat&   q2);

	unsigned    MaterialCount        () const;
	void        AddMaterial          (Mtl* pMaxMtl);
	int         GetMaterialIndex     (Mtl* pMaxMtl) const;
	Mtl*        GetMaterial          (unsigned uIndex) const;

	CShaderStandard* GetShader       (const std::string& strName);

	bool        IsValidGeomObject    (INode*   pMaxObjectNode);
	bool        IsAnimated           (INode*   pMaxNode, bool *pbPosOut, bool* pbSclOut, bool* pbRotOut);
	bool        HasAnimCameraSettings(INode*   pMaxCameraNode);
	bool        HasAnimLightSettings (INode*   pMaxLightNode);
	bool        IsKnownController    (Control* pMaxController);

	TriObject*  GetTriObjectFromNode (INode* pMaxNode, TimeValue t, bool *pDeleteIt);

	bool        HasLensFlareUserDef  (INode* pMaxNode);
	CLensFlare* BuildLensFlare       (const std::string& strUserDef);

	bool        FillCameraCombobox   (HWND hWndCombo, INode *pMaxNode);

	static ULONG CALLBACK DummyProc  (void *) { return 0; }

	// Data

	bool         m_bWinEnabled;

	CNode*       m_pRootNode;
	Interface*   m_pMaxInterface;
	IUtil*       m_pMaxUtil;

	HWND         m_hPanel;

	std::string  m_strFile;
	std::string  m_strTexturePath;
	std::string  m_strPreviewCamera;
	std::string  m_strAviFile;
	std::string  m_strCfgFile;
	EAnimType    m_eAnimType;

	CRenderVisitor::EMode m_ePreviewType;

	bool  m_bPreprocessMesh;
	bool  m_bUseLights;
	bool  m_bCopyTextures;

	int   m_nAnimCheckStep;
	int   m_nNumPosSamplesPerSec;
	int   m_nNumSclSamplesPerSec;
	int   m_nNumRotSamplesPerSec;
	int   m_nNumCamSettingsPerSec;
	int   m_nNumLightSettingsPerSec;
	int   m_nNumPosSamples;
	int   m_nNumSclSamples;
	int   m_nNumRotSamples;
	int   m_nNumCamSettings;
	int   m_nNumLightSettings;

	int   m_nNumNodes;
	int   m_nCurNode;

	int   m_nNumGeomObjects;
	int   m_nNumCameras;
	int   m_nNumLights;
	int   m_nNumHelpers;
	int   m_nNumLensFlares;
	int   m_nNumParticleSystems;

	float m_fStartTime;
	float m_fEndTime;
	float m_fDuration;
	float m_fViewportHeight;

	int   m_nPreviewResX;
	int   m_nPreviewResY;

	bool  m_bFullscreen;
	bool  m_bLoop;
	bool  m_bShowStats;
	bool  m_bRenderToAvi;

	bool  m_bPreview;

	std::vector<Mtl*>             m_vecMaterials;
	std::vector<CShaderStandard*> m_vecShaders;
	std::vector<INode*>			  m_vecBones;

	CCamera* m_pCamera;

};


#endif