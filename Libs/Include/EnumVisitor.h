// ==[ File ]===================================================================================
//
//  -Name     : EnumVisitor.h
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CEnumVisitor definition.
//
// =============================================================================================

/*

	19/03/2003  -  File created.


*/


#ifndef __ENUMVISITOR_H
#define __ENUMVISITOR_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CEnumVisitor
//
//  - Purpose   : Provide secuential access to nodes in a hierarchy.
//
// ---------------------------------------------------------------------------
class CEnumVisitor : public CNodeVisitor
{


public:


	CEnumVisitor (CNodeVisitor::ETraversalMode eTraversalMode);
	~CEnumVisitor();

	// From CNodeVisitor:

	bool Reset              ();
	bool VisitCamera        (CCamera*         pCamera);
	bool VisitHelper        (CHelper*         pHelper);
	bool VisitLight         (CLight*          pLight );
	bool VisitObject        (CObject*         pObject);
	bool VisitLensFlare     (CLensFlare*      pLensFlare);
	bool VisitParticleSystem(CParticleSystem* pParticleSystem);

	//

	int  GetNumNodes()           const { return m_nNumNodes;           }
	int  GetNumCameras()         const { return m_vecCameras.size();   }
	int  GetNumHelpers()         const { return m_vecHelpers.size();   }
	int  GetNumLights()          const { return m_vecLights.size();    }
	int  GetNumObjects()         const { return m_vecObjects.size();   }
	int  GetNumLensFlares()      const { return m_vecLensFlares.size();}
	int  GetNumParticleSystems() const { return m_vecPSystems.size();  }

	CNode*			 GetNode           (int i) { assert(i >= 0 && i < (int)m_vecNodes.size());      return m_vecNodes[i];      }
	CCamera*         GetCamera         (int i) { assert(i >= 0 && i < (int)m_vecCameras.size());    return m_vecCameras[i];    }
	CHelper*         GetHelper         (int i) { assert(i >= 0 && i < (int)m_vecHelpers.size());    return m_vecHelpers[i];    }
	CLight*          GetLight          (int i) { assert(i >= 0 && i < (int)m_vecLights.size());     return m_vecLights[i];     }
	CObject*         GetObject         (int i) { assert(i >= 0 && i < (int)m_vecObjects.size());    return m_vecObjects[i];    }
	CLensFlare*      GetLensFlare      (int i) { assert(i >= 0 && i < (int)m_vecLensFlares.size()); return m_vecLensFlares[i]; }
	CParticleSystem* GetParticleSystem (int i) { assert(i >= 0 && i < (int)m_vecPSystems.size());   return m_vecPSystems[i];   }


private:


	std::vector<CNode*>           m_vecNodes;
	std::vector<CCamera*>         m_vecCameras;
	std::vector<CHelper*>         m_vecHelpers;
	std::vector<CLight*>          m_vecLights;
	std::vector<CObject*>         m_vecObjects;
	std::vector<CLensFlare*>      m_vecLensFlares;
	std::vector<CParticleSystem*> m_vecPSystems;

	int m_nNumNodes;
	
};


#endif