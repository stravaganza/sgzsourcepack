// ==[ File ]===================================================================================
//
//  -Name     : ShaderList.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CShaderList definition.
//
// =============================================================================================

/*

	19/03/2003  -  File created.


*/


#ifndef __SHADERLIST_H
#define __SHADERLIST_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CShaderList
//
//  - Purpose   : Provide some functionality to deal with shaders, like
//                loading the needed shaders for a scene and assigning them,
//                loading their resources etc.
//
// ---------------------------------------------------------------------------
class CShaderList
{


public:


	CShaderList();
	~CShaderList();

	bool     ParseAndAssignShaders(CNode* pRootNode, CInput* pInput);
	bool     BuildListFromNodeTree(CNode* pRootNode);
	bool     LoadShaderResources  (const std::string& strDatfile   = "",
								   const std::string& strTexFolder = "",
								   UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL);
	bool     SetEvalTime          (float fTime);
	int      GetNumShaders        ();
	CShader* GetShader            (int  nIndex);

	void     Free                 (bool bDeleteShaders);


private:


	std::vector<CShader*> m_vecShaders;
};


#endif