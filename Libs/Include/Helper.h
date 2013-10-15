// ==[ File ]===================================================================================
//
//  -Name     : Helper.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CHelper definition.
//
// =============================================================================================

/*

	29/11/2002  -  File created.


*/


#ifndef __HELPER_H
#define __HELPER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CHelper
//
//  - Purpose   : Helper node class. Helpers are dummy nodes, they can be
//                in the scene node tree but they are not rendered.
//
// ---------------------------------------------------------------------------
class CHelper : public CNode
{


public:


	CHelper();
	~CHelper();

	// Get

	int GetNodeType   () const { return ID_NODE_HELPER;  }
	int GetNodeSubType() const { return ID_HELPER_POINT; }

	// Visitors

	bool AcceptVisitor(CNodeVisitor* pNodeVisitor) { return pNodeVisitor->VisitHelper(this); }


protected:


	bool FreeNodeData ();
	bool ReadNodeData (CInput*  pInput);
	bool WriteNodeData(COutput* pOutput) const;

};


#endif