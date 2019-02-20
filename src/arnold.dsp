# Microsoft Developer Studio Project File - Name="arnold" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=arnold - Win32 Debug_SDL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arnold.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arnold.mak" CFG="arnold - Win32 Debug_SDL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "arnold - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Debug_UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Release_UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Debug_SDL" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Release_UPX" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 Release_UNICODE_UPX" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 ReleaseDebug" (based on "Win32 (x86) Application")
!MESSAGE "arnold - Win32 ReleaseDebug_UNICODE" (based on "Win32 (x86) Application")
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
# ADD CPP /nologo /W3 /GX /O2 /I "src" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386 /out:"../arnold.exe" /libpath:"zlib/dll32"
# SUBTRACT LINK32 /debug

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "src" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_DEBUG.exe" /pdbtype:sept /libpath:"zlib/dll32"

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "arnold___Win32_Debug_UNICODE"
# PROP BASE Intermediate_Dir "arnold___Win32_Debug_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "arnold___Win32_Debug_UNICODE"
# PROP Intermediate_Dir "arnold___Win32_Debug_UNICODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "src" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "CPC_LSB_FIRST" /D "MULTIFACE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold2.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_DEBUG_UNICODE.exe" /pdbtype:sept /libpath:"zlib/dll32"

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnold___Win32_Release_UNICODE"
# PROP BASE Intermediate_Dir "arnold___Win32_Release_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "arnold___Win32_Release_UNICODE"
# PROP Intermediate_Dir "arnold___Win32_Release_UNICODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "src" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386 /out:"../arnold_UNICODE.exe" /libpath:"zlib/dll32"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "arnold___Win32_Debug_SDL"
# PROP BASE Intermediate_Dir "arnold___Win32_Debug_SDL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "arnold___Win32_Debug_SDL"
# PROP Intermediate_Dir "arnold___Win32_Debug_SDL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "sdl-1.2.5/include" /I "src" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "SDL" /D "CPC_LSB_FIRST" /D "MULTIFACE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_DEBUG.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib sdl.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_DEBUG_SDL.exe" /pdbtype:sept /libpath:"sdl-1.2.5/lib" /libpath:"zlib/dll32"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnold___Win32_Release_UPX"
# PROP BASE Intermediate_Dir "arnold___Win32_Release_UPX"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "arnold___Win32_Release_UPX"
# PROP Intermediate_Dir "arnold___Win32_Release_UPX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "src" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386 /out:"../arnold.exe" /libpath:"zlib/dll32"
# SUBTRACT LINK32 /debug
# Begin Custom Build
InputDir=\projects\arnold\arnold
InputPath=\projects\arnold\arnold\arnold.exe
InputName=arnold
SOURCE="$(InputPath)"

"$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(InputDir)\upx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnold___Win32_Release_UNICODE_UPX"
# PROP BASE Intermediate_Dir "arnold___Win32_Release_UNICODE_UPX"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "arnold___Win32_Release_UNICODE_UPX"
# PROP Intermediate_Dir "arnold___Win32_Release_UNICODE_UPX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "src" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_UNICODE.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386 /out:"../arnold_UNICODE.exe" /libpath:"zlib/dll32"
# SUBTRACT LINK32 /debug
# Begin Custom Build
InputDir=\projects\arnold\arnold
InputPath=\projects\arnold\arnold\arnold_UNICODE.exe
InputName=arnold_UNICODE
SOURCE="$(InputPath)"

"$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(InputDir)\upx $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnold___Win32_ReleaseDebug"
# PROP BASE Intermediate_Dir "arnold___Win32_ReleaseDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "arnold___Win32_ReleaseDebug"
# PROP Intermediate_Dir "arnold___Win32_ReleaseDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "src" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /out:"../arnold.exe"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_ReleaseDebug.exe" /libpath:"zlib/dll32"

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "arnold___Win32_ReleaseDebug_UNICODE"
# PROP BASE Intermediate_Dir "arnold___Win32_ReleaseDebug_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "arnold___Win32_ReleaseDebug_UNICODE"
# PROP Intermediate_Dir "arnold___Win32_ReleaseDebug_UNICODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "src" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "CPC_LSB_FIRST" /D "MULTIFACE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /out:"../arnold_UNICODE.exe"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ddraw.lib dsound.lib dxguid.lib dinput.lib winmm.lib comctl32.lib advapi32.lib shlwapi.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../arnold_UNICODE.exe" /libpath:"zlib/dll32"

!ENDIF 

# Begin Target

# Name "arnold - Win32 Release"
# Name "arnold - Win32 Debug"
# Name "arnold - Win32 Debug_UNICODE"
# Name "arnold - Win32 Release_UNICODE"
# Name "arnold - Win32 Debug_SDL"
# Name "arnold - Win32 Release_UPX"
# Name "arnold - Win32 Release_UNICODE_UPX"
# Name "arnold - Win32 ReleaseDebug"
# Name "arnold - Win32 ReleaseDebug_UNICODE"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "cpc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpc\amram2.c
# End Source File
# Begin Source File

SOURCE=.\cpc\amsdos.c
# End Source File
# Begin Source File

SOURCE=.\cpc\amsdos.h
# End Source File
# Begin Source File

SOURCE=.\cpc\amxms.c
# End Source File
# Begin Source File

SOURCE=.\cpc\amxms.h
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

SOURCE=.\cpc\audioevent.c
# End Source File
# Begin Source File

SOURCE=.\cpc\audioevent.h
# End Source File
# Begin Source File

SOURCE=.\cpc\autotype.c
# End Source File
# Begin Source File

SOURCE=.\autotype.h
# End Source File
# Begin Source File

SOURCE=.\cpc\autotype.h
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

SOURCE=.\cpc\cassette.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cassette.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cheatdb.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cheatdb.h
# End Source File
# Begin Source File

SOURCE=.\cpc\cpc.c
# End Source File
# Begin Source File

SOURCE=.\cpc\cpc.h
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

SOURCE=.\cpc\csw.c
# End Source File
# Begin Source File

SOURCE=.\cpc\csw.h
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\dbgitem.c
# End Source File
# Begin Source File

SOURCE=.\cpc\device.h
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

SOURCE=.\cpc\fdd.c
# End Source File
# Begin Source File

SOURCE=.\cpc\fdd.h
# End Source File
# Begin Source File

SOURCE=.\cpc\fdi.c
# End Source File
# Begin Source File

SOURCE=.\cpc\fdi.h
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

SOURCE=.\cpc\gunstick.c
# End Source File
# Begin Source File

SOURCE=.\cpc\gunstick.h
# End Source File
# Begin Source File

SOURCE=.\gunstick.h
# End Source File
# Begin Source File

SOURCE=.\cpc\headers.h
# End Source File
# Begin Source File

SOURCE=.\headers.h
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

SOURCE=.\cpc\debugger\item.h
# End Source File
# Begin Source File

SOURCE=.\cpc\kempston.c
# End Source File
# Begin Source File

SOURCE=.\cpc\kempston.h
# End Source File
# Begin Source File

SOURCE=.\cpc\magnum.c
# End Source File
# Begin Source File

SOURCE=.\cpc\magnum.h
# End Source File
# Begin Source File

SOURCE=.\magnum.h
# End Source File
# Begin Source File

SOURCE=.\cpc\memcard.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\memdump.c
# End Source File
# Begin Source File

SOURCE=.\cpc\debugger\memdump.h
# End Source File
# Begin Source File

SOURCE=.\cpc\memory.c
# End Source File
# Begin Source File

SOURCE=.\cpc\memory.h
# End Source File
# Begin Source File

SOURCE=.\cpc\messages.c
# End Source File
# Begin Source File

SOURCE=.\cpc\messages.h
# End Source File
# Begin Source File

SOURCE=.\messages.h
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

SOURCE=.\cpc\pal.c
# End Source File
# Begin Source File

SOURCE=.\cpc\pal.h
# End Source File
# Begin Source File

SOURCE=.\pal.h
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

SOURCE=.\cpc\ramrom.c
# End Source File
# Begin Source File

SOURCE=.\cpc\ramrom.h
# End Source File
# Begin Source File

SOURCE=.\ramrom.h
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

SOURCE=.\cpc\sampload.c
# End Source File
# Begin Source File

SOURCE=.\cpc\sampload.h
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

SOURCE=.\cpc\speech.c
# End Source File
# Begin Source File

SOURCE=.\cpc\spo256.c
# End Source File
# Begin Source File

SOURCE=.\cpc\spo256.h
# End Source File
# Begin Source File

SOURCE=.\cpc\tzx.h
# End Source File
# Begin Source File

SOURCE=.\cpc\tzxold.c
# End Source File
# Begin Source File

SOURCE=.\cpc\voc.c
# End Source File
# Begin Source File

SOURCE=.\cpc\voc.h
# End Source File
# Begin Source File

SOURCE=.\cpc\vortex.h
# End Source File
# Begin Source File

SOURCE=.\vortex.h
# End Source File
# Begin Source File

SOURCE=.\cpc\vrtxram.c
# End Source File
# Begin Source File

SOURCE=.\cpc\vrtxram.h
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

SOURCE=.\win\cassdlg.c
# End Source File
# Begin Source File

SOURCE=.\win\cassdlg.h
# End Source File
# Begin Source File

SOURCE=.\win\chtdb.c
# End Source File
# Begin Source File

SOURCE=.\win\chtdb.h
# End Source File
# Begin Source File

SOURCE=.\win\cmdline.c
# End Source File
# Begin Source File

SOURCE=.\win\cmdline.h
# End Source File
# Begin Source File

SOURCE=.\win\comboh.h
# End Source File
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

SOURCE=.\win\directx\di.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\dinput.c
# End Source File
# Begin Source File

SOURCE=.\win\dispcfg.c
# End Source File
# Begin Source File

SOURCE=.\win\directx\ds.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\dsound.c
# End Source File
# Begin Source File

SOURCE=.\win\file_list.c
# End Source File
# Begin Source File

SOURCE=.\win\file_list.h
# End Source File
# Begin Source File

SOURCE=.\win\filedlg.c
# End Source File
# Begin Source File

SOURCE=.\win\filedlg.h
# End Source File
# Begin Source File

SOURCE=.\win\filetool.c
# End Source File
# Begin Source File

SOURCE=.\win\filetool.h
# End Source File
# Begin Source File

SOURCE=.\win\directx\graphlib.h
# End Source File
# Begin Source File

SOURCE=.\win\host.c
# End Source File
# Begin Source File

SOURCE=.\win\zlibxtra\ioapi.c
# ADD CPP /I "zlib"
# End Source File
# Begin Source File

SOURCE=.\win\joy.c
# End Source File
# Begin Source File

SOURCE=.\win\joy.h
# End Source File
# Begin Source File

SOURCE=.\win\keystack.c
# End Source File
# Begin Source File

SOURCE=.\win\keystack.h
# End Source File
# Begin Source File

SOURCE=.\win\general\lnklist\lnklist.c
# End Source File
# Begin Source File

SOURCE=.\win\general\lnklist\lnklist.h
# End Source File
# Begin Source File

SOURCE=.\win\mess.h
# End Source File
# Begin Source File

SOURCE=.\win\multcfg.c
# End Source File
# Begin Source File

SOURCE=.\win\mylistvw.c
# End Source File
# Begin Source File

SOURCE=.\win\mylistvw.h
# End Source File
# Begin Source File

SOURCE=.\win\ramromcfg.c
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

SOURCE=.\win\romcfg.c
# End Source File
# Begin Source File

SOURCE=.\win\romcfg.h
# End Source File
# Begin Source File

SOURCE=.\win\scrsnap.c
# End Source File
# Begin Source File

SOURCE=.\win\scrsnap.h
# End Source File
# Begin Source File

SOURCE=.\CONTRIB\SRC\unicode.c
# End Source File
# Begin Source File

SOURCE=.\win\win.c
# End Source File
# Begin Source File

SOURCE=.\win\win.h
# End Source File
# Begin Source File

SOURCE=.\win\ym5prop.c
# End Source File
# Begin Source File

SOURCE=.\win\zipsupport.c
# End Source File
# Begin Source File

SOURCE=.\win\zipsupport.h
# End Source File
# Begin Source File

SOURCE=.\win\zipsupui.c
# End Source File
# Begin Source File

SOURCE=.\win\zipsupui.h
# End Source File
# Begin Source File

SOURCE=.\win\zmouse.h
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\roms\amsdose\amsdos.rom
# End Source File
# Begin Source File

SOURCE=.\win\arnold.bmp
# End Source File
# Begin Source File

SOURCE=.\win\arnold.ico
# End Source File
# Begin Source File

SOURCE=.\roms\cpc464e\basic.rom
# End Source File
# Begin Source File

SOURCE=.\roms\cpc6128e\basic.rom
# End Source File
# Begin Source File

SOURCE=.\roms\cpc664e\basic.rom
# End Source File
# Begin Source File

SOURCE=.\win\bin00001.bin
# End Source File
# Begin Source File

SOURCE=.\win\bin00002.bin
# End Source File
# Begin Source File

SOURCE=.\win\bin00003.bin
# End Source File
# Begin Source File

SOURCE=.\win\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\win\cart.ico
# End Source File
# Begin Source File

SOURCE=.\win\cass.ico
# End Source File
# Begin Source File

SOURCE=.\win\disk.ico
# End Source File
# Begin Source File

SOURCE=.\roms\kcc\kccbas.rom
# End Source File
# Begin Source File

SOURCE=.\roms\kcc\kccos.rom
# End Source File
# Begin Source File

SOURCE=.\roms\cpc464e\os.rom
# End Source File
# Begin Source File

SOURCE=.\roms\cpc6128e\os.rom
# End Source File
# Begin Source File

SOURCE=.\roms\cpc664e\os.rom
# End Source File
# Begin Source File

SOURCE=.\roms\ARNOR\PROTEXT.ROM
# End Source File
# Begin Source File

SOURCE=.\win\roms2.bin
# End Source File
# Begin Source File

SOURCE=.\win\snapshot.ico
# End Source File
# Begin Source File

SOURCE=.\roms\cpcplus\system.cpr
# End Source File
# Begin Source File

SOURCE=.\win\toolbar3.bmp
# End Source File
# Begin Source File

SOURCE=.\win\toolbar4.bmp
# End Source File
# Begin Source File

SOURCE=.\roms\ARNOR\UTOPIA.ROM
# End Source File
# End Group
# Begin Group "linux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\configure

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\configure.in

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\display.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\display.h

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\display_sdl.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\global.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\global.h

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\gtkui.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\gtkui.h

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\host.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\ifacegen.c

!IF  "$(CFG)" == "arnold - Win32 Release"

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\ifacegen.h

!IF  "$(CFG)" == "arnold - Win32 Release"

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\keyboard.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\keyboard_sdl.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\main.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Makefile.in

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optimizecflags.sh

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\sdlsound.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\sdlsound.h

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unix\settings.c

!IF  "$(CFG)" == "arnold - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Debug_SDL"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 Release_UNICODE_UPX"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "arnold - Win32 ReleaseDebug_UNICODE"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\arnold.html
# End Source File
# Begin Source File

SOURCE=.\win\arnold.manifest
# End Source File
# Begin Source File

SOURCE=.\build.txt
# End Source File
# Begin Source File

SOURCE=.\notes.txt
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
