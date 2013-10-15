// ==[ File ]===================================================================================
//
//  -Name     : FX_Common.h
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Headers of common routines for effects.
//
// =============================================================================================

/*

	22/12/2004  -  File created from part of Effect.h


*/


#ifndef __FX_COMMON_H
#define __FX_COMMON_H


#include "VarColor.h"
#include "VarCombo.h"
#include "VarFloat.h"
#include "VarInt.h"
#include "VarSpeed.h"
#include "VarString.h"

#include "CurveSegmentSet.h"
#include "CurveSegmentSetLinear.h"
#include "CurveSegmentSetStep.h"
#include "CurveSpline.h"
#include "CurveSplineBezier.h"
#include "CurveSplineCatmullRom.h"
#include "CurveSplineHermite.h"
#include "CurveSplineTCB.h"

#include "FuncConstant.h"
#include "FuncFFT.h"
#include "FuncInvSawTooth.h"
#include "FuncQuad.h"
#include "FuncRandom.h"
#include "FuncSawTooth.h"
#include "FuncSine.h"
#include "FuncTriangle.h"

#include "ResourceScene.h"
#include "ResourceSpline.h"
#include "ResourceTexture2D.h"


bool DefineAddTransformVars   ( CEffect*  pEffect);
bool EvaluateAddTransformVars (	CEffect*  pEffect,      float fTime,
								CMatrix*  pMtxAddLocal, CMatrix*  pMtxAddWorld,
								CVector3* pv3AddCamPos, CVector3* pv3AddCamAngles);


#endif