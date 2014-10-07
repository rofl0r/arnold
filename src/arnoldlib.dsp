# Microsoft Developer Studio Project File - Name="arnoldlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=arnoldlib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arnoldlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arnoldlib.mak" CFG="arnoldlib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "arnoldlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "arnoldlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "arnoldlib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "arnoldlib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "arnoldlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\release\arnold\lib"
# PROP Intermediate_Dir "..\release\arnold\lib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /dll /machine:I386 /out:"..\cpcload\src\res\arnoldlib/arnoldlib.dll"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLAINDLL_EXPORTS" /YX /FD /c
# ADD CPP /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"precomp.h" /FD /GF /c
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"..\LIB\arnoldlib.lib"

!ELSEIF  "$(CFG)" == "arnoldlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\debug\arnold\lib"
# PROP Intermediate_Dir "..\debug\arnold\lib"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLAINDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"precomp.h" /FD /GZ /GF /c
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"..\LIB\arnoldlib_d.lib"

!ELSEIF  "$(CFG)" == "arnoldlib - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnoldlib___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "arnoldlib___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\release\unicode\arnoldlib"
# PROP Intermediate_Dir "..\release\unicode\arnold\lib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /dll /machine:I386 /out:"..\cpcload\src\res\arnoldlib/arnoldlib.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /dll /machine:I386 /out:"..\cpcload\src\res\arnoldlib/arnoldlib.dll"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE CPP /MT /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"precomp.h" /FD /GF /c
# ADD CPP /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"precomp.h" /FD /GF /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\LIB\arnoldlib.lib"
# ADD LIB32 /nologo /out:"..\LIB\arnoldlibu.lib"

!ELSEIF  "$(CFG)" == "arnoldlib - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "arnoldlib___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "arnoldlib___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\debug\unicode\arnold\lib"
# PROP Intermediate_Dir "..\debug\unicode\arnold\lib"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE CPP /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"precomp.h" /FD /GZ /c
# SUBTRACT BASE CPP /nologo
# ADD CPP /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"precomp.h" /FD /GZ /c
# SUBTRACT CPP /nologo
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\LIB\arnoldlib_d.lib"
# ADD LIB32 /nologo /out:"..\LIB\arnoldlibu_d.lib"

!ENDIF 

# Begin Target

# Name "arnoldlib - Win32 Release"
# Name "arnoldlib - Win32 Debug"
# Name "arnoldlib - Win32 Unicode Release"
# Name "arnoldlib - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\precomp.c
# ADD CPP /Yc"precomp.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRC\CONTRIB\INCLUDE\autoexec.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\INCLUDE\console.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\INCLUDE\cpcapi.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\INCLUDE\cpcfirmware.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\INCLUDE\emushell.h
# End Source File
# Begin Source File

SOURCE=.\expbuf.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\libpngw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\precomp.h
# End Source File
# End Group
# Begin Group "win"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SRC\WIN\cpcemu.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\cpcemu.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\directx\dd.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\DIRECTX\ddraw.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\debugger.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\debugger.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\detectos.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\detectos.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\directx\di.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\DIRECTX\dinput.c
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\directx\ds.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\DIRECTX\dsound.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\filedlg.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\filedlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\global.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\global.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\directx\graphlib.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\host.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\iface.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\iface.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\joy.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\joy.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\GENERAL\LNKLIST\lnklist.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\general\lnklist\lnklist.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\myapp.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\mylistvw.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\mylistvw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\reg.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\reg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\resource.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\settings.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\treev.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\treev.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\win.c
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\win.h
# End Source File
# Begin Source File

SOURCE=.\SRC\WIN\ziphandle.h
# End Source File
# End Group
# Begin Group "cpc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SRC\CPC\amsdos.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\amsdos.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\arnold.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\arnold.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\asic.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\asic.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\asm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\audioevent.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\audioevent.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\bmp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\bmp.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\breakpt.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\breakpt.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cheatdb.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cheatdb.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cheatsys.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cheatsys.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\config.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\config.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cpc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cpc.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cpcdbg.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cpcdefs.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\cpcglob.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\crtc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\crtc.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\dbgitem.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\debug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\debug.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugmain.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugmain.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\device.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\dirstuff.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DISKIMAGE\diskimg.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\diskimage\diskimg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\dissasm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\dissasm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DISKIMAGE\dsk.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\diskimage\dsk.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\dumpym.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\dumpym.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\endian.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\expbuf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\expbuf.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DISKIMAGE\extdsk.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\diskimage\extdsk.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\fdc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\fdc.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\fdd.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\filetool.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\filetool.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\fnp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\fnp.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\garray.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\garray.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\gdebug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\gdebug.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\global.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\host.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DISKIMAGE\iextdsk.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\diskimage\iextdsk.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\ifacegen.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\ifacegen.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\item.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DISKIMAGE\maketrk.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\diskimage\maketrk.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\memdump.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\memdump.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\multface.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\multface.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\packedim.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\packedimage.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\DEBUGGER\parse.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\debugger\parse.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\printer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\printer.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\psg.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\psg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\psgplay.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\psgplay.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\render.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\render.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\render5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\riff.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\riff.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\romfn.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\romfn.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\sampload.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\sampload.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\scrsnap.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\scrsnap.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IFACEGEN\settings.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\snapshot.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\snapshot.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\snapv3.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\snapv3.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\spo256.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\spo256.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\tzx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\tzx.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\voc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\voc.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\wav.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\wav.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\westpha.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\westpha.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\yiq.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\yiq.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\Z80\z80.c
# ADD CPP /W3
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\z80.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\z80daa.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\z80funcs.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\z80funcs2.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\z80macros.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z80\z80tables.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z8536.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CPC\z8536.h
# End Source File
# End Group
# Begin Group "Contribution"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\autoexec.c
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\console.c
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\cpcapi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\emushell.c
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\libpngw.c
# ADD CPP /I "..\zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\resfile.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SRC\CONTRIB\SRC\ziphandle.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project
