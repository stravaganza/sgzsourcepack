# Microsoft Developer Studio Project File - Name="DemoSystem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DemoSystem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DemoSystem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DemoSystem.mak" CFG="DemoSystem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DemoSystem - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DemoSystem - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DemoSystem - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_LIB" /D "NDEBUG" /D "WIN32" /D "_MBCS" /Yu"DemoSystem.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\DemoSystem.lib"

!ELSEIF  "$(CFG)" == "DemoSystem - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Yu"DemoSystem.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\DemoSystemd.lib"

!ENDIF 

# Begin Target

# Name "DemoSystem - Win32 Release"
# Name "DemoSystem - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\Demo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ResourceList.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\SoundSystem.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Demo.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceList.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundSystem.cpp
# End Source File
# End Group
# Begin Group "Plugin System"

# PROP Default_Filter ""
# Begin Group "Plugins.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\Curve.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Effect.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Function.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Resource.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Var.h
# End Source File
# End Group
# Begin Group "Plugins.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Curve.cpp
# End Source File
# Begin Source File

SOURCE=.\Effect.cpp
# End Source File
# Begin Source File

SOURCE=.\Function.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource.cpp
# End Source File
# Begin Source File

SOURCE=.\Var.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Include\Plugin.h
# End Source File
# Begin Source File

SOURCE=.\PluginManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\PluginManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DemoSystem.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=..\..\Include\DemoSystem.h
# End Source File
# End Target
# End Project
