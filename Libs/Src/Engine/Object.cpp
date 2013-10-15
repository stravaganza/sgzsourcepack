// ==[ File ]===================================================================================
//
//  -Name     : Object.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CObject ìmplementation.
//
// =============================================================================================

/*

	03/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : CObject(bool bStaticMesh)
//
//  - Purpose   : CObject's constructor. bStatic mesh tells if the object's
//                gonna have a static mesh (true) or a dynamic one (false).
//                This is very important later when a shader is bound to the
//                object and the object is compiled.
//
// -----------------------------------------------------------------------------
CObject::CObject(bool bStaticMesh)
{
	m_fAlpha = 1.0f;

	m_uNumVertices = 0;
	m_uNumFaces    = 0;

	m_bStaticMesh      = bStaticMesh;
	m_bAffectsLighting = true;

	m_puFaceList       = NULL;
	m_pnVNormalRefs    = NULL;
	m_pv3FaceNormals   = NULL;
	m_pv3Vertices      = NULL;
	m_pv3ExpVertices   = NULL;
	m_pv3VertexNormals = NULL;
	m_pv4VertexColors  = NULL;
	m_pglbEdgeFlags    = NULL;

	m_ppBones          = NULL;
	m_pBlendedVertices = NULL;

	m_v4WireColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	// Bounding volume

	m_pBoundingVolume = new CBoundingBox;

	// Bump related

	m_pBumpLVectors = NULL;
	m_pBumpHVectors = NULL;
	m_pBumpTextureSpaceMatrices = NULL;

	// Shader

	m_strShader = "";
	m_pShader   = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : ~CObject()
//
//  - Purpose   : CObject's destructor.
//
// -----------------------------------------------------------------------------
CObject::~CObject()
{
	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : const CVector2* GetMapChannel(unsigned uMapChannel) const
//
//  - Purpose   : Gets object's nth (zero based) mapping channel array.
//
// -----------------------------------------------------------------------------
const CVector2* CObject::GetMapChannel(unsigned uMapChannel) const
{
	assert(uMapChannel < m_vecMapChannels.size());

	return m_vecMapChannels[uMapChannel];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : void SetMapChannel(unsigned uMapChannel, CVector2* pv2Mapping)
//
//  - Purpose   : Sets object's nth (zero based) mapping channel values.
//
// -----------------------------------------------------------------------------
void CObject::SetMapChannel(unsigned uMapChannel, CVector2* pv2Mapping)
{
	if(uMapChannel >= m_vecMapChannels.size())
	{
		while(uMapChannel > m_vecMapChannels.size())
		{
			m_vecMapChannels.push_back(NULL);
		}
		m_vecMapChannels.push_back(pv2Mapping);
	}
	else
	{
		//SAFE_DELETE_A(m_vecMapChannels[uMapChannel]);
		m_vecMapChannels[uMapChannel] = pv2Mapping;
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : unsigned GetNumDisplayLists() const
//
//  - Purpose   : Returns object's number of display lists.
//                This can be used to retrieve the number of passes the object
//                needs to be drawn.
//
// -----------------------------------------------------------------------------
unsigned CObject::GetNumDisplayLists() const
{
	return m_vecDisplayLists.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : GLuint GetDisplayList(unsigned uIndex) const
//
//  - Purpose   : Returns object's i-th display list (i-th render pass).
//                Display lists are usually created by object's shader,
//                but can also be assigned manually if we need to.
//                Here we use the convention of 0 meaning no display list
//                (pass with dinamic geometry, using arrays or whatever) and
//                > 0 values being the id of an opengl display list.
//
//  - Note      : See shader class and the rendervisitor to know about the
//                render logic.
//
// -----------------------------------------------------------------------------
GLuint CObject::GetDisplayList(unsigned uIndex) const
{
	assert(uIndex >= 0);
	assert(uIndex < m_vecDisplayLists.size());

	return m_vecDisplayLists[uIndex];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : void AddDisplayList(GLuint uList)
//
//  - Purpose   : Adds a new display list (pass) to our vector.
//
// -----------------------------------------------------------------------------
void CObject::AddDisplayList(GLuint uList)
{
	m_vecDisplayLists.push_back(uList);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : void ClearDisplayLists()
//
//  - Purpose   : Frees all display lists and clears the display list vector.
//
// -----------------------------------------------------------------------------
void CObject::ClearDisplayLists()
{
	std::vector<GLuint>::iterator it;	

	for(it = m_vecDisplayLists.begin(); it != m_vecDisplayLists.end(); ++it)
	{
		if(*it)
		{
			glDeleteLists(*it, 1);
		}
	}
		
	m_vecDisplayLists.clear();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool IsOpaque() const
//
//  - Purpose   : Returns true if the object is opaque, false if it has to be
//                rendered with blending.
//
// -----------------------------------------------------------------------------
bool CObject::IsOpaque() const
{
	bool bShaderOpaque = m_pShader != NULL ? m_pShader->IsOpaque() : true;

	return bShaderOpaque && (m_fAlpha > (1.0f - ALMOST_ZERO));
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ComputeExpVertices()
//
//  - Purpose   : Computes the expanded vertex list. This list is the result
//                of expanding the normal vertex list to the numfaces*3 mode,
//                like a nonindexed structure.
//                This list is often needed when drawing the object in wire
//                mode because the edge flag list is also stored this way.
//
// -----------------------------------------------------------------------------
bool CObject::ComputeExpVertices()
{
	SAFE_DELETE_A(m_pv3ExpVertices);

	assert(m_pv3Vertices);
	assert(m_puFaceList);

	m_pv3ExpVertices = new CVector3[m_uNumFaces * 3];
	assert(m_pv3ExpVertices);

	for(int nFace = 0, nIndex = 0; nFace < m_uNumFaces; nFace++, nIndex += 3)
	{
		m_pv3ExpVertices[nIndex + 0] = m_pv3Vertices[m_puFaceList[nIndex + 0]];
		m_pv3ExpVertices[nIndex + 1] = m_pv3Vertices[m_puFaceList[nIndex + 1]];
		m_pv3ExpVertices[nIndex + 2] = m_pv3Vertices[m_puFaceList[nIndex + 2]];
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ComputeNormals(bool bFaceNormals, bool bVertexNormals)
//
//  - Purpose   : Computes the vertex and/or face normals for this object.
//
// -----------------------------------------------------------------------------
bool CObject::ComputeNormals(bool bFaceNormals, bool bVertexNormals)
{
	assert(m_uNumVertices > 0);
	assert(m_uNumFaces    > 0);
	assert(m_pv3Vertices);
	assert(m_puFaceList);

	int      nCount, nIndex;
	CVector3 v3Vec1, v3Vec2;

	bool bNegScaling = false;

	if(GetAbsoluteTM().HasNegativeScale()) bNegScaling = true;

	// Compute face normals

	if(bFaceNormals)
	{
		if(!m_pv3FaceNormals)
		{
			m_pv3FaceNormals = new CVector3[m_uNumFaces];
			assert(m_pv3FaceNormals);
		}

		for(nCount = 0; nCount < m_uNumFaces; nCount++)
		{
			nIndex = nCount * 3;

			v3Vec1.Build(m_pv3Vertices[m_puFaceList[nIndex + 1]], m_pv3Vertices[m_puFaceList[nIndex + 0]]);
			v3Vec2.Build(m_pv3Vertices[m_puFaceList[nIndex + 1]], m_pv3Vertices[m_puFaceList[nIndex + 2]]);

			m_pv3FaceNormals[nCount] = v3Vec2 ^ v3Vec1;

			if(m_pv3FaceNormals[nCount] == CVector3::NULLVECTOR)
			{
				/*
				CVector3 v1 = m_pv3Vertices[m_puFaceList[nIndex + 0]];
				CVector3 v2 = m_pv3Vertices[m_puFaceList[nIndex + 1]];
				CVector3 v3 = m_pv3Vertices[m_puFaceList[nIndex + 2]];

				LOG.Write("\nNORMAL ERROR: %u %u(%f, %f, %f) %u(%f, %f, %f) %u(%f, %f, %f)", nCount, m_puFaceList[nIndex + 0], v1.X(), v1.Y(), v1.Z(), m_puFaceList[nIndex + 1], v2.X(), v2.Y(), v2.Z(), m_puFaceList[nIndex + 2], v3.X(), v3.Y(), v3.Z());
				*/
			}
			else
			{
				m_pv3FaceNormals[nCount].Normalize();
			}

			if(bNegScaling) m_pv3FaceNormals[nCount] = -m_pv3FaceNormals[nCount];
		}
	}

	// Compute vertex normals

	if(bVertexNormals)
	{
		if(!m_pv3FaceNormals)
		{
			return false;
		}

		if(!m_pv3VertexNormals)
		{
			m_pv3VertexNormals = new CVector3[m_uNumVertices];
			assert(m_pv3VertexNormals);
		}

		// Initialize to zero

		for(nCount = 0; nCount < m_uNumVertices; nCount++)
		{
			m_pv3VertexNormals[nCount].Set(0.0f, 0.0f, 0.0f);
		}

		// Compute

		for(nCount = 0; nCount < m_uNumFaces; nCount++)
		{
			nIndex = nCount * 3;

			m_pv3VertexNormals[m_puFaceList[nIndex + 0]] = m_pv3VertexNormals[m_puFaceList[nIndex + 0]] + m_pv3FaceNormals[nCount];
			m_pv3VertexNormals[m_puFaceList[nIndex + 1]] = m_pv3VertexNormals[m_puFaceList[nIndex + 1]] + m_pv3FaceNormals[nCount];
			m_pv3VertexNormals[m_puFaceList[nIndex + 2]] = m_pv3VertexNormals[m_puFaceList[nIndex + 2]] + m_pv3FaceNormals[nCount];
		}

		// Normalize

		for(nCount = 0; nCount < m_uNumVertices; nCount++)
		{
			bool bOwnNormal = true;

			if(m_pnVNormalRefs)
			{
				// Gets vertex normal from an earlier defined vertex.
				// It's always a duplicated vertex during mesh consolidation. It still needs
				// to get the normal from the average face normals, but since that topology
				// information was lost we need to do it this way.

				if(m_pnVNormalRefs[nCount] != -1)
				{
					m_pv3VertexNormals[nCount] = m_pv3VertexNormals[m_pnVNormalRefs[nCount]];
					bOwnNormal = false;
				}
			}

			if(bOwnNormal)
			{
				if(m_pv3VertexNormals[nCount] != CVector3::NULLVECTOR)
				{
					m_pv3VertexNormals[nCount].Normalize();
				}
			}
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : CMatrix ComputeTangentBasis(const CVector3& v0, const CVector3& v1, const CVector3& v2,
//                                            const CVector2& t0, const CVector2& t1, const CVector2& t2,
//                                            const CVector3& v3FaceNormal)
//
//
//  - Purpose   : Computes the face tangent space basis given the data of its 3 vertices.
//
// -----------------------------------------------------------------------------
CMatrix CObject::ComputeTangentBasis(const CVector3& v0, const CVector3& v1, const CVector3& v2,
									 const CVector2& t0, const CVector2& t1, const CVector2& t2,
									 const CVector3& v3FaceNormal)
{
	CMatrix mtxBasis;
	mtxBasis.SetIdentity();

    CVector3 cp;
    CVector3 e0(v1.X() - v0.X(), t1.X() - t0.X(), t1.Y() - t0.Y());
    CVector3 e1(v2.X() - v0.X(), t2.X() - t0.X(), t2.Y() - t0.Y());

    cp = e0 ^ e1;

    if(!IS_ZERO(cp.X()))
    {
        mtxBasis(0, 0) = -cp.Y() / cp.X();        
        mtxBasis(1, 0) = -cp.Z() / cp.X();
    }

    e0.SetX(v1.Y() - v0.Y());
    e1.SetX(v2.Y() - v0.Y());

    cp = e0 ^ e1;
    if(!IS_ZERO(cp.X()))
    {
        mtxBasis(0, 1) = -cp.Y() / cp.X();        
        mtxBasis(1, 1) = -cp.Z() / cp.X();
    }

    e0.SetX(v1.Z() - v0.Z());
    e1.SetX(v2.Z() - v0.Z());

    cp = e0 ^ e1;
    if(!IS_ZERO(cp.X()))
    {
        mtxBasis(0, 2) = -cp.Y() / cp.X();        
        mtxBasis(1, 2) = -cp.Z() / cp.X();
    }

    // tangent
    float oonorm = 1.0f / sqrtf(mtxBasis(0, 0) * mtxBasis(0, 0) + mtxBasis(0, 1) * mtxBasis(0, 1) + mtxBasis(0, 2) * mtxBasis(0, 2));
    mtxBasis(0, 0) *= oonorm;
    mtxBasis(0, 1) *= oonorm;
    mtxBasis(0, 2) *= oonorm;

    // binormal
    oonorm = 1.0f / sqrtf(mtxBasis(1, 0) * mtxBasis(1, 0) + mtxBasis(1, 1) * mtxBasis(1, 1) + mtxBasis(1, 2) * mtxBasis(1, 2));
    mtxBasis(1, 0) *= oonorm;
    mtxBasis(1, 1) *= oonorm;
    mtxBasis(1, 2) *= oonorm;

    // normal
    // compute the cross product TxB
    mtxBasis(2, 0) = mtxBasis(0, 1)*mtxBasis(1, 2) - mtxBasis(0, 2)*mtxBasis(1, 1);
    mtxBasis(2, 1) = mtxBasis(0, 2)*mtxBasis(1, 0) - mtxBasis(0, 0)*mtxBasis(1, 2);
    mtxBasis(2, 2) = mtxBasis(0, 0)*mtxBasis(1, 1) - mtxBasis(0, 1)*mtxBasis(1, 0);

    oonorm = 1.0f / sqrtf(mtxBasis(2, 0) * mtxBasis(2, 0) + mtxBasis(2, 1) * mtxBasis(2, 1) + mtxBasis(2, 2) * mtxBasis(2, 2));
    mtxBasis(2, 0) *= oonorm;
    mtxBasis(2, 1) *= oonorm;
    mtxBasis(2, 2) *= oonorm;

    // Gram-Schmidt orthogonalization process for B
    // compute the cross product B=NxT to obtain 
    // an orthogonal basis
    mtxBasis(1, 0) = mtxBasis(2, 1)*mtxBasis(0, 2) - mtxBasis(2, 2)*mtxBasis(0, 1);
    mtxBasis(1, 1) = mtxBasis(2, 2)*mtxBasis(0, 0) - mtxBasis(2, 0)*mtxBasis(0, 2);
    mtxBasis(1, 2) = mtxBasis(2, 0)*mtxBasis(0, 1) - mtxBasis(2, 1)*mtxBasis(0, 0);

    if(mtxBasis(2, 0) * v3FaceNormal.X() + mtxBasis(2, 1) * v3FaceNormal.Y() + mtxBasis(2, 2) * v3FaceNormal.Z() < 0.0f)
    {
        mtxBasis(2, 0) = -mtxBasis(2, 0);
        mtxBasis(2, 1) = -mtxBasis(2, 1);
        mtxBasis(2, 2) = -mtxBasis(2, 2);
    }

    return mtxBasis.Transposed();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ComputeTextureSpaceMatrices()
//
//  - Purpose   : Computes the texture space matrices for this object.
//                Texture space matrices represent texture's coordinate system
//                of a given vertex.
//                When we do bump mapping, we need to compute the light vector
//                and the half angle vector in texture space for each vertex.
//                This transformation is done using this texture space matrix.
//
// -----------------------------------------------------------------------------
bool CObject::ComputeTextureSpaceMatrices()
{
	assert(m_uNumVertices > 0);
	assert(m_pShader);
	assert(m_pShader->GetBumpChannel() != -1);

	int nCount;
	CVector3 v3T, v3B, v3N, v3TempT;

	if(!m_pBumpTextureSpaceMatrices)
	{
		m_pBumpTextureSpaceMatrices = new CMatrix[m_uNumVertices];
		assert(m_pBumpTextureSpaceMatrices);
	}

	if(!m_pBumpLVectors)
	{
		m_pBumpLVectors = new unsigned char[m_uNumVertices * 4];
		assert(m_pBumpLVectors);
	}

	if(!m_pBumpHVectors)
	{
		m_pBumpHVectors = new unsigned char[m_uNumVertices * 4];
		assert(m_pBumpHVectors);
	}

	for(nCount = 0; nCount < m_uNumVertices; nCount++)
	{
		m_pBumpTextureSpaceMatrices[nCount].SetIdentity();
	}

	CVector2* pv2BumpMapChannel = m_vecMapChannels[m_pShader->GetBumpChannel()];

	for(nCount = 0; nCount < m_uNumFaces; nCount++)
	{
		int nCount_3 = nCount * 3;

		CMatrix mtxBasis = ComputeTangentBasis(	m_pv3Vertices    [m_puFaceList[nCount_3 + 0]],
												m_pv3Vertices    [m_puFaceList[nCount_3 + 1]],
												m_pv3Vertices    [m_puFaceList[nCount_3 + 2]],
												pv2BumpMapChannel[m_puFaceList[nCount_3 + 0]],
												pv2BumpMapChannel[m_puFaceList[nCount_3 + 1]],
												pv2BumpMapChannel[m_puFaceList[nCount_3 + 2]],
												m_pv3FaceNormals[nCount]);

		// Add T, B and N to every vertex in the face

		CMatrix* pMatrix;
		CVector3 v3Normal;

		for(int nVtxCount = 0; nVtxCount < 3; nVtxCount++)
		{
			pMatrix  = &m_pBumpTextureSpaceMatrices[m_puFaceList[nCount_3 + nVtxCount]];

			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 4; j++)
					(*pMatrix)(i, j) += mtxBasis(i, j);

			(*pMatrix)(3, 3) = 1.0f;
		}
	}

	// Promediate

	for(nCount = 0; nCount < m_uNumVertices; nCount++)
	{
		v3T = m_pBumpTextureSpaceMatrices[nCount].XAxis();
		v3B = m_pBumpTextureSpaceMatrices[nCount].YAxis();
		v3N = m_pBumpTextureSpaceMatrices[nCount].ZAxis();

		v3T.Normalize();
		v3B.Normalize();
		v3N.Normalize();

		m_pBumpTextureSpaceMatrices[nCount].SetRow0(v3T.X(),  v3B.X(),  v3N.X(),  0.0f);
		m_pBumpTextureSpaceMatrices[nCount].SetRow1(v3T.Y(),  v3B.Y(),  v3N.Y(),  0.0f);
		m_pBumpTextureSpaceMatrices[nCount].SetRow2(v3T.Z(),  v3B.Z(),  v3N.Z(),  0.0f);
		m_pBumpTextureSpaceMatrices[nCount].SetRow3(0.0f,     0.0f,     0.0f,     1.0f);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ComputeBumpVectors(const CVector3& v3EyePos,
//                                        const CVector3& v3LightPos)
//
//  - Purpose   : Computes for each vertex its light vector (L) and its half
//                angle vector(H) in texture space coordinates. Since We use
//                object's texture space matrices, they need to have been
//                computed.
//
// -----------------------------------------------------------------------------
unsigned char RangeCompress(const float &f) { return (unsigned char)((f+1)*127.5f); }

bool CObject::ComputeBumpVectors(const CVector3& v3EyePos, const CVector3& v3LightPos)
{
	assert(m_uNumVertices > 0);
	assert(m_pBumpTextureSpaceMatrices);
	assert(m_pBumpLVectors);
	assert(m_pBumpHVectors);

	CVector3 v3Light, v3HalfAngle, v3Eye, v3CameraPos;

	CMatrix mtxLocalInverse = GetAbsoluteTM().Inverse();

	CVector3 v3LocalLight = mtxLocalInverse * v3LightPos;
	CVector3 v3LocalEye   = mtxLocalInverse * v3EyePos;

	CMatrix mtxInvTangentSpace;

	for(int nCount = 0; nCount < m_uNumVertices; nCount++)
	{
		v3Light.Build(m_pv3Vertices[nCount], v3LocalLight);
		v3Eye.Build  (m_pv3Vertices[nCount], v3LocalEye);

		v3Eye.Normalize();
		v3Light.Normalize();
		v3HalfAngle = (v3Light + v3Eye) * 0.5f;

		// Compute local L & H for vertex i

		m_pBumpTextureSpaceMatrices[nCount].Transform(&v3Light);
		m_pBumpTextureSpaceMatrices[nCount].Transform(&v3HalfAngle);

		v3Light.Normalize();
		v3HalfAngle.Normalize();

		int nCount_4 = nCount * 4;

		// Range compress (for combiners or pixel shaders)

		m_pBumpLVectors[nCount_4 + 0] = RangeCompress(v3Light.X());
		m_pBumpLVectors[nCount_4 + 1] = RangeCompress(v3Light.Y());
		m_pBumpLVectors[nCount_4 + 2] = RangeCompress(v3Light.Z());
		m_pBumpLVectors[nCount_4 + 3] = 0;

		m_pBumpHVectors[nCount_4 + 0] = RangeCompress(v3HalfAngle.X());
		m_pBumpHVectors[nCount_4 + 1] = RangeCompress(v3HalfAngle.Y());
		m_pBumpHVectors[nCount_4 + 2] = RangeCompress(v3HalfAngle.Z());
		m_pBumpHVectors[nCount_4 + 3] = 0;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ResolveBones()
//
//  - Purpose   : Resolves all bones based on the given node names.
//				  This method should be called once the entire scene has been
//				  loaded.
//
// -----------------------------------------------------------------------------
bool CObject::ResolveBones()
{
	if(m_vecBoneNames.size() < 1)
	{
		return true;
	}

	// Get root node

	CNode* pSceneRoot = this;

	while(pSceneRoot->GetParent())
	{
		pSceneRoot = pSceneRoot->GetParent();
	}

	// Search starting from the root all bone nodes

	SAFE_DELETE_A(m_ppBones);
	m_ppBones = new CHelper*[m_vecBoneNames.size()];
	assert(m_ppBones);

	int nCount, nResolved = 0;
	std::vector<std::string>::iterator it;

	for(nCount = 0, it = m_vecBoneNames.begin(); it != m_vecBoneNames.end(); ++it, nCount++)
	{
		CNode* pNode = pSceneRoot->FindNode(*it);

		if(pNode->GetNodeType() == ID_NODE_HELPER)
		{
			m_ppBones[nCount] = (CHelper*)pNode;
		}

		if(m_ppBones[nCount] == NULL)
		{
			LOG.Write("\n WARNING - CObject::ResolveBones(): Unresolved bone in node %s (%s)", GetNodeName().data(), (*it).data());
		}
		else
		{
			nResolved++;
		}
	}

	if(nResolved != m_vecBoneNames.size())
	{
		SAFE_DELETE_A(m_ppBones);
	}

	return m_ppBones != NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ComputeBlendedVertices()
//
//  - Purpose   : Computes the blended vertices of boned objects.
//                Bones should have been transformed
//
// -----------------------------------------------------------------------------
bool CObject::ComputeBlendedVertices()
{
	assert(m_pBlendedVertices);

	for(int i = 0; i < m_uNumVertices; i++)
	{
		m_pv3Vertices[i] = m_pBlendedVertices[i].ComputeVertex(m_ppBones);
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool TestVisibility(CCamera* pCamera)
//
//  - Purpose   : Returns true if the object is visible in the given camera's
//                frustum.
//
//  - Notes     : Remember to update the bounding volume if the object has
//                dynamic mesh!
//
// -----------------------------------------------------------------------------
bool CObject::TestVisibility(CCamera* pCamera)
{
	if(!m_pBoundingVolume)
	{
		return true;
	}

	if(!pCamera)
	{
		return true;
	}

	if(this->m_ppBones)
	{
		// TODO: perform test
		return true;
	}

	return m_pBoundingVolume->Visible(pCamera);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ComputeMassCenter(bOnlyWorld)
//
//  - Purpose   : Computes the mass center of this object.
//				  If bOnlyWorld is false, the whole mesh is processed to compute
//				  the new local mass center.
//				  In both cases, the world mass center will be computed using
//				  the local mass center and the world matrix.
//
// -----------------------------------------------------------------------------
bool CObject::ComputeMassCenter(bool bOnlyWorld)
{
	if(!bOnlyWorld)
	{
		CVector3 v3Min(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX);
		CVector3 v3Max(FLOAT_MIN, FLOAT_MIN, FLOAT_MIN);

		// Iterate over all vertices, getting min and max limits:

		for(int i = 0; i < this->GetNumVertices(); i++)
		{
			CVector3 v = this->GetVertices()[i];

			if(v.X() < v3Min.X()) v3Min.SetX(v.X());
			if(v.Y() < v3Min.Y()) v3Min.SetY(v.Y());
			if(v.Z() < v3Min.Z()) v3Min.SetZ(v.Z());

			if(v.X() > v3Max.X()) v3Max.SetX(v.X());
			if(v.Y() > v3Max.Y()) v3Max.SetY(v.Y());
			if(v.Z() > v3Max.Z()) v3Max.SetZ(v.Z());
		}

		m_v3LocalMassCenter = (v3Min + v3Max) * 0.5f;
	}

	m_v3WorldMassCenter = this->GetAbsoluteTM() * m_v3LocalMassCenter;

	return true;	
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : CVector3 GetWorldMassCenter() const
//
//  - Purpose   : Returns the object world mass center.
//
// -----------------------------------------------------------------------------
CVector3 CObject::GetWorldMassCenter() const
{
	return m_v3WorldMassCenter;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the object data.
//
// -----------------------------------------------------------------------------
bool CObject::FreeNodeData()
{
	m_uNumVertices = 0;
	m_uNumFaces    = 0;

	// Mesh

	SAFE_DELETE_A(m_puFaceList);
	SAFE_DELETE_A(m_pnVNormalRefs);
	SAFE_DELETE_A(m_pv3FaceNormals);
	SAFE_DELETE_A(m_pv3Vertices);
	SAFE_DELETE_A(m_pv3ExpVertices);
	SAFE_DELETE_A(m_pv3VertexNormals);
	SAFE_DELETE_A(m_pv4VertexColors);
	SAFE_DELETE_A(m_pglbEdgeFlags);

	SAFE_DELETE_A(m_ppBones);
	SAFE_DELETE_A(m_pBlendedVertices);
	m_vecBoneNames.clear();

	std::vector<CVector2*>::iterator it;
	
	for(it = m_vecMapChannels.begin(); it != m_vecMapChannels.end(); ++it)
	{
		SAFE_DELETE_A(*it);
	}

	m_vecMapChannels.clear();

	// Bounding volume

	SAFE_DELETE(m_pBoundingVolume);

	// Display lists

	ClearDisplayLists();

	// Shader

	m_strShader.erase();
	m_pShader = NULL;    // Shader must be freed externally (they can be shared)

	// Bump related

	SAFE_DELETE_A(m_pBumpLVectors);
	SAFE_DELETE_A(m_pBumpHVectors);
	SAFE_DELETE_A(m_pBumpTextureSpaceMatrices);

	// Animation

	return DeleteAnimation();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool ReadNodeData(CInput* pInput)
//
//  - Purpose   : Reads the object data from a source.
//
// -----------------------------------------------------------------------------
bool CObject::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0, uNumMapChannels;
	int      nObjectType, nAux;
	bool     bHasVertexColors = false, bSkipNormals = false, bHasEdgeFlags = false, bHasTextureSpaceMatrices = false;
	bool	 bHasVNormalRefs  = false, bHasBoneData = false;

	// Type

	if(!pInput->ReadInt(&nObjectType)) uError++;

	if(nObjectType == ID_OBJECT_STATIC)
	{
		m_bStaticMesh = true;
	}
	else if(nObjectType == ID_OBJECT_DYNAMIC)
	{
		m_bStaticMesh = false;
	}
	else
	{
		assert(!"CObject::ReadNodeData(): invalid object type");
	}

	// Mesh

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux != ID_OBJECT_MESHCHUNKSTART)
	{
		LOG.Write("\nERROR - CObject::ReadNodeData() of %s: expected mesh chunk start.", GetNodeName().data());
		return false;
	}

	if(!pInput->ReadInt((int*)(&m_uNumVertices)))  uError++;
	if(!pInput->ReadInt((int*)(&m_uNumFaces)))     uError++;
	if(!pInput->ReadInt((int*)(&uNumMapChannels))) uError++;

	int* pnPresence = NULL;

	if(uNumMapChannels > 0)
	{
		pnPresence = new int[uNumMapChannels];
		assert(pnPresence);

		if(pInput->ReadRawData(sizeof(int) * uNumMapChannels, 1, pnPresence) < 1)
		{
			uError++;
		}
	}

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux & 1)  bHasVertexColors   = true;
	if(nAux & 2)  bSkipNormals       = true;
	if(nAux & 4)  m_bAffectsLighting = false;
	if(nAux & 8)  bHasVNormalRefs    = true;
	if(nAux & 16) bHasBoneData       = true;

	if(!pInput->ReadInt(&nAux)) uError++; if(nAux) bHasEdgeFlags            = true;
	if(!pInput->ReadInt(&nAux)) uError++; if(nAux) bHasTextureSpaceMatrices = true;

	if(!(m_puFaceList        = new unsigned [m_uNumFaces * 3]))  uError++;
	if(!(m_pv3FaceNormals    = new CVector3 [m_uNumFaces * 3]))  uError++;
	if(!(m_pv3Vertices       = new CVector3 [m_uNumVertices] ))  uError++;
	if(!(m_pv3VertexNormals  = new CVector3 [m_uNumVertices] ))  uError++;

	assert(m_puFaceList);
	assert(m_pv3FaceNormals);
	assert(m_pv3Vertices);
	assert(m_pv3VertexNormals);

	if(bHasVNormalRefs)
	{
		if(!(m_pnVNormalRefs = new int[m_uNumVertices])) uError++;
		assert(m_pnVNormalRefs);
	}

	if(bHasVertexColors)
	{
		if(!(m_pv4VertexColors = new CVector4[m_uNumVertices])) uError++;
		assert(m_pv4VertexColors);
	}

	if(bHasEdgeFlags)
	{
		if(!(m_pglbEdgeFlags = new GLboolean[m_uNumFaces * 3])) uError++;
		assert(m_pglbEdgeFlags);
	}

	if(bHasTextureSpaceMatrices)
	{
		if(!(m_pBumpTextureSpaceMatrices = new CMatrix[m_uNumVertices])) uError++;
		assert(m_pBumpTextureSpaceMatrices);
	}

	if(uError)
	{
		FreeNodeData();
		return false;
	}

	if(pInput->ReadRawData(sizeof(unsigned)  * m_uNumFaces * 3, 1, m_puFaceList) < 1) uError++;

	if(bHasVNormalRefs) if(pInput->ReadRawData(sizeof(int) * m_uNumVertices, 1, m_pnVNormalRefs) < 1) uError++;

	if(!bSkipNormals) if(pInput->ReadRawData(sizeof(CVector3) * m_uNumFaces, 1, m_pv3FaceNormals) < 1) uError++;

	if(pInput->ReadRawData(sizeof(CVector3)  * m_uNumVertices,  1, m_pv3Vertices) < 1) uError++;

	if(!bSkipNormals) if(pInput->ReadRawData(sizeof(CVector3) * m_uNumVertices, 1, m_pv3VertexNormals) < 1) uError++;

	if(bSkipNormals) this->ComputeNormals(true, true);

	for(int n = 0; n < uNumMapChannels; n++)
	{
		CVector2* pMapping = NULL;

		if(pnPresence[n])
		{
			pMapping = new CVector2[m_uNumVertices];
			assert(pMapping);

			if(pInput->ReadRawData(sizeof(CVector2) * m_uNumVertices, 1, pMapping) < 1)
			{
				uError++;
			}
		}

		m_vecMapChannels.push_back(pMapping);
	}

	SAFE_DELETE_A(pnPresence);

	if(bHasVertexColors)
	{
		if(pInput->ReadRawData(sizeof(CVector3)  * m_uNumVertices, 1, m_pv4VertexColors)  < 1) uError++;
	}

	if(bHasEdgeFlags)
	{
		if(pInput->ReadRawData(sizeof(GLboolean) * m_uNumFaces * 3, 1, m_pglbEdgeFlags)   < 1) uError++;
	}

	if(bHasTextureSpaceMatrices)
	{
		//if(pInput->ReadRawData(sizeof(CMatrix) * m_uNumVertices, 1, m_pBumpTextureSpaceMatrices) < 1) uError++;

		m_pBumpLVectors = new unsigned char[m_uNumVertices * 4];
		m_pBumpHVectors = new unsigned char[m_uNumVertices * 4];

		assert(m_pBumpLVectors);
		assert(m_pBumpHVectors);
	}

	if(bHasBoneData)
	{
		m_pBlendedVertices = new CBlendedVertex[m_uNumVertices];
		assert(m_pBlendedVertices);

		int nNumBones;

		if(!pInput->ReadInt(&nNumBones)) uError++;
		int nCount;
		
		for(nCount = 0; nCount < nNumBones; nCount++)
		{
			std::string strBoneName;
			if(!pInput->ReadStringZero(&strBoneName)) uError++;
			m_vecBoneNames.push_back(strBoneName);
		}

		for(nCount = 0; nCount < m_uNumVertices; nCount++)
		{
			int nNumLinks;

			if(!pInput->ReadInt(&nNumLinks)) uError++;

			for(int nLink = 0; nLink < nNumLinks; nLink++)
			{
				CVector3 v3LocalVert;
				int		 nBone;
				float    fWeight;

				if(!v3LocalVert.Read(pInput))    uError++;
				if(!pInput->ReadInt(&nBone))     uError++;
				if(!pInput->ReadFloat(&fWeight)) uError++;

				m_pBlendedVertices[nCount].AddLink(v3LocalVert, nBone, fWeight);
			}
		}
	}

	if(!m_v4WireColor.Read(pInput)) uError++;

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux != ID_OBJECT_MESHCHUNKEND)
	{
		LOG.Write("\nERROR - CObject::ReadNodeData() of %s: expected mesh chunk end.", GetNodeName().data());
		return false;
	}

	// Animation

	if(!ReadAnimation(pInput)) uError++;

	// Shader

	if(!pInput->ReadStringZero(&m_strShader)) uError++;

	// Bounding volume

	if(m_pBoundingVolume)
	{
		m_pBoundingVolume->Build(this);
	}

	ComputeMassCenter(false);

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CObject
//  - prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Writes the object data in a given destiny.
//
// -----------------------------------------------------------------------------
bool CObject::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	// Type

	if(!pOutput->WriteInt(GetNodeSubType())) uError++;

	// Mesh

	if(!pOutput->WriteInt(ID_OBJECT_MESHCHUNKSTART))  uError++;

	if(!pOutput->WriteInt(m_uNumVertices))            uError++;
	if(!pOutput->WriteInt(m_uNumFaces))               uError++;
	if(!pOutput->WriteInt(m_vecMapChannels.size()))   uError++;

	std::vector<CVector2*>::const_iterator it;

	for(it = m_vecMapChannels.begin(); it != m_vecMapChannels.end(); ++it)
	{
		// Map channel presence flags

		if(!pOutput->WriteInt(*it ? 1 : 0))
		{
			uError++;
		}
	}

	int  nFlags = 0;
	bool bSkipNormals = true; // To optimize size
	bool bVNormalRefs = m_pnVNormalRefs != NULL;
	bool bBoneData    = m_vecBoneNames.size() > 1 && this->m_pBlendedVertices;
	
	if(m_pv4VertexColors)   nFlags |= (1 << 0);
	if(bSkipNormals)        nFlags |= (1 << 1);
	if(!m_bAffectsLighting) nFlags |= (1 << 2);
	if(bVNormalRefs)		nFlags |= (1 << 3);
	if(bBoneData)			nFlags |= (1 << 4);

	if(!pOutput->WriteInt(nFlags)) uError++;
	if(!pOutput->WriteInt(m_pglbEdgeFlags             ? 1 : 0)) uError++;
	if(!pOutput->WriteInt(m_pBumpTextureSpaceMatrices ? 1 : 0)) uError++;

	assert(m_puFaceList);
	assert(m_pv3FaceNormals);
	assert(m_pv3Vertices);
	assert(m_pv3VertexNormals);

	if(pOutput->WriteRawData(sizeof(unsigned) * m_uNumFaces * 3, 1, m_puFaceList) < 1) uError++;

	if(bVNormalRefs) if(pOutput->WriteRawData(sizeof(int) * m_uNumVertices, 1, m_pnVNormalRefs) < 1) uError++;

	if(!bSkipNormals) if(pOutput->WriteRawData(sizeof(CVector3) * m_uNumFaces, 1, m_pv3FaceNormals) < 1) uError++;

	if(pOutput->WriteRawData(sizeof(CVector3) * m_uNumVertices, 1, m_pv3Vertices) < 1) uError++;

	if(!bSkipNormals) if(pOutput->WriteRawData(sizeof(CVector3) * m_uNumVertices, 1, m_pv3VertexNormals) < 1) uError++;

	for(it = m_vecMapChannels.begin(); it != m_vecMapChannels.end(); ++it)
	{
		if(*it)
		{
			if(pOutput->WriteRawData(sizeof(CVector2) * m_uNumVertices, 1, *it) < 1)
			{
				uError++;
			}
		}
	}

	if(m_pv4VertexColors)
	{
		if(pOutput->WriteRawData(sizeof(CVector3) * m_uNumVertices, 1, m_pv4VertexColors) < 1) uError++;
	}

	if(m_pglbEdgeFlags)
	{
		if(pOutput->WriteRawData(sizeof(GLboolean) * m_uNumFaces * 3, 1, m_pglbEdgeFlags) < 1) uError++;
	}

	if(m_pBumpTextureSpaceMatrices)
	{
		//if(pOutput->WriteRawData(sizeof(CMatrix) * m_uNumVertices, 1, m_pBumpTextureSpaceMatrices) < 1) uError++;
	}

	if(bBoneData)
	{
		if(!pOutput->WriteInt(m_vecBoneNames.size())) uError++;

		int nCount;
		
		for(nCount = 0; nCount < m_vecBoneNames.size(); nCount++)
		{
			if(!pOutput->WriteStringZero(m_vecBoneNames[nCount])) uError++;
		}

		for(nCount = 0; nCount < m_uNumVertices; nCount++)
		{
			int nNumLinks = m_pBlendedVertices[nCount].GetNumLinks();

			if(!pOutput->WriteInt(nNumLinks)) uError++;

			for(int nLink = 0; nLink < nNumLinks; nLink++)
			{
				if(!m_pBlendedVertices[nCount].GetLocalVert(nLink).Write(pOutput))       uError++;
				if(!pOutput->WriteInt  (m_pBlendedVertices[nCount].GetBoneIndex(nLink))) uError++;
				if(!pOutput->WriteFloat(m_pBlendedVertices[nCount].GetWeight(nLink)))    uError++;
			}
		}
	}		

	if(!m_v4WireColor.Write(pOutput)) uError++;

	if(!pOutput->WriteInt(ID_OBJECT_MESHCHUNKEND)) uError++;

	// Animation

	if(!WriteAnimation(pOutput)) uError++;

	// Shader

	if(!pOutput->WriteStringZero(m_strShader)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBlendedVertex
//  - prototype : CBlendedVertex()
//
//  - Purpose   : CBlendedVertex's default constructor.
//
// -----------------------------------------------------------------------------
CBlendedVertex::CBlendedVertex()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBlendedVertex
//  - prototype : ~CBlendedVertex()
//
//  - Purpose   : CBlendedVertex's destructor.
//
// -----------------------------------------------------------------------------
CBlendedVertex::~CBlendedVertex()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBlendedVertex
//  - prototype : bool AddLink(const CVector3& v3LocalVert, int nBoneIndex, float fWeight)
//
//  - Purpose   : Adds a new link to a bone.
//                v3LocalVert is the vertex in bone local coordinates.
//                nBoneIndex is the index to the bone it's linked to.
//                fWeight is the blend weight for that link.
//
//  - Note		: All weights should be normalized (the sum should be 1.0).
//
// -----------------------------------------------------------------------------
bool CBlendedVertex::AddLink(const CVector3& v3LocalVert, int nBoneIndex, float fWeight)
{
	m_vecLocalVerts.push_back(v3LocalVert);
	m_vecBones.push_back(nBoneIndex);
	m_vecWeights.push_back(fWeight);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBlendedVertex
//  - prototype : bool DeleteAll()
//
//  - Purpose   : Removes all link references.
//
// -----------------------------------------------------------------------------
bool CBlendedVertex::DeleteAll()
{
	m_vecLocalVerts.clear();
	m_vecBones.clear();
	m_vecWeights.clear();

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBlendedVertex
//  - prototype : CVector3 ComputeVertex(CHelper** ppBoneList)
//
//  - Purpose   : Computes the vertex based on all bones and weights.
//                Bones should have been previously transformed to the desired time.
//
// -----------------------------------------------------------------------------
CVector3 CBlendedVertex::ComputeVertex(CHelper** ppBoneList)
{
	CVector3 v3Result;

	for(int i = 0; i < m_vecBones.size(); i++)
	{
		v3Result = v3Result + (ppBoneList[m_vecBones[i]]->GetAbsoluteTM() * m_vecLocalVerts[i]) * m_vecWeights[i];
	}

	return v3Result;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CBlendedVertex
//  - prototype : CBlendedVertex& operator = (const CBlendedVertex& vertex)
//
//  - Purpose   : Assign operator.
//
// -----------------------------------------------------------------------------
CBlendedVertex& CBlendedVertex::operator = (const CBlendedVertex& vertex)
{
	m_vecLocalVerts = vertex.m_vecLocalVerts;
	m_vecBones      = vertex.m_vecBones;
	m_vecWeights    = vertex.m_vecWeights;

	return *this;
}