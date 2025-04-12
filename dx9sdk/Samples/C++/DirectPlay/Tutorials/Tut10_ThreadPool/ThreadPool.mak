# Microsoft Developer Studio Generated NMAKE File, Based on ThreadPool.dsp
!IF "$(CFG)" == ""
CFG=ThreadPool - Win32 Debug Unicode
!MESSAGE No configuration specified. Defaulting to ThreadPool - Win32 Debug Unicode.
!ENDIF 

!IF "$(CFG)" != "ThreadPool - Win32 Debug Unicode" && "$(CFG)" != "ThreadPool - Win32 Release Unicode" && "$(CFG)" != "ThreadPool - Win32 Release" && "$(CFG)" != "ThreadPool - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ThreadPool.mak" CFG="ThreadPool - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ThreadPool - Win32 Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "ThreadPool - Win32 Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "ThreadPool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ThreadPool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ThreadPool - Win32 Debug Unicode"

OUTDIR=.\ThreadPool___Win32_Debug_Unicode
INTDIR=.\ThreadPool___Win32_Debug_Unicode
# Begin Custom Macros
OutDir=.\ThreadPool___Win32_Debug_Unicode
# End Custom Macros

ALL : "$(OUTDIR)\ThreadPool.exe"


CLEAN :
	-@erase "$(INTDIR)\dxutil.obj"
	-@erase "$(INTDIR)\ThreadPool.obj"
	-@erase "$(INTDIR)\ThreadPool.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ThreadPool.exe"
	-@erase "$(OUTDIR)\ThreadPool.ilk"
	-@erase "$(OUTDIR)\ThreadPool.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\common\include" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Fp"$(INTDIR)\ThreadPool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ThreadPool.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ThreadPool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=dplay.lib dxguid.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ThreadPool.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ThreadPool.exe" /pdbtype:sept /IGNORE:4089 /IGNORE:4078 
LINK32_OBJS= \
	"$(INTDIR)\ThreadPool.obj" \
	"$(INTDIR)\dxutil.obj" \
	"$(INTDIR)\ThreadPool.res"

"$(OUTDIR)\ThreadPool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ThreadPool - Win32 Release Unicode"

OUTDIR=.\ThreadPool___Win32_Release_Unicode
INTDIR=.\ThreadPool___Win32_Release_Unicode
# Begin Custom Macros
OutDir=.\ThreadPool___Win32_Release_Unicode
# End Custom Macros

ALL : "$(OUTDIR)\ThreadPool.exe"


CLEAN :
	-@erase "$(INTDIR)\dxutil.obj"
	-@erase "$(INTDIR)\ThreadPool.obj"
	-@erase "$(INTDIR)\ThreadPool.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ThreadPool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\..\common\include" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Fp"$(INTDIR)\ThreadPool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ThreadPool.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ThreadPool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=dplay.lib dxguid.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ThreadPool.pdb" /machine:I386 /out:"$(OUTDIR)\ThreadPool.exe" /IGNORE:4089 /IGNORE:4078 
LINK32_OBJS= \
	"$(INTDIR)\ThreadPool.obj" \
	"$(INTDIR)\dxutil.obj" \
	"$(INTDIR)\ThreadPool.res"

"$(OUTDIR)\ThreadPool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ThreadPool - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ThreadPool.exe"


CLEAN :
	-@erase "$(INTDIR)\dxutil.obj"
	-@erase "$(INTDIR)\ThreadPool.obj"
	-@erase "$(INTDIR)\ThreadPool.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ThreadPool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\..\common\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Fp"$(INTDIR)\ThreadPool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ThreadPool.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ThreadPool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=dplay.lib dxguid.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ThreadPool.pdb" /machine:I386 /out:"$(OUTDIR)\ThreadPool.exe" /IGNORE:4089 /IGNORE:4078 
LINK32_OBJS= \
	"$(INTDIR)\ThreadPool.obj" \
	"$(INTDIR)\dxutil.obj" \
	"$(INTDIR)\ThreadPool.res"

"$(OUTDIR)\ThreadPool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ThreadPool - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ThreadPool.exe"


CLEAN :
	-@erase "$(INTDIR)\dxutil.obj"
	-@erase "$(INTDIR)\ThreadPool.obj"
	-@erase "$(INTDIR)\ThreadPool.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ThreadPool.exe"
	-@erase "$(OUTDIR)\ThreadPool.ilk"
	-@erase "$(OUTDIR)\ThreadPool.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\common\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Fp"$(INTDIR)\ThreadPool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ThreadPool.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ThreadPool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=dplay.lib dxguid.lib comctl32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ThreadPool.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ThreadPool.exe" /pdbtype:sept /IGNORE:4089 /IGNORE:4078 
LINK32_OBJS= \
	"$(INTDIR)\ThreadPool.obj" \
	"$(INTDIR)\dxutil.obj" \
	"$(INTDIR)\ThreadPool.res"

"$(OUTDIR)\ThreadPool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ThreadPool.dep")
!INCLUDE "ThreadPool.dep"
!ELSE 
!MESSAGE Warning: cannot find "ThreadPool.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ThreadPool - Win32 Debug Unicode" || "$(CFG)" == "ThreadPool - Win32 Release Unicode" || "$(CFG)" == "ThreadPool - Win32 Release" || "$(CFG)" == "ThreadPool - Win32 Debug"
SOURCE=.\ThreadPool.cpp

"$(INTDIR)\ThreadPool.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ThreadPool.rc

"$(INTDIR)\ThreadPool.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\..\..\Common\src\dxutil.cpp

"$(INTDIR)\dxutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

