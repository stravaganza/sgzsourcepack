# Microsoft Developer Studio Project File - Name="MaxTools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MaxTools - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MaxTools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MaxTools.mak" CFG="MaxTools - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MaxTools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxTools - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MaxTools - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MaxTools_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /Ob2 /I "..\..\..\Libs\include" /I "..\..\..\Libs\external\include" /I "c:\archiv~1\3dsmax6\maxsdk\include" /I "c:\archiv~1\3dsMax6\maxsdk\cssdk\include" /I "c:\archiv~1\3dsMax6\maxsdk\samples\Materials" /I "..\..\..\Libs\external\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MaxTools_EXPORTS" /Yu"MaxTools.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 vfw32.lib comctl32.lib maxutil.lib geom.lib core.lib mesh.lib paramblk2.lib bmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib freeimage.lib glu32.lib BaseLib.lib Engine.lib /nologo /dll /machine:I386 /nodefaultlib:"libc" /out:"..\..\bin\MaxTools\SGZMaxTools.dlu" /libpath:"c:\archiv~1\3dsmax6\maxsdk\lib" /libpath:"..\..\..\Libs\lib\\" /libpath:"..\..\..\Libs\external\lib"
# SUBTRACT LINK32 /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "MaxTools - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MaxTools_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Libs\include" /I "..\..\..\Libs\external\include" /I "c:\archiv~1\3dsmax6\maxsdk\include" /I "c:\archiv~1\3dsMax6\maxsdk\cssdk\include" /I "c:\archiv~1\3dsMax6\maxsdk\samples\Materials" /I "..\..\..\Libs\external\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MaxTools_EXPORTS" /Yu"MaxTools.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vfw32.lib comctl32.lib maxutil.lib geom.lib core.lib mesh.lib paramblk2.lib bmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib freeimage.lib glu32.lib BaseLibd.lib Engined.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libc" /out:"..\..\bin\MaxTools\SGZMaxToolsd.dlu" /pdbtype:sept /libpath:"c:\archiv~1\3dsmax6\maxsdk\lib" /libpath:"..\..\..\Libs\lib\\" /libpath:"..\..\..\Libs\external\lib"

!ENDIF 

# Begin Target

# Name "MaxTools - Win32 Release"
# Name "MaxTools - Win32 Debug"
# Begin Group "Documentation"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=".\File specifications.txt"
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BuildAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildNodes.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildPhysique.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildShaders.cpp
# End Source File
# Begin Source File

SOURCE=.\ClassMaxTools.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProc.cpp
# End Source File
# Begin Source File

SOURCE=.\DllMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\WinPreview.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ClassDesc.h
# End Source File
# Begin Source File

SOURCE=.\ClassMaxTools.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\WinPreview.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MaxTools.rc
# End Source File
# Begin Source File

SOURCE=.\title.bmp
# End Source File
# End Group
# Begin Group "MAX Particles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SuperSpraySupport.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MaxTools.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\MaxTools.h
# End Source File
# End Target
# End Project
