#ifndef __FXTEXTUREONOBJECT_H
#define __FXTEXTUREONOBJECT_H


class CFXTextureOnObject : public CEffect
{


public:


	CFXTextureOnObject();
	~CFXTextureOnObject();

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


private:


	struct SShaderChanged
	{
		SShaderChanged(	CShaderStandard* shader,
						CShaderStandard::ELayerType layer,
						UtilGL::Texturing::CTexture* tex1,
						UtilGL::Texturing::CTexture* tex2)
		{
			pShader = shader;
			eLayer  = layer;
			pOldTexture = tex1;
			pNewTexture = tex2;
		}

		CShaderStandard* pShader;
		CShaderStandard::ELayerType eLayer;
		UtilGL::Texturing::CTexture* pOldTexture;
		UtilGL::Texturing::CTexture* pNewTexture;
	};

	typedef std::vector<SShaderChanged> VECSHADERSCHANGED;

	VECSHADERSCHANGED m_vecShadersChanged;

	float       m_fAnimStart, m_fAnimEnd;
	std::string m_strCamera;
	CResource*  m_pResScene;
	CResource*  m_pResTexture;
};


#endif