# Microsoft Developer Studio Project File - Name="edit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=edit - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "edit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "edit.mak" CFG="edit - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "edit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "edit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "edit - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "edit - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "edit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\release\edit"
# PROP Intermediate_Dir "..\..\release\edit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MT /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib dxguid.lib dinput.lib arnoldlib.lib unzip32.lib libpng.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\BIN/edit32.exe" /libpath:"..\..\lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "edit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\debug\edit"
# PROP Intermediate_Dir "..\..\debug\edit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib dinput.lib arnolddll_d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\BIN/edit32_d.exe" /pdbtype:sept /libpath:"..\..\lib"

!ELSEIF  "$(CFG)" == "edit - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Unicode Release"
# PROP BASE Intermediate_Dir "Unicode Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\release\unicode\edit"
# PROP Intermediate_Dir "..\..\release\unicode\edit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GF /c
# ADD CPP /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386 /out:"BIN/edit.exe"
# ADD LINK32 winmm.lib dxguid.lib dinput.lib arnolddllw.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"zlib32_d.lib" /nodefaultlib:"unzip32_d.lib" /nodefaultlib:"libpng_d.lib" /out:"..\..\BIN/edit32u.exe" /libpath:"..\..\lib"

!ELSEIF  "$(CFG)" == "edit - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unicode Debug"
# PROP BASE Intermediate_Dir "Unicode Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\debug\unicode\edit"
# PROP Intermediate_Dir "..\..\debug\unicode\edit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /GF /c
# ADD CPP /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"BIN/edit_d.exe" /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib dinput.lib arnolddllw_d.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\BIN/edit32u_d.exe" /pdbtype:sept /libpath:"..\..\lib"

!ENDIF 

# Begin Target

# Name "edit - Win32 Release"
# Name "edit - Win32 Debug"
# Name "edit - Win32 Unicode Release"
# Name "edit - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\app.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\edit.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\edit.rc
# ADD BASE RSC /l 0x406 /i "SRC"
# ADD RSC /l 0x409 /i "SRC" /i "..\..\arnold\src\win"
# End Source File
# Begin Source File

SOURCE=.\SRC\editfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\frame.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\mydoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\MyView.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRC\app.h
# End Source File
# Begin Source File

SOURCE=.\SRC\edit.h
# End Source File
# Begin Source File

SOURCE=.\SRC\edit.inl
# End Source File
# Begin Source File

SOURCE=.\SRC\frame.h
# End Source File
# Begin Source File

SOURCE=.\SRC\helpers.h
# End Source File
# Begin Source File

SOURCE=.\SRC\mydoc.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MyFrame.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MyView.h
# End Source File
# Begin Source File

SOURCE=.\SRC\resource.h
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\SRC\RES\app.ICO
# End Source File
# Begin Source File

SOURCE=.\res\edit.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\RES\edit.rc2
# End Source File
# Begin Source File

SOURCE=.\SRC\RES\mainfram.bmp
# End Source File
# End Group
# Begin Group "IOAPI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\IOAPI\UNZIP\ioapi.c
# ADD CPP /I "..\..\zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\IOAPI\ioapiw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project
