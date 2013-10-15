// ==[ File ]===================================================================================
//
//  -Name     : ParticleSystem.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CParticleSystem definition.
//
// =============================================================================================

/*

	01/03/2003  -  File created.

*/


#ifndef __PARTICLESYSTEM_H
#define __PARTICLESYSTEM_H


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CParticleSystem
//
//  - Purpose   : Base class for particle systems
//
// ---------------------------------------------------------------------------
class CParticleSystem : public CRenderable
{


public:


	CParticleSystem() { }
	virtual ~CParticleSystem() { }

	virtual bool InitSystem()              = 0;
	virtual int  GetNumParticles()   const = 0;
	virtual bool HasAliveParticles() const = 0;
	virtual bool SetEvalTime(float fTime)  = 0;
	virtual bool Render()                  = 0;

	virtual bool LoadResources(	const std::string& strDatfile   = "",
								const std::string& strTexFolder = "",
								UtilGL::Texturing::STexLoadOptions* pTexOptions = NULL) = 0;
};


#endif