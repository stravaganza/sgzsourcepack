# Microsoft Developer Studio Project File - Name="Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="Engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_LIB" /D "NDEBUG" /D "WIN32" /D "_MBCS" /Yu"Engine.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\Engine.lib"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Yu"Engine.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\Engined.lib"

!ENDIF 

# Begin Target

# Name "Engine - Win32 Release"
# Name "Engine - Win32 Debug"
# Begin Group "GeoMath"

# PROP Default_Filter ""
# Begin Group "GeoMath (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\GeoMath.h
# End Source File
# End Group
# Begin Group "GeoMath.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MathUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\Ray.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector2.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector3.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector4.cpp
# End Source File
# End Group
# End Group
# Begin Group "Animation"

# PROP Default_Filter ""
# Begin Group "Animation (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\Animation.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnimPos.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnimPosSampled.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnimRot.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnimRotSampled.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnimScl.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AnimSclSampled.h
# End Source File
# End Group
# Begin Group "Animation (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Animation.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimPosSampled.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimRotSampled.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimSclSampled.cpp
# End Source File
# End Group
# End Group
# Begin Group "SceneGraph"

# PROP Default_Filter ""
# Begin Group "SceneGraph (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\BoundingBox.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\BoundingVolume.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Camera.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Helper.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Light.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Node.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Object.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Renderable.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Scene.h
# End Source File
# End Group
# Begin Group "SceneGraph (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Helper.cpp
# End Source File
# Begin Source File

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\Renderable.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# End Group
# End Group
# Begin Group "NodeVisitors"

# PROP Default_Filter ""
# Begin Group "NodeVisitors (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\EnumVisitor.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\LightingVisitor.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\RenderVisitor.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TransformVisitor.h
# End Source File
# End Group
# Begin Group "NodeVisitors (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EnumVisitor.cpp
# End Source File
# Begin Source File

SOURCE=.\LightingVisitor.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderVisitor.cpp
# End Source File
# Begin Source File

SOURCE=.\TransformVisitor.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\NodeVisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\NodeVisitor.h
# End Source File
# End Group
# Begin Group "Shaders"

# PROP Default_Filter ""
# Begin Group "Shaders (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\ShaderStandard.h
# End Source File
# End Group
# Begin Group "Shaders (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ShaderStandard.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Shader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\Shader.h
# End Source File
# Begin Source File

SOURCE=.\ShaderList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\ShaderList.h
# End Source File
# End Group
# Begin Group "Special FX"

# PROP Default_Filter ""
# Begin Group "Special FX (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\LensFlare.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ParticleSystemStd.h
# End Source File
# End Group
# Begin Group "Special FX (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemStd.cpp
# End Source File
# End Group
# End Group
# Begin Group "SceneView"

# PROP Default_Filter ""
# Begin Group "SceneView (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\AviRenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\SceneView.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\WindowGL.h
# End Source File
# End Group
# Begin Group "SceneView (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AviRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneView.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowGL.cpp
# End Source File
# End Group
# End Group
# Begin Group "OpenGL Util"

# PROP Default_Filter ""
# Begin Group "OpenGL Util (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\Extensions.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\RenderContext.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Rendering.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\States.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Texturing.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Transforming.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\UtilGL.h
# End Source File
# End Group
# Begin Group "OpenGL Util (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Extensions.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderContext.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering.cpp
# End Source File
# Begin Source File

SOURCE=.\States.cpp
# End Source File
# Begin Source File

SOURCE=.\Texturing.cpp
# End Source File
# Begin Source File

SOURCE=.\Transforming.cpp
# End Source File
# Begin Source File

SOURCE=.\UtilGL.cpp
# End Source File
# End Group
# Begin Group "Extensions Lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\External\src\extgl\extgl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\extgl.h
# End Source File
# End Group
# End Group
# Begin Group "ID's"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\IDs.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Group "Misc (h)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\AdvancedParser.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\PerlinNoise.h
# End Source File
# End Group
# Begin Group "Misc (cpp)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AdvancedParser.cpp
# End Source File
# Begin Source File

SOURCE=.\PerlinNoise.cpp
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Engine.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=..\..\Include\Engine.h
# End Source File
# End Target
# End Project
