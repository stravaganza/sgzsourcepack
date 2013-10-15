// ==[ File ]===================================================================================
//
//  -Name     : BuildPhysique.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Physique plugin related routines.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"
#include "PhyExp.h"


// ==[ Implementation ]=========================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : Modifier* GetPhysiqueModifier(INode* pMaxNode)
//
//  - Purpose   : Returns the Physique modifier of a given node if present.
//
// -----------------------------------------------------------------------------
Modifier* CStravaganzaMaxTools::GetPhysiqueModifier(INode* pMaxNode)
{
	Object* pMaxObject = pMaxNode->GetObjectRef();

	if(!pMaxObject)
	{
		return NULL;
	}

	while(pMaxObject->SuperClassID() == GEN_DERIVOB_CLASS_ID && pMaxObject)
	{
		// Yes -> Cast.
		IDerivedObject* pMaxDerivedObject = (IDerivedObject *)(pMaxObject);				

		// Iterate over all entries of the modifier stack.
		int nModStackIndex = 0;

		while(nModStackIndex < pMaxDerivedObject->NumModifiers())
		{
			// Get current modifier.
			Modifier* pMaxModifier = pMaxDerivedObject->GetModifier(nModStackIndex);

			// Is this Physique?
			if(pMaxModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
			{
				return pMaxModifier;
			}

			nModStackIndex++;
		}

		pMaxObject = pMaxDerivedObject->GetObjRef();
	}

	// Not found.
	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : int BuildPhysiqueData(INode* pMaxNode,
//										CObject* pObject,
//                                      std::vector<std::string>    &vecBoneNames,
//										std::vector<CBlendedVertex> &vecBlendedVertices)
//
//  - Purpose   : Builds the bone data for a given node. Returns the number
//                of bones processed (0 = failure).
//
// -----------------------------------------------------------------------------
int	CStravaganzaMaxTools::BuildPhysiqueData(INode* pMaxNode,
											CObject* pObject,
											std::vector<std::string>    &vecBoneNames,
											std::vector<CBlendedVertex> &vecBlendedVertices)

{
	int nCount       = 0;
	int nBoneCount   = 0;

	Modifier          *pPhyModifier  = NULL; // Physique modifier
	IPhysiqueExport   *pPhyExport    = NULL; // Physique export interface
	IPhyContextExport *pPhyObjExport = NULL; // Physique object export interface

	vecBoneNames.clear();
	vecBlendedVertices.clear();

	// Build bone list

	std::vector<INode*> vecMaxBones;

	if(!AddNodeBones(vecMaxBones, pMaxNode))
	{
		LOG.Write("\nWARNING - Error building node %s bone list", pMaxNode->GetName());
		return 0;
	}

	// Build bones name list

	for(nBoneCount = 0; nBoneCount < vecMaxBones.size(); nBoneCount++)
	{
		vecBoneNames.push_back(vecMaxBones[nBoneCount]->GetName());
	}

	// Get Physique modifier

	if(pPhyModifier = GetPhysiqueModifier(pMaxNode))
	{
		pPhyExport = (IPhysiqueExport *)pPhyModifier->GetInterface(I_PHYINTERFACE);

		if(pPhyExport == NULL)
		{
			LOG.Write("\nWARNING - Couldn't get Physique export interface.\nFailed with node %s.", pMaxNode->GetName());
			return 0;
		}
	}

	// Get physique object export interface

	pPhyObjExport = pPhyExport->GetContextInterface(pMaxNode);

	if(pPhyObjExport == NULL)
	{
		pPhyModifier->ReleaseInterface(I_PHYINTERFACE, pPhyExport);
		LOG.Write("\nWARNING - Unable to get physique context export.\nFailed with node %s.", pMaxNode->GetName());
		return 0;
	}

	// Convert to rigid for time independent vertex assignment
	// Allow blending to export multi-link assignments

	pPhyObjExport->ConvertToRigid(true);
	pPhyObjExport->AllowBlending(true);

	// Build deformable vertex list

	bool bOK = true;
	int  nBlendedCount = 0, nBlendedRigidCount = 0, nFloatingCount = 0;

	for(nCount = 0; nCount < pPhyObjExport->GetNumberVertices(); nCount++)
	{
		IPhyVertexExport       *pPhyVertExport;
		IPhyBlendedRigidVertex *pPhyBRVertexExport;
		IPhyRigidVertex        *pPhyRigidVertexExport;
		IPhyFloatingVertex	   *pPhyFloatingVertex;

		pPhyVertExport  = pPhyObjExport->GetVertexInterface(nCount);

		CBlendedVertex blendedVertex;
		float fTotalWeight   = 0.0f;
		bool  bFloatingBones = false;

		// Floating Vertex

		pPhyFloatingVertex = pPhyObjExport->GetFloatingVertexInterface(nCount);

		if(pPhyFloatingVertex)
		{
			bFloatingBones = true;

			CVector3 v3OffsetVector;
			float    fWeight;

			// More than one bone

			int nNumVtxBones = pPhyFloatingVertex->GetNumberNodes();

//			LOG.Write("\n%u - Floating, with %u bones", nCount, nNumVtxBones);

			for(nBoneCount = 0; nBoneCount < nNumVtxBones; nBoneCount++)
			{
				int nIndex = GetBoneIndex(vecMaxBones, pPhyFloatingVertex->GetNode(nBoneCount));

				if(nIndex == -1)
				{
					LOG.Write("\nWARNING - Unable to get bone index (%s)", pPhyFloatingVertex->GetNode(nBoneCount)->GetName());
					bOK = false;
					break;
				}

				float fTotal;

				v3OffsetVector = Point3ToVector3(pPhyFloatingVertex->GetOffsetVector(nBoneCount));
				fWeight        = pPhyFloatingVertex->GetWeight(nBoneCount, fTotal);
				fTotalWeight  += fWeight;//fTotal;
				//fWeight = fTotal;

//				LOG.Write("\n     Weight = %f (%s)", fWeight, pPhyFloatingVertex->GetNode(nBoneCount)->GetName());

				blendedVertex.AddLink(v3OffsetVector, nIndex, fWeight);
			}

//			LOG.Write("\n     Total = %f", fTotalWeight);

			if(!ARE_EQUAL(fTotalWeight, 1.0f))
			{
				LOG.Write("\n WARNING - Vertex %u has total weights %f", nCount, fTotalWeight);
			}

			nFloatingCount++;

			pPhyObjExport->ReleaseVertexInterface(pPhyFloatingVertex);
		}

		if(pPhyVertExport)
		{
			if(pPhyVertExport->GetVertexType() & BLENDED_TYPE)
			{
				CVector3 v3OffsetVector;
				float    fWeight;

				// More than one bone

				pPhyBRVertexExport = (IPhyBlendedRigidVertex *)pPhyVertExport;
				int nNumVtxBones   = pPhyBRVertexExport->GetNumberNodes();

//				LOG.Write("\n%u - Blended, with %u bones", nCount, nNumVtxBones);

				for(nBoneCount = 0; nBoneCount < nNumVtxBones; nBoneCount++)
				{
					int nIndex = GetBoneIndex(vecMaxBones, pPhyBRVertexExport->GetNode(nBoneCount));

					if(nIndex == -1)
					{
						LOG.Write("\nWARNING - Unable to get bone index (%s)", pPhyBRVertexExport->GetNode(nBoneCount)->GetName());
						bOK = false;
						break;
					}

					v3OffsetVector = Point3ToVector3(pPhyBRVertexExport->GetOffsetVector(nBoneCount));
					fWeight        = pPhyBRVertexExport->GetWeight(nBoneCount);
					fTotalWeight  += fWeight;

//					LOG.Write("\n     Weight = %f (%s)", fWeight, pPhyBRVertexExport->GetNode(nBoneCount)->GetName());

					blendedVertex.AddLink(v3OffsetVector, nIndex, fWeight);
				}

//				LOG.Write("\n     Total = %f", fTotalWeight);

				if(!ARE_EQUAL(fTotalWeight, 1.0f))
				{
					LOG.Write("\n WARNING - Vertex %u has total weights %f", nCount, fTotalWeight);
				}

				nBlendedCount++;
			}
			else
			{
				CVector3 v3OffsetVector;
				float    fWeight;

				// Single bone

				pPhyRigidVertexExport = (IPhyRigidVertex *)pPhyVertExport;

				int nIndex = GetBoneIndex(vecMaxBones, pPhyRigidVertexExport->GetNode());

				if(nIndex == -1)
				{
					LOG.Write("\nWARNING - Unable to get bone index (%s)", pPhyRigidVertexExport->GetNode()->GetName());
					bOK = false;
					break;
				}

				v3OffsetVector = Point3ToVector3(pPhyRigidVertexExport->GetOffsetVector());
				fWeight        = 1.0f;
				fTotalWeight   = 1.0f;

//				LOG.Write("\n%u - Rigid (%s)", nCount, pPhyRigidVertexExport->GetNode()->GetName());

				blendedVertex.AddLink(v3OffsetVector, nIndex, fWeight);

				nBlendedRigidCount++;
			}

			pPhyObjExport->ReleaseVertexInterface(pPhyVertExport);
		}

		for(int i = 0; i < blendedVertex.GetNumLinks(); i++)
		{
			// Normalize
			blendedVertex.SetWeight(i, blendedVertex.GetWeight(i) / fTotalWeight);
		}

		vecBlendedVertices.push_back(blendedVertex);
	}

	pPhyExport->ReleaseContextInterface(pPhyObjExport);
	pPhyModifier->ReleaseInterface(I_PHYINTERFACE, pPhyExport);

	if(!bOK)
	{
		vecMaxBones.clear();
		vecBoneNames.clear();
		vecBlendedVertices.clear();
	}
	else
	{
		LOG.Write("\nPhysique: %u bones, %u vertices (%u rigid, %u rigidblended, %u floating)",
				  vecBoneNames.size(),
				  vecBlendedVertices.size(),
				  nBlendedRigidCount,
				  nBlendedCount,
				  nFloatingCount);
	}

	return vecMaxBones.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool AddNodeBones(std::vector<INode*> &vecMaxBones, INode *pMaxNode)
//
//  - Purpose   : Adds all bones belonging to the node to the given list.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::AddNodeBones(std::vector<INode*> &vecMaxBones, INode *pMaxNode)
{
	int  nCount       = 0;
	int  nBoneCount   = 0;
	bool bOK          = true;

	Modifier          *pPhyModifier  = NULL; // Physique modifier
	IPhysiqueExport   *pPhyExport    = NULL; // Physique export interface
	IPhyContextExport *pPhyObjExport = NULL; // Physique object export interface

	// Get Physique modifier

	if(pPhyModifier = GetPhysiqueModifier(pMaxNode))
	{
		pPhyExport = (IPhysiqueExport *)pPhyModifier->GetInterface(I_PHYINTERFACE);

		if(pPhyExport == NULL)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// Get physique object export interface

	pPhyObjExport = pPhyExport->GetContextInterface(pMaxNode);

	if(pPhyObjExport == NULL)
	{
		pPhyModifier->ReleaseInterface(I_PHYINTERFACE, pPhyExport);
		return false;
	}

	pPhyObjExport->ConvertToRigid(true);
	pPhyObjExport->AllowBlending(true);

	INode *pMaxBone;

	// Build bone list

	LOG.Write("\nObject %s Bone List:", pMaxNode->GetName());

	for(nCount = 0; nCount < pPhyObjExport->GetNumberVertices(); nCount++)
	{
		IPhyVertexExport       *pPhyVertExport;
		IPhyBlendedRigidVertex *pPhyBRVertexExport;
		IPhyRigidVertex        *pPhyRigidVertexExport;
		IPhyFloatingVertex	   *pPhyFloatingVertex;

		pPhyVertExport = pPhyObjExport->GetVertexInterface(nCount);

		if(pPhyVertExport)
		{
			if(pPhyVertExport->GetVertexType() & BLENDED_TYPE)
			{
				pPhyBRVertexExport = (IPhyBlendedRigidVertex *)pPhyVertExport;

				for(nBoneCount = 0; nBoneCount < pPhyBRVertexExport->GetNumberNodes(); nBoneCount++)
				{
					pMaxBone = pPhyBRVertexExport->GetNode(nBoneCount);

					if(IsBoneInList(vecMaxBones, pMaxBone) == false)
					{
//						LOG.Write("\n  %s (blended)", pMaxBone->GetName());
						vecMaxBones.push_back(pMaxBone);
					}
				}
			}
			else
			{
				pPhyRigidVertexExport = (IPhyRigidVertex *)pPhyVertExport;

				pMaxBone = pPhyRigidVertexExport->GetNode();

				if(IsBoneInList(vecMaxBones, pMaxBone) == false)
				{
//					LOG.Write("\n  %s (rigid)", pMaxBone->GetName());
					vecMaxBones.push_back(pMaxBone);
				}
			}

			pPhyObjExport->ReleaseVertexInterface(pPhyVertExport);
		}

		pPhyFloatingVertex = pPhyObjExport->GetFloatingVertexInterface(nCount);

		if(pPhyFloatingVertex)
		{
			for(nBoneCount = 0; nBoneCount < pPhyFloatingVertex->GetNumberNodes(); nBoneCount++)
			{
				pMaxBone = pPhyFloatingVertex->GetNode(nBoneCount);

				if(IsBoneInList(vecMaxBones, pMaxBone) == false)
				{
//					LOG.Write("\n  %s (floating)", pMaxBone->GetName());
					vecMaxBones.push_back(pMaxBone);
				}
			}

			pPhyObjExport->ReleaseVertexInterface(pPhyFloatingVertex);
		}
	}

	pPhyExport->ReleaseContextInterface(pPhyObjExport);
	pPhyModifier->ReleaseInterface(I_PHYINTERFACE, pPhyExport);

	return bOK;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool IsBoneInList(std::vector<INode*> &vecMaxBones, INode *pBone)
//
//  - Purpose   : Checks if the bone is in the given list.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::IsBoneInList(std::vector<INode*> &vecMaxBones, INode *pBone)
{
	int nCount;

	for(nCount = 0; nCount < vecMaxBones.size(); nCount++)
	{
		if(vecMaxBones[nCount] == pBone)
		{
			return true;
		}
	}

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : int GetBoneIndex(std::vector<INode *> &vecMaxBones, INode *pBone)
//
//  - Purpose   : Returns the index of the bone in the list.
//
// -----------------------------------------------------------------------------
int CStravaganzaMaxTools::GetBoneIndex(std::vector<INode *> &vecMaxBones, INode *pBone)
{
	int nCount;

	for(nCount = 0; nCount < vecMaxBones.size(); nCount++)
	{
		if(vecMaxBones[nCount] == pBone)
		{
			return nCount;
		}
	}

	LOG.Write("\nWARNING - GetBoneIndex() failed!");
	return -1;
}
