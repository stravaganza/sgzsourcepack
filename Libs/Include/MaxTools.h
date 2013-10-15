#ifndef __MAXTOOLS_H
#define __MAXTOOLS_H


// ==[ Constants ]==============================================================================


#ifndef DLL_EXPORT
	#define DLL_EXPORT	__declspec(dllexport)
#endif

// Unique Class ID

#define SGZMAXTOOLS_CLASS_ID Class_ID(0x302f37c3, 0x59220da1)

// 1 MAX tick = 4.8 miliseconds. ticks * (1 / 4.8f)

#define TICKS_TO_SECONDS(n) ((n) * 0.00020833333333333333333333333333f)
#define SECONDS_TO_TICKS(n) ((n) * 4800.0f)

#define CFGFILE "sgzmaxcfg.cfg"


// ==[ Headers ]================================================================================


#pragma warning(disable : 4786) // Map warnings


// Standard includes
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <string>
#include <algorithm>

// Resources
#include "resource.h"

// 3dsMax SDK
#include <Max.h>
#include <istdplug.h>
#include <stdmat.h>
#include <decomp.h>
#include <shape.h>
#include <interpik.h>
#include <plugapi.h>
#include <utilapi.h>
#include <bmmlib.h>
#include <simpobj.h>

// BaseLib
#include <BaseLib.h>

// Engine
#include <Engine.h>


// ==[ Globals ]================================================================================


class CStravaganzaMaxTools;
class CSGZMaxToolsClassDesc;

// DllMain.cpp:

extern HINSTANCE                  hInstance;
extern CStravaganzaMaxTools       g_SGZMaxTools;
extern CSGZMaxToolsClassDesc      g_MaxToolsClassDesc;


// ==[ Headers ]================================================================================


// MaxTools

#include "ClassDesc.h"
#include "ClassMaxTools.h"
#include "WinPreview.h"
#include "SuperSpraySupport.h"


#endif