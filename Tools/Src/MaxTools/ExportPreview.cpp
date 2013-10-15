// ==[ File ]===================================================================================
//
//  -Name     : ExportPreview.cpp
//  -Proyect  : MaxTools
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Preview and Export functionality.
//
// =============================================================================================

/*

	19/12/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "MaxTools.h"

	
// ==[ Implementation ]=========================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : PrepareExportPreview(bool bPreview)
//
//  - Purpose   : Prepares the plugin for exporting/previewing. Should be called
//                every time before Export() or Preview().
//                Returns false if some error occured and the exporting/previewing
//                cannot be started.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::PrepareExportPreview(bool bPreview)
{
	this->SaveConfig();
	m_bPreview = bPreview;

	// Disable panel interaction while exporting/previewing

	if(m_hPanel)
	{
		m_bWinEnabled = false;
		EnableWindow(m_hPanel, FALSE);
	}

	if(!g_SGZMaxTools.BuildSceneData())
	{
		return false;
	}

	if(m_bPreview == false)
	{
		// Export file specified?

		if(!m_strFile.data() || m_strFile == "")
		{
			CLogger::ErrorWindow("Specify a file first!");

			SAFE_DELETE(m_pRootNode);
			m_vecMaterials.clear();
			m_vecShaders.clear();

			if(m_hPanel)
			{
				m_bWinEnabled = true;
				EnableWindow(m_hPanel, TRUE);
			}

			return false;
		}
	}

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : FinishExportPreview()
//
//  - Purpose   : Should be called every time after an Export() or Preview().
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::FinishExportPreview()
{
	m_vecMaterials.clear();
	m_vecShaders.clear();
	m_vecBones.clear();

	// Enable panel when done

	if(m_hPanel)
	{
		m_bWinEnabled = true;
		EnableWindow(m_hPanel, TRUE);
	}

	return TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool Export()
//
//  - Purpose   : Exports the MAX scene with all the parameters specified by
//                the user using the dialog.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::Export()
{
	assert(m_pRootNode);

	LOG.Write("\n-Exporting to file %s", m_strFile.data());

	// Create file

	COutputFile outputFile;

	LOG.Write("\n-Creating output file.");

	if(outputFile.Open(m_strFile, false, false) == false)
	{
		CLogger::ErrorWindow("Unable to open file %s", m_strFile.data());
		LOG.Write(" Error!");
		return FALSE;
	}

	LOG.Write(" OK.");
	LOG.Write("\n-Exporting scene:\n");
	m_pRootNode->WriteTopologyToLog();
	LOG.Write("\n");

	// Export node tree

	m_pRootNode->WriteNode(&outputFile, true);
	outputFile.Close();

	// Export shader file

	LOG.Write("\n\n-Exporting %u shaders", m_vecMaterials.size());
	ExportShaders();
	LOG.Write("\n");

	std::vector<CShaderStandard*>::iterator it;

	for(it = m_vecShaders.begin(); it < m_vecShaders.end(); ++it)
	{
		delete *it;
	}

	SAFE_DELETE(m_pRootNode);

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CStravaganzaMaxTools
//
//  - prototype : bool Preview()
//
//  - Purpose   : Renders a real time preview of MAX scene.
//
// -----------------------------------------------------------------------------
bool CStravaganzaMaxTools::Preview()
{
	assert(m_pRootNode);

	HWND hWnd;
	MSG  msg;

	// Create new render context

	UtilGL::RenderContext::Create(32, 24, 8);

	// Create preview window

	CWindowGL* pWindowGL = new CWinPreview;
	hWnd = pWindowGL->Create("Preview", NULL, m_nPreviewResX, m_nPreviewResY, m_bFullscreen, 0);
	pWindowGL->SetAsActiveWindow();
	pWindowGL->SetAspect((1.0f / g_SGZMaxTools.GetViewportHeight()) * 1.3333f);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Create scene view

	CSceneView sceneView;
	sceneView.SetNodeData(m_pRootNode);
	sceneView.SetActiveCamera(m_strPreviewCamera);
	sceneView.LoadResources();

	// Setup avi render if requested

	int nAVIFPS     = 50;
	int nFrameCount = 0;

	CAviRenderer aviRenderer;

	if(m_bRenderToAvi)
	{
		aviRenderer.StartRender(pWindowGL, m_strAviFile, nAVIFPS);
	}

	// Start bucle

	Interval validRange = m_pMaxInterface->GetAnimRange();
	Point3   backCol    = m_pMaxInterface->GetBackGround(SECONDS_TO_TICKS(m_fStartTime), validRange);

	glClearColor(backCol.x, backCol.y, backCol.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(pWindowGL->GetHDC());

	float fTime = 0.0f;
	CTimer timer;
	timer.Start();

	pWindowGL->SetCenteredViewport(1.0f, m_fViewportHeight);
	
	while(true)
	{ 
		// Get message from thread's queue

		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				// Free and return

				if(m_bRenderToAvi)
				{
					aviRenderer.StopRender();
				}

				sceneView.Free();
				m_pRootNode = NULL;
				delete pWindowGL;
				UtilGL::RenderContext::Delete();
				return true;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Update time

		if(m_bRenderToAvi)
		{
			fTime = nFrameCount * (1.0f / nAVIFPS);
			nFrameCount++;
		}
		else
		{
			fTime = timer.Seconds();
		}

		// Check loop

		if(fTime > m_fDuration)
		{
			if(m_bLoop)
			{
				if(m_bRenderToAvi)
				{
					nFrameCount = 0;
				}
				else
				{
					timer.Stop();
					timer.Start();
					fTime = 0.0f;
				}
			}
			else pWindowGL->Destroy();
		}

		// Render

		if(pWindowGL->IsValid())
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			sceneView.Transform(pWindowGL, fTime);
			sceneView.Render   (pWindowGL, fTime, m_ePreviewType, m_bShowStats);
			SwapBuffers(pWindowGL->GetHDC());

			if(m_bRenderToAvi)
			{
				aviRenderer.SaveFrame();
			}
		}
	}

	return true;
}
