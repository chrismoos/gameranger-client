# Microsoft Developer Studio Project File - Name="gameranger" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=gameranger - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gameranger.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gameranger.mak" CFG="gameranger - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gameranger - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gameranger - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gameranger - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "C:\wxWindows-2.4.2/lib/msw" /I "C:\wxWindows-2.4.2/include" /I "C:\wxWindows-2.4.2/contrib/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "C:\wxWindows-2.4.2/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlib.lib regex.lib png.lib jpeg.lib tiff.lib wxmsw.lib wxxrc.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\wxWindows-2.4.2/lib" /libpath:"C:\wxWindows-2.4.2/contrib/lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gameranger - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gameranger___Win32_Debug"
# PROP BASE Intermediate_Dir "gameranger___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "gameranger___Win32_Debug"
# PROP Intermediate_Dir "gameranger___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "C:\wxWindows-2.4.2/lib/mswd" /I "C:\wxWindows-2.4.2/include" /I "C:\wxWindows-2.4.2/contrib/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /Fp"Debug/gameranger.pch" /YX /Fo"Debug/" /Fd"Debug/" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "C:\wxWindows-2.4.2/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlibd.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswd.lib wxxrcd.lib /nologo /subsystem:windows /pdb:"Debug/gameranger.pdb" /debug /machine:I386 /out:"Debug/gameranger.exe" /pdbtype:sept /libpath:"C:\wxWindows-2.4.2/lib" /libpath:"C:\wxWindows-2.4.2/contrib/lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "gameranger - Win32 Release"
# Name "gameranger - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gameranger.rc
# End Source File
# Begin Source File

SOURCE=.\src\GRChangeIconWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRConnecStatusWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRGameRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRGameRoomWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRIconCache.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRLobby.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRLoginWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRLogWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRMainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRPremiumUserInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRPremiumUserInfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRPrivateMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRRegWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRSecurity.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRUser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRUserInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GRUserInfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\GRBaseDefs.h
# End Source File
# Begin Source File

SOURCE=.\src\GRChangeIconWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRConnectStatusWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRGameRoom.h
# End Source File
# Begin Source File

SOURCE=.\src\GRGameRoomWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRIcon.h
# End Source File
# Begin Source File

SOURCE=.\src\GRIconCache.h
# End Source File
# Begin Source File

SOURCE=.\src\GRLobby.h
# End Source File
# Begin Source File

SOURCE=.\src\GRLoginWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRLogWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRMainWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRPlugin.h
# End Source File
# Begin Source File

SOURCE=.\src\GRPremiumUserInfo.h
# End Source File
# Begin Source File

SOURCE=.\src\GRPremiumUserInfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRPrivateMessage.h
# End Source File
# Begin Source File

SOURCE=.\src\GRProfile.h
# End Source File
# Begin Source File

SOURCE=.\src\GRProtocol.h
# End Source File
# Begin Source File

SOURCE=.\src\GRRegWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\GRSecurity.h
# End Source File
# Begin Source File

SOURCE=.\src\GRUser.h
# End Source File
# Begin Source File

SOURCE=.\src\GRUserInfo.h
# End Source File
# Begin Source File

SOURCE=.\src\GRUserInfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\Main.h
# End Source File
# Begin Source File

SOURCE=.\src\memdebug.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\BUGS.txt
# End Source File
# Begin Source File

SOURCE=.\CHANGE_LOG.txt
# End Source File
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=.\README.txt
# End Source File
# Begin Source File

SOURCE=.\TODO.txt
# End Source File
# End Target
# End Project
