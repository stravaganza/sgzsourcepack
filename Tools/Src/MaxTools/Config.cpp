// ==[ File ]===================================================================================
//
//  -Name     : Config.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : ReadConfig / SaveConfig methods.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : void ReadConfig(bool bLoadDefaults = false)
//
//  - Purpose   : Initializes the tool parameters, reading the previous
//                configuration from a cfg file or creating default values.
//                If bLoadDefaults is true, the config file won't be read and
//                default values will be assigned.
//
// -----------------------------------------------------------------------------
void CStravaganzaMaxTools::ReadConfig(bool bLoadDefaults)
{
	if(!m_hPanel) return;

	ISpinnerControl* pMaxSpin;
	ICustEdit*       pMaxEdit;
	CInputFile cfgFile;

	m_bPreprocessMesh         = true;
	m_bUseLights              = true;
	m_eAnimType               = ANIMATION_SAMPLING;
	m_nAnimCheckStep          = 20;
	m_nNumPosSamplesPerSec    = 30;
	m_nNumSclSamplesPerSec    = 30;
	m_nNumRotSamplesPerSec    = 30;
	m_nNumCamSettingsPerSec   = 15;
	m_nNumLightSettingsPerSec = 15;

	m_strFile                 = "";
	m_bCopyTextures           = true;
	m_strTexturePath          = "";	

	m_ePreviewType            = CRenderVisitor::MODE_SOLID;
	m_bFullscreen             = false;
	m_bLoop                   = true;
	m_nPreviewResX            = 800;
	m_nPreviewResY            = 600;
	m_fViewportHeight         = 1.0f;
	m_bShowStats              = true;
	m_bRenderToAvi            = false;
	m_strAviFile              = "";
	m_strPreviewCamera        = "";

	if(cfgFile.Open(m_strCfgFile, false) && !bLoadDefaults)
	{
		int nValue;

		// Common options

		cfgFile.ReadInt(&nValue);
		m_bPreprocessMesh = nValue ? true : false;

		cfgFile.ReadInt(&nValue);
		m_bUseLights = nValue ? true : false;

		m_fStartTime = TICKS_TO_SECONDS(m_pMaxInterface->GetAnimRange().Start());
		m_fEndTime   = TICKS_TO_SECONDS(m_pMaxInterface->GetAnimRange().End());

		cfgFile.ReadInt(&m_nAnimCheckStep);
		cfgFile.ReadInt(&m_nNumPosSamplesPerSec);
		cfgFile.ReadInt(&m_nNumSclSamplesPerSec);
		cfgFile.ReadInt(&m_nNumRotSamplesPerSec);
		cfgFile.ReadInt(&m_nNumCamSettingsPerSec);
		cfgFile.ReadInt(&m_nNumLightSettingsPerSec);

		cfgFile.ReadInt(&nValue);

		switch(nValue)
		{
		case ANIMATION_SAMPLING: m_eAnimType = ANIMATION_SAMPLING; break;
		case ANIMATION_MAXKEYS:  m_eAnimType = ANIMATION_MAXKEYS;  break;
		}

		// Export options

		cfgFile.ReadStringZero(&m_strFile);
		cfgFile.ReadInt(&nValue);
		m_bCopyTextures = nValue ? true : false;
		cfgFile.ReadStringZero(&m_strTexturePath);

		// Preview options

		cfgFile.ReadStringZero(&m_strPreviewCamera);
		FillCameraCombobox(GetDlgItem(m_hPanel, IDC_COMBO_CAMERA), m_pMaxInterface->GetRootNode());

		cfgFile.ReadInt(&nValue);
		switch(nValue)
		{
		case CRenderVisitor::MODE_WIREFRAME:  m_ePreviewType = CRenderVisitor::MODE_WIREFRAME;  break;
		case CRenderVisitor::MODE_SOLID:      m_ePreviewType = CRenderVisitor::MODE_SOLID;      break;
		case CRenderVisitor::MODE_FULLDETAIL: m_ePreviewType = CRenderVisitor::MODE_FULLDETAIL; break;
		}

		cfgFile.ReadInt(&nValue);
		m_bFullscreen = nValue ? true : false;

		cfgFile.ReadInt(&nValue);
		m_bLoop = nValue ? true : false;

		cfgFile.ReadInt  (&m_nPreviewResX);
		cfgFile.ReadInt  (&m_nPreviewResY);
		cfgFile.ReadFloat(&m_fViewportHeight);
		cfgFile.ReadInt  (&nValue);
		m_bShowStats = nValue ? true : false;
		cfgFile.ReadInt  (&nValue);
		m_bRenderToAvi = nValue ? true : false;
		cfgFile.ReadStringZero(&m_strAviFile);
	}

	cfgFile.Close();

	// Common

	CheckDlgButton(m_hPanel, IDC_CHECK_PREPROCESSMESHES, m_bPreprocessMesh ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_hPanel, IDC_CHECK_USELIGHTS,        m_bUseLights      ? BST_CHECKED : BST_UNCHECKED);

	EnableWindow(GetDlgItem(m_hPanel, IDC_RADIO_USEMAXKEYS), FALSE);

	switch(m_eAnimType)
	{
	case ANIMATION_SAMPLING:

		CheckRadioButton(m_hPanel, IDC_RADIO_USESAMPLING, IDC_RADIO_USEMAXKEYS, IDC_RADIO_USESAMPLING);
		break;

	case ANIMATION_MAXKEYS:

		CheckRadioButton(m_hPanel, IDC_RADIO_USESAMPLING, IDC_RADIO_USEMAXKEYS, IDC_RADIO_USEMAXKEYS);
		break;
	}

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_STARTTIME));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_STARTTIME), EDITTYPE_FLOAT);
	pMaxSpin->SetLimits(m_fStartTime, m_fEndTime, TRUE);
	pMaxSpin->SetScale(0.01f);
	pMaxSpin->SetValue(m_fStartTime, FALSE);
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_ENDTIME));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_ENDTIME), EDITTYPE_FLOAT);
	pMaxSpin->SetLimits(m_fStartTime, m_fEndTime, TRUE);
	pMaxSpin->SetScale(0.01f);
	pMaxSpin->SetValue(m_fEndTime, FALSE);
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_ANIMCHECKSTEP));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_ANIMCHECKSTEP), EDITTYPE_INT);
	pMaxSpin->SetLimits(0, 100, TRUE);
	pMaxSpin->SetScale(1);
	pMaxSpin->SetValue(m_nAnimCheckStep, FALSE);
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_POSSAMPLINGFREQ));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_POSSAMPLINGFREQ), EDITTYPE_INT);
	pMaxSpin->SetLimits(0, 100, TRUE);
	pMaxSpin->SetScale(1);
	pMaxSpin->SetValue(m_nNumPosSamplesPerSec, FALSE);
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_SCLSAMPLINGFREQ));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_SCLSAMPLINGFREQ), EDITTYPE_INT);
	pMaxSpin->SetLimits(0, 100, TRUE);
	pMaxSpin->SetScale(1);
	pMaxSpin->SetValue(m_nNumSclSamplesPerSec, FALSE);
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_ROTSAMPLINGFREQ));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_ROTSAMPLINGFREQ), EDITTYPE_INT);
	pMaxSpin->SetLimits(0, 100, TRUE);
	pMaxSpin->SetScale(1);
	pMaxSpin->SetValue(m_nNumRotSamplesPerSec, FALSE);
	ReleaseISpinner(pMaxSpin);

	// Export

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_DESTINATIONFILE));
	pMaxEdit->SetText((char*)m_strFile.data());
	pMaxEdit->SetLeading(2);
	ReleaseICustEdit(pMaxEdit);

	CheckDlgButton(m_hPanel, IDC_CHECK_COPYTEXTURES, m_bCopyTextures ? BST_CHECKED : BST_UNCHECKED);

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_TEXTUREFOLDER));
	pMaxEdit->SetText((char*)m_strTexturePath.data());
	pMaxEdit->SetLeading(2);
	ReleaseICustEdit(pMaxEdit);

	EnableWindow(GetDlgItem(m_hPanel, IDC_EDIT_TEXTUREFOLDER),   m_bCopyTextures ? TRUE : FALSE);
	EnableWindow(GetDlgItem(m_hPanel, IDC_BUTTON_TEXTUREFOLDER), m_bCopyTextures ? TRUE : FALSE);

	// Preview

	switch(m_ePreviewType)
	{
	case CRenderVisitor::MODE_WIREFRAME:
		
		CheckRadioButton(m_hPanel, IDC_RADIO_WIREFRAME, IDC_RADIO_FULLDETAIL, IDC_RADIO_WIREFRAME);
		break;

	case CRenderVisitor::MODE_SOLID:
		
		CheckRadioButton(m_hPanel, IDC_RADIO_WIREFRAME, IDC_RADIO_FULLDETAIL, IDC_RADIO_SOLID);
		break;

	case CRenderVisitor::MODE_FULLDETAIL:
		
		CheckRadioButton(m_hPanel, IDC_RADIO_WIREFRAME, IDC_RADIO_FULLDETAIL, IDC_RADIO_FULLDETAIL);
		break;
	}

	CheckDlgButton(m_hPanel, IDC_CHECK_FULLSCREEN, m_bFullscreen ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_hPanel, IDC_CHECK_LOOP,       m_bLoop       ? BST_CHECKED : BST_UNCHECKED);

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_RESW));
	pMaxEdit->SetText(m_nPreviewResX);
	ReleaseICustEdit(pMaxEdit);

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_RESH));
	pMaxEdit->SetText(m_nPreviewResY);
	ReleaseICustEdit(pMaxEdit);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_VIEWPORTHEIGHT));
	pMaxSpin->LinkToEdit(GetDlgItem(m_hPanel, IDC_EDIT_VIEWPORTHEIGHT), EDITTYPE_FLOAT);
	pMaxSpin->SetLimits(0.0f, 1.0f, TRUE);
	pMaxSpin->SetScale(0.01f);
	pMaxSpin->SetValue(m_fViewportHeight, FALSE);
	ReleaseISpinner(pMaxSpin);

	CheckDlgButton(m_hPanel, IDC_CHECK_SHOWSTATS,   m_bShowStats   ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_hPanel, IDC_CHECK_RENDERTOAVI, m_bRenderToAvi ? BST_CHECKED : BST_UNCHECKED);

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_AVIFILE));
	pMaxEdit->SetText((char*)m_strAviFile.data());
	pMaxEdit->SetLeading(2);
	ReleaseICustEdit(pMaxEdit);

	EnableWindow(GetDlgItem(m_hPanel, IDC_EDIT_AVIFILE),   m_bRenderToAvi ? TRUE : FALSE);
	EnableWindow(GetDlgItem(m_hPanel, IDC_BUTTON_AVIFILE), m_bRenderToAvi ? TRUE : FALSE);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : void SaveConfig()
//
//  - Purpose   : Saves the panel parameters to a file.
//
// -----------------------------------------------------------------------------
void CStravaganzaMaxTools::SaveConfig()
{
	if(!m_hPanel) return;

	ISpinnerControl* pMaxSpin;
	ICustEdit*       pMaxEdit;

	int  nSelectedItem;
	char szAuxBuffer[MAX_PATH];
	COutputFile cfgFile;

	// GET SETTINGS FROM PANEL

	// Common

	m_bPreprocessMesh = IsDlgButtonChecked(m_hPanel, IDC_CHECK_PREPROCESSMESHES) == BST_CHECKED ? true : false;
	m_bUseLights      = IsDlgButtonChecked(m_hPanel, IDC_CHECK_USELIGHTS)        == BST_CHECKED ? true : false;

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_STARTTIME));
	m_fStartTime = pMaxSpin->GetFVal();
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_ENDTIME));
	m_fEndTime = pMaxSpin->GetFVal();
	ReleaseISpinner(pMaxSpin);

	if(IsDlgButtonChecked(m_hPanel, IDC_RADIO_USESAMPLING) == BST_CHECKED)
	{
		m_eAnimType = ANIMATION_SAMPLING;
	}
	else if(IsDlgButtonChecked(m_hPanel, IDC_RADIO_USEMAXKEYS) == BST_CHECKED)
	{
		m_eAnimType = ANIMATION_MAXKEYS;
	}

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_ANIMCHECKSTEP));
	m_nAnimCheckStep = pMaxSpin->GetIVal();
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_POSSAMPLINGFREQ));
	m_nNumPosSamplesPerSec = pMaxSpin->GetIVal();
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_SCLSAMPLINGFREQ));
	m_nNumSclSamplesPerSec = pMaxSpin->GetIVal();
	ReleaseISpinner(pMaxSpin);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_ROTSAMPLINGFREQ));
	m_nNumRotSamplesPerSec = pMaxSpin->GetIVal();
	ReleaseISpinner(pMaxSpin);

	// Export

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_DESTINATIONFILE));
	pMaxEdit->GetText(szAuxBuffer, MAX_PATH);
	ReleaseICustEdit(pMaxEdit);
	m_strFile = szAuxBuffer;

	m_bCopyTextures = IsDlgButtonChecked(m_hPanel, IDC_CHECK_COPYTEXTURES) == BST_CHECKED ? true : false;

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_TEXTUREFOLDER));
	pMaxEdit->GetText(szAuxBuffer, MAX_PATH);
	ReleaseICustEdit(pMaxEdit);
	m_strTexturePath = szAuxBuffer;

	// Preview

	// Get selected combo box camera

	nSelectedItem = SendMessage(GetDlgItem(m_hPanel, IDC_COMBO_CAMERA), CB_GETCURSEL, 0, 0);

	if(nSelectedItem == CB_ERR) // Items?
	{
		m_strPreviewCamera = "";
	}
	else
	{
		// Get selected item

		memset(szAuxBuffer, 0, MAX_PATH);

		if(SendMessage(GetDlgItem(m_hPanel, IDC_COMBO_CAMERA), CB_GETLBTEXT, (WPARAM)nSelectedItem, (LPARAM)(LPCSTR)szAuxBuffer) == CB_ERR)
		{
			CLogger::ErrorWindow("Error getting camera selection");
		}
		else
		{
			m_strPreviewCamera = szAuxBuffer;
		}
	}

	if(IsDlgButtonChecked(m_hPanel, IDC_RADIO_SOLID) == BST_CHECKED)
	{
		m_ePreviewType = CRenderVisitor::MODE_SOLID;
	}
	else if(IsDlgButtonChecked(m_hPanel, IDC_RADIO_WIREFRAME) == BST_CHECKED)
	{
		m_ePreviewType = CRenderVisitor::MODE_WIREFRAME;
	}
	else if(IsDlgButtonChecked(m_hPanel, IDC_RADIO_FULLDETAIL) == BST_CHECKED)
	{
		m_ePreviewType = CRenderVisitor::MODE_FULLDETAIL;
	}

	m_bFullscreen = IsDlgButtonChecked(m_hPanel, IDC_CHECK_FULLSCREEN) == BST_CHECKED ? true : false;
	m_bLoop       = IsDlgButtonChecked(m_hPanel, IDC_CHECK_LOOP)       == BST_CHECKED ? true : false;

	int  nAux;
	BOOL valid;

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_RESW));
	nAux = pMaxEdit->GetInt(&valid);
	if(valid) m_nPreviewResX = nAux; else pMaxEdit->SetText(m_nPreviewResX);
	ReleaseICustEdit(pMaxEdit);

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_RESH));
	nAux = pMaxEdit->GetInt(&valid);
	if(valid) m_nPreviewResY = nAux; else pMaxEdit->SetText(m_nPreviewResY);
	ReleaseICustEdit(pMaxEdit);

	pMaxSpin = GetISpinner(GetDlgItem(m_hPanel, IDC_SPIN_VIEWPORTHEIGHT));
	m_fViewportHeight = pMaxSpin->GetFVal();
	ReleaseISpinner(pMaxSpin);

	m_bShowStats   = IsDlgButtonChecked(m_hPanel, IDC_CHECK_SHOWSTATS)   == BST_CHECKED ? true : false;
	m_bRenderToAvi = IsDlgButtonChecked(m_hPanel, IDC_CHECK_RENDERTOAVI) == BST_CHECKED ? true : false;

	pMaxEdit = GetICustEdit(GetDlgItem(m_hPanel, IDC_EDIT_AVIFILE));
	pMaxEdit->GetText(szAuxBuffer, MAX_PATH);
	ReleaseICustEdit(pMaxEdit);
	m_strAviFile = szAuxBuffer;

	// SAVE SETTINGS

	if(cfgFile.Open(m_strCfgFile, false, false) == false)
	{
		return;
	}

	cfgFile.WriteInt(m_bPreprocessMesh ? 1 : 0);
	cfgFile.WriteInt(m_bUseLights      ? 1 : 0);

	cfgFile.WriteInt(m_nAnimCheckStep);
	cfgFile.WriteInt(m_nNumPosSamplesPerSec);
	cfgFile.WriteInt(m_nNumSclSamplesPerSec);
	cfgFile.WriteInt(m_nNumRotSamplesPerSec);
	cfgFile.WriteInt(m_nNumCamSettingsPerSec);
	cfgFile.WriteInt(m_nNumLightSettingsPerSec);

	cfgFile.WriteInt(m_eAnimType);

	// Export options

	cfgFile.WriteStringZero(m_strFile);
	cfgFile.WriteInt(m_bCopyTextures ? 1 : 0);
	cfgFile.WriteStringZero(m_strTexturePath);

	// Preview options

	cfgFile.WriteStringZero(m_strPreviewCamera);
	cfgFile.WriteInt(m_ePreviewType);
	cfgFile.WriteInt(m_bFullscreen ? 1 : 0);
	cfgFile.WriteInt(m_bLoop       ? 1 : 0);
	cfgFile.WriteInt(m_nPreviewResX);
	cfgFile.WriteInt(m_nPreviewResY);
	cfgFile.WriteFloat(m_fViewportHeight);
	cfgFile.WriteInt(m_bShowStats   ? 1 : 0);
	cfgFile.WriteInt(m_bRenderToAvi ? 1 : 0);
	cfgFile.WriteStringZero(m_strAviFile);

	cfgFile.Close();
}
