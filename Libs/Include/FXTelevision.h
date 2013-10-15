#ifndef __FXTELEVISION_H
#define __FXTELEVISION_H


class CFXTelevision : public CEffect
{


public:


	CFXTelevision();
	~CFXTelevision();

	static const std::string CLASSNAME;

	// From CEffect

	std::string	GetClassName	() const { return CLASSNAME;      }
	std::string	GetGroupName	() const { return "Misc Effects"; }

	bool		LoadOnCreate    () const { return true;      } // LoadData() should be called on creation.

	bool		LoadData		(CResourceList* pResourceList);
	bool		Free			();
	bool		DoFrame			(CDemo* pDemo, float fEffectTime, float fDemoTime);


private:


	UtilGL::Texturing::CTexture2D m_texture;

	int m_nResX, m_nResY;
	unsigned char* m_pucOwnTexture;

};


#endif