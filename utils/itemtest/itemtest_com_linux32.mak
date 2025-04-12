ifneq "$(LINUX_TOOLS_PATH)" ""
TOOL_PATH = $(LINUX_TOOLS_PATH)/
endif

NAME=itemtest_com
SRCROOT=../..
TARGET_PLATFORM=linux32
TARGET_PLATFORM_EXT=
USE_VALVE_BINDIR=0
PWD:=$(shell $(TOOL_PATH)pwd)
# If no configuration is specified, "release" will be used.
ifeq "$(CFG)" ""
	CFG = release
endif

#
#
# CFG=debug
#
#

ifeq "$(CFG)" "debug"

GCC_ExtraCompilerFlags=
GCC_ExtraLinkerFlags=-Wl,--no-as-needed -Wl,--as-needed
GCC_CustomVersionScript=
EntryPoint=
IgnoreAllDefaultLibraries=no
BufferSecurityCheck=Yes
SymbolVisibility=hidden
TreatWarningsAsErrors=false
OptimizerLevel=-gdwarf-4 -g2 $(OptimizerLevel_CompilerSpecific)
SystemLibraries=
DLL_EXT=.so
SYM_EXT=.dbg
FORCEINCLUDES= 
DEFINES= -DVPC -DDEBUG -D_DEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DEXENAME=itemtest_com -DDEV_BUILD -DFRAME_POINTER_OMISSION_DISABLED -DFBXSDK_NEW_API -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2 -DPROJECTDIR=/home/universe/Documents/source/utils/itemtest -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2 -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
INCLUDEDIRS += ../../common ../../public ../../public/tier0 ../../public/tier1 ../../thirdparty/emscripten/system/include ../../thirdparty/fbx/FbxSdk/2015.1/include 
CONFTYPE=exe
GAMEOUTPUTFILE=
TARGETCOPIES=
OUTPUTFILE=../../../game/itemtest_com


POSTBUILDCOMMAND=call ../../vpc_scripts/valve_p4_edit.cmd ../../../game/bin/itemtest.com ../..
copy "$(TargetDir)"$(TargetFileName) ../../../game/bin/itemtest.com
if ERRORLEVEL 1 goto BuildEventFailed
call ../../vpc_scripts/valve_p4_edit.cmd ../../../game/bin\$(TargetName).pdb ../..
copy "$(TargetDir)"$(TargetName).pdb ../../../game/bin\$(TargetName).pdb
if ERRORLEVEL 1 goto BuildEventFailed
goto BuildEventOK
:BuildEventFailed
echo *** ERROR! PostBuildStep FAILED for $(ProjectName)! COM is probably running. ***
del /q "$(TargetDir)"$(TargetFileName)
exit 1
:BuildEventOK




CPPFILES= \
    ../../public/interpolatortypes.cpp \
    ../../public/movieobjects/movieobjects.cpp \
    ../../public/tier0/memoverride.cpp \
    ../../public/XUnzip.cpp \
    ../../public/XZip.cpp \
    itemtestapp.cpp \
    itemtest_con.cpp \
    runexe.cpp \


LIBFILES = \
    ../../lib/public/linux32/datamodel.a \
    ../../lib/public/linux32/dmserializers.a \
    ../../lib/public/linux32/fbxutils.a \
    ../../lib/public/linux32/movieobjects.a \
    ../../lib/public/linux32/itemtest_lib.a \
    ../../lib/public/linux32/appframework.a \
    ../../lib/public/linux32/bitmap.a \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/tier2.a \
    ../../lib/public/linux32/tier3.a \
    ../../lib/public/linux32/vtf.a \
    -L../../lib/public/linux32 -lsteam_api \


LIBFILENAMES = \
    ../../lib/public/linux32/appframework.a \
    ../../lib/public/linux32/bitmap.a \
    ../../lib/public/linux32/datamodel.a \
    ../../lib/public/linux32/dmserializers.a \
    ../../lib/public/linux32/fbxutils.a \
    ../../lib/public/linux32/itemtest_lib.a \
    ../../lib/public/linux32/libsteam_api.so \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/movieobjects.a \
    ../../lib/public/linux32/tier2.a \
    ../../lib/public/linux32/tier3.a \
    ../../lib/public/linux32/vtf.a \



OTHER_DEPENDENCIES = \


-include $(OBJ_DIR)/_other_deps.P


# Include the base makefile now.
include $(SRCROOT)/devtools/makefile_base_posix.mak



ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/interpolatortypes.P
endif

$(OBJ_DIR)/interpolatortypes.o : $(abspath ../../public/interpolatortypes.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/movieobjects.P
endif

$(OBJ_DIR)/movieobjects.o : $(abspath ../../public/movieobjects/movieobjects.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/memoverride.P
endif

$(OBJ_DIR)/memoverride.o : $(abspath ../../public/tier0/memoverride.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/XUnzip.P
endif

$(OBJ_DIR)/XUnzip.o : $(abspath ../../public/XUnzip.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/XZip.P
endif

$(OBJ_DIR)/XZip.o : $(abspath ../../public/XZip.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/itemtestapp.P
endif

$(OBJ_DIR)/itemtestapp.o : $(abspath itemtestapp.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/itemtest_con.P
endif

$(OBJ_DIR)/itemtest_con.o : $(abspath itemtest_con.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/runexe.P
endif

$(OBJ_DIR)/runexe.o : $(abspath runexe.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)


endif # (CFG=debug)



#
#
# CFG=release
#
#

ifeq "$(CFG)" "release"

GCC_ExtraCompilerFlags=
GCC_ExtraLinkerFlags=-Wl,--no-as-needed -Wl,--as-needed
GCC_CustomVersionScript=
EntryPoint=
IgnoreAllDefaultLibraries=no
BufferSecurityCheck=Yes
SymbolVisibility=hidden
TreatWarningsAsErrors=false
OptimizerLevel=-gdwarf-4 -g2 $(OptimizerLevel_CompilerSpecific)
SystemLibraries=
DLL_EXT=.so
SYM_EXT=.dbg
FORCEINCLUDES= 
DEFINES= -DVPC -DNDEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DEXENAME=itemtest_com -DDEV_BUILD -DFRAME_POINTER_OMISSION_DISABLED -DFBXSDK_NEW_API -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2 -DPROJECTDIR=/home/universe/Documents/source/utils/itemtest -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2 -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
INCLUDEDIRS += ../../common ../../public ../../public/tier0 ../../public/tier1 ../../thirdparty/emscripten/system/include ../../thirdparty/fbx/FbxSdk/2015.1/include 
CONFTYPE=exe
GAMEOUTPUTFILE=
TARGETCOPIES=
OUTPUTFILE=../../../game/itemtest_com


POSTBUILDCOMMAND=call ../../vpc_scripts/valve_p4_edit.cmd ../../../game/bin/itemtest.com ../..
copy "$(TargetDir)"$(TargetFileName) ../../../game/bin/itemtest.com
if ERRORLEVEL 1 goto BuildEventFailed
call ../../vpc_scripts/valve_p4_edit.cmd ../../../game/bin\$(TargetName).pdb ../..
copy "$(TargetDir)"$(TargetName).pdb ../../../game/bin\$(TargetName).pdb
if ERRORLEVEL 1 goto BuildEventFailed
goto BuildEventOK
:BuildEventFailed
echo *** ERROR! PostBuildStep FAILED for $(ProjectName)! COM is probably running. ***
del /q "$(TargetDir)"$(TargetFileName)
exit 1
:BuildEventOK




CPPFILES= \
    ../../public/interpolatortypes.cpp \
    ../../public/movieobjects/movieobjects.cpp \
    ../../public/tier0/memoverride.cpp \
    ../../public/XUnzip.cpp \
    ../../public/XZip.cpp \
    itemtestapp.cpp \
    itemtest_con.cpp \
    runexe.cpp \


LIBFILES = \
    ../../lib/public/linux32/datamodel.a \
    ../../lib/public/linux32/dmserializers.a \
    ../../lib/public/linux32/fbxutils.a \
    ../../lib/public/linux32/movieobjects.a \
    ../../lib/public/linux32/itemtest_lib.a \
    ../../lib/public/linux32/appframework.a \
    ../../lib/public/linux32/bitmap.a \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/tier2.a \
    ../../lib/public/linux32/tier3.a \
    ../../lib/public/linux32/vtf.a \
    -L../../lib/public/linux32 -lsteam_api \


LIBFILENAMES = \
    ../../lib/public/linux32/appframework.a \
    ../../lib/public/linux32/bitmap.a \
    ../../lib/public/linux32/datamodel.a \
    ../../lib/public/linux32/dmserializers.a \
    ../../lib/public/linux32/fbxutils.a \
    ../../lib/public/linux32/itemtest_lib.a \
    ../../lib/public/linux32/libsteam_api.so \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/movieobjects.a \
    ../../lib/public/linux32/tier2.a \
    ../../lib/public/linux32/tier3.a \
    ../../lib/public/linux32/vtf.a \



OTHER_DEPENDENCIES = \


-include $(OBJ_DIR)/_other_deps.P


# Include the base makefile now.
include $(SRCROOT)/devtools/makefile_base_posix.mak



ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/interpolatortypes.P
endif

$(OBJ_DIR)/interpolatortypes.o : $(abspath ../../public/interpolatortypes.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/movieobjects.P
endif

$(OBJ_DIR)/movieobjects.o : $(abspath ../../public/movieobjects/movieobjects.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/memoverride.P
endif

$(OBJ_DIR)/memoverride.o : $(abspath ../../public/tier0/memoverride.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/XUnzip.P
endif

$(OBJ_DIR)/XUnzip.o : $(abspath ../../public/XUnzip.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/XZip.P
endif

$(OBJ_DIR)/XZip.o : $(abspath ../../public/XZip.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/itemtestapp.P
endif

$(OBJ_DIR)/itemtestapp.o : $(abspath itemtestapp.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/itemtest_con.P
endif

$(OBJ_DIR)/itemtest_con.o : $(abspath itemtest_con.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/runexe.P
endif

$(OBJ_DIR)/runexe.o : $(abspath runexe.cpp) $(PWD)/itemtest_com_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)


endif # (CFG=release)



