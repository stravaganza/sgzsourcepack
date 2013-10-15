// ==[ File ]===================================================================================
//
//  -Name     : Var.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CVar implementation.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


std::string CVar::GETCLASSTYPE()
{
	static std::string CLASSTYPE = "Variable";
	return CLASSTYPE;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : CVar()
//
//  - Purpose   : CVar's default constructor.
//
// -----------------------------------------------------------------------------
CVar::CVar()
{
	m_pEffectToBeReloaded = NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : std::string GetVarName() const
//
//  - Purpose   : Returns variable name.
//
// -----------------------------------------------------------------------------
std::string CVar::GetVarName() const
{
	return m_strName;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : std::string SetVarName()
//
//  - Purpose   : Sets variable name.
//
// -----------------------------------------------------------------------------
void CVar::SetVarName(const std::string& strName)
{
	m_strName = strName;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : std::string GetVarDescription() const
//
//  - Purpose   : Returns the variable description string.
//
// -----------------------------------------------------------------------------
std::string CVar::GetVarDescription() const
{
	return m_strDescription;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : void SetVarDescription(const std::string& strDescription)
//
//  - Purpose   : Sets the variable description string.
//
// -----------------------------------------------------------------------------
void CVar::SetVarDescription(const std::string& strDescription)
{
	m_strDescription = strDescription;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : void ReloadFXOnChange(CEffect *pEffect)
//
//  - Purpose   : Tells the variable to reload the given effect every time the
//				  variable changes its value.
//
// -----------------------------------------------------------------------------
void CVar::ReloadFXOnChange(CEffect *pEffect)
{
	m_pEffectToBeReloaded = pEffect;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CVar
//  - prototype : void NotifyChanged()
//
//  - Purpose   : Called by child classes in order to tell the base class
//				  their values changed.
//
// -----------------------------------------------------------------------------
void CVar::NotifyChanged()
{
	if(m_pEffectToBeReloaded)
	{
		// If an effect must be reloaded (ReloadFXOnChange() has been
		// called for this var), do it.

		if(m_pEffectToBeReloaded->GetDemo())
		{
			bool bShowErrors = CEffect::GetShowErrors();
			CEffect::SetShowErrors(false);

			m_pEffectToBeReloaded->Free();
			m_pEffectToBeReloaded->LoadData(m_pEffectToBeReloaded->GetDemo()->GetResourceList());

			CEffect::SetShowErrors(bShowErrors);
		}
	}
}
