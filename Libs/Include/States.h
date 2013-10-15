// ==[ File ]===================================================================================
//
//  -Name     : States.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::States namespace. Render state utilities.
//
// =============================================================================================

/*

	28/01/2002  -  File created.


*/


#ifndef __STATES_H
#define __STATES_H


namespace UtilGL
{

namespace States
{


// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : ERenderState
//
//  - Purpose   : Enumeration of all possible renderstates we can handle.
//
// ---------------------------------------------------------------------------
enum ERenderState
{
	BFCULLING = 0,
	CULLSIDE,
	FACEORIENTATION,
	SHADEMODE,
	POLYGONMODE,
	BLENDING,
	FOG,
	ZBUFFER,
	ZBUFFERWRITE,
	ZFUNC,
	STENCILBUFFER,
	READBUFFER,
	DRAWBUFFER,
	ALPHATEST,
	LIGHTING,
	LIGHT0,
	LIGHT1,
	LIGHT2,
	LIGHT3,
	LIGHT4,
	LIGHT5,
	LIGHT6,
	LIGHT7,
	SRCBLEND,
	DSTBLEND,

	LAST_RENDERSTATE
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : ETexturingUnitState
//
//  - Purpose   : Enumeration of all possible texturing unit states we can
//                handle.
//                Due to the fact that in a multitexture capable hardware
//                each texture unit has its own state, we must store states
//                for each unit to have a properly working cache.
//                I could have made a unique enumeration including renderstates
//                and texture states, but it wouldn't be so clean.
//                What we do is overload the Get() and Set() methods to handle
//                both kind of enumerations, and use one cache or another.
//
// ---------------------------------------------------------------------------
enum ETexturingUnitState
{
	TEXTURE1D = 0,
	TEXTURE2D,
	TEXTURE3D,
	TEXTURECUBEMAP, // GL_ARB_texture_cube_map
	GENU,
	GENV,
	GENQ,
	GENR,
	UGENFUNC,
	VGENFUNC,
	QGENFUNC,
	RGENFUNC,

	LAST_TEXTURINGUNITSTATE
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : EBoolState
//
//  - Purpose   : Enabled/Disabled boolean renderstate value.
//
// ---------------------------------------------------------------------------
enum EBoolState
{
	DISABLED = 0,
	ENABLED  = 1
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : ECullSide
//
//  - Purpose   : Possible states of CULLSIDE.
//
// ---------------------------------------------------------------------------
enum ECullSide
{
	CULLSIDE_FRONT = 0,
	CULLSIDE_BACK,
	CULLSIDE_FRONTANDBACK
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : ECullSide
//
//  - Purpose   : Possible states of FACEORIENTATION
//
// ---------------------------------------------------------------------------
enum EFaceOrientation
{
	FACEORIENTATION_CW = 0,
	FACEORIENTATION_CCW
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : ECullSide
//
//  - Purpose   : Possible states of SHADEMODE
//
// ---------------------------------------------------------------------------
enum EShadeMode
{
	SHADEMODE_FLAT = 0,
	SHADEMODE_GOURAUD
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : EPolygonMode
//
//  - Purpose   : Possible states of POLYGONMODE.
//
// ---------------------------------------------------------------------------
enum EPolygonMode
{
	POLYGONMODE_POINT = 0,
	POLYGONMODE_LINE,
	POLYGONMODE_FILL
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : EBlendFunc
//
//  - Purpose   : Possible states of SRCBLEND / DSTBLEND.
//
// ---------------------------------------------------------------------------
enum EBlendFunc
{
	BLEND_ZERO = 0,
	BLEND_ONE,
	BLEND_SRCCOLOR,
	BLEND_INVSRCCOLOR,
	BLEND_DSTCOLOR,
	BLEND_INVDSTCOLOR,
	BLEND_SRCALPHA,
	BLEND_INVSRCALPHA,
	BLEND_DSTALPHA,
	BLEND_INVDSTALPHA,
	BLEND_SRCALPHASATURATE
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : EBlendFunc
//
//  - Purpose   : Possible states of ALPHAFUNC / ZFUNC.
//
// ---------------------------------------------------------------------------
enum ECmpFunc
{
	CMP_NEVER = 0,
	CMP_LESS,
	CMP_EQUAL,
	CMP_LESSEQUAL,
	CMP_GREATER,
	CMP_NOTEQUAL,
	CMP_GREATEREQUAL,
	CMP_ALWAYS
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : ETexGenFunc
//
//  - Purpose   : Possible states of GENU / GENV.
//
// ---------------------------------------------------------------------------
enum ETexGenFunc
{
	TEXGEN_LOCALLINEAR = 0,
	TEXGEN_VIEWLINEAR,
	TEXGEN_SPHEREMAP,
	TEXGEN_NORMALMAP,     // GL_ARB_texture_cube_map
	TEXGEN_REFLECTIONMAP, // GL_ARB_texture_cube_map
};

// --[ Enumeration ]----------------------------------------------------------
//
//  - Name      : EColorBufferType
//
//  - Purpose   : Possible states of READBUFFER / DRAWBUFFER.
//
// ---------------------------------------------------------------------------
enum EColorBufferType
{
	COLORBUFFER_NONE = 0,
	COLORBUFFER_FRONT,
	COLORBUFFER_BACK,
	COLORBUFFER_FRONTANDBACK
};



// ==[ Functions  ]=============================================================================


bool SetDefaults();

int  Get (ERenderState        eState);
int  Get (ETexturingUnitState eState);
bool Set (ERenderState        eState, int nValue);
bool Set (ETexturingUnitState eState, int nValue);

bool Lock(ERenderState        eState, bool bLockState);
bool Lock(ETexturingUnitState eState, bool bLockState);

bool InvalidateCache(); // System only.

ERenderState GetLight(unsigned uLight); // Help when iterating over lights

void  SetRedScale   (float fScale);
void  SetGreenScale (float fScale);
void  SetBlueScale  (float fScale);
void  SetAlphaScale (float fScale);
void  SetRedBias    (float fBias);
void  SetGreenBias  (float fBias);
void  SetBlueBias   (float fBias);
void  SetAlphaBias  (float fBias);
float GetRedScale   ();
float GetGreenScale ();
float GetBlueScale  ();
float GetAlphaScale ();
float GetRedBias    ();
float GetGreenBias  ();
float GetBlueBias   ();
float GetAlphaBias  ();

void  SetColor           (const CVector4& v4Color);
void  SetMaterialAmbient (GLenum face, const CVector4& v4Color);
void  SetMaterialDiffuse (GLenum face, const CVector4& v4Color);
void  SetMaterialSpecular(GLenum face, const CVector4& v4Color);


} // namespace States

} // namespace UtilGL


#endif

