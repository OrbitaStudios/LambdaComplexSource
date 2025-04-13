ifneq "$(LINUX_TOOLS_PATH)" ""
TOOL_PATH = $(LINUX_TOOLS_PATH)/
endif

NAME=bugreporter_public
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
GCC_ExtraLinkerFlags=
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
DEFINES= -DVPC -DDEBUG -D_DEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DDLLNAME=bugreporter_public -DBINK_VIDEO -DGL_GLEXT_PROTOTYPES -DDX_TO_GL_ABSTRACTION -DUSE_SDL -DDEV_BUILD -DFRAME_POINTER_OMISSION_DISABLED -Dbugreporter_public_EXPORTS -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2 -DPROJECTDIR=/home/universe/Documents/EmSource/utils/bugreporter_public -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2 -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
INCLUDEDIRS += ../../common ../../public ../../public/tier0 ../../public/tier1 ../../thirdparty/SDL2 ../common 
CONFTYPE=dll
IMPORTLIBRARY=
GAMEOUTPUTFILE=../../../game/bin/bugreporter_public.so
TARGETCOPIES=
OUTPUTFILE=$(OBJ_DIR)/bugreporter_public.so


POSTBUILDCOMMAND=/bin/true



CPPFILES= \
    ../../public/blockingudpsocket.cpp \
    ../../public/tier0/memoverride.cpp \
    bugreporter_public.cpp \
    bugreporter_upload.cpp \


LIBFILES = \
    ../../lib/public/linux32/tier1.a \
    ../../lib/public/linux32/mathlib.a \
    -L../../lib/public/linux32 -ltier0 \
    -L../../lib/public/linux32 -lvstdlib \


LIBFILENAMES = \
    ../../lib/public/linux32/libtier0.so \
    ../../lib/public/linux32/libvstdlib.so \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/tier1.a \



OTHER_DEPENDENCIES = \


-include $(OBJ_DIR)/_other_deps.P


# Include the base makefile now.
include $(SRCROOT)/devtools/makefile_base_emscripten.mak



ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/blockingudpsocket.P
endif

$(OBJ_DIR)/blockingudpsocket.o : $(abspath ../../public/blockingudpsocket.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/memoverride.P
endif

$(OBJ_DIR)/memoverride.o : $(abspath ../../public/tier0/memoverride.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bugreporter_public.P
endif

$(OBJ_DIR)/bugreporter_public.o : $(abspath bugreporter_public.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bugreporter_upload.P
endif

$(OBJ_DIR)/bugreporter_upload.o : $(abspath bugreporter_upload.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
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
GCC_ExtraLinkerFlags=
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
DEFINES= -DVPC -DNDEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DDLLNAME=bugreporter_public -DBINK_VIDEO -DGL_GLEXT_PROTOTYPES -DDX_TO_GL_ABSTRACTION -DUSE_SDL -DDEV_BUILD -DFRAME_POINTER_OMISSION_DISABLED -Dbugreporter_public_EXPORTS -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2 -DPROJECTDIR=/home/universe/Documents/EmSource/utils/bugreporter_public -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2 -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
INCLUDEDIRS += ../../common ../../public ../../public/tier0 ../../public/tier1 ../../thirdparty/SDL2 ../common 
CONFTYPE=dll
IMPORTLIBRARY=
GAMEOUTPUTFILE=../../../game/bin/bugreporter_public.so
TARGETCOPIES=
OUTPUTFILE=$(OBJ_DIR)/bugreporter_public.so


POSTBUILDCOMMAND=/bin/true



CPPFILES= \
    ../../public/blockingudpsocket.cpp \
    ../../public/tier0/memoverride.cpp \
    bugreporter_public.cpp \
    bugreporter_upload.cpp \


LIBFILES = \
    ../../lib/public/linux32/tier1.a \
    ../../lib/public/linux32/mathlib.a \
    -L../../lib/public/linux32 -ltier0 \
    -L../../lib/public/linux32 -lvstdlib \


LIBFILENAMES = \
    ../../lib/public/linux32/libtier0.so \
    ../../lib/public/linux32/libvstdlib.so \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/tier1.a \



OTHER_DEPENDENCIES = \


-include $(OBJ_DIR)/_other_deps.P


# Include the base makefile now.
include $(SRCROOT)/devtools/makefile_base_emscripten.mak



ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/blockingudpsocket.P
endif

$(OBJ_DIR)/blockingudpsocket.o : $(abspath ../../public/blockingudpsocket.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/memoverride.P
endif

$(OBJ_DIR)/memoverride.o : $(abspath ../../public/tier0/memoverride.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bugreporter_public.P
endif

$(OBJ_DIR)/bugreporter_public.o : $(abspath bugreporter_public.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bugreporter_upload.P
endif

$(OBJ_DIR)/bugreporter_upload.o : $(abspath bugreporter_upload.cpp) $(PWD)/bugreporter_public_linux32.mak $(SRCROOT)/devtools/makefile_base_emscripten.mak $(OTHER_DEPENDENCIES)
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)


endif # (CFG=release)



