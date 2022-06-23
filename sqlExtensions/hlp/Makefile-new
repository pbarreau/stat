#############################################################################
# Makefile for building: sqlite.dll 
#
# https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents
#
#############################################################################


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

CFLAGS        = -fomit-frame-pointer -O2 -Wall -W -Wextra -Wunused-parameter $(DEFINES)
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
### $(eval $(call mYTemplate_1, $@,$<))
### @echo -e "Generation de l'objet -> $@ depuis $<"
##@#$(call MyTemplate_1, $@, "FERER")
### $(foreach prog, $(OBJ_DLL), $(eval $(MyTemplate_1, $(prog))))
#
map = $(foreach a, $(2), $(call $(1), $(a)))

.PHONY : all clean clobber msys2 mingw newdir
#all: $(TARGET_0) $(TARGET_1) $(TARGET_2) $(OBJ_DLL) | $(NEW_DIR)
all: $(TARGET_0) $(TARGET_1) $(TARGET_2) | $(NEW_DIR)
#all: $(TARGET_2)  | $(NEW_DIR)

define MyTemplate_1 =
$(1):
	@echo -e "\n-> TOTO de $@ depuis $(subst $(OBJ_DIR), src, $(1))\n"
	@echo "ici->$(info $$(OBJ_DIR))"
endef


$(TARGET_0): $(DIR_OBJ)dlfcn.o
	@echo -e "lib dynamic loader : $< -> $@\n\n\n"
	@$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $< $(LIBS_1)

##$(TARGET_2): $(T2_SRCS) $(T2_LIBS)
$(TARGET_2): $(T2_SRCS) $(TARGET_0)| $(NEW_DIR)
	@echo -e "exe : $^ -> $@\n"
	@$(CC) $(CFLAGS) $(INCPATH) $^ -o $@ $(LIBS_2)

$(DIR_LIB)lib$(MY_QT_EXE)-%-$(MY_ARCH).dll:$(DIR_OBJ)%.o
	@echo -e "lib : $< -> $@\n"
	@$(CC) $(CFLAGS) $(INCPATH) -shared $< -o $@


$(DIR_OBJ)%.o:$(DIR_SRC)%.c | $(NEW_DIR)
	@echo -e "obj : $< -> $@\n\n"
	@$(CC) -o $@ -c $< $(CFLAGS) $(INCPATH)

$(NEW_DIR):
	@mkdir -p $@

clobber:
	rm -r $(NEW_DIR)

## all: $(TMP_DLL) $(info $$SRC is [${OBJ_SRC}])
## # all: $(DLL_TG_1) $(EXE_TG_1)
## 
## ## $(DIR_OBJ)%.o:$(DIR_SRC)%.c
## ## 	@echo -> TATA de $@ depuis $<
## ## 	$(CC) -o $@ -c $< $(CFLAGS) $(INCPATH)
## ## 	@echo 
## 
## $(DIR_LIB)*.dll:$(DIR_OBJ)*.o
## 	@echo -> TITI de $@ depuis $<
## 
## $(firstword  $(TMP_DLL)): $(firstword $(TMP_OBJ))
## 	@echo -e "\n-> TOTO de $@ depuis $<\n\n"
## 	$(CC) $(CFLAGS) $(INCPATH) $(LFLAGS) -shared -o $@ $< $(LIBS_1)
## 	TMP_OBJ=${(wordlist 2, $(words $(TMP_OBJ)), $(TMP_OBJ))}
## 	TMP_DLL=${(wordlist 2, $(words $(TMP_DLL)), $(TMP_DLL))}
## 
## $(firstword  $(TMP_OBJ)): $(firstword $(OBJ_SRC))
## 	@echo -e "\n-> generation de $@ depuis $<\n\n"
## 	$(CC) -o $@ -c $< $(CFLAGS) $(INCPATH)
## 
# $(DLLS_TRG): $(OBJ_DLL)

# $(DIR_LIB)%.dll:$(DIR_OBJ)$(subst -$(MY_ARCH),, $(notdir $(patsubst %.dll,%.o,$@)))
	#$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $<
#	@echo -e "\n-------\n\n"

## $(DIR_LIB)$(DLL_FIRST):$(DIR_OBJ)$(SRC_1:.c=.o)
## 	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS_1)
## 
## $(DIR_LIB)$(DLL_SQL):
## 	gcc $(CFLAGS) -shared -o $@ $(DIR_SRC)$(SRC_2)
## 
## $(DIR_LIB)$(DLL_MATH):$(DIR_OBJ)$(SRC_3:.c=.o)
## 	$(CC) $(CFLAGS) $(INCPATH) -shared -o $@ $^ $(LIBS)
## 
## $(EXE_TG_1):$(DIR_OBJ)$(SRC_2:.c=.o) $(DIR_OBJ)$(SRC_4:.c=.o)
## 	$(CC) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBS_2)
## 
clean::
	rm -f $(addprefix $(DIR_OBJ),$(SRC:.c=.o)) $(TARGET_1)

arch:
	@echo $(MY_ARCH)


## Apres compilation en mode cli sql
## PRAGMA compile_options; pour verifier

msys21: newdir simple_1 simple_2
msys22: newdir simple11 simple22
mingw: newdir simple_3 simple_4
newdir: $(NEW_DIR)

## sqlite msys2 compilation sans define utilisateur
simple_1: ./src/shell.c ./src/sqlite3.c
	gcc $^ -I./inc -L./lib -ldl -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite msys2 compilation AVEC define utilisateur
simple_2: ./src/shell.c ./src/sqlite3.c
	gcc $^ $(CFLAGS) $(INCPATH) -ldl -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite msys2 compilation sans define utilisateur
simple11: ./src/shell.c ./src/sqlite3.c ./lib/libStatPgm-dlfcn-i686.dll
	gcc $^ -I./inc -L./lib -lStatPgm-dlfcn-i686 -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite msys2 compilation AVEC define utilisateur
simple22: ./src/shell.c ./src/sqlite3.c ./lib/libStatPgm-dlfcn-i686.dll
	gcc $^ $(CFLAGS) $(INCPATH) -lStatPgm-dlfcn-i686 -pthread -lm -o $(DIR_TST)/mon_cli_msys2-$(MY_ARCH)-$@

## sqlite mingw compilation sans define utilisateur
simple_3: ./src/shell.c ./src/sqlite3.c
	gcc $^ -I./inc -L./lib -pthread -lm -o $(DIR_TST)/mon_cli_mingw-$(MY_ARCH)-$@

## sqlite mingw compilation AVEC define utilisateur
simple_4: ./src/shell.c ./src/sqlite3.c
	gcc $^ $(CFLAGS) $(INCPATH) -pthread -lm -o $(DIR_TST)/mon_cli_mingw-$(MY_ARCH)-$@


## ------------------------------
## Resultat des tests
#
# Faire attention a l'itilisation de l'option -shared de gcc
# compil ok mais probleme à l'execution de l'application
#
# Sous l'environnement de de compilation QT5.10.1
# mingw32-make -f Makefile-new mingw
# compil OK on peut lancer l'executable depuis windows directement
# --
# Sous l'environnement mingw64 (cible mingw32) idem que pour QT
# Sous l'environnement mingw64 (cible mingw64) idem que pour QT
#
# Sous l'environnement mingw64 (cible msys2)
# make -f Makefile-new mingw
# make -f Makefile-new msys2
# compil OK on peut lancer l'executable depuis windows directement
#
