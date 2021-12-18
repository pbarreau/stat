#############################################################################
# Makefile for building: sqlite.dll 
#############################################################################


####### Compiler, tools and options

CC            = gcc
CXX           = g++

D_SQLITE      = SQLITE_THREADSAFE=2 SQLITE_TEMP_STORE=2
D_SQLITE     += SQLITE_ENABLE_LOAD_EXTENSION SQLITE_ENABLE_COLUMN_METADATA
D_SQLITE     += SQLITE_ALLOW_URI_AUTHORITY SQLITE_ENABLE_ATOMIC_WRITE SQLITE_ENABLE_BATCH_ATOMIC_WRITE
D_SQLITE     += SQLITE_ENABLE_BYTECODE_VTAB SQLITE_ENABLE_COLUMN_METADATA
D_SQLITE     += SQLITE_ENABLE_BYTECODE_VTAB SQLITE_ENABLE_COLUMN_METADATA
D_SQLITE     += SQLITE_ENABLE_FTS4 SQLITE_ENABLE_FTS5 SQLITE_ENABLE_GEOPOLY 
D_SQLITE     += SQLITE_ENABLE_MATH_FUNCTIONS SQLITE_ENABLE_JSON1 SQLITE_ENABLE_MEMORY_MANAGEMENT
D_SQLITE     += SQLITE_ENABLE_OFFSET_SQL_FUNC SQLITE_ENABLE_PREUPDATE_HOOK
D_SQLITE     += SQLITE_ENABLE_RBU SQLITE_ENABLE_RTREE SQLITE_ENABLE_SORTER_REFERENCES
D_SQLITE     += SQLITE_ENABLE_SESSION SQLITE_ENABLE_PREUPDATE_HOOK
D_SQLITE     += SQLITE_ENABLE_UPDATE_DELETE_LIMIT
D_SQLITE     += SQLITE_SOUNDEX 
DEF_SQLITE    = $(addprefix -D, $(D_SQLITE))
DEFINES      += $(DEF_SQLITE)

LIBS_1        += -lpsapi

LINKER        = $(CXX)
LFLAGS        = -Wl,-s -Wl,-subsystem,windows -mthreads 
#LIBS          = -lmingw32 -LC:/Devel/Qt5.10.1/5.10.1/mingw53_32/lib -lStatPgm-dl -L./lib
LIBS          = -lStatPgm-dl -L./lib
LIBS_2        += $(LIBS) -lpthread -lStatPgm-sqlite -lStatPgm-sqMath

VPAT          = ./src:./inc

DLL_FIRST     = libStatPgm-dl.dll 
DLL_SQL       = libStatPgm-sqlite.dll 
DLL_MATH      = libStatPgm-sqMath.dll
EXE           = cli.exe
####### Output directory

INC_DIR       = ./inc/
SRC_DIR   = ./src/
OBJ_DIR   = ./obj/
DLL_DIR   = ./lib/
EXE_DIR   = ./out/

####### Files
	#$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS)
SRC_1	      =	dlfcn.c
SRC_2	      =	sqlite3.c 
SRC_3	      =	extension-functions.c
SRC_4	      =	shell.c
SRC       = $(SRC_1) $(SRC_2) $(SRC_3) $(SRC_4)

INCPATH       = -I$(INC_DIR) -L$(DLL_DIR)
CFLAGS        = -fomit-frame-pointer -O2 -Wall -W -Wextra $(DEFINES)

DLL_TG_1 = $(addprefix $(DLL_DIR),$(DLL_FIRST) $(DLL_SQL) $(DLL_MATH))
EXE_TG_1 = $(addprefix $(EXE_DIR),$(EXE))

all: $(DLL_TG_1) $(EXE_TG_1)

$(OBJ_DIR)%.o:$(SRC_DIR)%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCPATH)

$(DLL_DIR)$(DLL_FIRST):$(OBJ_DIR)$(SRC_1:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS_1)

$(DLL_DIR)$(DLL_SQL):
	gcc $(CFLAGS) -shared -o $@ $(SRC_DIR)$(SRC_2)

$(DLL_DIR)$(DLL_MATH):$(OBJ_DIR)$(SRC_3:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS)

$(EXE_TG_1):$(OBJ_DIR)$(SRC_2:.c=.o) $(OBJ_DIR)$(SRC_4:.c=.o)
	$(CC) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBS_2)

.PHONY:clean
clean::
	rm -f $(addprefix $(OBJ_DIR),$(SRC:.c=.o)) $(DLL_TG_1) $(DLL_TG_2) $(EXE_TG_1)
