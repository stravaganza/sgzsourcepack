// ==[ File ]===================================================================================
//
//  -Name     : IDs.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Lots of tokens, flags... :)
//
// =============================================================================================

/*

	10/11/2002  -  File created.


*/


#ifndef __IDS_H
#define __IDS_H


enum
{
	// General

	ID_CHUNKEND = 1000,

	// Nodes

	ID_NODE_EMPTY,
	ID_NODE_SCENE,
	ID_NODE_OBJECT,
	ID_NODE_CAMERA,
	ID_NODE_LIGHT,
	ID_NODE_HELPER,
	ID_NODE_LENSFLARE,
	ID_NODE_PARTICLESYSTEM,

	// Animation

	ID_ANIM_CHUNKSTART,
	ID_ANIM_CHUNKPOS,
	ID_ANIM_CHUNKROT,
	ID_ANIM_CHUNKSCL,
	ID_ANIM_POSSAMPLED,
	ID_ANIM_ROTSAMPLED,
	ID_ANIM_SCLSAMPLED,

	// Scene

	ID_SCENE_CHUNKSTART,
	ID_SCENE_STANDARD,

	// Object

	ID_OBJECT_STATIC,
	ID_OBJECT_DYNAMIC,
	ID_OBJECT_MESHCHUNKSTART,
	ID_OBJECT_MESHCHUNKEND,

	// Camera

	ID_CAMERA_STANDARD,
	ID_CAMERA_SETTINGSCHUNKSTART,
	ID_CAMERA_SETTINGSCHUNKEND,

	// Light

	ID_LIGHT_OMNI,
	ID_LIGHT_SPOT,
	ID_LIGHT_DIRECTIONAL,
	ID_LIGHT_SETTINGSCHUNKSTART,
	ID_LIGHT_SETTINGSCHUNKEND,

	// Helper

	ID_HELPER_POINT,

	// Lensflare

	ID_LENSFLARE_STANDARD,

	// Particle system

	ID_PARTICLESYSTEM_STANDARD,

	// Fogs

	ID_FOG_NONE,
	ID_FOG_LINEAR,
	ID_FOG_EXP,
	ID_FOG_EXP2

};


#endif
