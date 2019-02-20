# Microsoft Developer Studio Project File - Name="arnold" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=arnold - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arnold.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arnold.mak" CFG="arnold - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "arnold - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "arnold - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold.exe"

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold2.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "arnold - Win32 Release"
# Name "arnold - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "cpc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpc\amsdos.c
# End Source File
# Begin Source File

SOURCE=.\cpc\amsdos.h
# End Source File
# Begin Source File

SOURCE=.\cpc\arnold.c
# End Source File
# Begin Source File

SOURCE=.\cpc\arnold.h
# End Source File
# Begin Source File

SOURCE=.\cpc\asic.c
# End Source File
# Begin Source File

SOURCE=.\cpc\asic.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\asm.h
# End Source File
# Begin Source File

SOURCE=.\cpc\audioevent.c
# End Source File
# Begin Source File

SOURCE=.\cpc\audioevent.h
# End Source File
# Begin Source File

SOURCE=.\cpc\bmp.c
# End Source File
# Begin Source File

SOURCE=.\cpc\bmp.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\breakpt.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\breakpt.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cheatdb.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cheatdb.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cheatsys.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cheatsys.h
# End Source File
# Begin Source File

SOURCE=.\cpc\config.c
# End Source File
# Begin Source File

SOURCE=.\cpc\config.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cpc.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cpc.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cpcdbg.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cpcdefs.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cpcglob.h
# End Source File
# Begin Source File

SOURCE=.\cpc\crtc.c
# End Source File
# Begin Source File

SOURCE=.\cpc\crtc.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\dbgitem.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\debug.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\debug.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugmain.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugmain.h
# End Source File
# Begin Source File

SOURCE=.\cpc\device.h
# End Source File
# Begin Source File

SOURCE=.\cpc\dirstuff.c
# End Source File
# Begin Source File

SOURCE=.\cpc\dirstuff.h
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\diskimg.c
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\diskimg.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\dissasm.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\dissasm.h
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\dsk.c
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\dsk.h
# End Source File
# Begin Source File

SOURCE=.\cpc\dumpym.c
# End Source File
# Begin Source File

SOURCE=.\cpc\dumpym.h
# End Source File
# Begin Source File

SOURCE=.\cpc\endian.h
# End Source File
# Begin Source File

SOURCE=.\cpc\expbuf.c
# End Source File
# Begin Source File

SOURCE=.\cpc\expbuf.h
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\extdsk.c
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\extdsk.h
# End Source File
# Begin Source File

SOURCE=.\cpc\fdc.c
# End Source File
# Begin Source File

SOURCE=.\cpc\fdc.h
# End Source File
# Begin Source File

SOURCE=.\cpc\fdd.h
# End Source File
# Begin Source File

SOURCE=.\ifacegen\filetool.c
# End Source File
# Begin Source File

SOURCE=.\ifacegen\filetool.h
# End Source File
# Begin Source File

SOURCE=.\ifacegen\fnp.c
# End Source File
# Begin Source File

SOURCE=.\ifacegen\fnp.h
# End Source File
# Begin Source File

SOURCE=.\cpc\garray.c
# End Source File
# Begin Source File

SOURCE=.\cpc\garray.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\gdebug.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\gdebug.h
# End Source File
# Begin Source File

SOURCE=.\ifacegen\global.h
# End Source File
# Begin Source File

SOURCE=.\cpc\host.h
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\iextdsk.c
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\iextdsk.h
# End Source File
# Begin Source File

SOURCE=.\ifacegen\ifacegen.c
# End Source File
# Begin Source File

SOURCE=.\ifacegen\ifacegen.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\item.h
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\maketrk.c
# End Source File
# Begin Source File

SOURCE=.\cpc\diskimage\maketrk.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\memdump.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\memdump.h
# End Source File
# Begin Source File

SOURCE=.\cpc\multface.c
# End Source File
# Begin Source File

SOURCE=.\cpc\multface.h
# End Source File
# Begin Source File

SOURCE=.\cpc\packedim.h
# End Source File
# Begin Source File

SOURCE=.\cpc\packedimage.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\parse.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\parse.h
# End Source File
# Begin Source File

SOURCE=.\cpc\printer.c
# End Source File
# Begin Source File

SOURCE=.\cpc\printer.h
# End Source File
# Begin Source File

SOURCE=.\cpc\psg.c
# End Source File
# Begin Source File

SOURCE=.\cpc\psg.h
# End Source File
# Begin Source File

SOURCE=.\cpc\psgplay.c
# End Source File
# Begin Source File

SOURCE=.\cpc\psgplay.h
# End Source File
# Begin Source File

SOURCE=.\cpc\render.c
# End Source File
# Begin Source File

SOURCE=.\cpc\render.h
# End Source File
# Begin Source File

SOURCE=.\cpc\render5.c
# End Source File
# Begin Source File

SOURCE=.\cpc\riff.c
# End Source File
# Begin Source File

SOURCE=.\cpc\riff.h
# End Source File
# Begin Source File

SOURCE=.\ifacegen\romfn.c
# End Source File
# Begin Source File

SOURCE=.\ifacegen\romfn.h
# End Source File
# Begin Source File

SOURCE=.\cpc\sampload.c
# End Source File
# Begin Source File

SOURCE=.\cpc\sampload.h
# End Source File
# Begin Source File

SOURCE=.\cpc\scrsnap.c
# End Source File
# Begin Source File

SOURCE=.\cpc\scrsnap.h
# End Source File
# Begin Source File

SOURCE=.\ifacegen\settings.h
# End Source File
# Begin Source File

SOURCE=.\cpc\snapshot.c
# End Source File
# Begin Source File

SOURCE=.\cpc\snapshot.h
# End Source File
# Begin Source File

SOURCE=.\cpc\snapv3.c
# End Source File
# Begin Source File

SOURCE=.\cpc\snapv3.h
# End Source File
# Begin Source File

SOURCE=.\cpc\spo256.c
# End Source File
# Begin Source File

SOURCE=.\cpc\spo256.h
# End Source File
# Begin Source File

SOURCE=.\cpc\tzx.c
# End Source File
# Begin Source File

SOURCE=.\cpc\tzx.h
# End Source File
# Begin Source File

SOURCE=.\cpc\voc.c
# End Source File
# Begin Source File

SOURCE=.\cpc\voc.h
# End Source File
# Begin Source File

SOURCE=.\cpc\wav.c
# End Source File
# Begin Source File

SOURCE=.\cpc\wav.h
# End Source File
# Begin Source File

SOURCE=.\cpc\westpha.c
# End Source File
# Begin Source File

SOURCE=.\cpc\westpha.h
# End Source File
# Begin Source File

SOURCE=.\cpc\yiq.c
# End Source File
# Begin Source File

SOURCE=.\cpc\yiq.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80.c
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80daa.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80funcs.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80funcs2.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80macros.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z80\z80tables.h
# End Source File
# Begin Source File

SOURCE=.\cpc\z8536.c
# End Source File
# Begin Source File

SOURCE=.\cpc\z8536.h
# End Source File
# End Group
# Begin Group "win"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win\cpcemu.c
# End Source File
# Begin Source File

SOURCE=.\win\cpcemu.h
# End Source File
# Begin Source File

SOURCE=.\win\cpcemu.rc
# End Source File
# Begin Source File

SOURCE=.\win\directx\dd.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\ddraw.c
# End Source File
# Begin Source File

SOURCE=.\win\debugger.c
# End Source File
# Begin Source File

SOURCE=.\win\debugger.h
# End Source File
# Begin Source File

SOURCE=.\win\detectos.c
# End Source File
# Begin Source File

SOURCE=.\win\detectos.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\di.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\dinput.c
# End Source File
# Begin Source File

SOURCE=.\win\directx\ds.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\dsound.c
# End Source File
# Begin Source File

SOURCE=.\win\filedlg.c
# End Source File
# Begin Source File

SOURCE=.\win\filedlg.h
# End Source File
# Begin Source File

SOURCE=.\win\global.c
# End Source File
# Begin Source File

SOURCE=.\win\global.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\graphlib.h
# End Source File
# Begin Source File

SOURCE=.\win\host.c
# End Source File
# Begin Source File

SOURCE=.\win\iface.c
# End Source File
# Begin Source File

SOURCE=.\win\iface.h
# End Source File
# Begin Source File

SOURCE=.\win\joy.c
# End Source File
# Begin Source File

SOURCE=.\win\joy.h
# End Source File
# Begin Source File

SOURCE=.\win\general\lnklist\lnklist.c
# End Source File
# Begin Source File

SOURCE=.\win\general\lnklist\lnklist.h
# End Source File
# Begin Source File

SOURCE=.\win\myapp.c
# End Source File
# Begin Source File

SOURCE=.\win\myapp.h
# End Source File
# Begin Source File

SOURCE=.\win\mylistvw.c
# End Source File
# Begin Source File

SOURCE=.\win\mylistvw.h
# End Source File
# Begin Source File

SOURCE=.\win\reg.c
# End Source File
# Begin Source File

SOURCE=.\win\reg.h
# End Source File
# Begin Source File

SOURCE=.\win\resource.h
# End Source File
# Begin Source File

SOURCE=.\win\settings.c
# End Source File
# Begin Source File

SOURCE=.\win\treev.c
# End Source File
# Begin Source File

SOURCE=.\win\treev.h
# End Source File
# Begin Source File

SOURCE=.\win\win.c
# End Source File
# Begin Source File

SOURCE=.\win\win.h
# End Source File
# Begin Source File

SOURCE=.\win\ziphandle.c
# End Source File
# Begin Source File

SOURCE=.\win\ziphandle.h
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\expbuf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
