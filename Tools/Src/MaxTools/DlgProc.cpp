// ==[ File ]===================================================================================
//
//  -Name     : DlgProc.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Dialog panel procedure.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


// ==[ Methods ]================================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : BOOL CALLBACK ToolDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//
//  - Purpose   : Tool dialog box procedure.
//
// -----------------------------------------------------------------------------
BOOL CALLBACK CStravaganzaMaxTools::ToolDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ICustEdit* pMaxEdit;
	FilterList maxFilterList;

	OPENFILENAME ofn;

	char szAuxBuffer[MAX_PATH];

	switch(uMsg)
	{
		case WM_INITDIALOG:

			g_SGZMaxTools.m_hPanel = hWnd;
			g_SGZMaxTools.ReadConfig();
			
			return TRUE;

		case WM_COMMAND:

			switch(LOWORD(wParam))
			{

			case IDC_BUTTON_LOADDEFAULTS:

				g_SGZMaxTools.ReadConfig(true);
				g_SGZMaxTools.SaveConfig();

				break;

			case IDC_RADIO_USESAMPLING:
			case IDC_RADIO_USEMAXKEYS:

				EnableWindow(GetDlgItem(hWnd, IDC_EDIT_POSSAMPLINGFREQ), IsDlgButtonChecked(hWnd, IDC_RADIO_USEMAXKEYS) == BST_CHECKED ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT_SCLSAMPLINGFREQ), IsDlgButtonChecked(hWnd, IDC_RADIO_USEMAXKEYS) == BST_CHECKED ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT_ROTSAMPLINGFREQ), IsDlgButtonChecked(hWnd, IDC_RADIO_USEMAXKEYS) == BST_CHECKED ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_SPIN_POSSAMPLINGFREQ), IsDlgButtonChecked(hWnd, IDC_RADIO_USEMAXKEYS) == BST_CHECKED ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_SPIN_SCLSAMPLINGFREQ), IsDlgButtonChecked(hWnd, IDC_RADIO_USEMAXKEYS) == BST_CHECKED ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_SPIN_ROTSAMPLINGFREQ), IsDlgButtonChecked(hWnd, IDC_RADIO_USEMAXKEYS) == BST_CHECKED ? TRUE : FALSE);
				
				return TRUE;

			case IDC_CHECK_COPYTEXTURES:

				if(IsDlgButtonChecked(hWnd, IDC_CHECK_COPYTEXTURES) == BST_CHECKED)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_EDIT_TEXTUREFOLDER),   TRUE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_TEXTUREFOLDER), TRUE);
				}
				else
				{
					EnableWindow(GetDlgItem(hWnd, IDC_EDIT_TEXTUREFOLDER),   FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_TEXTUREFOLDER), FALSE);
				}

				return TRUE;

			case IDC_BUTTON_DESTINATIONFILE:

				maxFilterList.Append(_T("SGZ scene files (*.sgz)"));
				maxFilterList.Append(_T("*.sgz"));

				memset(&ofn, 0, sizeof(OPENFILENAME));
				memset(szAuxBuffer, 0, MAX_PATH);

				ofn.lStructSize       = sizeof(OPENFILENAME);
				ofn.hwndOwner         = NULL;
				ofn.hInstance         = NULL;//hInstance;
				ofn.lpstrFilter       = maxFilterList;
				ofn.lpstrCustomFilter = NULL;
				ofn.nMaxCustFilter    = 0;
				ofn.nFilterIndex      = 1;
				ofn.lpstrFile         = szAuxBuffer;
				ofn.nMaxFile          = MAX_PATH;
				ofn.lpstrFileTitle    = NULL;
				ofn.nMaxFileTitle     = 0;
				ofn.lpstrInitialDir   = NULL;
				ofn.lpstrTitle        = "Choose scene file";
				ofn.Flags             = OFN_OVERWRITEPROMPT;

				if(GetSaveFileName(&ofn))
				{
					g_SGZMaxTools.m_strFile = ofn.lpstrFile;
					pMaxEdit = GetICustEdit(GetDlgItem(hWnd, IDC_EDIT_DESTINATIONFILE));
					pMaxEdit->SetText((char*)g_SGZMaxTools.m_strFile.data());
					ReleaseICustEdit(pMaxEdit);
				}

				return TRUE;

			case IDC_BUTTON_TEXTUREFOLDER:

				BROWSEINFO   bi;
				LPITEMIDLIST pidl;

				memset(&bi, 0, sizeof(BROWSEINFO));

				bi.hwndOwner = hWnd;
				bi.lpszTitle = "Choose destination folder";

				pidl = SHBrowseForFolder(&bi);

				if(pidl)
				{
					SHGetPathFromIDList(pidl, szAuxBuffer);
					g_SGZMaxTools.m_strTexturePath = szAuxBuffer;
					pMaxEdit = GetICustEdit(GetDlgItem(hWnd, IDC_EDIT_TEXTUREFOLDER));
					pMaxEdit->SetText((char*)g_SGZMaxTools.m_strTexturePath.data());
					ReleaseICustEdit(pMaxEdit);
				}

				return TRUE;

			case IDC_CHECK_RENDERTOAVI:

				EnableWindow(GetDlgItem(hWnd, IDC_EDIT_AVIFILE),   IsDlgButtonChecked(hWnd, IDC_CHECK_RENDERTOAVI) == BST_CHECKED ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_AVIFILE), IsDlgButtonChecked(hWnd, IDC_CHECK_RENDERTOAVI) == BST_CHECKED ? TRUE : FALSE);

				return TRUE;

			case IDC_BUTTON_AVIFILE:

				maxFilterList.Append(_T("AVI files (*.avi)"));
				maxFilterList.Append(_T("*.avi"));

				memset(&ofn, 0, sizeof(OPENFILENAME));
				memset(szAuxBuffer, 0, MAX_PATH);

				ofn.lStructSize       = sizeof(OPENFILENAME);
				ofn.hwndOwner         = NULL;
				ofn.hInstance         = NULL;//hInstance;
				ofn.lpstrFilter       = maxFilterList;
				ofn.lpstrCustomFilter = NULL;
				ofn.nMaxCustFilter    = 0;
				ofn.nFilterIndex      = 1;
				ofn.lpstrFile         = szAuxBuffer;
				ofn.nMaxFile          = MAX_PATH;
				ofn.lpstrFileTitle    = NULL;
				ofn.nMaxFileTitle     = 0;
				ofn.lpstrInitialDir   = NULL;
				ofn.lpstrTitle        = "Choose avi file";
				ofn.Flags             = OFN_OVERWRITEPROMPT;

				if(GetSaveFileName(&ofn))
				{
					g_SGZMaxTools.m_strAviFile = ofn.lpstrFile;
					pMaxEdit = GetICustEdit(GetDlgItem(hWnd, IDC_EDIT_AVIFILE));
					pMaxEdit->SetText((char*)g_SGZMaxTools.m_strAviFile.data());
					ReleaseICustEdit(pMaxEdit);
				}

				return TRUE;

			case IDC_BUTTON_PREVIEW:
			case IDC_BUTTON_EXPORT:

				if(LOWORD(wParam) == IDC_BUTTON_PREVIEW)
				{
					g_SGZMaxTools.m_bPreview = true;
				}
				else if(LOWORD(wParam) == IDC_BUTTON_EXPORT)
				{
					g_SGZMaxTools.m_bPreview = false;
				}

				if(!g_SGZMaxTools.PrepareExportPreview(g_SGZMaxTools.m_bPreview))
				{
					// Error
					return TRUE;
				}

				if(g_SGZMaxTools.m_bPreview) g_SGZMaxTools.Preview();
				else						 g_SGZMaxTools.Export();

				g_SGZMaxTools.FinishExportPreview();

				return TRUE;

			default:

				break;
			}

			break;

		default:

			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool FillCameraCombobox(HWND hWndCombo, INode *pMaxNode)
//
//  - Purpose   : Fills a combo box with all cameras found in a hierarchy.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::FillCameraCombobox(HWND hWndCombo, INode *pMaxNode)
{
	if(pMaxNode == m_pMaxInterface->GetRootNode())
	{
		SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);
	}

	ObjectState os = pMaxNode->EvalWorldState(0);

	// If it's a camera, add it to the combo

	if(os.obj)
	{
		if(os.obj->SuperClassID() == CAMERA_CLASS_ID)
		{
			int nIndex = SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)pMaxNode->GetName());

			if(nIndex == 0)
			{
				SendMessage(hWndCombo, CB_SETCURSEL, (WPARAM)nIndex, 0);
			}
			else if(m_strPreviewCamera != "")
			{
				if(!strcmp(pMaxNode->GetName(), m_strPreviewCamera.data()))
				{
					SendMessage(hWndCombo, CB_SETCURSEL, (WPARAM)nIndex, 0);
				}
			}
		}
	}

	// Recurse

	for(int nCount = 0; nCount < pMaxNode->NumberOfChildren(); nCount++)
	{
		FillCameraCombobox(hWndCombo, pMaxNode->GetChildNode(nCount));
	}

	return true;
}