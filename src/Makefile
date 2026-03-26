BIN_FILE=${MUD_DIR}/bin/${MUD_BIN}
#PROF    = -DARMAGEDDON
#PROF	= -D__CYGWIN32__
ifeq "$(value BARECC)" ""
BARECC	= g++
endif
CC      = @echo " Compiling $(PROF) $(basename $@)..."; $(BARECC)
LCC     = @echo " Linking    $@..."; $(BARECC)
#TIME    = time -h
#PROF   = -DDMALLOC
#LINKDMALL = -ldmalloc
NOCRYPT =
#Uncomment the next line if you want request support
#DBUGFLG = -DREQUESTS
MAKE	= make
ifeq "$(value CPUs)" ""
CPUs	= -j2
endif
ifeq "$(shell uname)" "FreeBSD"
CUSTOM_C_FLAGS = -nostdinc++ -isystem /usr/include/c++/v1
CUSTOM_L_FLAGS = -lkvm -liconv -lc++
MAKE	= gmake
endif
ILIBS	= -I/usr/local/include -I/usr/include/libxml2 -I/opt/homebrew/include -I. -I./classes -I./classes/player -I./classes/math
LLIBS	= -L/usr/local/lib -lm -lcrypt -lxml2 -lpqxx -lpq -lpthread $(CUSTOM_L_FLAGS)
C_FLAGS = -O2 -ggdb3 -Wall -Wfatal-errors -std=c++20 $(CUSTOM_C_FLAGS) $(PROF) $(NOCRYPT) $(DBUGFLG) $(ILIBS)
#-D_GLIBCXX_USE_C99
#L_FLAGS = -gdwarf-2

OBJ_DIR = ../obj
C_FILES = $(wildcard classes/*.cpp) $(wildcard classes/*/*.cpp) $(wildcard *.cpp)
O_FILES = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(C_FILES))

TEST_OBJ_DIR = $(OBJ_DIR)/test
TEST_BIN_FILE = $(TEST_OBJ_DIR)/test
TEST_C_FILES = $(wildcard test/*.cpp)
TEST_ONLY_O_FILES = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_C_FILES))
TEST_O_FILES = $(patsubst $(OBJ_DIR)/main.o,,$(O_FILES)) $(TEST_ONLY_O_FILES)


swmud:
	@echo "Making..."
	@$(TIME) $(MAKE) -s $(CPUs) $(BIN_FILE)
	@echo "Done."

$(BIN_FILE): $(O_FILES)
	@rm -f $(BIN_FILE)
	$(LCC) $(L_FLAGS) -o $(BIN_FILE) $(O_FILES) $(LLIBS) $(LINKDMALL)

$(OBJ_DIR)/%.o: %.cpp
	@test -d $(@D) || mkdir -p $(@D)
	$(CC) -c $(C_FLAGS) $< -o $@

%.cpp: %.h
	@touch $@

clean:
	@echo "Removing object files..."
	@rm -f $(O_FILES)
	@echo "Done."

cleanall:
	@echo "Removing All (binary, object files & tests)..."
	@rm -f $(BIN_FILE) $(O_FILES) $(TEST_BIN_FILE) $(TEST_O_FILES)
	@echo "Done."


$(TEST_BIN_FILE): $(TEST_O_FILES)
	@rm -f $(TEST_BIN_FILE)
	$(LCC) $(L_FLAGS) -o $(TEST_BIN_FILE) $(TEST_O_FILES) $(LLIBS) $(LINKDMALL)

test: cleantest $(TEST_BIN_FILE)
	@echo "Running tests..."
	@$(TEST_BIN_FILE); echo "Exit status: $$?"
	@echo "Done."

cleantest:
	@echo "Removing test object files and the binary..."
	@rm -f $(TEST_ONLY_O_FILES) $(TEST_BIN_FILE)
	@echo "Done."
