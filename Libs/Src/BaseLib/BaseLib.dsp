# Microsoft Developer Studio Project File - Name="BaseLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=BaseLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BaseLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BaseLib.mak" CFG="BaseLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BaseLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BaseLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BaseLib - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\external\include\zlib" /I "..\..\external\include\unrarlib" /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_LIB" /D "NDEBUG" /D "WIN32" /D "_MBCS" /Yu"BaseLib.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\BaseLib.lib"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\external\include\zlib" /I "..\..\external\include\unrarlib" /I "..\..\include" /I "..\..\external\include" /I "..\..\external\include\stlport" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Yu"BaseLib.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\BaseLibd.lib"

!ENDIF 

# Begin Target

# Name "BaseLib - Win32 Release"
# Name "BaseLib - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Include\Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\DatFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Factory.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Input.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\InputFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\InputMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\InputRarFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\InputZipFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Logger.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MemTracer.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Output.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\OutputFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\OutputMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Parser.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Timer.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\WriterASCII.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Bitmap.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DatFile.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InputFile.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InputMemory.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InputRarFile.cpp
# End Source File
# Begin Source File

SOURCE=.\InputZipFile.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Logger.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MemTracer.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OutputFile.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OutputMemory.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Parser.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Timer.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WriterASCII.cpp

!IF  "$(CFG)" == "BaseLib - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseLib - Win32 Debug"

# ADD CPP /Yu"BaseLib.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Group "zlib.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\External\Include\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\infblock.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\infutil.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\unzip.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=..\..\External\Include\zlib\zutil.h
# End Source File
# End Group
# Begin Group "zlib.c"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\External\src\zlib\adler32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\compress.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\crc32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\deflate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\gzio.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\infblock.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\infcodes.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\inffast.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\inflate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\inftrees.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\infutil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\trees.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\uncompr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\unzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\External\src\zlib\zutil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Group "unrarlib"

# PROP Default_Filter ""
# Begin Group "unrarlib.c"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\External\src\unrarlib\unrarlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "unrarlib.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\External\Include\unrarlib\unrarlib.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\BaseLib.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=..\..\Include\BaseLib.h
# End Source File
# Begin Source File

SOURCE=.\Patterns.txt
# End Source File
# End Target
# End Project
