#############################################################################
# Makefile for building: sqlite.dll 
#############################################################################


####### Compiler, tools and options

CC            = gcc
CXX           = g++
INCPATH       = -I./inc 
DEFINES       = -DUNICODE -D_UNICODE -DWIN32 -DCOMPILE_SQLITE_EXTENSIONS_AS_LOADABLE_MODULE
CFLAGS        = $(INCPATH) -fomit-frame-pointer -O2 -Wall -W -Wextra $(DEFINES)
LIBS_1        += -lpsapi

LINKER        = g++
LFLAGS        = -Wl,-s -Wl,-subsystem,windows -mthreads 
LIBS          = -lmingw32 -LC:/Devel/Qt5.10.1/5.10.1/mingw53_32/lib -lStatPgm-dl -L./lib
LIBS_2        += $(LIBS) -lpthread -lStatPgm-sqlite -lStatPgm-sqMath

VPAT          = ./src:./inc

DLL_FIRST     = libStatPgm-dl.dll 
DLL_SQL       = libStatPgm-sqlite.dll 
DLL_MATH      = libStatPgm-sqMath.dll
EXE           = cli.exe
####### Output directory

SRC_DIR   = ./src/
OBJ_DIR   = ./obj/
DLL_DIR   = ./lib/
EXE_DIR   = ./out/

####### Files
SRC_1	      =	dlfcn.c
SRC_2	      =	sqlite3.c 
SRC_3	      =	extension-functions.c
SRC_4	      =	shell.c
SRC       = $(SRC_1) $(SRC_2) $(SRC_3) $(SRC_4)

DLL_TG_1 = $(addprefix $(DLL_DIR),$(DLL_FIRST) $(DLL_SQL) $(DLL_MATH))
EXE_TG_1 = $(addprefix $(EXE_DIR),$(EXE))

all: $(DLL_TG_1) $(EXE_TG_1)

$(OBJ_DIR)%.o:$(SRC_DIR)%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(DLL_DIR)$(DLL_FIRST):$(OBJ_DIR)$(SRC_1:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS_1)

$(DLL_DIR)$(DLL_SQL):$(OBJ_DIR)$(SRC_2:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS)

$(DLL_DIR)$(DLL_MATH):$(OBJ_DIR)$(SRC_3:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS)

$(EXE_TG_1):$(OBJ_DIR)$(SRC_2:.c=.o) $(OBJ_DIR)$(SRC_4:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBS_2)

.PHONY:clean
clean::
	rm -f $(addprefix$(OBJ_DIR),$(SRC:.c=.o)) $(DLL_TG_1) $(DLL_TG_2) $(EXE_TG_1)
