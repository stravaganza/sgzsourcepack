// ==[ File ]===================================================================================
//
//  -Name     : Renderable.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CRenderable ìmplementation.
//
// =============================================================================================

/*

	02/04/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderable
//  - prototype : void ComputeViewPos(const CMatrix& mtxView)
//
//  - Purpose   : Computes the center of the object in view space.
//
// -----------------------------------------------------------------------------
void CRenderable::ComputeViewPos(const CMatrix& mtxView)
{
	m_v3ViewPos = mtxView * GetWorldMassCenter();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CRenderable
//  - prototype : bool operator < (const CRenderable& renderable);
//
//  - Purpose   : Less than operator, used to sort renderables before render.
//				  First will be the opaques, which will be sorted first by shader
//				  and then from front to back. Then the translucent, which will be
//				  sorted from back to front in order to compute translucency properly.
//				  Then the postrender effects (lensflares f.e, which need the whole
//				  scene to be rendered first).
//
// -----------------------------------------------------------------------------
bool CRenderable::operator < (const CRenderable& renderable)
{
	if(this->IsPostRender())
	{
		return false;
	}
	else if(renderable.IsPostRender())
	{
		return true;
	}

	if(this->IsOpaque() && !renderable.IsOpaque())
	{
		// Opaque against translucent
		return true;
	}
	else if(!this->IsOpaque() && renderable.IsOpaque())
	{
		// Translucent against opaque
		return false;
	}

	// Both opaque?

	if(this->IsOpaque() && renderable.IsOpaque())
	{
		// Sort by shader, if both have the same shader sort by Z
		if(this->GetShader() == renderable.GetShader())
		{
			return m_v3ViewPos.Z() > renderable.m_v3ViewPos.Z();
		}
		else
		{
			return this->GetShader() < renderable.GetShader();
		}
	}

	// Both translucent
	return m_v3ViewPos.Z() < renderable.m_v3ViewPos.Z();
}
