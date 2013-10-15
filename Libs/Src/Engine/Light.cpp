// ==[ File ]===================================================================================
//
//  -Name     : Light.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CLight ìmplementation.
//
// =============================================================================================

/*

	02/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


// ==[ Class implementation ]===================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : CLight()
//
//  - Purpose   : CLight's default constructor.
//
// -----------------------------------------------------------------------------
CLight::CLight()
{
	m_nLightType    = ID_LIGHT_OMNI;

	m_uLSNumSamples = 0;
	m_fLSEndTime    = FLOAT_MIN;
	m_fLSStartTime  = FLOAT_MAX;

	SetDefaultSettings();

	m_pLSSamples = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : ~CLight()
//
//  - Purpose   : CLight's destructor.
//
// -----------------------------------------------------------------------------
CLight::~CLight()
{
	DeleteNode();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : void SetLightType(int nType)
//
//  - Purpose   : Sets the light type (omni, directional or spot).
//
// -----------------------------------------------------------------------------
void CLight::SetLightType(int nType)
{
	assert(nType == ID_LIGHT_OMNI || nType == ID_LIGHT_DIRECTIONAL || nType == ID_LIGHT_SPOT);

	m_nLightType = nType;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : void SetDefaultSettings()
//
//  - Purpose   : Sets the default light settings values.
//
// -----------------------------------------------------------------------------
void CLight::SetDefaultSettings()
{
	m_LSCurSettings.v4Ambient.Set (0.15f, 0.15f, 0.15f, 1.0f);
	m_LSCurSettings.v4Diffuse.Set (0.50f, 0.50f, 0.50f, 1.0f);
	m_LSCurSettings.v4Specular.Set(1.00f, 1.00f, 1.00f, 1.0f);

	m_LSCurSettings.fAttenConstant  = 1.0f;
	m_LSCurSettings.fAttenLinear    = 0.0f;
	m_LSCurSettings.fAttenQuadratic = 0.0f;

	m_LSCurSettings.fSpotCutoff     = 180.0f; // 0.0f to 90.0f (spot), 180.0f (omni).
	m_LSCurSettings.fSpotExponent   = 100.0f;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool SetAnimLightSettingsData(TSettings* pSamples,
//                                              unsigned   uNumSamples,
//                                              unsigned   uFrecuency,
//                                              float      fStartTime)
//
//  - Purpose   : Set's the animated light settings sample data.
//
// -----------------------------------------------------------------------------
bool CLight::SetAnimLightSettingsData(TSettings* pSamples, unsigned uNumSamples, unsigned uFrecuency, float fStartTime)
{
	SAFE_DELETE_A(m_pLSSamples);

	if(pSamples && uNumSamples > 0)
	{
		m_pLSSamples    = pSamples;
		m_uLSNumSamples = uNumSamples;
		m_uLSFrecuency  = uFrecuency;
		m_fLSStartTime  = fStartTime;
		m_fLSEndTime    = fStartTime + (uNumSamples * (1.0f / uFrecuency));
	}
	else
	{
		m_pLSSamples    = NULL;
		m_uLSNumSamples = 0;
		m_uLSFrecuency  = 0;
		m_fLSStartTime  = FLOAT_MAX;
		m_fLSEndTime    = FLOAT_MIN;
		m_bLSLoop       = false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool FreeAnimLightSettingsData()
//
//  - Purpose   : Frees the animated light settings data.
//
// -----------------------------------------------------------------------------
bool CLight::FreeAnimLightSettingsData()
{
	SAFE_DELETE_A(m_pLSSamples);

	m_uLSNumSamples = 0;

	m_fLSEndTime    = FLOAT_MIN;
	m_fLSStartTime  = FLOAT_MAX;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool ValidAnimLightSettingsTime(float fTime)
//
//  - Purpose   : Checks if the time provided is in the valid animated light
//                settings range.
//                Returns true if it's valid, false if it isn't.
//
// -----------------------------------------------------------------------------
bool CLight::ValidAnimLightSettingsTime(float fTime)
{
	if(!m_bLSLoop)
	{
		if(fTime > m_fLSEndTime)   return false;
		if(fTime < m_fLSStartTime) return false;
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool SetAnimLightSettingsEvalTime(float fTime)
//
//  - Purpose   : Evaluates the animation sample data to set the light values
//                according to the time value given.
//
// -----------------------------------------------------------------------------
bool CLight::SetAnimLightSettingsEvalTime(float fTime)
{
	if(!m_pLSSamples)
	{
		return true;
	}

	if(!m_bLSLoop)
	{
		if(!ValidAnimLightSettingsTime(fTime))
		{
			return false;
		}
	}
	else
	{
		ComputeLoop(m_fLSStartTime, m_fLSEndTime, &fTime);
	}
	
	unsigned uSample = m_uLSFrecuency * (fTime - m_fLSStartTime);
	float    fPeriod = 1.0f / m_uLSFrecuency;
	float    fT      = (fTime - (m_fLSStartTime + (uSample * fPeriod))) / fPeriod;

	if(uSample < 0 || uSample >= (m_uLSNumSamples - 1))
	{
		return false;
	}

	m_LSCurSettings.fAttenConstant  = ComputeLERP(m_pLSSamples[uSample].fAttenConstant,  m_pLSSamples[uSample + 1].fAttenConstant,  fT);
	m_LSCurSettings.fAttenLinear    = ComputeLERP(m_pLSSamples[uSample].fAttenLinear,    m_pLSSamples[uSample + 1].fAttenLinear,    fT);
	m_LSCurSettings.fAttenQuadratic = ComputeLERP(m_pLSSamples[uSample].fAttenQuadratic, m_pLSSamples[uSample + 1].fAttenQuadratic, fT);
	m_LSCurSettings.fSpotCutoff     = ComputeLERP(m_pLSSamples[uSample].fSpotCutoff,     m_pLSSamples[uSample + 1].fSpotCutoff,     fT);
	m_LSCurSettings.fSpotExponent   = ComputeLERP(m_pLSSamples[uSample].fSpotExponent,   m_pLSSamples[uSample + 1].fSpotExponent,   fT);
	m_LSCurSettings.v4Ambient       = m_pLSSamples[uSample].v4Ambient.LERP (m_pLSSamples[uSample + 1].v4Ambient,  fT);
	m_LSCurSettings.v4Diffuse       = m_pLSSamples[uSample].v4Diffuse.LERP (m_pLSSamples[uSample + 1].v4Diffuse,  fT);
	m_LSCurSettings.v4Specular      = m_pLSSamples[uSample].v4Specular.LERP(m_pLSSamples[uSample + 1].v4Specular, fT);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool ReadSettings(CInput* pInput)
//
//  - Purpose   : Reads both the static and animated light settings from a
//                data source.
//
// -----------------------------------------------------------------------------
bool CLight::ReadSettings(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	int      nAux;
	unsigned uError = 0;

	// Check

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux != ID_LIGHT_SETTINGSCHUNKSTART)
	{
		LOG.Write("\nERROR - CLight::ReadSettings() of %s: Expected light settings chunk start.", GetNodeName().data());
		return false;
	}

	// Initial settings

	if(pInput->ReadRawData(sizeof(TSettings), 1, &m_LSCurSettings) != 1)
	{
		LOG.Write("\nERROR - CLight::ReadSettings() of %s: Error reading static settings.", GetNodeName().data());
	}

	// Animated settings

	if(!pInput->ReadInt  ((int*)&m_uLSNumSamples)) uError++;
	if(!pInput->ReadInt  ((int*)&m_uLSFrecuency))  uError++;
	if(!pInput->ReadFloat(&m_fLSStartTime))        uError++;
	if(!pInput->ReadFloat(&m_fLSEndTime))          uError++;
	if(!pInput->ReadInt  (&nAux))                  uError++;

	m_bLSLoop = nAux ? true : false;

	SAFE_DELETE_A(m_pLSSamples);

	if(m_uLSNumSamples > 0)
	{
		LOG.Write("\n  Animated light settings: %u samples.", m_uLSNumSamples);

		m_pLSSamples = new TSettings[m_uLSNumSamples];
		assert(m_pLSSamples);

		if(pInput->ReadRawData(sizeof(TSettings) * m_uLSNumSamples, 1, m_pLSSamples) != 1)
		{
			LOG.Write("\nERROR - CLight::ReadSettings() of %s: Error reading animated setting samples.", GetNodeName().data());
			uError++;
		}
	}

	// Check

	if(!pInput->ReadInt(&nAux)) uError++;

	if(nAux != ID_LIGHT_SETTINGSCHUNKEND)
	{
		LOG.Write("\nERROR - CLight::ReadSettings() of %s: Expected light settings chunk start.", GetNodeName().data());
		return false;
	}

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool WriteSettings(COutput* pOutput) const
//
//  - Purpose   : Writes both the static and animated light settings to
//                the output passed.
//
// -----------------------------------------------------------------------------
bool CLight::WriteSettings(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(!pOutput->WriteInt(ID_LIGHT_SETTINGSCHUNKSTART)) uError++;

	// Initial settings

	if(pOutput->WriteRawData(sizeof(TSettings), 1, &m_LSCurSettings) != 1)
	{
		LOG.Write("\nERROR - CLight::WriteSettings() of %s: Error writing static settings.", GetNodeName().data());
	}

	// Animated settings

	if(!pOutput->WriteInt  (m_uLSNumSamples))   uError++;
	if(!pOutput->WriteInt  (m_uLSFrecuency))    uError++;
	if(!pOutput->WriteFloat(m_fLSStartTime))    uError++;
	if(!pOutput->WriteFloat(m_fLSEndTime))      uError++;
	if(!pOutput->WriteInt  (m_bLSLoop ? 1 : 0)) uError++;

	if(m_uLSNumSamples > 0)
	{
		LOG.Write("\n  Animated light settings: %u samples.", m_uLSNumSamples);

		if(pOutput->WriteRawData(sizeof(TSettings) * m_uLSNumSamples, 1, m_pLSSamples) != 1)
		{
			LOG.Write("\nERROR - CLight::WriteSettings() of %s: Error writing animated setting samples.", GetNodeName().data());
			uError++;
		}
	}

	if(!pOutput->WriteInt(ID_LIGHT_SETTINGSCHUNKEND)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool FreeNodeData()
//
//  - Purpose   : Frees the light data.
//
// -----------------------------------------------------------------------------
bool CLight::FreeNodeData()
{
	unsigned uError = 0;

	if(!FreeAnimLightSettingsData()) uError++;

	SetDefaultSettings();

	return uError == 0;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool ReadNodeData(CInput* pInput)
//
//  - Purpose   : Reads the light data from a source.
//
// -----------------------------------------------------------------------------
bool CLight::ReadNodeData(CInput* pInput)
{
	assert(pInput);
	assert(pInput->Ready());

	unsigned uError = 0;

	if(!pInput->ReadInt(&m_nLightType)) uError++;

	if(!ReadSettings (pInput)) uError++;
	if(!ReadAnimation(pInput)) uError++;

	return uError == 0;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CLight
//  - prototype : bool WriteNodeData(COutput* pOutput) const
//
//  - Purpose   : Writes the light data in a given destiny.
//
// -----------------------------------------------------------------------------
bool CLight::WriteNodeData(COutput* pOutput) const
{
	assert(pOutput);
	assert(pOutput->Ready());

	unsigned uError = 0;

	if(!pOutput->WriteInt(m_nLightType)) uError++;

	if(!WriteSettings (pOutput)) uError++;
	if(!WriteAnimation(pOutput)) uError++;

	return uError == 0;
}
