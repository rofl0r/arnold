# Microsoft Developer Studio Project File - Name="arnolddll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=arnolddll - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arnolddll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arnolddll.mak" CFG="arnolddll - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "arnolddll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "arnolddll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "arnolddll - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "arnolddll - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "arnolddll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\release\arnold\dll"
# PROP Intermediate_Dir "..\release\arnold\dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARNOLD_EXPORTS" /Yu"precomp.h" /FD /c
# ADD CPP /MT /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARNOLD_EXPORTS" /Yu"precomp.h" /FD /GF /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib dxguid.lib dinput.lib winmm.lib comctl32.lib /nologo /dll /machine:I386 /out:"..\bin/arnold.dll" /implib:"..\lib/arnolddll.lib" /libpath:"..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "arnolddll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\debug\arnold\dll"
# PROP Intermediate_Dir "..\debug\arnold\dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARNOLD_EXPORTS" /Yu"precomp.h" /FD /GZ /c
# ADD CPP /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARNOLD_EXPORTS" /Yu"precomp.h" /FD /GZ /GF /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib uuid.lib dxguid.lib dinput.lib winmm.lib comctl32.lib /nologo /dll /debug /machine:I386 /out:"..\bin/arnold_d.dll" /implib:"..\lib\arnolddll_d.lib" /pdbtype:sept /libpath:"..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "arnolddll - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnolddll___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "arnolddll___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\release\unicode\arnold\dll"
# PROP Intermediate_Dir "..\release\unicode\arnold\dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MD /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARNOLD_EXPORTS" /Yu"precomp.h" /FD /GF /c
# SUBTRACT BASE CPP /nologo
# ADD CPP /MT /W4 /GX /O1 /D "_USRDLL" /D "ARNOLD_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"precomp.h" /FD /GF /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib /nologo /dll /machine:I386 /out:"..\..\bin/arnold.dll"
# ADD LINK32 libpng.lib unzip32.lib zlib32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib dxguid.lib dinput.lib winmm.lib comctl32.lib /nologo /dll /machine:I386 /nodefaultlib:"zlib32_d.lib" /nodefaultlib:"unzip32_d.lib" /nodefaultlib:"libpng_d.lib" /out:"..\bin/arnoldw.dll" /implib:"..\lib/arnolddllw.lib" /libpath:"..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "arnolddll - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "arnolddll___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "arnolddll___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\debug\unicode\arnold\dll"
# PROP Intermediate_Dir "..\debug\unicode\arnold\dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARNOLD_EXPORTS" /Yu"precomp.h" /FD /GZ /GF /c
# SUBTRACT BASE CPP /nologo
# ADD CPP /MDd /W4 /Gm /GX /Zi /Od /D "_USRDLL" /D "ARNOLD_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"precomp.h" /FD /GZ /GF /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib /nologo /dll /debug /machine:I386 /out:"..\..\bin/arnold_d.dll" /pdbtype:sept
# ADD LINK32 zlib32_d.lib unzip32_d.lib libpng_d.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib dxguid.lib dinput.lib winmm.lib comctl32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libpng.lib" /nodefaultlib:"unzip32.lib" /nodefaultlib:"zlib32.lib" /out:"..\bin/arnoldw_d.dll" /implib:"..\lib/arnolddllw_d.lib" /pdbtype:sept /libpath:"..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "arnolddll - Win32 Release"
# Name "arnolddll - Win32 Debug"
# Name "arnolddll - Win32 Unicode Release"
# Name "arnolddll - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\WIN\cpcemu.rc
# End Source File
# Begin Source File

SOURCE=.\SRC\precomp.c
# ADD CPP /W3 /Yc"precomp.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRC\precomp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Contribution"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SRC\WIN\DLL\dll.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\DLL\dll.def
# End Source File
# Begin Source File

SOURCE=..\IOAPI\ioapiw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\unicode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
