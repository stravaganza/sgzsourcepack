// ==[ File ]===================================================================================
//
//  -Name     : WinPreview.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Preview window class implementation.
//
// =============================================================================================

/*

	15/03/2003  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CWinPreview
//
//  - prototype : LRESULT WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
//
//  - Purpose   : Preview window procedure.
//
// -----------------------------------------------------------------------------
LRESULT CWinPreview::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CCamera*    pCamera;
	static bool bFullscreen = g_SGZMaxTools.GetFullscreen();

	switch(uMessage)
	{

	case WM_CREATE:

		return 0;

	case WM_CLOSE:

		this->Destroy();
		return TRUE;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	case WM_SIZE:

		if(IsActiveWindow())
		{
			// Establish new viewport

			this->SetCenteredViewport(1.0f, g_SGZMaxTools.GetViewportHeight());
			UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

			pCamera = g_SGZMaxTools.GetCamera();

			if(pCamera)
			{
				float fVerticalFOV = CCamera::GetVerticalFOV(pCamera->FOV(), this->GetAspect());
				gluPerspective(fVerticalFOV, this->GetAspect(), pCamera->Near(), pCamera->Far());
			}
			else
			{
				gluPerspective(60.0f, this->GetAspect(), 1.0f, 3000.0f);
			}
		}

		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
			case VK_RETURN:

				bFullscreen = !bFullscreen;
				this->SetSize(g_SGZMaxTools.GetPreviewResX(), g_SGZMaxTools.GetPreviewResY());
				this->SetFullscreen(bFullscreen);
				break;

			case VK_ESCAPE:

				this->Destroy();
				return TRUE;
		}

		break;

	default:

		break;

	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}