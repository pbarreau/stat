#############################################################################
# Makefile for building: sqlite.dll 
#
# https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents
#
#############################################################################

# mingw32-make clobber
# mingw32-make all
#
# cd out
# shell-x86_64
# PRAGMA compile_options; pour verifier
# 
# Tests
# mingw32-make arch
# mingw32-make msys21
# mingw32-make msys22
# mingw32-make mingw


####### Compiler, tools and options

CC            = gcc
CXX           = g++
LINKER        = $(CXX)

MY_ARCH       = $(shell gcc -dumpmachine | sed 's/-.*$\//')

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


# --- A generer

####### Output directory
#
# Affectation immediate -> :=
# Affectation differee ->   =
#
DIR_INC      := ./inc/
DIR_SRC      := ./src/
DIR_OBJ      := ./obj/
DIR_LIB      := ./lib/
DIR_OUT      := ./out/
DIR_TST      := ./tst/

VPAT         := $(DIR_INC):$(DIR_SRC)

INCPATH      := -I$(DIR_INC) -L$(DIR_LIB)

NEW_DIR      := $(DIR_OBJ) $(DIR_LIB) $(DIR_OUT) $(DIR_TST)

#CFLAGS        = -fomit-frame-pointer -O2 -Wall -W -Wextra -Wunused-parameter $(DEFINES)
CFLAGS        = -fomit-frame-pointer -O2 $(DEFINES)
LFLAGS        = -Wl,-s -Wl,-subsystem,windows -mthreads 

LIBS_1       := -lpsapi

####### Files
SRC_1        := dlfcn.c sqlite3.c extension-functions.c
SRC_2        := shell.c sqlite3.c
SRC          := $(SRC_1) $(SRC_2)

MY_QT_EXE    := StatPgm
TARGET_0     := $(addprefix $(DIR_LIB)lib$(MY_QT_EXE)-,$(addsuffix -$(MY_ARCH).dll,$(basename $(firstword $(SRC_1)))))
TARGET_1     := $(addprefix $(DIR_LIB)lib$(MY_QT_EXE)-,$(addsuffix -$(MY_ARCH).dll,$(basename $(SRC_1))))
TARGET_2     := $(addprefix $(DIR_OUT),$(addsuffix -$(MY_ARCH).exe,$(basename $(firstword $(SRC_2)))))
T2_LIBS      := $(wordlist 2,3, $(TARGET_1))
T2_FLAGS     := $(subst lib,-l,$(notdir $(basename $(TARGET_1))))
T2_SRCS      := $(addprefix $(DIR_SRC), $(SRC_2))
OBJ_DLL      := $(patsubst %.c, %.o, $(addprefix $(DIR_OBJ),$(SRC_1)))
OBJ_SRC      := $(addprefix $(DIR_SRC),$(SRC_1))

EXE_TG_1      = $(addsuffix -$(MY_ARCH).exe, $(addprefix $(DIR_OUT),$(TARGET_2)))

LIBS_2        = -l$(MY_QT_EXE)-dlfcn-$(MY_ARCH) -lpthread 
LIBS_3       := $(addprefix -l$(MY_QT_EXE)-,$(addsuffix -$(MY_ARCH),$(basename $(SRC_1))))
LIBS          = $(LIBS_1) $(LIBS_2)

## ---------------------------------------------------------------------------
# https://www.gnu.org/software/make/manual/make.html#Foreach-Function
map = $(foreach a, $(2), $(call $(1), $(a)))

define MyTemplate_1 =
$(1):
	@echo -e "\n-> TOTO de $@ depuis $(subst $(OBJ_DIR), src, $(1))\n"
	@echo "ici->$(info $$(OBJ_DIR))"
endef

# .PHONY : all clean clobber msys2 mingw newdir
.PHONY : all clean clobber mingw 

## ---------------------------------------------------------------------------
all: $(TARGET_0) $(TARGET_1) $(TARGET_2) | $(NEW_DIR)

win: all mingw msysB
	
$(DIR_OBJ)%.o:$(DIR_SRC)%.c | $(NEW_DIR)
	@echo -e "[OBJ]\t[$<]\t->\t[$@]"
	@$(CC) -o $@ -c $< $(CFLAGS) $(INCPATH)

$(TARGET_0): $(DIR_OBJ)dlfcn.o
	@echo -e "[LIB_1]\t[$<]\t->\t[$@]\n"
	@$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $< $(LIBS_1)

$(DIR_LIB)lib$(MY_QT_EXE)-%-$(MY_ARCH).dll:$(DIR_OBJ)%.o
	@echo -e "[LIB_2]\t[$<]\t->\t[$@]\n"
	@$(CC) $(CFLAGS) $(INCPATH) -shared $< -o $@

$(TARGET_2): $(T2_SRCS) $(TARGET_0)| $(NEW_DIR)
	@echo -e "[EXE]\t[$^]\n\t\t->\t[$@]"
	@$(CC) $(CFLAGS) $(INCPATH) $^ -o $@ $(LIBS_2)
	@echo -e "\nExecuter $@ puis taper : 'PRAGMA compile_options;'\n" 

## ---------------------------------------------------------------------------
msysA: mk_reps simple1a simple2a show_cmd
msysB: mk_reps simple1b simple2b show_cmd
mingw: mk_reps simple1c simple2c show_cmd

mk_reps : $(NEW_DIR)

show_cmd:
	@echo -e "\nExecuter puis taper : 'PRAGMA compile_options;'\n" 

simple_0: ./src/shell.c ./src/sqlite3.c 
	target="$(DIR_TST)mon_cli_mingw-$(MY_ARCH)-$@";
## sqlite msysA compilation sans define utilisateur
simple1a: ./src/shell.c ./src/sqlite3.c
	@echo -e "[EXE]\tcible\t$@\n[$^]\t->\t[$(DIR_TST)mon_cli_msys2-$(MY_ARCH)-$@]\n"
	@gcc $^ -I./inc -L./lib -ldl -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite msysA compilation AVEC define utilisateur
simple2a: ./src/shell.c ./src/sqlite3.c
	@echo -e "[EXE]\tcible\t$@\n[$^]\t->\t[$(DIR_TST)mon_cli_msys2-$(MY_ARCH)-$@]\n"
	@gcc $^ $(CFLAGS) $(INCPATH) -ldl -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite msysB compilation sans define utilisateur$(DIR_LIB)lib$(MY_QT_EXE)-%-$(MY_ARCH).dll
simple1b: ./src/shell.c ./src/sqlite3.c $(DIR_LIB)lib$(MY_QT_EXE)-dlfcn-$(MY_ARCH).dll
	@echo -e "[EXE]\tcible\t$@\n[$^]\t->\t[$(DIR_TST)mon_cli_msys2-$(MY_ARCH)-$@]\n"
	@gcc $^ -I./inc -L./lib -lStatPgm-dlfcn-$(MY_ARCH) -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite msysB compilation AVEC define utilisateur
simple2b: ./src/shell.c ./src/sqlite3.c $(DIR_LIB)lib$(MY_QT_EXE)-dlfcn-$(MY_ARCH).dll
	@echo -e "[EXE]\tcible\t$@\n[$^]\t->\t[$(DIR_TST)mon_cli_msys2-$(MY_ARCH)-$@]\n"
	@gcc $^ $(CFLAGS) $(INCPATH) -l$(MY_QT_EXE)-dlfcn-$(MY_ARCH) -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite mingw compilation sans define utilisateur
simple1c: ./src/shell.c ./src/sqlite3.c 
	@echo -e "[EXE]\tcible\t$@\n[$^]\t->\t[$(DIR_TST)mon_cli_mingw-$(MY_ARCH)-$@]\n"
	@gcc $^ -I./inc -L./lib -pthread -lm -o $(DIR_TST)mon_cli_mingw-$(MY_ARCH)-$@

## sqlite mingw compilation AVEC define utilisateur
simple2c: ./src/shell.c ./src/sqlite3.c
	@echo -e "[EXE]\tcible\t$@\n[$^]\t->\t[$(DIR_TST)mon_cli_mingw-$(MY_ARCH)-$@]\n"
	@gcc $^ $(CFLAGS) $(INCPATH) -pthread -lm -o $(DIR_TST)mon_cli_mingw-$(MY_ARCH)-$@
## ---------------------------------------------------------------------------
$(NEW_DIR):
	@mkdir -p $@

clobber:
	@rm -rf $(NEW_DIR)

clean:
	@rm -f $(DIR_OBJ)*.o
	@rm -f $(DIR_LIB)*.dll
	@rm -f $(DIR_OUT)*.exe

clean_tst:
	@rm -f $(DIR_TST)*.exe

clean1a:
	rm -f $(addprefix $(DIR_OBJ),$(SRC:.c=.o)) $(TARGET1a)

arch:
	echo $(MY_ARCH)
	echo $(addprefix $(DIR_OBJ),$(SRC:.c=.o)) $(TARGET1a)

