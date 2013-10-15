// OpenGL renderstate managing

// ==[ File ]===================================================================================
//
//  -Name     : Renderstates.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : OpenGL renderstate managing.
//
// =============================================================================================

/*

	11/12/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

extern SContextData* _pContextData;

namespace States
{


// ==[ Constants ]==============================================================================


#define INVALID_STATE_VALUE -0xFF


// ==[ Functions  ]=============================================================================


// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool SetDefaults()
//
//  - Purpose   : Sets OpenGL default renderstates.
//                Includes both renderstate and texture unit states.
//
// -----------------------------------------------------------------------------
bool SetDefaults()
{
	assert(UtilGL::IsReady());

	// Renderstates:

	Set(BFCULLING,       ENABLED);
	Set(CULLSIDE,        CULLSIDE_BACK);
	Set(FACEORIENTATION, FACEORIENTATION_CCW);
	Set(SHADEMODE,       SHADEMODE_GOURAUD);
	Set(POLYGONMODE,     POLYGONMODE_FILL);
	Set(BLENDING,        DISABLED);
	Set(FOG,             DISABLED);
	Set(ZBUFFER,         ENABLED);
	Set(ZBUFFERWRITE,    ENABLED);
	Set(ZFUNC,           CMP_LESS);
	Set(STENCILBUFFER,   DISABLED);
	Set(READBUFFER,      COLORBUFFER_BACK);
	Set(DRAWBUFFER,      COLORBUFFER_BACK);
	Set(ALPHATEST,       DISABLED);
	Set(LIGHTING,        DISABLED);
	Set(LIGHT0,          DISABLED);
	Set(LIGHT1,          DISABLED);
	Set(LIGHT2,          DISABLED);
	Set(LIGHT3,          DISABLED);
	Set(LIGHT4,          DISABLED);
	Set(LIGHT5,          DISABLED);
	Set(LIGHT6,          DISABLED);
	Set(LIGHT7,          DISABLED);
	Set(SRCBLEND,        BLEND_ONE);
	Set(DSTBLEND,        BLEND_ZERO);

	// Texture unit states:

	for(int nUnitCount = 0; nUnitCount < UtilGL::Texturing::GetNumUnits(); nUnitCount++)
	{
		if(UtilGL::Extensions::GetAvailableExtensions()->ARB_multitexture)
		{
			UtilGL::Texturing::SetCurrentUnit(nUnitCount);
		}

		Set(TEXTURE1D,      DISABLED);
		Set(TEXTURE2D,      DISABLED);
		Set(TEXTURE3D,      DISABLED);
		Set(TEXTURECUBEMAP, DISABLED);
		Set(GENU,           DISABLED);
		Set(GENV,           DISABLED);
		Set(GENQ,           DISABLED);
		Set(GENR,           DISABLED);
		Set(UGENFUNC,       TEXGEN_VIEWLINEAR);
		Set(VGENFUNC,       TEXGEN_VIEWLINEAR);
		Set(QGENFUNC,       TEXGEN_VIEWLINEAR);
		Set(RGENFUNC,       TEXGEN_VIEWLINEAR);
	}

	if(UtilGL::Extensions::GetAvailableExtensions()->ARB_multitexture)
	{
		UtilGL::Texturing::SetCurrentUnit(0);
	}

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	glLineWidth(1.0f);

	glEnable(GL_NORMALIZE);

	_pContextData->_fRedBias  = 0.0f, _pContextData->_fGreenBias  = 0.0f, _pContextData->_fBlueBias  = 0.0f, _pContextData->_fAlphaBias  = 0.0f;
	_pContextData->_fRedScale = 1.0f, _pContextData->_fGreenScale = 1.0f, _pContextData->_fBlueScale = 1.0f, _pContextData->_fAlphaScale = 1.0f;

	if(UtilGL::RenderContext::GetCurrentSamples() > 1)
	{
		glEnable(GL_MULTISAMPLE_ARB);
	}

	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : Set(ERenderState eState, int nValue)
//
//  - Purpose   : Sets a renderstate value. If it's a redundant call, no
//                state change will be made. We also update our cache.
//
// -----------------------------------------------------------------------------
bool Set(ERenderState eState, int nValue)
{
	assert(UtilGL::IsReady());
	assert(eState != LAST_RENDERSTATE);

	// Locked?
	if(_pContextData->_pbLockedRenderStates[eState])
	{
		return true;
	}

	// Reduntant call?
	if(_pContextData->_pnRenderStates[eState] == nValue)
	{
		return true;
	}

	// Ok. Change render state value.

	bool bError = false;

	switch(eState)
	{

	case BFCULLING: nValue == DISABLED ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);

		break;

	case CULLSIDE:

		switch(nValue)
		{
		case CULLSIDE_FRONT:        glCullFace(GL_FRONT);          break;
		case CULLSIDE_BACK:         glCullFace(GL_BACK);           break;
		case CULLSIDE_FRONTANDBACK: glCullFace(GL_FRONT_AND_BACK); break;
		default: bError = true; break;
		}

		break;

	case FACEORIENTATION:
		
		switch(nValue)
		{
		case FACEORIENTATION_CCW: glFrontFace(GL_CCW); break;
		case FACEORIENTATION_CW:  glFrontFace(GL_CW);  break;
		default: bError = true; break;
		}

		break;

	case SHADEMODE:

		switch(nValue)
		{
		case SHADEMODE_FLAT:    glShadeModel(GL_FLAT);   break;
		case SHADEMODE_GOURAUD: glShadeModel(GL_SMOOTH); break;
		default: bError = true; break;
		}

		break;

	case POLYGONMODE:

		switch(nValue)
		{
		case POLYGONMODE_POINT: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		case POLYGONMODE_LINE:  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  break;
		case POLYGONMODE_FILL:  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  break;
		default: bError = true; break;
		}

		break;

	case BLENDING:      nValue == DISABLED ? glDisable(GL_BLEND)       : glEnable(GL_BLEND);      break;
	case FOG:           nValue == DISABLED ? glDisable(GL_FOG)         : glEnable(GL_FOG);        break;
	case ZBUFFER:       nValue == DISABLED ? glDisable(GL_DEPTH_TEST)  : glEnable(GL_DEPTH_TEST); break;
	case ZBUFFERWRITE:  nValue == DISABLED ? glDepthMask(0)            : glDepthMask(1);          break;

	case ZFUNC:

		switch(nValue)
		{
		case CMP_NEVER:        glDepthFunc(GL_NEVER);    break;
		case CMP_LESS:         glDepthFunc(GL_LESS);     break;
		case CMP_EQUAL:        glDepthFunc(GL_EQUAL);    break;
		case CMP_LESSEQUAL:    glDepthFunc(GL_LEQUAL);   break;
		case CMP_GREATER:      glDepthFunc(GL_GREATER);  break;
		case CMP_NOTEQUAL:     glDepthFunc(GL_NOTEQUAL); break;
		case CMP_GREATEREQUAL: glDepthFunc(GL_GEQUAL);   break;
		case CMP_ALWAYS:       glDepthFunc(GL_ALWAYS);   break;
		default: bError = true; break;
		}

		break;

	case STENCILBUFFER: nValue == DISABLED ? glDisable(GL_STENCIL_TEST) : glEnable(GL_STENCIL_TEST);

		break;

	case READBUFFER:

		switch(nValue)
		{
		case COLORBUFFER_NONE:         glReadBuffer(GL_NONE);           break;
		case COLORBUFFER_FRONT:        glReadBuffer(GL_FRONT);          break;
		case COLORBUFFER_BACK:         glReadBuffer(GL_BACK);           break;
		case COLORBUFFER_FRONTANDBACK: glReadBuffer(GL_FRONT_AND_BACK); break;
		default: break;
		}

		break;

	case DRAWBUFFER:

		switch(nValue)
		{
		case COLORBUFFER_NONE:         glDrawBuffer(GL_NONE);           break;
		case COLORBUFFER_FRONT:        glDrawBuffer(GL_FRONT);          break;
		case COLORBUFFER_BACK:         glDrawBuffer(GL_BACK);           break;
		case COLORBUFFER_FRONTANDBACK: glDrawBuffer(GL_FRONT_AND_BACK); break;
		default: break;
		}

		break;

	case ALPHATEST: nValue == DISABLED ? glDisable(GL_ALPHA_TEST) : glEnable(GL_ALPHA_TEST);

		break;

	case LIGHTING: nValue == DISABLED ? glDisable(GL_LIGHTING) : glEnable(GL_LIGHTING);

		break;

	case LIGHT0:
	case LIGHT1:
	case LIGHT2:
	case LIGHT3:
	case LIGHT4:
	case LIGHT5:
	case LIGHT6:
	case LIGHT7:

		nValue == DISABLED ? glDisable(GL_LIGHT0 + (eState - LIGHT0)) : glEnable(GL_LIGHT0 + (eState - LIGHT0));

		break;

	case SRCBLEND:

		switch(nValue)
		{
		case BLEND_ZERO:             _pContextData->_glSrcBlend = GL_ZERO;					break;
		case BLEND_ONE:              _pContextData->_glSrcBlend = GL_ONE;					break;
		case BLEND_SRCCOLOR:         _pContextData->_glSrcBlend = GL_SRC_COLOR;			break;
		case BLEND_INVSRCCOLOR:      _pContextData->_glSrcBlend = GL_ONE_MINUS_SRC_COLOR;	break;
		case BLEND_DSTCOLOR:         _pContextData->_glSrcBlend = GL_DST_COLOR;			break;
		case BLEND_INVDSTCOLOR:      _pContextData->_glSrcBlend = GL_ONE_MINUS_DST_COLOR;	break;
		case BLEND_SRCALPHA:         _pContextData->_glSrcBlend = GL_SRC_ALPHA;			break;
		case BLEND_INVSRCALPHA:      _pContextData->_glSrcBlend = GL_ONE_MINUS_SRC_ALPHA;	break;
		case BLEND_DSTALPHA:         _pContextData->_glSrcBlend = GL_DST_ALPHA;			break;
		case BLEND_INVDSTALPHA:      _pContextData->_glSrcBlend = GL_ONE_MINUS_DST_ALPHA;	break;
		case BLEND_SRCALPHASATURATE: _pContextData->_glSrcBlend = GL_SRC_ALPHA_SATURATE;	break;
		default: bError = true; break;
		}

		if(!bError)
		{
			glBlendFunc(_pContextData->_glSrcBlend, _pContextData->_glDstBlend);
		}

		break;

	case DSTBLEND:

		switch(nValue)
		{
		case BLEND_ZERO:             _pContextData->_glDstBlend = GL_ZERO;					break;
		case BLEND_ONE:              _pContextData->_glDstBlend = GL_ONE;					break;
		case BLEND_SRCCOLOR:         _pContextData->_glDstBlend = GL_SRC_COLOR;			break;
		case BLEND_INVSRCCOLOR:      _pContextData->_glDstBlend = GL_ONE_MINUS_SRC_COLOR;	break;
		case BLEND_DSTCOLOR:         _pContextData->_glDstBlend = GL_DST_COLOR;			break;
		case BLEND_INVDSTCOLOR:      _pContextData->_glDstBlend = GL_ONE_MINUS_DST_COLOR;	break;
		case BLEND_SRCALPHA:         _pContextData->_glDstBlend = GL_SRC_ALPHA;			break;
		case BLEND_INVSRCALPHA:      _pContextData->_glDstBlend = GL_ONE_MINUS_SRC_ALPHA;	break;
		case BLEND_DSTALPHA:         _pContextData->_glDstBlend = GL_DST_ALPHA;			break;
		case BLEND_INVDSTALPHA:      _pContextData->_glDstBlend = GL_ONE_MINUS_DST_ALPHA;	break;
		case BLEND_SRCALPHASATURATE: _pContextData->_glDstBlend = GL_SRC_ALPHA_SATURATE;	break;
		default: bError = true; break;
		}

		if(!bError)
		{
			glBlendFunc(_pContextData->_glSrcBlend, _pContextData->_glDstBlend);
		}

		break;

	default:

		bError = true; break;

	}

	if(bError)
	{
		LOG.Write("\nWARNING - GLUtil::States::Set(): Invalid state change. State = %u, value = %u.", eState, nValue);
		return false;
	}

	_pContextData->_pnRenderStates[eState] = nValue;
	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool Set(ETexturingUnitState eState, int nValue)
//
//  - Purpose   : Sets a render(texture)state value. If it's a redundant call no
//                state change will be made.
//                We also update our cache.
//
// -----------------------------------------------------------------------------
bool Set(ETexturingUnitState eState, int nValue)
{
	assert(UtilGL::IsReady());
	assert(eState != LAST_TEXTURINGUNITSTATE);

	bool bError = false;
	int  nUnit  = UtilGL::Texturing::GetCurrentUnit();
	
	// Locked?
	if(_pContextData->_ppbLockedTexturingUnitStates[nUnit][eState])
	{
		return true;
	}

	// Reduntant call?
	if(_pContextData->_ppnTexturingUnitStates[nUnit][eState] == nValue)
	{
		return true;
	}

	// Ok. Change render state value.

	switch(eState)
	{

	case TEXTURE1D:      nValue == DISABLED ? glDisable(GL_TEXTURE_1D)           : glEnable(GL_TEXTURE_1D);           break;
	case TEXTURE2D:      nValue == DISABLED ? glDisable(GL_TEXTURE_2D)           : glEnable(GL_TEXTURE_2D);           break;
	case TEXTURE3D:      nValue == DISABLED ? glDisable(GL_TEXTURE_3D)           : glEnable(GL_TEXTURE_3D);           break;
	case TEXTURECUBEMAP: nValue == DISABLED ? glDisable(GL_TEXTURE_CUBE_MAP_ARB) : glEnable(GL_TEXTURE_CUBE_MAP_ARB); break;
	case GENU:           nValue == DISABLED ? glDisable(GL_TEXTURE_GEN_S)        : glEnable(GL_TEXTURE_GEN_S);        break;
	case GENV:           nValue == DISABLED ? glDisable(GL_TEXTURE_GEN_T)        : glEnable(GL_TEXTURE_GEN_T);        break;
	case GENQ:           nValue == DISABLED ? glDisable(GL_TEXTURE_GEN_Q)        : glEnable(GL_TEXTURE_GEN_Q);        break;
	case GENR:           nValue == DISABLED ? glDisable(GL_TEXTURE_GEN_R)        : glEnable(GL_TEXTURE_GEN_R);        break;

	case UGENFUNC:
	case VGENFUNC:
	case QGENFUNC:
	case RGENFUNC:

		GLenum whichGLCoord;
		GLint  whichGLMode;

		switch(eState)
		{
		case UGENFUNC: whichGLCoord = GL_S; break;
		case VGENFUNC: whichGLCoord = GL_T; break;
		case QGENFUNC: whichGLCoord = GL_Q; break;
		case RGENFUNC: whichGLCoord = GL_R; break;
		default: bError = true; break;
		}

		switch(nValue)
		{
		case TEXGEN_LOCALLINEAR:   whichGLMode = GL_OBJECT_LINEAR;      break;
		case TEXGEN_VIEWLINEAR:    whichGLMode = GL_EYE_LINEAR;         break;
		case TEXGEN_SPHEREMAP:     whichGLMode = GL_SPHERE_MAP;         break;
		case TEXGEN_NORMALMAP:     whichGLMode = GL_NORMAL_MAP_ARB;     break;
		case TEXGEN_REFLECTIONMAP: whichGLMode = GL_REFLECTION_MAP_ARB; break;
		default: bError = true; break;
		}

		if(!bError)
		{
			glTexGeni(whichGLCoord, GL_TEXTURE_GEN_MODE, whichGLMode);
		}

		break;

	default:

		assert(!"UtilGL::States::Get(). Unexpected texturestate!"); // Shouldn't happen.
		break;

	}

	if(bError)
	{
		LOG.Write("\nWARNING - GLUtil::States::Set() Invalid texturingunit state change. State = %u, value = %u.", eState, nValue);
		return false;
	}

	_pContextData->_ppnTexturingUnitStates[nUnit][eState] = nValue;
	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : int Get(ERenderState eState)
//
//  - Purpose   : Returns a renderstate value. Instead of using an OpenGL call
//                (which can be very expensive), we quey our own render state
//                cache. We only call OpenGL when neccesary (i.e. when we didn't
//                chached the value yet).
//
// -----------------------------------------------------------------------------
int Get(ERenderState eState)
{
	assert(UtilGL::IsReady());
	assert(eState != LAST_RENDERSTATE);

	int nGLValue;
	int pnGLValue2[2];

	// Check our cache first:

	if(_pContextData->_pnRenderStates[eState] != INVALID_STATE_VALUE)
	{
		return _pContextData->_pnRenderStates[eState];
	}

	// Cache hasn't the value yet:

	bool bError = false;

	switch(eState)
	{

	case BFCULLING:

		if(glIsEnabled(GL_CULL_FACE)) _pContextData->_pnRenderStates[BFCULLING] = ENABLED;
		else                          _pContextData->_pnRenderStates[BFCULLING] = DISABLED;

		break;

	case CULLSIDE:

		glGetIntegerv(GL_CULL_FACE_MODE, &nGLValue);

		switch(nGLValue)
		{
		case GL_FRONT:          _pContextData->_pnRenderStates[CULLSIDE] = CULLSIDE_FRONT;        break;
		case GL_BACK:           _pContextData->_pnRenderStates[CULLSIDE] = CULLSIDE_BACK;         break;
		case GL_FRONT_AND_BACK:	_pContextData->_pnRenderStates[CULLSIDE] = CULLSIDE_FRONTANDBACK; break;
		}

		break;

	case FACEORIENTATION:

		glGetIntegerv(GL_FRONT_FACE, &nGLValue);

		switch(nGLValue)
		{
		case GL_CW:  _pContextData->_pnRenderStates[FACEORIENTATION] = FACEORIENTATION_CW;  break;
		case GL_CCW: _pContextData->_pnRenderStates[FACEORIENTATION] = FACEORIENTATION_CCW; break;
		}

		break;

	case SHADEMODE:

		glGetIntegerv(GL_SHADE_MODEL, &nGLValue);

		switch(nGLValue)
		{
		case GL_FLAT:   _pContextData->_pnRenderStates[SHADEMODE] = SHADEMODE_FLAT;    break;
		case GL_SMOOTH: _pContextData->_pnRenderStates[SHADEMODE] = SHADEMODE_GOURAUD; break;
		}

		break;

	case POLYGONMODE:

		glGetIntegerv(GL_POLYGON_MODE, pnGLValue2);

		// pnGLValue2[1] returns the backfacing value.
		// It should be the same since we modify both when changing this state.

		switch(pnGLValue2[0])
		{
		case GL_POINT: _pContextData->_pnRenderStates[POLYGONMODE] = POLYGONMODE_POINT; break;
		case GL_LINE:  _pContextData->_pnRenderStates[POLYGONMODE] = POLYGONMODE_LINE;  break;
		case GL_FILL:  _pContextData->_pnRenderStates[POLYGONMODE] = POLYGONMODE_FILL;  break;
		}

		break;

	case BLENDING:

		if(glIsEnabled(GL_BLEND)) _pContextData->_pnRenderStates[BLENDING] = ENABLED;
		else                      _pContextData->_pnRenderStates[BLENDING] = DISABLED;

		break;

	case FOG:

		if(glIsEnabled(GL_FOG)) _pContextData->_pnRenderStates[FOG] = ENABLED;
		else                    _pContextData->_pnRenderStates[FOG] = DISABLED;

		break;

	case ZBUFFER:

		if(glIsEnabled(GL_DEPTH_TEST)) _pContextData->_pnRenderStates[ZBUFFER] = ENABLED;
		else                           _pContextData->_pnRenderStates[ZBUFFER] = DISABLED;

		break;

	case ZBUFFERWRITE:

		if(glIsEnabled(GL_DEPTH_WRITEMASK)) _pContextData->_pnRenderStates[ZBUFFERWRITE] = ENABLED;
		else                                _pContextData->_pnRenderStates[ZBUFFERWRITE] = DISABLED;

		break;

	case ZFUNC:

		glGetIntegerv(GL_DEPTH_FUNC, &nGLValue);

		switch(nGLValue)
		{
		case GL_NEVER:    _pContextData->_pnRenderStates[ZFUNC] = CMP_NEVER;        break;
		case GL_LESS:     _pContextData->_pnRenderStates[ZFUNC] = CMP_LESS;         break;
		case GL_EQUAL:    _pContextData->_pnRenderStates[ZFUNC] = CMP_EQUAL;        break;
		case GL_LEQUAL:   _pContextData->_pnRenderStates[ZFUNC] = CMP_LESSEQUAL;    break;
		case GL_GREATER:  _pContextData->_pnRenderStates[ZFUNC] = CMP_GREATER;      break;
		case GL_NOTEQUAL: _pContextData->_pnRenderStates[ZFUNC] = CMP_NOTEQUAL;     break;
		case GL_GEQUAL:   _pContextData->_pnRenderStates[ZFUNC] = CMP_GREATEREQUAL; break;
		case GL_ALWAYS:   _pContextData->_pnRenderStates[ZFUNC] = CMP_ALWAYS;       break;
		}

		break;

	case STENCILBUFFER:

		if(glIsEnabled(GL_STENCIL_TEST)) _pContextData->_pnRenderStates[STENCILBUFFER] = ENABLED;
		else                             _pContextData->_pnRenderStates[STENCILBUFFER] = DISABLED;

		break;

	case READBUFFER:

		glGetIntegerv(GL_READ_BUFFER, &nGLValue);

		// We skip some return values since we have no way to change them.

		switch(nGLValue)
		{
		case GL_NONE:           _pContextData->_pnRenderStates[READBUFFER] = COLORBUFFER_NONE;         break;
		case GL_FRONT:          _pContextData->_pnRenderStates[READBUFFER] = COLORBUFFER_FRONT;        break;
		case GL_BACK:           _pContextData->_pnRenderStates[READBUFFER] = COLORBUFFER_BACK;         break;
		case GL_FRONT_AND_BACK: _pContextData->_pnRenderStates[READBUFFER] = COLORBUFFER_FRONTANDBACK; break;
		}

		break;

	case DRAWBUFFER:

		glGetIntegerv(GL_DRAW_BUFFER, &nGLValue);

		// We skip some return values since we have no way to change them.

		switch(nGLValue)
		{
		case GL_NONE:           _pContextData->_pnRenderStates[DRAWBUFFER] = COLORBUFFER_NONE;         break;
		case GL_FRONT:          _pContextData->_pnRenderStates[DRAWBUFFER] = COLORBUFFER_FRONT;        break;
		case GL_BACK:           _pContextData->_pnRenderStates[DRAWBUFFER] = COLORBUFFER_BACK;         break;
		case GL_FRONT_AND_BACK: _pContextData->_pnRenderStates[DRAWBUFFER] = COLORBUFFER_FRONTANDBACK; break;
		}

		break;

	case ALPHATEST:

		if(glIsEnabled(GL_ALPHA_TEST)) _pContextData->_pnRenderStates[ALPHATEST] = ENABLED;
		else                           _pContextData->_pnRenderStates[ALPHATEST] = DISABLED;

		break;

	case LIGHTING:

		if(glIsEnabled(GL_LIGHTING)) _pContextData->_pnRenderStates[LIGHTING] = ENABLED;
		else                         _pContextData->_pnRenderStates[LIGHTING] = DISABLED;

		break;

	case LIGHT0:
	case LIGHT1:
	case LIGHT2:
	case LIGHT3:
	case LIGHT4:
	case LIGHT5:
	case LIGHT6:
	case LIGHT7:

		if(glIsEnabled(GL_LIGHT0 + (eState - LIGHT0))) _pContextData->_pnRenderStates[eState] = ENABLED;
		else                                           _pContextData->_pnRenderStates[eState] = DISABLED;

		break;

	case SRCBLEND:

		glGetIntegerv(GL_BLEND_SRC, &nGLValue);

		switch(nGLValue)
		{

		case GL_ZERO:                _pContextData->_pnRenderStates[SRCBLEND] = BLEND_ZERO;             break;
		case GL_ONE:                 _pContextData->_pnRenderStates[SRCBLEND] = BLEND_ONE;              break;
		case GL_SRC_COLOR:           _pContextData->_pnRenderStates[SRCBLEND] = BLEND_SRCCOLOR;         break;
		case GL_ONE_MINUS_SRC_COLOR: _pContextData->_pnRenderStates[SRCBLEND] = BLEND_INVSRCCOLOR;      break;
		case GL_DST_COLOR:           _pContextData->_pnRenderStates[SRCBLEND] = BLEND_DSTCOLOR;         break;
		case GL_ONE_MINUS_DST_COLOR: _pContextData->_pnRenderStates[SRCBLEND] = BLEND_INVDSTCOLOR;      break;
		case GL_SRC_ALPHA:           _pContextData->_pnRenderStates[SRCBLEND] = BLEND_SRCALPHA;         break;
		case GL_ONE_MINUS_SRC_ALPHA: _pContextData->_pnRenderStates[SRCBLEND] = BLEND_INVSRCALPHA;      break;
		case GL_DST_ALPHA:           _pContextData->_pnRenderStates[SRCBLEND] = BLEND_DSTALPHA;         break;
		case GL_ONE_MINUS_DST_ALPHA: _pContextData->_pnRenderStates[SRCBLEND] = BLEND_INVDSTALPHA;      break;
		case GL_SRC_ALPHA_SATURATE:  _pContextData->_pnRenderStates[SRCBLEND] = BLEND_SRCALPHASATURATE; break;

		}

		break;

	case DSTBLEND:

		glGetIntegerv(GL_BLEND_DST, &nGLValue);

		switch(nGLValue)
		{

		case GL_ZERO:                _pContextData->_pnRenderStates[DSTBLEND] = BLEND_ZERO;             break;
		case GL_ONE:                 _pContextData->_pnRenderStates[DSTBLEND] = BLEND_ONE;              break;
		case GL_SRC_COLOR:           _pContextData->_pnRenderStates[DSTBLEND] = BLEND_SRCCOLOR;         break;
		case GL_ONE_MINUS_SRC_COLOR: _pContextData->_pnRenderStates[DSTBLEND] = BLEND_INVSRCCOLOR;      break;
		case GL_DST_COLOR:           _pContextData->_pnRenderStates[DSTBLEND] = BLEND_DSTCOLOR;         break;
		case GL_ONE_MINUS_DST_COLOR: _pContextData->_pnRenderStates[DSTBLEND] = BLEND_INVDSTCOLOR;      break;
		case GL_SRC_ALPHA:           _pContextData->_pnRenderStates[DSTBLEND] = BLEND_SRCALPHA;         break;
		case GL_ONE_MINUS_SRC_ALPHA: _pContextData->_pnRenderStates[DSTBLEND] = BLEND_INVSRCALPHA;      break;
		case GL_DST_ALPHA:           _pContextData->_pnRenderStates[DSTBLEND] = BLEND_DSTALPHA;         break;
		case GL_ONE_MINUS_DST_ALPHA: _pContextData->_pnRenderStates[DSTBLEND] = BLEND_INVDSTALPHA;      break;
		case GL_SRC_ALPHA_SATURATE:  _pContextData->_pnRenderStates[DSTBLEND] = BLEND_SRCALPHASATURATE; break;

		}

		break;

	default:

		bError = true;
		assert(!"UtilGL::States::Get(). Unexpected state!"); // Shouldn't happen.
		break;

	}

	if(bError)
	{
		LOG.Write("\nWARNING - GLUtil::States::Get() Invalid state request. State = %u", eState);
		return -1;
	}

	return _pContextData->_pnRenderStates[eState];
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : int Get(ETexturingUnitState eState)
//
//  - Purpose   : Returns a render(texture)state value. Instead of using an OpenGL
//                call (which can be very expensive), we quey our own render state
//                cache. We only call OpenGL when neccesary (i.e. when we didn't
//                chached the value yet).
//
// -----------------------------------------------------------------------------
int Get(ETexturingUnitState eState)
{
	assert(UtilGL::IsReady());
	assert(eState != LAST_RENDERSTATE);

	int nUnit = UtilGL::Texturing::GetCurrentUnit();
	int nGLValue;

	// Check our cache first:

	if(_pContextData->_ppnTexturingUnitStates[nUnit][eState] != INVALID_STATE_VALUE)
	{
		return _pContextData->_ppnTexturingUnitStates[nUnit][eState];
	}

	// Cache hastn't received the value yet.

	switch(eState)
	{

	case TEXTURE1D:

		if(glIsEnabled(GL_TEXTURE_1D)) _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURE1D] = ENABLED;
		else                           _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURE1D] = DISABLED;

		break;

	case TEXTURE2D:

		if(glIsEnabled(GL_TEXTURE_2D)) _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURE2D] = ENABLED;
		else                           _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURE2D] = DISABLED;

		break;

	case TEXTURE3D:

		if(glIsEnabled(GL_TEXTURE_3D)) _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURE3D] = ENABLED;
		else                           _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURE3D] = DISABLED;

		break;

	case TEXTURECUBEMAP:

		if(glIsEnabled(GL_TEXTURE_CUBE_MAP_ARB)) _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURECUBEMAP] = ENABLED;
		else                                     _pContextData->_ppnTexturingUnitStates[nUnit][TEXTURECUBEMAP] = DISABLED;

		break;

	case GENU:

		if(glIsEnabled(GL_TEXTURE_GEN_S)) _pContextData->_ppnTexturingUnitStates[nUnit][GENU] = ENABLED;
		else                              _pContextData->_ppnTexturingUnitStates[nUnit][GENU] = DISABLED;

		break;

	case GENV:

		if(glIsEnabled(GL_TEXTURE_GEN_T)) _pContextData->_ppnTexturingUnitStates[nUnit][GENV] = ENABLED;
		else                              _pContextData->_ppnTexturingUnitStates[nUnit][GENV] = DISABLED;

		break;

	case GENQ:

		if(glIsEnabled(GL_TEXTURE_GEN_Q)) _pContextData->_ppnTexturingUnitStates[nUnit][GENQ] = ENABLED;
		else                              _pContextData->_ppnTexturingUnitStates[nUnit][GENQ] = DISABLED;

		break;

	case GENR:

		if(glIsEnabled(GL_TEXTURE_GEN_R)) _pContextData->_ppnTexturingUnitStates[nUnit][GENR] = ENABLED;
		else                              _pContextData->_ppnTexturingUnitStates[nUnit][GENR] = DISABLED;

		break;

	case UGENFUNC:
	case VGENFUNC:
	case QGENFUNC:
	case RGENFUNC:

		GLenum whichGLCoord;

		switch(eState)
		{
		case UGENFUNC: whichGLCoord = GL_S; break;
		case VGENFUNC: whichGLCoord = GL_T; break;
		case QGENFUNC: whichGLCoord = GL_Q; break;
		case RGENFUNC: whichGLCoord = GL_R; break;
		}

		glGetTexGeniv(whichGLCoord, GL_TEXTURE_GEN_MODE, &nGLValue);

		switch(nGLValue)
		{
		case GL_OBJECT_LINEAR:      _pContextData->_ppnTexturingUnitStates[nUnit][eState] = TEXGEN_LOCALLINEAR;   break;
		case GL_EYE_LINEAR:         _pContextData->_ppnTexturingUnitStates[nUnit][eState] = TEXGEN_VIEWLINEAR;    break;
		case GL_SPHERE_MAP:         _pContextData->_ppnTexturingUnitStates[nUnit][eState] = TEXGEN_SPHEREMAP;     break;
		case GL_NORMAL_MAP_ARB:     _pContextData->_ppnTexturingUnitStates[nUnit][eState] = TEXGEN_NORMALMAP;     break;
		case GL_REFLECTION_MAP_ARB: _pContextData->_ppnTexturingUnitStates[nUnit][eState] = TEXGEN_REFLECTIONMAP; break;
		}

		break;

	default:

		assert(!"UtilGL::States::Get(). Unexpected texturestate!"); // Shouldn't happen.
		break;

	}

	return _pContextData->_ppnTexturingUnitStates[nUnit][eState];
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool Lock(ERenderState eState, bool bLockState)
//
//  - Purpose   : Locks a renderstate, preventing it from changing.
//
// -----------------------------------------------------------------------------
bool Lock(ERenderState eState, bool bLockState)
{
	assert(UtilGL::IsReady());
	assert(eState != LAST_RENDERSTATE);

	_pContextData->_pbLockedRenderStates[eState] = bLockState;

	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool Lock(ETexturingUnitState eState, bool bLockState)
//
//  - Purpose   : Locks a TexturingUnit state, preventing it from changing.
//
// -----------------------------------------------------------------------------
bool Lock(ETexturingUnitState eState, bool bLockState)
{
	assert(UtilGL::IsReady());
	assert(eState != LAST_TEXTURINGUNITSTATE);

	_pContextData->_ppbLockedTexturingUnitStates[UtilGL::Texturing::GetCurrentUnit()][eState] = bLockState;

	return true;
}


// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : bool InvalidateCache()
//
//  - Purpose   : Invalidates our renderstate caché, forcing Set() and
//                Get() to use OpenGL functions first to fill caché
//                with valid values.
//
//  - Notes     : Don't use this function. Called by the system only (on startup).
//                This function also unlocks all locked renderstates.
//
// -----------------------------------------------------------------------------
bool InvalidateCache()
{
	int nStateCount, nUnitCount;

	// Renderstates:

	for(nStateCount = 0; nStateCount < LAST_RENDERSTATE; nStateCount++)
	{
		_pContextData->_pnRenderStates      [nStateCount] = INVALID_STATE_VALUE;
		_pContextData->_pbLockedRenderStates[nStateCount] = false;
	}

	// Texturing states:

	for(nUnitCount = 0; nUnitCount < MAX_TEXTURINGUNITS; nUnitCount++)
	{
		for(nStateCount = 0; nStateCount < LAST_TEXTURINGUNITSTATE; nStateCount++)
		{
			_pContextData->_ppnTexturingUnitStates      [nUnitCount][nStateCount] = INVALID_STATE_VALUE;
			_pContextData->_ppbLockedTexturingUnitStates[nUnitCount][nStateCount] = false;
		}
	}

	return true;
}

// --[  Function  ]---------------------------------------------------------------
//
//  - prototype : ERenderState GetLight(unsigned uLight);
//
//  - Purpose   : Returns the enum value of a given light
//
// -----------------------------------------------------------------------------
ERenderState GetLight(unsigned uLight)
{
	assert(uLight < UtilGL::GetMaxLights());

	switch(uLight)
	{
	case 0:  return LIGHT0;
	case 1:  return LIGHT1;
	case 2:  return LIGHT2;
	case 3:  return LIGHT3;
	case 4:  return LIGHT4;
	case 5:  return LIGHT5;
	case 6:  return LIGHT6;

	default: break;
	}

	return LIGHT7;
}

// Color / Material methods

void  SetRedScale   (float fScale) { _pContextData->_fRedScale   = fScale; }
void  SetGreenScale (float fScale) { _pContextData->_fGreenScale = fScale; }
void  SetBlueScale  (float fScale) { _pContextData->_fBlueScale  = fScale; }
void  SetAlphaScale (float fScale) { _pContextData->_fAlphaScale = fScale; }
void  SetRedBias    (float fBias)  { _pContextData->_fRedBias    = fBias;  }
void  SetGreenBias  (float fBias)  { _pContextData->_fGreenBias  = fBias;  }
void  SetBlueBias   (float fBias)  { _pContextData->_fBlueBias   = fBias;  }
void  SetAlphaBias  (float fBias)  { _pContextData->_fAlphaBias  = fBias;  }
float GetRedScale   ()             { return _pContextData->_fRedScale;     }
float GetGreenScale ()             { return _pContextData->_fGreenScale;   }
float GetBlueScale  ()             { return _pContextData->_fBlueScale;    }
float GetAlphaScale ()             { return _pContextData->_fAlphaScale;   }
float GetRedBias    ()             { return _pContextData->_fRedBias;      }
float GetGreenBias  ()             { return _pContextData->_fGreenBias;    }
float GetBlueBias   ()             { return _pContextData->_fBlueBias;     }
float GetAlphaBias  ()             { return _pContextData->_fAlphaBias;    }

void SetColor(const CVector4& v4Color)
{
	glColor4f(	v4Color.X() * _pContextData->_fRedScale   + _pContextData->_fRedBias,
				v4Color.Y() * _pContextData->_fGreenScale + _pContextData->_fGreenBias,
				v4Color.Z() * _pContextData->_fBlueScale  + _pContextData->_fBlueBias,
				v4Color.W() * _pContextData->_fAlphaScale + _pContextData->_fAlphaBias);
}

void SetMaterialAmbient(GLenum face, const CVector4& v4Color)
{
	GLfloat v[4];

	v[0] = v4Color.X() * _pContextData->_fRedScale   + _pContextData->_fRedBias,
	v[1] = v4Color.Y() * _pContextData->_fGreenScale + _pContextData->_fGreenBias,
	v[2] = v4Color.Z() * _pContextData->_fBlueScale  + _pContextData->_fBlueBias,
	v[3] = v4Color.W() * _pContextData->_fAlphaScale + _pContextData->_fAlphaBias;

	glMaterialfv(face, GL_AMBIENT, v);
}

void SetMaterialDiffuse(GLenum face, const CVector4& v4Color)
{
	GLfloat v[4];

	v[0] = v4Color.X() * _pContextData->_fRedScale   + _pContextData->_fRedBias,
	v[1] = v4Color.Y() * _pContextData->_fGreenScale + _pContextData->_fGreenBias,
	v[2] = v4Color.Z() * _pContextData->_fBlueScale  + _pContextData->_fBlueBias,
	v[3] = v4Color.W() * _pContextData->_fAlphaScale + _pContextData->_fAlphaBias;

	glMaterialfv(face, GL_DIFFUSE, v);

}

void SetMaterialSpecular(GLenum face, const CVector4& v4Color)
{
	GLfloat v[4];

	v[0] = v4Color.X() * _pContextData->_fRedScale   + _pContextData->_fRedBias,
	v[1] = v4Color.Y() * _pContextData->_fGreenScale + _pContextData->_fGreenBias,
	v[2] = v4Color.Z() * _pContextData->_fBlueScale  + _pContextData->_fBlueBias,
	v[3] = v4Color.W() * _pContextData->_fAlphaScale + _pContextData->_fAlphaBias;

	glMaterialfv(face, GL_SPECULAR, v);
}


} // namespace States

} // namespace UtilGLL