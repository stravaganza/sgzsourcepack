// ==[ File ]===================================================================================
//
//  -Name     :	Scene.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CScene definition.
//
// =============================================================================================

/*

	29/11/2002  -  File created.


*/


#ifndef __SCENE_H
#define __SCENE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CScene
//
//  - Purpose   : Scene node class. This node is mainly used to have a root
//                node of a whole hierarchy, which represents the scene.
//                It can also import/export all this data from/to a file.
//
// ---------------------------------------------------------------------------
class CScene : public CNode
{


public:


	CScene();
	~CScene();

	// Node type/subtype

	int GetNodeType   () const { return ID_NODE_SCENE;     }
	int GetNodeSubType() const { return ID_SCENE_STANDARD; }

	// Get

	CVector4 GetAmbientLight()                      const { return m_v4Ambient;          }
	CVector4 GetFogColor()                          const { return m_v4FogColor;         }
	int      GetFogType()                           const { return m_nFogType;           }
	float    GetFogNear()                           const { return m_fFogNear;           }
	float    GetFogFar()                            const { return m_fFogFar;            }
	float    GetFogDensity()                        const { return m_fFogDensity;        }

	// Set

	void     SetAmbientLight(const CVector4& v4Ambient)   { m_v4Ambient   = v4Ambient;   }
	void     SetFogColor    (const CVector4& v4FogColor)  { m_v4FogColor  = v4FogColor;  }
	void     SetFogType     (int             nFogType)    { m_nFogType    = nFogType;    }
	void     SetFogNear     (float           fFogNear)    { m_fFogNear    = fFogNear;    }
	void     SetFogFar      (float           fFogFar)     { m_fFogFar     = fFogFar;     }
	void     SetFogDensity  (float           fFogDensity) { m_fFogDensity = fFogDensity; }

	// Visitors

	bool AcceptVisitor(CNodeVisitor* pNodeVisitor) { return pNodeVisitor->VisitScene(this); }


protected:


	bool FreeNodeData ();
	bool ReadNodeData (CInput*  pInput);
	bool WriteNodeData(COutput* pOutput) const;


	CVector4 m_v4Ambient;
	CVector4 m_v4FogColor;
	int      m_nFogType;
	float    m_fFogNear;
	float    m_fFogFar;
	float    m_fFogDensity;

};


#endif