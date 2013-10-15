// ==[ File ]===================================================================================
//
//  -Name     : UtilGL.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL namespace. OpenGL utilities.
//
// =============================================================================================

/*

	11/12/2001  -  File created.


*/


#ifndef __UTILGL_H
#define __UTILGL_H


namespace UtilGL
{
	bool  IsReady();
	bool  CheckError();
	const char* GetErrorMessage();

#define MAX_TEXTURINGUNITS 8

	int GetMaxLights();

	struct SContextData
	{
		SContextData()  { SetDefaults(); }
		~SContextData() {                }

		bool SetDefaults();

		// Extensions

		ExtensionTypes*	_pAvailableExtensions;

		// Render Context

		HGLRC					_hRC;
		PIXELFORMATDESCRIPTOR	_pfd;

		int						_nColorBits;
		int						_nDepthBits;
		int						_nStencilBits;
		int						_nDestAlpha;

		std::map<int, int>		_mapMultisampleModes; // <samples, pixelformat>
		int						_nNumSamples;

		// Render States

		int						_pnRenderStates      [States::LAST_RENDERSTATE];
		bool					_pbLockedRenderStates[States::LAST_RENDERSTATE];

		int						_ppnTexturingUnitStates      [MAX_TEXTURINGUNITS][States::LAST_TEXTURINGUNITSTATE];
		bool					_ppbLockedTexturingUnitStates[MAX_TEXTURINGUNITS][States::LAST_TEXTURINGUNITSTATE];

		GLenum					_glSrcBlend;
		GLenum					_glDstBlend;

		float					_fRedBias;
		float					_fGreenBias;
		float					_fBlueBias;
		float					_fAlphaBias;
        float					_fRedScale;
		float					_fGreenScale;
		float					_fBlueScale;
		float					_fAlphaScale;

		// Texturing

		std::map<std::string, int>	_mapTexHandle;    // Key = texname, value = handle
		std::map<int, int>			_mapTexRefCount;  // Key = handle,  value = num times loaded.
		int							_nTexActiveUnit;
		int							_nTexUsedMem;

		// Transforming

		CMatrix					_worldMatrix;
		CMatrix					_viewMatrix;
		std::stack<CMatrix>		_stcMatrices;
	};

	bool			ShareFrom(SContextData* pContextData);
	SContextData*	GetContextData();

}



#endif
