#ifndef __FXTWISTEDOBJECT_H
#define __FXTWISTEDOBJECT_H


class CFXTwistedObject : public CEffect
{


public:


	CFXTwistedObject();
	~CFXTwistedObject();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME; }
	std::string	GetGroupName	() const { return "Scenes";  }

	int         GetSupportedCommandsCount	()      const { return 1;         }
	std::string GetCommandSyntax			(int i) const { return "#Camera"; }
	bool        ExecuteCommand				(const std::string& strCommand);

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);

	static void	PreRender       (CRenderable* pRenderable, void* pData);
	static void	PostRender      (CRenderable* pRenderable, void* pData);


private:


	struct STwistedObject
	{
		STwistedObject(CObject* pObj)
		{
			pObject      = pObj;
			bStatic      = pObject->GetNodeSubType() == ID_OBJECT_STATIC;
			pDeformVerts = new CVector3[pObject->GetNumVertices()];
			pObjectVerts = pObject->GetVertices();
			assert(pDeformVerts);
		}

		CObject*  pObject;
		CVector3* pDeformVerts;
		CVector3* pObjectVerts;
		bool      bStatic;
		CMatrix   matrix;

	private:

		STwistedObject() { }
	};

	typedef std::vector<STwistedObject> TWISOBJVECTOR;

	TWISOBJVECTOR m_vecTwisObjects;

	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
};


#endif