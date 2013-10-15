// ==[ File ]===================================================================================
//
//  -Name     : Node.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CNode definition.
//
// =============================================================================================

/*

	10/11/2002  -  File created.
	26/03/2003  -  Merged CAnimatable methods


*/


#ifndef __NODE_H
#define __NODE_H


// ==[ Class definitions ]======================================================================


class CNodeVisitor;


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CNode
//
//  - Purpose   : Base class of all nodes in our scene trees.
//                Includes typing, naming, hierarchy, reading/writing,
//                visitors and animation methods.
//
// ---------------------------------------------------------------------------
class CNode
{


public:


	CNode();
	virtual ~CNode() { }

	// Type

	virtual int   GetNodeType          () const = 0;
	virtual int   GetNodeSubType       () const = 0;

	// Naming

	std::string   GetNodeName          ()                     const { return m_strName;            }
	void          SetNodeName          (const std::string& strName) { m_strName = strName;         }

	// User defined string

	std::string   GetUserDefinedString ()                     const { return m_strUserDefString;   }
	void          SetUserDefinedString (const std::string& string)  { m_strUserDefString = string; }

	// Hierarchy

	CNode*        GetParent            ()                     const { return m_pParent;            }
	int           GetNumChilds         ()                     const { return m_vecChilds.size();   }
	CNode*        GetChild             (unsigned n)           const;
	bool          AttachChild          (CNode*   pNode);
	bool          DetachFromParent     ();
	bool          AssignChildsToParent ();
	CNode*        FindNode             (const std::string& strName);
	void          WriteTopologyToLog   ();

	// Read / Write

	static CNode* ReadNode             (CInput*  pInput,  CNode* pParent);
	bool          WriteNode            (COutput* pOutput, bool bChilds = true);

	bool          StartNodeChunk       (CNode* pNode, COutput* pOutput, COutput** pNewOutput);
	bool          FinishNodeChunk      (bool bWriteChunkData);

	// Visitors

	virtual bool  AcceptVisitor        (CNodeVisitor* pNodeVisitor) = 0;

	// Animation

	CMatrix GetRelativeTM  ()                      const { return m_relTM;   }
	CMatrix GetAbsoluteTM  ()                      const { return m_absTM;   }
	void    SetRelativeTM  (const CMatrix& matrix)       { m_relTM = matrix; }
	void    SetAbsoluteTM  (const CMatrix& matrix)       { m_absTM = matrix; }

	bool    ComputeRelativeTM(float fTime, CMatrix* pMtxOut);
	bool    ComputeAbsoluteTM(float fTime, CMatrix* pMtxOut);

	void    GetInitialScl  (CQuaternion* pqAxesOut, CVector3* pv3SclOut);
	void    GetInitialPos  (CVector3*    pv3PosOut);
	void    GetInitialRot  (CQuaternion* pqRotOut);

	void    SetInitialScl  (const CQuaternion& qAxes, const CVector3& v3Scl);
	void    SetInitialPos  (const CVector3& v3Pos);
	void    SetInitialRot  (const CQuaternion& qRot);

	float   GetAnimEvalTime()             const { return m_fEvalTime; }
	bool    SetAnimEvalTime(float fTime, bool bComputeAbsoluteTM);
	bool    IsAnimated     () const;
	bool    DeleteAnimation();

	bool    ReadAnimation (CInput*  pInput);
	bool    WriteAnimation(COutput* pOutput) const;

	const CAnimScl* GetSclAnimation() const { return m_pSclAnimation; }
	const CAnimRot* GetRotAnimation() const { return m_pRotAnimation; }
	const CAnimPos* GetPosAnimation() const { return m_pPosAnimation; }

	bool SetSclAnimation(CAnimScl* pAnimScl);
	bool SetRotAnimation(CAnimRot* pAnimRot);
	bool SetPosAnimation(CAnimPos* pAnimPos);


protected:


	// Should be called by child destructors

	bool          DeleteNode           ();

	// Virtual read/write node data methods.

	virtual bool  FreeNodeData         ()                       = 0;
	virtual bool  ReadNodeData         (CInput*  pInput)        = 0;
	virtual bool  WriteNodeData        (COutput* pOutput) const = 0;


private:


	typedef std::vector<CNode*>::iterator Nodeitr;

	std::string m_strName, m_strUserDefString;

	CNode*               m_pParent;
	std::vector<CNode*>  m_vecChilds;

	struct SChunkData
	{
		SChunkData(CNode* pNode, COutput* pOldOutput, COutputFile* pNewOutputFile, const std::string& strFileName) :
		m_pNode(pNode), m_pOldOutput(pOldOutput), m_pNewOutputFile(pNewOutputFile), m_strFileName(strFileName) { }

		CNode*       m_pNode;
		COutput*     m_pOldOutput;
		COutputFile* m_pNewOutputFile;
		std::string  m_strFileName;
	};

	std::stack<SChunkData> m_stkOutputs;
	static unsigned s_uTmpFileCount;

	// Animation

	float       m_fEvalTime;

	CMatrix     m_relTM, m_absTM;

	CVector3    m_v3InitialPos, m_v3InitialScale;
	CQuaternion m_qInitialRot,  m_qInitialScaleAxes;

	CAnimScl*   m_pSclAnimation;
	CAnimRot*   m_pRotAnimation;
	CAnimPos*   m_pPosAnimation;
};


#endif