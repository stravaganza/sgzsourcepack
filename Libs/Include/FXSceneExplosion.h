#ifndef __FXSCENEEXPLOSION_H
#define __FXSCENEEXPLOSION_H


class CFXSceneExplosion : public CEffect
{


public:


	CFXSceneExplosion();
	~CFXSceneExplosion();

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


	struct SExplosion
	{
		SExplosion(CObject* pObj, float fIntensity, float fGrav)
		{
			pObject      = pObj;
			fStart       = 0.0f;
			bStatic      = pObject->GetNodeSubType() == ID_OBJECT_STATIC;

			fGravity     = fGrav;

			uOldNumVertices     = pObject->GetNumVertices();
			puOldFaceList       = pObject->GetFaceList();
			pv3OldVertices      = pObject->GetVertices();
			pv3OldVertexNormals = pObject->GetVertexNormals();

			uNewNumVertices      = pObject->GetNumFaces() * 3;
			puNewFaceList        = new unsigned[uNewNumVertices];
			pv3NewVertices       = new CVector3[uNewNumVertices];
			pv3NewStartVertices  = new CVector3[uNewNumVertices];
			pv3NewVertexNormals  = new CVector3[uNewNumVertices];

			pv3ExplVectors   = new CVector3[uNewNumVertices];

			int i;

			CVector3 v3Center(0, 0, 0);

			for(i = 0; i < pObject->GetNumVertices(); i++)
			{
				v3Center = v3Center + pObject->GetVertices()[i];
			}

			v3Center = v3Center / pObject->GetNumVertices();

			for(i = 0; i < pObject->GetNumFaces(); i++)
			{
				CVector3 v3FaceCenter(0, 0, 0);

				for(int v = 0; v < 3; v++)
				{
					v3FaceCenter = v3FaceCenter + pObject->GetVertices()[pObject->GetFaceList()[i * 3 + v]];
				}

				v3FaceCenter = v3FaceCenter / 3;

				CVector3 v3Expl = CVector3(v3Center, v3FaceCenter);
				v3Expl.Normalize();
				v3Expl = v3Expl * fIntensity;

				for(v = 0; v < 3; v++)
				{
					pv3ExplVectors[i * 3 + v] = v3Expl;
				}
			}

			for(i = 0; i < pObject->GetNumMapChannels(); i++)
			{
				CVector2* pMapping = new CVector2[uNewNumVertices];
				m_vecNewMapChannels.push_back(pMapping);
				m_vecOldMapChannels.push_back((CVector2*)pObject->GetMapChannel(i));
			}

			// Build new mesh with no vertex sharing

			for(i = 0; i < pObject->GetNumFaces(); i++)
			{
				for(int v = 0; v < 3; v++)
				{
					int nIndex = i * 3 + v;

					puNewFaceList      [nIndex] = nIndex;
					pv3NewVertices     [nIndex] = pObject->GetVertices()[pObject->GetFaceList()[nIndex]];
					pv3NewStartVertices[nIndex] = pv3NewVertices[nIndex];
					pv3NewVertexNormals[nIndex] = pObject->GetVertexNormals()[pObject->GetFaceList()[nIndex]];

					for(int m = 0; m < pObject->GetNumMapChannels(); m++)
					{
						if(pObject->GetMapChannel(m))
						{
							m_vecNewMapChannels[m][nIndex] = pObject->GetMapChannel(m)[pObject->GetFaceList()[nIndex]];
						}
					}
				}
			}
		}

		void Delete()
		{
			SAFE_DELETE_A(puNewFaceList);
			SAFE_DELETE_A(pv3NewVertices);
			SAFE_DELETE_A(pv3NewStartVertices);
			SAFE_DELETE_A(pv3NewVertexNormals);
			SAFE_DELETE_A(pv3ExplVectors);

			int i;

			for(i = 0; i < pObject->GetNumMapChannels(); i++)
			{
				SAFE_DELETE(m_vecNewMapChannels[i]);
			}

			m_vecNewMapChannels.clear();
		}

		CObject*  pObject;

		float     fStart, fTime, fDuration;
		float     fGravity;

		unsigned  uOldNumVertices;
		unsigned* puOldFaceList;
		CVector3* pv3OldVertices;
		CVector3* pv3OldVertexNormals;
		std::vector<CVector2*> m_vecOldMapChannels;

		unsigned  uNewNumVertices;
		unsigned* puNewFaceList;
		CVector3* pv3NewVertices;
		CVector3* pv3NewStartVertices;
		CVector3* pv3NewVertexNormals;
		std::vector<CVector2*> m_vecNewMapChannels;

		CVector3* pv3ExplVectors;

		bool      bStatic;

	private:

		SExplosion() { }
	};

	typedef std::vector<SExplosion> VECEXPLOSIONS;

	VECEXPLOSIONS m_vecExplosions;

	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
};


#endif