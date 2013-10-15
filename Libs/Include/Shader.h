// ==[ File ]===================================================================================
//
//  -Name     : Shader.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CShader definition.
//
// =============================================================================================

/*

	07/01/2003  -  File created.


*/


#ifndef __SHADER_H
#define __SHADER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CShader
//
//  - Purpose   : CShader is the base class for all shaders.
//                A shader is similar to a material, but has some extra
//                parameters that make it more complex and powerful.
//
// ---------------------------------------------------------------------------
class CShader
{


public:


	CShader();
	virtual ~CShader() { }

	static CShader* Parse			(CInput* pInput, const std::string& strShaderName);

	std::string     GetName			()                           const { return m_strName;    }
	void            SetName			(const std::string& strName)       { m_strName = strName; }

	// Implement in childs:

	virtual bool    WriteASCII		(CWriterASCII* pWriter) const = 0;

	virtual bool    CompileObject	(CObject* pObject) const = 0;

	virtual bool    LoadResources	(const std::string& strDatfile   = "",
									 const std::string& strTexFolder = "",
									 UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL) = 0;

	virtual bool    FreeResources	() = 0;

	virtual bool    SetEvalTime		(float fTime) = 0;
	virtual bool    Begin			()            = 0;
	virtual bool    End				()            = 0;

	virtual int     GetNumPasses	() const                      = 0;
	virtual bool    PrepareStates	(int nPass)                   = 0;
	virtual bool    PrepareObject	(int nPass, CObject* pObject) = 0;

	virtual bool    IsOpaque		() const = 0;
	virtual int     GetBumpChannel	() const = 0;


protected:


	virtual bool    ParseShader(CInput* pInput) = 0;


private:


	std::string     m_strName;
};


#endif