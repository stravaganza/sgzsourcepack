// ==[ File ]===================================================================================
//
//  -Name     : Object.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CObject definition.
//
// =============================================================================================

/*

	30/12/2002  -  File created.
	19/02/2004  -  Added bone functionality.


*/


#ifndef __OBJECT_H
#define __OBJECT_H


// ==[ Class definitions ]======================================================================


class CBoundingVolume;
class CShader;
class CBlendedVertex;
class CHelper;


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CObject
//
//  - Purpose   : Geometric object node class.
//
// ---------------------------------------------------------------------------
class CObject : public CRenderable
{


public:


	CObject(bool bStaticMesh);
	~CObject();

	// Get

	inline int  GetNodeType   () const { return ID_NODE_OBJECT;                                       }
	inline int  GetNodeSubType() const { return m_bStaticMesh ? ID_OBJECT_STATIC : ID_OBJECT_DYNAMIC; }

	inline bool        HasStaticMesh    () const { return m_bStaticMesh;       }
	inline bool		   AffectsLighting	() const { return m_bAffectsLighting;  }
	inline bool		   HasBones			() const { return m_ppBones != NULL;   }
	inline bool		   NeedsBoneResolve () const { return (m_vecBoneNames.size() > 1) && (m_ppBones == NULL); }

	inline float       GetAlpha         () const { return m_fAlpha;            }
	inline unsigned    GetNumVertices   () const { return m_uNumVertices;      }
	inline unsigned    GetNumFaces      () const { return m_uNumFaces;         }
	inline unsigned*   GetFaceList      () const { return m_puFaceList;        }
	inline CVector3*   GetFaceNormals   () const { return m_pv3FaceNormals;    }
	inline CVector3*   GetVertices      () const { return m_pv3Vertices;       }
	inline CVector3*   GetExpVertices   () const { return m_pv3ExpVertices;    }
	inline CVector3*   GetVertexNormals () const { return m_pv3VertexNormals;  }
	inline int*        GetVNormalRefs   () const { return m_pnVNormalRefs;     }
	inline CVector4*   GetVertexColors  () const { return m_pv4VertexColors;   }
	inline GLboolean*  GetEdgeFlags     () const { return m_pglbEdgeFlags;     }
	inline CVector4    GetWireColor     () const { return m_v4WireColor;       }
	unsigned char*     GetBumpLVectors  () const { return m_pBumpLVectors;     }
	unsigned char*     GetBumpHVectors  () const { return m_pBumpHVectors;     }
	const CVector2*    GetMapChannel    (unsigned uMapChannel) const;

	inline int         GetNumMapChannels()       const { return m_vecMapChannels.size(); }

	inline CMatrix*    GetTextureSpaceMatrices() const { return m_pBumpTextureSpaceMatrices; }

	// Set

#define CHANGE_POINTER(p, newp) { (p) = (newp); } // SAFE_DELETE_A(p) 

	inline void SetStaticMesh    (bool bStatic)                         { m_bStaticMesh      = bStatic; }
	inline void SetAffectLighting(bool bAffect)							{ m_bAffectsLighting = bAffect; }

	inline void SetAlpha         (float fAlpha)                         { m_fAlpha       = fAlpha; }
	inline void SetFaceList      (int uCount, unsigned* puFaceList)     { m_uNumFaces    = uCount; CHANGE_POINTER(m_puFaceList,  puFaceList);  }
	inline void SetVertices      (int uCount, CVector3* pv3Vertices)    { m_uNumVertices = uCount; CHANGE_POINTER(m_pv3Vertices, pv3Vertices); }
	inline void SetFaceNormals   (CVector3*   pv3FaceNormals)           { CHANGE_POINTER(m_pv3FaceNormals,   pv3FaceNormals);     }
	inline void SetVertexNormals (CVector3*   pv3VertexNormals)         { CHANGE_POINTER(m_pv3VertexNormals, pv3VertexNormals);   }
	inline void SetVNormalRefs	 (int*		  pnVNormalRefs)			{ CHANGE_POINTER(m_pnVNormalRefs,    pnVNormalRefs);      }
	inline void SetVertexColors  (CVector4*   pv4VertexColors)          { CHANGE_POINTER(m_pv4VertexColors,  pv4VertexColors);    }
	inline void SetEdgeFlags     (GLboolean*  pglbEdgeFlags)            { CHANGE_POINTER(m_pglbEdgeFlags,    pglbEdgeFlags);      }
	inline void SetWireColor     (CVector4&   v4WireColor)              { m_v4WireColor = v4WireColor;                            }
	inline void SetBumpLVectors  (unsigned char* pucVectors)            { CHANGE_POINTER(m_pBumpLVectors,    pucVectors);         }
	inline void SetBumpHVectors  (unsigned char* pucVectors)            { CHANGE_POINTER(m_pBumpHVectors,    pucVectors);         }
	void        SetMapChannel    (unsigned    uMapChannel, CVector2* pv2Mapping);

	inline void SetBoneNames	  (const std::vector<std::string>& vecBoneNames) { m_vecBoneNames = vecBoneNames;             }
	inline void SetBlendedVertices(CBlendedVertex *pBlendedVertices)  { CHANGE_POINTER(m_pBlendedVertices, pBlendedVertices); }

	// Display lists (n lists mean n passes)

	unsigned    GetNumDisplayLists() const;
	GLuint      GetDisplayList    (unsigned uIndex) const;
	void        AddDisplayList    (GLuint uList);
	void        ClearDisplayLists ();

	// Shaders

	const CShader* GetShader        () const { return m_pShader;           }
	const char*    GetShaderName    () const { return m_strShader.c_str(); }

	inline void    SetShader        (CShader* pShader)             { m_pShader   = pShader;   }
	inline void    SetShaderName    (const std::string& strShader) { m_strShader = strShader; }

	// Other methods:

	bool     IsPostRender                 () const { return false; }
	bool     IsOpaque                     () const;
	bool     ComputeExpVertices           ();
	bool     ComputeNormals               (bool bFaceNormals, bool bVertexNormals);
	bool     ComputeTextureSpaceMatrices  ();
	bool     ComputeBumpVectors           (const CVector3& v3EyePos, const CVector3& v3LightPos);
	bool	 ResolveBones				  ();
	bool	 ComputeBlendedVertices		  ();
	bool     TestVisibility               (CCamera* pCamera);
	bool	 ComputeMassCenter			  (bool bOnlyWorld);
	CVector3 GetWorldMassCenter           () const; 

	CBoundingVolume* GetBoundingVolume() { return m_pBoundingVolume;   }
	
	// Visitors

	bool AcceptVisitor(CNodeVisitor* pNodeVisitor) { return pNodeVisitor->VisitObject(this); }

	// Bones


protected:


	bool FreeNodeData ();
	bool ReadNodeData (CInput*  pInput);
	bool WriteNodeData(COutput* pOutput) const;

	CMatrix  ComputeTangentBasis(const CVector3& v0, const CVector3& v1, const CVector3& v2,
                                 const CVector2& t0, const CVector2& t1, const CVector2& t2,
                                 const CVector3& v3FaceNormal);

	// Mesh

	bool       m_bStaticMesh;
	bool       m_bAffectsLighting;

	float      m_fAlpha;

	unsigned   m_uNumVertices;
	unsigned   m_uNumFaces;

	unsigned*  m_puFaceList;
	int*	   m_pnVNormalRefs;
	CVector3*  m_pv3FaceNormals;
	CVector3*  m_pv3Vertices;
	CVector3*  m_pv3ExpVertices;
	CVector3*  m_pv3VertexNormals;
	CVector4*  m_pv4VertexColors;
	GLboolean* m_pglbEdgeFlags;

	CVector4   m_v4WireColor;

	std::vector<CVector2*> m_vecMapChannels;

	// Bones

	std::vector<std::string> m_vecBoneNames;
	CHelper**				 m_ppBones;
	CBlendedVertex*			 m_pBlendedVertices;

	// Bounding volume

	CBoundingVolume* m_pBoundingVolume;
	CVector3		 m_v3LocalMassCenter;
	CVector3		 m_v3WorldMassCenter;

	// Display lists

	std::vector<GLuint> m_vecDisplayLists;

	// Shader

	std::string m_strShader;
	CShader*    m_pShader;

	// Bump related

	unsigned char* m_pBumpLVectors;
	unsigned char* m_pBumpHVectors;
	CMatrix*       m_pBumpTextureSpaceMatrices;
};

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CBlendedVertex
//
//  - Purpose   : Deformable vertex.
//
// ---------------------------------------------------------------------------
class CBlendedVertex
{


public:


	CBlendedVertex();
	~CBlendedVertex();

	int      GetNumLinks	()          const { return m_vecLocalVerts.size(); }
	CVector3 GetLocalVert	(int nLink) const { return m_vecLocalVerts[nLink]; }
	int		 GetBoneIndex	(int nLink) const { return m_vecBones     [nLink]; }
	float    GetWeight		(int nLink) const { return m_vecWeights   [nLink]; }

	void	 SetWeight		(int nLink, float fWeight) { m_vecWeights[nLink] = fWeight; }

	bool	 AddLink		(const CVector3& v3LocalVert, int nBoneIndex, float fWeight);
	bool	 DeleteAll();

	CVector3 ComputeVertex	(CHelper** ppBoneList);


	CBlendedVertex& operator = (const CBlendedVertex& vertex);


private:


	std::vector<CVector3>  m_vecLocalVerts;
	std::vector<int>       m_vecBones;
	std::vector<float>     m_vecWeights;

};


#endif