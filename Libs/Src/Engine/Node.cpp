// ==[ File ]===================================================================================
//
//  -Name     : Node.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CNode ìmplementation.
//
// =============================================================================================

/*

	16/11/2002  -  File created.
	26/04/2003  -  Merged CAnimatable functions


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


unsigned CNode::s_uTmpFileCount = 0;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : CNode()
//
//  - Purpose   : CNode's default constructor.
//
// -----------------------------------------------------------------------------
CNode::CNode()
{
	m_pParent = NULL;
	m_strName = "CNode default name";
	m_strUserDefString = "";

	m_fEvalTime = 0.0f;

	m_v3InitialScale.Set(1.0f, 1.0f, 1.0f);

	m_pSclAnimation = NULL;
	m_pRotAnimation = NULL;
	m_pPosAnimation = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool DeleteNode()
//
//  - Purpose   : Deletes all hierarchy below and including the caller's node.
//
// -----------------------------------------------------------------------------
bool CNode::DeleteNode()
{
	unsigned uError = 0;
	Nodeitr it;

	// Deallocate data

	LOG.Write("\nDeleting %s", GetNodeName().data());

	if(!FreeNodeData())
	{
		uError++;
	}

	if(!DeleteAnimation())
	{
		uError++;
	}
/*
	if(m_pParent)
	{
		DetachFromParent();
	}
*/
	// Recurse into children if requested

	for(it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		LOG.IncIndentation(2);
		assert(*it);
		delete *it;
		LOG.DecIndentation(2);
	}

	m_vecChilds.clear();

	m_strName = "";
	m_pParent = NULL;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : CNode* GetChild(unsigned n) const;
//
//  - Purpose   : Returns the n-th child or NULL if n is an invalid index.
//
// -----------------------------------------------------------------------------
CNode* CNode::GetChild(unsigned n) const
{
	if(n >= 0 && n < m_vecChilds.size())
	{
		return m_vecChilds[n];
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool AttachChild(CNode* pNode);
//
//  - Purpose   : Attaches a node, making it child from the callers object.
//
// -----------------------------------------------------------------------------
bool CNode::AttachChild(CNode* pNode)
{
	assert(pNode);

#ifdef _DEBUG

	Nodeitr it;

	// In debug mode, check for attaching a node to itself

	if(pNode == this)
	{
		LOG.Write("\nWARNING - CNode::AttachChild(): Node %s cannot be attached to itself.", GetNodeName().data());
		return false;
	}

	// In debug mode, check if already present:

	for(it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		assert(*it);
		if((*it) == pNode)
		{
			LOG.Write("\nWARNING - CNode::AttachChild(): Node %s has already child %s (%s) %u (%u).", GetNodeName().data(), pNode->GetNodeName().data(), (*it)->GetNodeName().data(), pNode, *it);
			return false;
		}
	}

#endif

	m_vecChilds.push_back(pNode);
	pNode->m_pParent = this;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool DetachFromParent()
//
//  - Purpose   : Detaches the node from its parent.
//
// -----------------------------------------------------------------------------
bool CNode::DetachFromParent()
{
	CNode* pParent = GetParent();

	if(pParent == NULL)
	{
		return false;
	}

	// Look for the node in the parent's child node list, and delete it.

	Nodeitr it;

	for(it = pParent->m_vecChilds.begin(); it != pParent->m_vecChilds.end(); ++it)
	{
		if(*it == this)
		{
			pParent->m_vecChilds.erase(it);
			return true;
		}
	}

	// Not found:

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool AssignChildsToParent()
//
//  - Purpose   : Assigns all childs to the parent node. This can usually be
//                the step before deleting a node to preserve the hierarchy.
//
// -----------------------------------------------------------------------------
bool CNode::AssignChildsToParent()
{
	if(m_pParent == NULL)
	{
		return false;
	}

	Nodeitr it;

	for(it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		(*it)->m_pParent = m_pParent;
		m_pParent->AttachChild(*it);
	}

	m_vecChilds.clear();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : CNode* FindNode(const std::string& strName)
//
//  - Purpose   : Searches in the node and its subhierarchy for a node whose
//                name matches the parameter. Returns a pointer to the node
//                found or NULL if no match was found.
//
// -----------------------------------------------------------------------------
CNode* CNode::FindNode(const std::string& strName)
{
	assert(m_strName.data());
	assert(strName.data());

	if(!stricmp(m_strName.c_str(), strName.c_str()))
	{
		return this;
	}

	// Recurse into children:

	Nodeitr it;

	for(it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		CNode* pFound = (*it)->FindNode(strName);

		if(pFound)
		{
			return pFound;
		}
	}

	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void WriteTopologyToLog()
//
//  - Purpose   : Writes the hierarchy tree topology in the log file.
//
// -----------------------------------------------------------------------------
void CNode::WriteTopologyToLog()
{
	LOG.Write("\nNode \"%s\"", GetNodeName().data());

	std::vector<CNode*>::iterator it;

	for(it = m_vecChilds.begin(); it != m_vecChilds.end(); ++it)
	{
		assert(*it);
		LOG.IncIndentation(2);
		(*it)->WriteTopologyToLog();
		LOG.DecIndentation(2);
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : static CNode* ReadNode(CInput* pInput, CNode* pParent)
//
//  - Purpose   : Reads a node (including its hierarchy if available) from
//                a data source.
//                Returns a pointer to the root node.
//
//  - Note      : A normal call would have pParent as NULL. Non-NULL values
//                would attach the read nodes to that parent.
//
// -----------------------------------------------------------------------------
CNode* CNode::ReadNode(CInput* pInput, CNode* pParent)
{
	assert(pInput);
	assert(pInput->Ready());

	int nError     = 0, nReadCur, nID = 0, nChilds = 0, nEndChunk = 0;
	int nChunkSize = 2; // chunk start + end signatures.

	// 'Push' read cursor:

	nReadCur = pInput->GetPosition();

	// Read common node data:

	std::string strName;
	std::string strUserProp;

	if(!pInput->ReadInt(&nID))                nError++;
	if(!pInput->ReadInt(&nChunkSize))         nError++;
	if(!pInput->ReadStringZero(&strName))     nError++;
	if(!pInput->ReadStringZero(&strUserProp)) nError++;
	if(!pInput->ReadInt(&nChilds))            nError++;

	CNode* pNode = NULL;

	if(!nError)
	{
		// Instance correct node:

		switch(nID)
		{

		case ID_NODE_SCENE:          pNode = new CScene;             assert(pNode); break;
		case ID_NODE_OBJECT:         pNode = new CObject(true);      assert(pNode); break;
		case ID_NODE_CAMERA:         pNode = new CCamera;            assert(pNode); break;
		case ID_NODE_LIGHT:          pNode = new CLight;             assert(pNode); break;
		case ID_NODE_HELPER:         pNode = new CHelper;            assert(pNode); break;
		case ID_NODE_LENSFLARE:      pNode = new CLensFlare;         assert(pNode); break;
		case ID_NODE_PARTICLESYSTEM:

			int nSubType;
			pInput->ReadInt(&nSubType);

			switch(nSubType)
			{

			case ID_PARTICLESYSTEM_STANDARD: pNode = new CParticleSystemStd; assert(pNode); break;
			default:

				LOG.Write("\nERROR - CNode::ReadNode() of %s: Unknown particle system ID.", strName.data());
				break;
			}

			break;

		case ID_NODE_EMPTY:

			nError++;
			LOG.Write("\nWARNING - Empty node found.");
			break;

		default:

			LOG.Write("\nERROR - CNode::ReadNode() of %s: Unknown node ID.", strName.data());
			nError++;
		}

		// Read specific data:

		if(!nError)
		{
			LOG.Write("\nReading node %s", strName.data());

			pNode->SetNodeName(strName);
			pNode->SetUserDefinedString(strUserProp);

			if(!pNode->ReadNodeData(pInput))
			{
				CLogger::NotifyWindow("CNode::ReadNode() of %s: Error calling CNode::ReadData().", strName.data());
				nError++;
			}

			pInput->ReadInt(&nEndChunk);

			if(nEndChunk != ID_CHUNKEND)
			{
				CLogger::NotifyWindow("CNode::ReadNode() of %s: Chunk end not found.", strName.data());
				nError++;
			}
		}
	}

	// Jump over node

	if(!pInput->SetPosition(nReadCur + nChunkSize, CInput::SEEKFROM_START))
	{
		nError++;
	}

	// Handle errors:

	if(nError)
	{
		// Childs must have a parent anyways:

		if(pNode)
		{
			delete pNode;
		}

		LOG.Write("\nWARNING - There was an error reading node %s, creating dummy.", strName.data());
		pNode = new CHelper;
		assert(pNode);
		pNode->SetNodeName(std::string("Dummy node for") + strName + ")");
	}

	// Read child nodes:

	for(int i = 0; i < nChilds; i++)
	{
		LOG.IncIndentation(2);
		CNode* pChild = ReadNode(pInput, pNode);
		LOG.DecIndentation(2);

		if(pChild)
		{
			pNode->AttachChild(pChild);
		}
	}

	return pNode;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool WriteNode(COutput* pOutput, bool bChilds = true)
//
//  - Purpose   : Writes a node to a data output object (file, memory stream...).
//                Subhierarchy is written if bChilds is true.
//                Returns true if succesfull or false if an error occured.
//
// -----------------------------------------------------------------------------
bool CNode::WriteNode(COutput* pOutput, bool bChilds)
{
	assert(pOutput);
	assert(pOutput->Ready());

	LOG.Write("\nWriting %s", GetNodeName().data());

	int nError = 0;

	// Start chunk and get it's new output handle

	COutput* pNewOutput = NULL;

	if(!StartNodeChunk(this, pOutput, &pNewOutput))
	{
		LOG.Write("\nERROR - CNode::WriteNode(). Error creating chunk for node %s", GetNodeName().data());
		nError++;
	}

	// Write particular node data

	if(!nError)
	{
		if(!WriteNodeData(pNewOutput))
		{
			LOG.Write("\nERROR - CNode::WriteNode(). Error writing data for node %s", GetNodeName().data());
			nError++;
		}
	}

	// Write chunk

	if(!FinishNodeChunk(!nError ? true : false))
	{
		LOG.Write("\nERROR - CNode::WriteNode(). Error finishing chunk for node %s", GetNodeName().data());
		nError++;
	}

	if(pNewOutput)
	{
		delete pNewOutput;
	}

	// Output hierarchy if we have to

	if(bChilds)
	{
		for(int i = 0; i < m_vecChilds.size(); i++)
		{
			CNode* pChild = m_vecChilds[i];
			assert(pChild);

			LOG.IncIndentation(2);

			if(!pChild->WriteNode(pOutput, true))
			{
				nError++;
			}

			LOG.DecIndentation(2);
		}
	}

	return nError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool StartNodeChunk(CNode*    pNode,
//                                    COutput&  output,
//                                    COutput** ppNewOutput)
//
//  - Purpose   : Creates a new temporal file to start a node chunk and redirects
//                writing to it. File output will be handled through the
//                handle assigned to ppNewOutput.
//                When FinishNodeChunk() is called, it writes to output
//                all data before that call and after the last StartNodeChunk().
//                It will not only write the chunk data, but also some
//                parameters like the chunk size, node name, node child count...
//
//  - Note      : See StartNodeChunk() in action on CNode::WriteNode().
//                StartNodeChunk()/FinishChunk() utility is mainly to know how
//                big (in bytes) a chunk is, so we can write this value as a
//                parameter of the chunk itself.
//                When we have an error reading a node, we simple can skip
//                it jumping over it using this size value.
//                Some other lower level (not node) chunk marks are only used
//                to check if the cursor is positioned correctly.
//
// -----------------------------------------------------------------------------
bool CNode::StartNodeChunk(CNode* pNode, COutput* pOutput, COutput** ppNewOutput)
{
	assert(pNode);
	assert(pOutput);
	assert(ppNewOutput);

	COutputFile *pNewOutputFile = new COutputFile;

	char szTempPath[MAX_PATH];
	char szTempFile[MAX_PATH];

	// Get user temp path

	int nChars = GetTempPath(MAX_PATH, szTempPath);

	// Create temporal file

	if(szTempPath[nChars - 1] != '\\')
	{
		strcat(szTempPath, "\\");
	}

	sprintf(szTempFile, "sgztemp%u", s_uTmpFileCount);
	strcat(szTempPath, szTempFile);

	s_uTmpFileCount++;

	if(!pNewOutputFile->Open(szTempPath, false, false))
	{
		LOG.Write("\nERROR - CNode::StartNodeChunk() : Couldn't open %s", szTempPath);
		return false;
	}

	// Push a new chunk register

	SChunkData newChunk(pNode, pOutput, pNewOutputFile, szTempPath);
	m_stkOutputs.push(newChunk);

	*ppNewOutput = pNewOutputFile;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool FinishNodeChunk(bool bWriteChunkData)
//
//  - Purpose   : Writes the last node chunk created by StartNodeChunk() to the
//                previous output. Also writes chunk node parameters.
//                If bWriteChunkData is true, the chunk data stored in a
//                temporal file will be written. If it is false, only the chunk
//                header is written, making it an empty chunk.
//
//  - Note      : See FinishNodeChunk() in action on CNode::WriteNode().
//                Read StartNodeChunk()'s description to know more about this
//                method.
//
// -----------------------------------------------------------------------------
bool CNode::FinishNodeChunk(bool bWriteChunkData)
{
	assert(!m_stkOutputs.empty());

	unsigned uError = 0;

	// Get last chunk register

	SChunkData lastChunk = m_stkOutputs.top();
	m_stkOutputs.pop();

	// Close chunk file

	if(!lastChunk.m_pNewOutputFile->Close())
	{
		LOG.Write("\nERROR - CNode::FinishNodeChunk(): Error closing chunk file (write).");
		uError++;
	}

	// Get Size (final chunk size includes file size plus some parameters)

	int   nChunkSize = sizeof(int) * 4 + (lastChunk.m_pNode->GetNodeName().size() + 1) + (lastChunk.m_pNode->GetUserDefinedString().size() + 1);
	int   nFileSize;
	char* pcBuffer = NULL;
	CInputFile inputFile;

	if(bWriteChunkData)
	{
		if(!inputFile.Open(lastChunk.m_strFileName, false))
		{
			LOG.Write("\nERROR - CNode::FinishNodeChunk(): Error opening chunk file.");
			uError++;
		}

		nFileSize  = inputFile.GetLength();
		nChunkSize += nFileSize;

		// Read chunk

		pcBuffer = new char[nFileSize];
		assert(pcBuffer);

		if(!inputFile.ReadRawData(nFileSize, 1, pcBuffer))
		{
			LOG.Write("\nERROR - CNode::FinishNodeChunk(): Error reading chunk data.");
			uError++;
		}
			
		if(!inputFile.Close())
		{
			LOG.Write("\nERROR - CNode::FinishNodeChunk(): Error closing chunk file (read).");
			uError++;
		}
	}

	if(!DeleteFile(lastChunk.m_strFileName.data()))
	{
		LOG.Write("\nWARNING - CNode::FinishNodeChunk(): Error deleting chunk file.");
	}

	// Write chunk parameters plus chunk data on previous output

	assert(lastChunk.m_pOldOutput);
	assert(lastChunk.m_pOldOutput->Ready());

	if(!lastChunk.m_pOldOutput->WriteInt(lastChunk.m_pNode->GetNodeType()))                 uError++; // Node Type
	if(!lastChunk.m_pOldOutput->WriteInt(nChunkSize))                                       uError++; // Chunk Size
	if(!lastChunk.m_pOldOutput->WriteStringZero(lastChunk.m_pNode->GetNodeName()))          uError++; // Node name
	if(!lastChunk.m_pOldOutput->WriteStringZero(lastChunk.m_pNode->GetUserDefinedString())) uError++; // Node user defined properties buffer
	if(!lastChunk.m_pOldOutput->WriteInt(lastChunk.m_pNode->GetNumChilds()))                uError++; // Node child count

	// Temporal file content (Node data).

	if(bWriteChunkData)
	{
		if(!lastChunk.m_pOldOutput->WriteRawData(nFileSize, 1, pcBuffer))
		{
			LOG.Write("\nERROR - CNode::FinishNodeChunk(): Error copying chunk data.");
			uError++;
		}

		SAFE_DELETE_A(pcBuffer);
	}

	if(!lastChunk.m_pOldOutput->WriteInt(ID_CHUNKEND)) uError++; // Chunk end signature

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool ComputeRelativeTM(float fTime, CMatrix *pMtxOut)
//
//  - Purpose   : Computes the relative transformation matrix for the given time.
//                (doesn't include parent transform)
//                Returns false if there's no animation data for the given time
//                (*pMtxOut will be the static matrix).
//
//  - Note      : Use CTransformVisitor when transforming whole node hierarchies.
//
// -----------------------------------------------------------------------------
bool CNode::ComputeRelativeTM(float fTime, CMatrix *pMtxOut)
{
	assert(pMtxOut);

	CVector3    v3Pos   (m_v3InitialPos);
	CVector3    v3Scale (m_v3InitialScale);
	CQuaternion qRot    (m_qInitialRot);
	CQuaternion qSclAxes(m_qInitialScaleAxes);

	bool bPos = false, bScl = false, bRot = false;

	// Get transformation values

	if(m_pSclAnimation)
	{
		if(m_pSclAnimation->HasData())
		{
			if(m_pSclAnimation->Evaluate(fTime, &qSclAxes, &v3Scale))
			{
				bScl = true;
			}
		}
	}

	if(m_pRotAnimation)
	{
		if(m_pRotAnimation->HasData())
		{
			if(m_pRotAnimation->Evaluate(fTime, &qRot))
			{
				bRot = true;
			}
		}
	}

	if(m_pPosAnimation)
	{
		if(m_pPosAnimation->HasData())
		{
			if(m_pPosAnimation->Evaluate(fTime, &v3Pos))
			{
				bPos = true;
			}
		}
	}

	// Compute transformation matrix:
	// Trl(pos) * Rot(rot) * Rot(sclAxes) * Scl(scale) * Rot(sclAxes.inv) = TM

	*pMtxOut = qSclAxes.Matrix().Inverse();
	pMtxOut->Scale(v3Scale.X(), v3Scale.Y(), v3Scale.Z());
	*pMtxOut = qSclAxes.Matrix() * *pMtxOut;

	*pMtxOut = qRot.Matrix() * *pMtxOut ;

	pMtxOut->Translate(v3Pos.X(), v3Pos.Y(), v3Pos.Z());

	return bPos || bScl || bRot;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool ComputeAbsoluteTM(float fTime, CMatrix* pMtxOut)
//
//  - Purpose   : Computes the absolute transformation matrix for the given time.
//                (includes parent transform)
//                Returns true if at least one node in the parental hierarchy
//                had animation data for the given time.
//
//  - Note      : Use CTransformVisitor when transforming whole node hierarchies.
//
// -----------------------------------------------------------------------------
bool CNode::ComputeAbsoluteTM(float fTime, CMatrix* pMtxOut)
{
	bool   bOk   = false;
	CNode* pNode = this;

	pMtxOut->SetIdentity();

	while(pNode)
	{
		CMatrix mtxRelative;
		bOk = bOk || pNode->ComputeRelativeTM(fTime, &mtxRelative);
		*pMtxOut = mtxRelative * *pMtxOut;
		pNode    = pNode->GetParent();
	}

	return bOk;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void GetInitialScl(CQuaternion* pqAxesOut, CVector3* pv3SclOut)
//
//  - Purpose   : Gets the initial scale value of the animation.
//
// -----------------------------------------------------------------------------
void CNode::GetInitialScl(CQuaternion* pqAxesOut, CVector3* pv3SclOut)
{
	assert(pqAxesOut);
	assert(pv3SclOut);

	*pqAxesOut = m_qInitialScaleAxes;
	*pv3SclOut = m_v3InitialScale;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void GetInitialPos(CVector3* pv3PosOut)
//
//  - Purpose   : Gets the initial position value of the animation.
//
// -----------------------------------------------------------------------------
void CNode::GetInitialPos(CVector3* pv3PosOut)
{
	assert(pv3PosOut);

	*pv3PosOut = m_v3InitialPos;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void GetInitialRot(CQuaternion* pqRotOut)
//
//  - Purpose   : Gets the initial rotation value of the animation.
//
// -----------------------------------------------------------------------------
void CNode::GetInitialRot(CQuaternion* pqRotOut)
{
	assert(pqRotOut);

	*pqRotOut = m_qInitialRot;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void SetInitialScl(const CQuaternion& qAxes, const CVector3& v3Scl)
//
//  - Purpose   : Sets the initial scale value of the animation.
//
// -----------------------------------------------------------------------------
void CNode::SetInitialScl(const CQuaternion& qAxes, const CVector3& v3Scl)
{
	m_qInitialScaleAxes = qAxes;
	m_v3InitialScale    = v3Scl;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void SetInitialPos(const CVector3& v3Pos)
//
//  - Purpose   : Sets the initial position value of the animation.
//
// -----------------------------------------------------------------------------
void CNode::SetInitialPos(const CVector3& v3Pos)
{
	m_v3InitialPos = v3Pos;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : void SetInitialRot(const CQuaternion& qRot)
//
//  - Purpose   : Sets the initial rotation value of the animation.
//
// -----------------------------------------------------------------------------
void CNode::SetInitialRot(const CQuaternion& qRot)
{
	m_qInitialRot = qRot;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool SetAnimEvalTime(float fTime, bool bComputeAbsoluteTM)
//
//  - Purpose   : Evaluates the object animation to update its relative and
//                absolute (optionally) transformation matrices.
//                Returns true if succesfull, false if for the given time there's
//                no scl, rot or translation animation data.
//
// -----------------------------------------------------------------------------
bool CNode::SetAnimEvalTime(float fTime, bool bComputeAbsoluteTM)
{
	bool bOk = false;

	bOk = bOk || ComputeRelativeTM(fTime, &m_relTM);

	if(bComputeAbsoluteTM)
	{
		bOk = bOk && ComputeAbsoluteTM(fTime, &m_absTM);
	}

	if(bOk)
	{
		m_fEvalTime = fTime;
	}

	return bOk;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool IsAnimated() const
//
//  - Purpose   : Tells if there's animation data present.
//
// -----------------------------------------------------------------------------
bool CNode::IsAnimated() const
{
	if(m_pSclAnimation)	if(m_pSclAnimation->HasData()) return true;
	if(m_pRotAnimation)	if(m_pRotAnimation->HasData()) return true;
	if(m_pPosAnimation)	if(m_pPosAnimation->HasData()) return true;

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool DeleteAnimation()
//
//  - Purpose   : Deletes the animation data.
//
// -----------------------------------------------------------------------------
bool CNode::DeleteAnimation()
{
	SAFE_DELETE(m_pSclAnimation);
	SAFE_DELETE(m_pRotAnimation);
	SAFE_DELETE(m_pPosAnimation);

	return true;
}
// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool ReadAnimation(CInput* pInput)
//
//  - Purpose   : Reads the animation data of an already opened source.
//                Cursor is supposed to be right after a ID_ANIM_CHUNKSTART
//                value.
//                Returns true if succesful, false if not. If succesful, cursor
//                is placed right after the chunk, otherwise its position is
//                unpredictable.
//
// -----------------------------------------------------------------------------
bool CNode::ReadAnimation(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	bool bContinue = true;
	int  nChunkStart, nID, nSubID;

	if(!pInput->ReadInt(&nChunkStart))    return false;
	if(nChunkStart != ID_ANIM_CHUNKSTART) return false;

	// Static transformation matrix

	if(!m_relTM.Read(pInput)) return false;

	m_absTM = m_relTM;

	// Initial transformation values

	if(m_v3InitialPos.Read(pInput)      == false) return false;
	if(m_v3InitialScale.Read(pInput)    == false) return false;
	if(m_qInitialScaleAxes.Read(pInput) == false) return false;
	if(m_qInitialRot.Read(pInput)       == false) return false;

	LOG.IncIndentation(2);

	while(bContinue)
	{
		// Animation type:

		if(!pInput->ReadInt(&nID))	    return false;
		if(nID == ID_CHUNKEND)          break;
		if(!pInput->ReadInt(&nSubID))	return false;

		// Instance and read depending on the subtype:

		CAnimation* pAnimation = CAnimation::GetNewInstance(nSubID);
		assert(pAnimation);
		if(!pAnimation->Read(pInput))
		{
			SAFE_DELETE(pAnimation);
			return false;
		}

		// Do proper casting.

		switch(nID)
		{
			
		case ID_ANIM_CHUNKPOS: m_pPosAnimation = reinterpret_cast<CAnimPos*>(pAnimation); break;
		case ID_ANIM_CHUNKROT: m_pRotAnimation = reinterpret_cast<CAnimRot*>(pAnimation); break;
		case ID_ANIM_CHUNKSCL: m_pSclAnimation = reinterpret_cast<CAnimScl*>(pAnimation); break;

		default:

			LOG.Write("\nCNode::ReadAnimation(). Unknown animation chunk type");
			LOG.DecIndentation(2);
			return false;
		}
	}

	LOG.DecIndentation(2);

	// Integrity
	return nID == ID_CHUNKEND;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool WriteAnimation(COutput* pOutput) const
//
//  - Purpose   : Writes the animation data to a destiny.
//
// -----------------------------------------------------------------------------
bool CNode::WriteAnimation(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(pOutput->WriteInt(ID_ANIM_CHUNKSTART) == false) uError++;

	// Static transformation matrix

	if(!m_relTM.Write(pOutput)) uError++;

	// Initial transformation values

	if(m_v3InitialPos.Write(pOutput)      == false) uError++;
	if(m_v3InitialScale.Write(pOutput)    == false) uError++;
	if(m_qInitialScaleAxes.Write(pOutput) == false) uError++;
	if(m_qInitialRot.Write(pOutput)       == false) uError++;

	// Animation

	if(m_pPosAnimation)
	{
		if(m_pPosAnimation->Write(pOutput) == false) uError++;
	}

	if(m_pRotAnimation)
	{
		if(m_pRotAnimation->Write(pOutput) == false) uError++;
	}

	if(m_pSclAnimation)
	{
		if(m_pSclAnimation->Write(pOutput) == false) uError++;
	}

	if(pOutput->WriteInt(ID_CHUNKEND) == false) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool SetSclAnimation(CAnimScl* pAnimScl)
//
//  - Purpose   : Sets the scale animation (overriding current, which will be
//                freed). Passing NULL would delete current scale animation.
//
// -----------------------------------------------------------------------------
bool CNode::SetSclAnimation(CAnimScl* pAnimScl)
{
	if(m_pSclAnimation) if(!m_pSclAnimation->Delete()) return false;
	if(!pAnimScl)       if(!m_pSclAnimation->Delete()) return false;

	m_pSclAnimation = pAnimScl;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool SetRotAnimation(CAnimRot* pAnimRot)
//
//  - Purpose   : Sets the rotation animation (overriding current, which will
//                be freed). Passing NULL would delete current rot animation.
//
// -----------------------------------------------------------------------------
bool CNode::SetRotAnimation(CAnimRot* pAnimRot)
{
	if(m_pRotAnimation)	if(!m_pRotAnimation->Delete()) return false;
	if(!pAnimRot)		if(!m_pRotAnimation->Delete()) return false;

	m_pRotAnimation = pAnimRot;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CNode
//  - prototype : bool SetPosAnimation(CAnimPos* pAnimPos)
//
//  - Purpose   : Sets the position animation (overriding current, which will
//                be freed). Passing NULL would delete current pos animation.
//
// -----------------------------------------------------------------------------
bool CNode::SetPosAnimation(CAnimPos* pAnimPos)
{
	if(m_pPosAnimation)	if(!m_pPosAnimation->Delete()) return false;
	if(!pAnimPos)       if(!m_pPosAnimation->Delete()) return false;

	m_pPosAnimation = pAnimPos;

	return true;
}