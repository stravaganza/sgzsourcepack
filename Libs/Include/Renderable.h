// ==[ File ]===================================================================================
//
//  -Name     : Renderable.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CRenderable definition.
//
// =============================================================================================

/*

	02/04/2003  -  File created.


*/


#ifndef __RENDERABLE_H
#define __RENDERABLE_H


// ==[ Class definitions ]======================================================================

class CShader;

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CRenderable
//
//  - Purpose   : Base class for all drawable objects (processed by the
//                CRenderVisitor).
//                I decided to avoid a CRenderable::Render() method because
//                it would restrict some things (reusing shader states for
//                example, it would have required to exchange info between
//                the different Render() methods and I didn't like the idea).
//                I placed all render calls in the CRenderVisitor instead.
//                The main purpose of this class is to provide a generic way
//                to store different kind of renderable nodes and also
//                have a method to sort a vector of renderables through the
//                < operator.
//                There's also some functionality to add custom pre/post
//                render callbacks to a drawable node.
//
// ---------------------------------------------------------------------------
class CRenderable : public CNode
{


public:


	CRenderable()  { m_pPreRenderCallback = m_pPostRenderCallback = NULL;
					 m_pPreRenderData     = m_pPostRenderData     = NULL; }
	~CRenderable() { }

	virtual bool  IsPostRender()       const = 0;
	virtual bool  IsOpaque()           const = 0;
	virtual const CShader* GetShader() const = 0;

	virtual CVector3 GetWorldMassCenter() const { return GetAbsoluteTM().Position(); };

	typedef void (*RENDERCALLBACK) (CRenderable*, void*);

	void    SetPreRenderCallback (RENDERCALLBACK pCallback, void* pData) { m_pPreRenderCallback  = pCallback; m_pPreRenderData  = pData; }
	void    SetPostRenderCallback(RENDERCALLBACK pCallback, void* pData) { m_pPostRenderCallback = pCallback; m_pPostRenderData = pData; }

	void    ExecutePreRenderCallback()  { if(m_pPreRenderCallback)  m_pPreRenderCallback (this, m_pPreRenderData ); }
	void    ExecutePostRenderCallback() { if(m_pPostRenderCallback) m_pPostRenderCallback(this, m_pPostRenderData); }


private:


	RENDERCALLBACK m_pPreRenderCallback;
	RENDERCALLBACK m_pPostRenderCallback;
	void*          m_pPreRenderData;
	void*          m_pPostRenderData;

	CVector3       m_v3ViewPos;


public:


	CVector3 GetViewPos    () const { return m_v3ViewPos; }
	void	 ComputeViewPos(const CMatrix& mtxView);

	bool operator < (const CRenderable& renderable);

	// Same as operator < but for pointer vectors.

	static bool SortPredicate(CRenderable* p1, CRenderable* p2)
	{
		return *p1 < *p2;
	}
};


#endif