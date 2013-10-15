#include "Plugins.h"
#include "PluginFactory.h"
#include "ResourceSpline.h"


const std::string CResourceSpline::CLASSNAME("Spline");

REGISTER_PLUGIN(CPluginFactory, CResourceSpline)


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceSpline
//  - Prototype : CResourceSpline()
//
//  - Purpose   : CResourceSpline's default constructor.
//
// -----------------------------------------------------------------------------
CResourceSpline::CResourceSpline()
{

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceSpline
//  - Prototype : ~CResourceSpline()
//
//  - Purpose   : CResourceSpline's destructor.
//
// -----------------------------------------------------------------------------
CResourceSpline::~CResourceSpline()
{
	Free(false);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceSpline
//  - Prototype : bool Load(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
//
//  - Purpose   : Loads the spline specifiying optionally a datfile ("" for
//                no datafile). pTexOptions is ignored.
//
// -----------------------------------------------------------------------------
bool CResourceSpline::Load(const std::string& strDatFile, UtilGL::Texturing::STexLoadOptions* pTexOptions)
{
	Free(true);

	// Load Splines

	CInput* pInput = LoadFileFromDat(GetResourceFile(), false, strDatFile);
	if(!pInput)
	{
		return false;
	}

	CParser parser;

	parser.Init(pInput);
	parser.ResetPos();

	while(parser.Find("#Spline", CParser::SEARCH_FROMCURPOS) != -1)
	{
		CAdvancedParser splineParser;
		CVector3        point;
		CInputMemory    inputMem;
		std::string     strSpline;

		std::vector<CVector3> vecPoints;
		std::vector<CVector3>::iterator it;

		if(!parser.ReadStringBetweenChars('{', '}', &strSpline, CParser::SEARCH_FROMCURPOS, true))
		{
			continue;
		}

		inputMem.Open((char*)strSpline.data(), strSpline.length());
		splineParser.Init(&inputMem);
		splineParser.ResetPos();

		while(splineParser.ReadKeyVector3("#Point", '=', &point, CParser::SEARCH_FROMCURPOS))
		{
			vecPoints.push_back(point);
		}

		if(vecPoints.size() < 1)
		{
			continue;
		}

		float fIncTime = 1.0f / (vecPoints.size() - 1);
		float fT       = 0.0f;

		CSplineCatmullRom* pSpline = new CSplineCatmullRom;

		for(it = vecPoints.begin(); it != vecPoints.end(); ++it)
		{
			CSplineCatmullRom::TKey key;
			key.fTime   = fT;
			key.v3Value = *it;
			pSpline->AddKey(key);

			fT += fIncTime;
		}

		// Just in case of some precision loose:

		pSpline->SetKeyTime(pSpline->GetNumKeys() - 1, 1.0f);
		m_vecSplines.push_back(pSpline);

		splineParser.Close();
		inputMem.Close();
	}

	parser.Close();
	pInput->Close();
	delete pInput;
	return m_vecSplines.size() > 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CResourceSpline
//  - Prototype : bool Free(bool bFreeForReload)
//
//  - Purpose   : Frees the spline.
//
// -----------------------------------------------------------------------------
bool CResourceSpline::Free(bool bFreeForReload)
{
	if(HasDependencies() && !bFreeForReload)
	{
		ShowDependencyWarning();
	}

	VECPSPLINES::iterator it;

	for(it = m_vecSplines.begin(); it != m_vecSplines.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_vecSplines.clear();
	return true;
}