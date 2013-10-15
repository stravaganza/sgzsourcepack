#include "DemoSystem.h"


// DEMO CLASS ///////////////////////////////////////////////////////////////////


class CDemoPlayer : public CDemo
{


public:


	static std::string strDatFile;


	CDemoPlayer()  { }
	~CDemoPlayer() { }

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

			if(IsActiveWindow())
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

		default:

			break;

		}

		return DefWindowProc(hWnd, uMessage, wParam, lParam);
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
			CLogger::ErrorWindow("Selected pixel format not hardware accelerated.");
			return false;
		}

		if(!UtilGL::Extensions::GetAvailableExtensions()->OpenGL12)
		{
			CLogger::ErrorWindow("No OpenGL 1.2 compatible driver found.\nPlease install new drivers.");
			return false;
		}

		return true;
	}
};


// WINMAIN //////////////////////////////////////////////////////////////////////


std::string CDemoPlayer::strDatFile = "";


int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine,
				   int       nCmdShow)
{
	if(strstr(lpCmdLine, "-log"))
	{
		LOG.Start("log.txt");
		CMemTracer::Instance().SetEnabled(true);
	}

	if(strstr(lpCmdLine, "-nodestalpha"))
	{
		UtilGL::RenderContext::UseDestinationAlpha(0);
	}
	else
	{
		UtilGL::RenderContext::UseDestinationAlpha(8);
	}

	CDemoPlayer demo;

	UtilGL::Texturing::STexLoadOptions texOptions;
	texOptions.SetDefaults();

	CDemoPlayer::strDatFile = FileExists("demo.dat") ? "demo.dat" : "";

	if(!CPluginManager::Instance().Init(""))
	{
		return FALSE;
	}

	if(!demo.LoadScript("demo.txt", CDemoPlayer::strDatFile))
	{
		return FALSE;
	}

	return demo.Run(CDemoPlayer::strDatFile, &texOptions) ? TRUE : FALSE;
}
