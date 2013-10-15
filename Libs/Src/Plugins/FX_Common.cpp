// ==[ File ]===================================================================================
//
//  -Name     : FX_Common.cpp
//  -Proyect  : Plugins
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Common routines for effects.
//
// =============================================================================================

/*

	22/12/2004  -  File created from part of Effect.cpp


*/


// ==[ Headers ]================================================================================


#include "Plugins.h"


// ==[ Implementation ]=========================================================================


bool DefineAddTransformVars(CEffect* pEffect)
{
	pEffect->DefineVar("Local X Pos",    CVarFloat::CLASSNAME);
	pEffect->DefineVar("Local Y Pos",    CVarFloat::CLASSNAME);
	pEffect->DefineVar("Local Z Pos",    CVarFloat::CLASSNAME);
	pEffect->DefineVar("Local X Angle",  CVarFloat::CLASSNAME);
	pEffect->DefineVar("Local Y Angle",  CVarFloat::CLASSNAME);
	pEffect->DefineVar("Local Z Angle",  CVarFloat::CLASSNAME);

	pEffect->DefineVar("World X Pos",    CVarFloat::CLASSNAME);
	pEffect->DefineVar("World Y Pos",    CVarFloat::CLASSNAME);
	pEffect->DefineVar("World Z Pos",    CVarFloat::CLASSNAME);
	pEffect->DefineVar("World X Angle",  CVarFloat::CLASSNAME);
	pEffect->DefineVar("World Y Angle",  CVarFloat::CLASSNAME);
	pEffect->DefineVar("World Z Angle",  CVarFloat::CLASSNAME);

	pEffect->DefineVar("Camera X Pos",   CVarFloat::CLASSNAME);
	pEffect->DefineVar("Camera Y Pos",   CVarFloat::CLASSNAME);
	pEffect->DefineVar("Camera Z Pos",   CVarFloat::CLASSNAME);
	pEffect->DefineVar("Camera X Angle", CVarFloat::CLASSNAME);
	pEffect->DefineVar("Camera Y Angle", CVarFloat::CLASSNAME);
	pEffect->DefineVar("Camera Z Angle", CVarFloat::CLASSNAME);

	return true;
}

bool EvaluateAddTransformVars(	CEffect*  pEffect,      float fTime,
								CMatrix*  pMtxAddLocal, CMatrix*  pMtxAddWorld,
								CVector3* pv3AddCamPos, CVector3* pv3AddCamAngles)
{
	CVarFloat::CValueFloat valFloat;

	if(pMtxAddLocal)
	{
		pMtxAddLocal->SetIdentity();
		pEffect->EvaluateVar("Local X Angle",  fTime, &valFloat); pMtxAddLocal->Rotate(valFloat.GetValue(), 1.0f, 0.0f, 0.0f);
		pEffect->EvaluateVar("Local Y Angle",  fTime, &valFloat); pMtxAddLocal->Rotate(valFloat.GetValue(), 0.0f, 1.0f, 0.0f);
		pEffect->EvaluateVar("Local Z Angle",  fTime, &valFloat); pMtxAddLocal->Rotate(valFloat.GetValue(), 0.0f, 0.0f, 1.0f);
		pEffect->EvaluateVar("Local X Pos",    fTime, &valFloat); (*pMtxAddLocal)(0, 3) = valFloat.GetValue();
		pEffect->EvaluateVar("Local Y Pos",    fTime, &valFloat); (*pMtxAddLocal)(1, 3) = valFloat.GetValue();
		pEffect->EvaluateVar("Local Z Pos",    fTime, &valFloat); (*pMtxAddLocal)(2, 3) = valFloat.GetValue();
	}

	if(pMtxAddWorld)
	{
		pMtxAddWorld->SetIdentity();
		pEffect->EvaluateVar("World X Angle",  fTime, &valFloat); pMtxAddWorld->Rotate(valFloat.GetValue(), 1.0f, 0.0f, 0.0f);
		pEffect->EvaluateVar("World Y Angle",  fTime, &valFloat); pMtxAddWorld->Rotate(valFloat.GetValue(), 0.0f, 1.0f, 0.0f);
		pEffect->EvaluateVar("World Z Angle",  fTime, &valFloat); pMtxAddWorld->Rotate(valFloat.GetValue(), 0.0f, 0.0f, 1.0f);
		pEffect->EvaluateVar("World X Pos",    fTime, &valFloat); (*pMtxAddWorld)(0, 3) = valFloat.GetValue();
		pEffect->EvaluateVar("World Y Pos",    fTime, &valFloat); (*pMtxAddWorld)(1, 3) = valFloat.GetValue();
		pEffect->EvaluateVar("World Z Pos",    fTime, &valFloat); (*pMtxAddWorld)(2, 3) = valFloat.GetValue();
	}

	if(pv3AddCamAngles)
	{
		pEffect->EvaluateVar("Camera X Angle", fTime, &valFloat); pv3AddCamAngles->SetX(valFloat.GetValue());
		pEffect->EvaluateVar("Camera Y Angle", fTime, &valFloat); pv3AddCamAngles->SetY(valFloat.GetValue());
		pEffect->EvaluateVar("Camera Z Angle", fTime, &valFloat); pv3AddCamAngles->SetZ(valFloat.GetValue());
	}

	if(pv3AddCamPos)
	{
		pEffect->EvaluateVar("Camera X Pos",   fTime, &valFloat); pv3AddCamPos->SetX(valFloat.GetValue());
		pEffect->EvaluateVar("Camera Y Pos",   fTime, &valFloat); pv3AddCamPos->SetY(valFloat.GetValue());
		pEffect->EvaluateVar("Camera Z Pos",   fTime, &valFloat); pv3AddCamPos->SetZ(valFloat.GetValue());
	}

	return true;
}
