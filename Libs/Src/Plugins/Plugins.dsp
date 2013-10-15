# Microsoft Developer Studio Project File - Name="Plugins" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Plugins - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Plugins.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Plugins.mak" CFG="Plugins - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Plugins - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plugins - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Plugins - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGINS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_WINDOWS" /D "_USRDLL" /D "PLUGINS_EXPORTS" /D "NDEBUG" /D "WIN32" /D "_MBCS" /Yu"Plugins.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib freeimage.lib bass.lib glu32.lib /nologo /dll /machine:I386 /nodefaultlib:"libc" /out:"..\..\bin\sgz_plugins.dll" /implib:"..\..\lib\sgz_plugins.lib" /libpath:"..\..\lib" /libpath:"..\..\external\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Plugins - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGINS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_WINDOWS" /D "_USRDLL" /D "PLUGINS_EXPORTS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Yu"Plugins.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib freeimage.lib bass.lib glu32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libc" /out:"..\..\bin\sgz_pluginsd.dll" /implib:"..\..\lib\sgz_pluginsd.lib" /pdbtype:sept /libpath:"..\..\lib" /libpath:"..\..\external\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Plugins - Win32 Release"
# Name "Plugins - Win32 Debug"
# Begin Group "Effects"

# PROP Default_Filter ""
# Begin Group "Effects.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\FXBillboardScene.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXBoxBlur.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXBoxBlurTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXClearFramebuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXCreateExplodeBranches.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXCreationRays.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXCrossFade.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXDiscoFloor.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXDistortedBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXDistortion.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXExplodingBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXFade.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXFadeInvert.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXFog.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXMosaic.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXMotionBlur.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXMotionBlurTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXQuadGalore.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXQuadGalore2.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRadialBlur.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRadialBlur2.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRadialBlur2Texture.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRadialBlurTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRecursiveBlur.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRecursiveBlurTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRenderToTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRings.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRTTHilights.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRTTMotionBlurHilights.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXRTTRadialHilights.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXScene.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSceneConstruction.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSceneDOF.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSceneExplosion.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSceneGlow.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXShinyScene.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSplineBillboards.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSplineEvaluator.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSplineEvParticles.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSplineEvSprite.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSplineEvStencilSprite.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXSplineEvTextured.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXStencilMaskBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXStencilState.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXTelevision.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXTelevisionBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXTextureOnObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXTransExplodingCubes.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXTransToCubedBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXTwistedObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXViewport.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXViewportPixels.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FXWater.h
# End Source File
# End Group
# Begin Group "Effects.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FXBillboardScene.cpp
# End Source File
# Begin Source File

SOURCE=.\FXBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FXBoxBlur.cpp
# End Source File
# Begin Source File

SOURCE=.\FXBoxBlurTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\FXClearFramebuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\FXCreateExplodeBranches.cpp
# End Source File
# Begin Source File

SOURCE=.\FXCreationRays.cpp
# End Source File
# Begin Source File

SOURCE=.\FXCrossFade.cpp
# End Source File
# Begin Source File

SOURCE=.\FXDiscoFloor.cpp
# End Source File
# Begin Source File

SOURCE=.\FXDistortedBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FXDistortion.cpp
# End Source File
# Begin Source File

SOURCE=.\FXExplodingBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FXFade.cpp
# End Source File
# Begin Source File

SOURCE=.\FXFadeInvert.cpp
# End Source File
# Begin Source File

SOURCE=.\FXFog.cpp
# End Source File
# Begin Source File

SOURCE=.\FXMosaic.cpp
# End Source File
# Begin Source File

SOURCE=.\FXMotionBlur.cpp
# End Source File
# Begin Source File

SOURCE=.\FXMotionBlurTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\FXQuadGalore.cpp
# End Source File
# Begin Source File

SOURCE=.\FXQuadGalore2.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRadialBlur.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRadialBlur2.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRadialBlur2Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRadialBlurTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRecursiveBlur.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRecursiveBlurTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRenderToTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRings.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRTTHilights.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRTTMotionBlurHilights.cpp
# End Source File
# Begin Source File

SOURCE=.\FXRTTRadialHilights.cpp
# End Source File
# Begin Source File

SOURCE=.\FXScene.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSceneConstruction.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSceneDOF.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSceneExplosion.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSceneGlow.cpp
# End Source File
# Begin Source File

SOURCE=.\FXShinyScene.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSplineBillboards.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSplineEvaluator.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSplineEvParticles.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSplineEvSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSplineEvStencilSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\FXSplineEvTextured.cpp
# End Source File
# Begin Source File

SOURCE=.\FXStencilMaskBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FXStencilState.cpp
# End Source File
# Begin Source File

SOURCE=.\FXTelevision.cpp
# End Source File
# Begin Source File

SOURCE=.\FXTelevisionBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FXTextureOnObject.cpp
# End Source File
# Begin Source File

SOURCE=.\FXTransExplodingCubes.cpp
# End Source File
# Begin Source File

SOURCE=.\FXTransToCubedBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FXTwistedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\FXViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\FXViewportPixels.cpp
# End Source File
# Begin Source File

SOURCE=.\FXWater.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\FX_Common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\FX_Common.h
# End Source File
# End Group
# Begin Group "Variables"

# PROP Default_Filter ""
# Begin Group "Variables.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\VarColor.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VarCombo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VarFloat.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VarInt.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VarSpeed.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VarString.h
# End Source File
# End Group
# Begin Group "Variables.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VarColor.cpp
# End Source File
# Begin Source File

SOURCE=.\VarCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\VarFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\VarInt.cpp
# End Source File
# Begin Source File

SOURCE=.\VarSpeed.cpp
# End Source File
# Begin Source File

SOURCE=.\VarString.cpp
# End Source File
# End Group
# End Group
# Begin Group "Curves"

# PROP Default_Filter ""
# Begin Group "Curves.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\CurveSegmentSet.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSegmentSetLinear.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSegmentSetStep.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSpline.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSplineBezier.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSplineCatmullRom.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSplineHermite.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CurveSplineTCB.h
# End Source File
# End Group
# Begin Group "Curves.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CurveSegmentSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSegmentSetLinear.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSegmentSetStep.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSpline.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSplineBezier.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSplineCatmullRom.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSplineHermite.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSplineTCB.cpp
# End Source File
# End Group
# End Group
# Begin Group "Functions"

# PROP Default_Filter ""
# Begin Group "Functions.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\FuncConstant.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncFFT.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncInvSawTooth.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncQuad.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncRandom.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncSawTooth.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncSine.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FuncTriangle.h
# End Source File
# End Group
# Begin Group "Functions.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FuncConstant.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncFFT.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncInvSawTooth.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncQuad.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncSawTooth.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncSine.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncTriangle.cpp
# End Source File
# End Group
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Group "Resources.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\ResourceScene.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ResourceSpline.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ResourceTexture2D.h
# End Source File
# End Group
# Begin Group "Resources.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ResourceScene.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceSpline.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceTexture2D.cpp
# End Source File
# End Group
# End Group
# Begin Group "Filters"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\PluginFactory.h
# End Source File
# Begin Source File

SOURCE=.\Plugins.cpp
# ADD CPP /Yc""
# End Source File
# Begin Source File

SOURCE=..\..\Include\Plugins.h
# End Source File
# End Target
# End Project
