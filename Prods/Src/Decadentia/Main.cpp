#pragma warning(disable:4786) // 255 char id truncation.
#pragma warning(disable:4018) // Signed/unsigned mismatch
#pragma warning(disable:4244) // Conversion with possible loss of data
#pragma warning(disable:4503) // Decorated name length exceeded


#include "DemoSystem.h"
#include "Resource.h"


std::string g_strDatFile;

HINSTANCE g_hInstance;

int   g_nAASamples  = 1;
float g_fAspect     = 2.0f;
bool  g_bLoop       = false;
bool  g_bLowQuality = false; // Not implemented
bool  g_bWindowed   = false;

char* g_pszAspects[] =
{
	"4:3", "5:4", "16:9", "16:10", "2:1", NULL
};

float g_pfAspects[] =
{
	1.3333f, 1.25f, 1.7777f, 1.6f, 2.0f, -1.0f
};


// CONFIG DIALOG PROCEDURE //////////////////////////////////////////////////////


BOOL CALLBACK ConfigDlgProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	int i, index, nTempSamples, nTempPixelFormat;

	DWORD dwStyle;

	static char szTemp[MAX_PATH];
	static int  winx, winy, downx, downy;
	static bool bDrag = false;

	POINT mousepos;
	GetCursorPos(&mousepos);

	switch(uMessage)
	{

	case WM_INITDIALOG:

		// Put into taskbar

		dwStyle = GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_APPWINDOW;
		SetWindowLong(hWnd, GWL_EXSTYLE, dwStyle);

		// Init combo boxes

		SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA),     CB_RESETCONTENT, 0, 0);
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_ASPECT), CB_RESETCONTENT, 0, 0);

		i = 0;

		// Init Aspect combo box

		while(g_pszAspects[i])
		{
		    index = SendMessage(GetDlgItem(hWnd, IDC_COMBO_ASPECT), CB_ADDSTRING, 0, (LPARAM)(LPCSTR)g_pszAspects[i]);
			SendMessage(GetDlgItem(hWnd, IDC_COMBO_ASPECT), CB_SETITEMDATA, index, i);

			if(i == 0)
			{
				SendMessage(GetDlgItem(hWnd, IDC_COMBO_ASPECT), CB_SETCURSEL, (WPARAM)index, 0);
			}

			i++;
		}

		// Init Antialiasing combo box

		index = SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_ADDSTRING, 0, (LPARAM)(LPCSTR)"Disabled");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_SETCURSEL, (WPARAM)index, 0);
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_SETITEMDATA, index, 0);

		for(i = 0; i < UtilGL::RenderContext::GetNumMultisampleModes(); i++)
		{
			UtilGL::RenderContext::GetNumMultisamples(i, &nTempSamples, &nTempPixelFormat);

			sprintf(szTemp, "%u Samples", nTempSamples);

			index = SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_ADDSTRING, 0, (LPARAM)(LPCSTR)szTemp);
			SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_SETITEMDATA, index, nTempSamples);
		}

		// Init checkboxes

		CheckDlgButton(hWnd, IDC_CHECK_LOOP,     g_bLoop     ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hWnd, IDC_CHECK_WINDOWED, g_bWindowed ? BST_CHECKED : BST_UNCHECKED);

		SetCapture(hWnd);
		return 0;

	case WM_COMMAND:

		if(LOWORD(wParam) == IDOK)
		{
			// Get AA
			index = SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_GETCURSEL, 0, 0);

			if(index > 0)
			{
				g_nAASamples = SendMessage(GetDlgItem(hWnd, IDC_COMBO_AA), CB_GETITEMDATA, index, 0);
			}

			// Get Aspect
			index = SendMessage(GetDlgItem(hWnd, IDC_COMBO_ASPECT), CB_GETCURSEL, 0, 0);
			index = SendMessage(GetDlgItem(hWnd, IDC_COMBO_ASPECT), CB_GETITEMDATA, index, 0);
			g_fAspect = g_pfAspects[index];

			// Get Loop
			g_bLoop = IsDlgButtonChecked(hWnd, IDC_CHECK_LOOP) == BST_CHECKED;

			// Get Windowed
			g_bWindowed = IsDlgButtonChecked(hWnd, IDC_CHECK_WINDOWED) == BST_CHECKED;

			EndDialog(hWnd, 1);
			return TRUE;
		}

		if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, 0);
			return TRUE;
		}

		return FALSE;

	case WM_LBUTTONDOWN:

		downx = LOWORD(lParam);
		downy = HIWORD(lParam);
		bDrag = true;

		return TRUE;

	case WM_LBUTTONUP:

		bDrag = false;

		return TRUE;

	case WM_MOUSEMOVE:

		if(bDrag)
		{
			SetWindowPos(hWnd, HWND_TOPMOST, mousepos.x - downx, mousepos.y - downy, 0, 0, SWP_NOSIZE);
			return TRUE;
		}

		return FALSE;

	default:

		break;

	}

	return FALSE;
}


// DEMO CLASS ///////////////////////////////////////////////////////////////////


class CDemoDecadentia : public CDemo
{


public:


	CDemoDecadentia()  { m_pTexLoading = NULL; }
	~CDemoDecadentia() { }

	// Window Procedure

	LRESULT WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
	{
		switch(uMessage)
		{

		case WM_CREATE:

			return 0;

		case WM_CLOSE:

			this->RequestExit();
			return TRUE;

		case WM_DESTROY:

			PostQuitMessage(0);
			return TRUE;

		case WM_SIZE:

			if(this->IsActiveWindow())
			{
				this->SetDemoViewport();
				UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
				gluPerspective(60.0f, this->GetAspect(), 1.0f, 3000.0f);
			}

			return TRUE;

		case WM_KEYDOWN:

			if(wParam == VK_ESCAPE)
			{
				this->RequestExit();
				return TRUE;
			}

			break;

		default:

			break;

		}

		return ::DefWindowProc(hWnd, uMessage, wParam, lParam);
	}

	bool CheckRequirements()
	{
		PIXELFORMATDESCRIPTOR pfd;

		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

		if(!UtilGL::RenderContext::GetCurrentPixelFormat(&pfd))
		{
			CLogger::ErrorWindow("Error retrieving pixel format");
			return false;
		}

		if(!(!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED)))
		{
			CLogger::ErrorWindow("Selected pixel format not hardware accelerated");
			return false;
		}

		if(!UtilGL::Extensions::GetAvailableExtensions()->OpenGL12)
		{
			CLogger::ErrorWindow("No OpenGL 1.2 compatible driver found.\nPlease install new drivers.");
			return false;
		}

		if(!UtilGL::Extensions::GetAvailableExtensions()->ARB_texture_cube_map)
		{
			CLogger::ErrorWindow("No ARB_texture_cube_map extension found");
			return false;
		}

		if(!DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DEMOCONFIG), NULL, ConfigDlgProc))
		{
			return false;
		}

		if(g_nAASamples > 1)
		{
			// Re create render context with AA support

			UtilGL::RenderContext::UseMultisampling(g_nAASamples);
			UtilGL::RenderContext::Delete();

			if(!UtilGL::RenderContext::Create(pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits))
			{
				return false;
			}
		}

		if(g_bLowQuality)
		{
			this->FreeScript();

			if(!this->LoadScript("demolow.txt", g_strDatFile))
			{
				return false;
			}
		}

		this->SetLoop  (g_bLoop);
		this->SetAspect(g_fAspect);
		this->ConfigDemoWindow(this->GetDemoWindowWidth(), this->GetDemoWindowHeight(), g_bWindowed == false);

		return true;
	}

	void OnLoadStart()
	{
		std::string strLoadBmp("data\\textures\\loading\\loading.jpg");

		CInput* pInput = LoadFileFromDat(strLoadBmp, false, g_strDatFile);

		if(pInput)
		{
			UtilGL::Texturing::STexLoadOptions texOptions;

			m_pTexLoading = new UtilGL::Texturing::CTexture2D;
			m_pTexLoading->Load(pInput, strLoadBmp, GetFileExt(strLoadBmp), false, &texOptions);

			delete pInput;
		}
	}

	void OnLoadFinish()
	{
		SAFE_DELETE(m_pTexLoading);
	}

	void OnLoading(float fPercentage)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 90, 800, 420);

		UtilGL::Rendering::DrawCenteredQuad(m_pTexLoading,
									0.5f, 0.5f, 1.28f, 1.219f,
									0.0f, CVector4(1.0f, 1.0f, 1.0f, 1.0f),
									UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);

		float fX     = 0.37875f;
		float fY     = 0.60714f;
		float fWidth = 0.24000f;

		UtilGL::Rendering::DrawRect(NULL,
									fX, fY, fWidth * fPercentage * 0.01f, 0.007142857f,
									0.0f, CVector4(0.48627f, 0.0f, 0.0f, 1.0f),
									UtilGL::States::BLEND_ONE, UtilGL::States::BLEND_ZERO);

		glFinish();
		SwapBuffers(this->GetHDC());
	}

	void OnFrameStart  (float fDemoTime, int nNumFrame) { LOG.Write("\nFrame %u, time %f", nNumFrame, fDemoTime); }
	void OnFrameFinish (float fDemoTime, int nNumFrame) { LOG.Write("\nEndframe"); }

	void OnEffectStart (CEffect* pEffect) { LOG.Write("\n  -%s", pEffect->GetFXName().data()); }


private:


	UtilGL::Texturing::CTexture2D* m_pTexLoading;

};


// WINMAIN //////////////////////////////////////////////////////////////////////


int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine,
				   int       nCmdShow)
{
	g_hInstance = hInstance;

	if(!strstr(lpCmdLine, "-nodestalpha"))
	{
		UtilGL::RenderContext::UseDestinationAlpha(8);
	}

	if(strstr(lpCmdLine, "-log"))
	{
		LOG.Start("log.txt");
		CMemTracer::Instance().SetEnabled(true);
	}

	CDemoDecadentia demo;

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();

	g_strDatFile = FileExists("demo.dat") ? "demo.dat" : "";

	if(!CPluginManager::Instance().Init(""))
	{
		return FALSE;
	}

	if(!demo.LoadScript("demo.txt", g_strDatFile))
	{
		return FALSE;
	}

	BOOL result = demo.Run(g_strDatFile, &texOptions) ? TRUE : FALSE;

	return result;
}
