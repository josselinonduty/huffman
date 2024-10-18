# ------------ Main configuration ------------
CC=gcc
BINDIR=bin
EXEC=huffman
# ------------ Documentation configuration ------------
DOCSDIR=docs
DOCS=doxygen
DOCSCONFIG=Doxyfile
# ------------ Debug configuration ------------
DEBUG=valgrind
DFLAGS=--leak-check=full --show-leak-kinds=all --track-origins=yes
# ------------ Build configuration ------------
SRCDIR=src
LIBDIR=lib
INCLUDEDIR=include
SRC=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJ=$(SRC:%.c=%.o)
CFLAGSBASE=-Wall -fPIC -pedantic -std=c99 -I$(INCLUDEDIR) -I$(LIBDIR)/jlib/include
CFLAGS=$(CFLAGSBASE) -O3
LDFLAGSBASE=-L$(LIBDIR)/jlib/bin/lib -ljlib
LDFLAGS=$(LDFLAGSBASE)
# ------------ Test configuration ------------
TESTDIR=tests
TEST=run
CFLAGSTEST=$(CFLAGSBASE) -I$(TESTDIR)/$(INCLUDEDIR)
LDFLAGSTEST=$(LDFLAGSBASE) -lcunit
SRCTEST=$(wildcard $(TESTDIR)/$(SRCDIR)/*.c) $(wildcard $(TESTDIR)/$(SRCDIR)/**/*.c)
OBJTEST=$(filter-out $(SRCDIR)/$(EXEC).o, $(OBJ)) $(SRCTEST:%.c=%.o)
# ------------ Lint configuration ------------
LINT=indent
LINTFLAGS=-nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4  -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1
# ------------ Coverage configuration ------------
COV=gcov
COVDIR=coverage
CFLAGSCOV=$(CFLAGSBASE) -fprofile-arcs -ftest-coverage -I$(TESTDIR)/$(INCLUDEDIR)
LDFLAGSCOV=$(LDFLAGSBASE) -lcunit -lgcov
COVR=gcovr
# ------------ End of configuration ------------

# ---------------------------------
#               Targets            
# ---------------------------------

.PHONY: all docs lint debug debug/headless build build/lib
.PHONY: tests coverage coverage/init install/debian changelog
.PHONY: clean/all clean clean/objects clean/exec clean/docs clean/debug 

all: build

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	@echo "Building $<..."
	@$(CC) -o $@ -c $< $(CFLAGS)

$(SRCDIR)/%/%.o: $(SRCDIR)/%/%.c
	@echo "Building $<..."
	@$(CC) -o $@ -c $< $(CFLAGS)

docs: $(DOCSDIR)
	@mkdir -p $(DOCSDIR)
	$(DOCS) $(DOCSCONFIG)

lint: $(SRC) $(SRCTEST)
	@for file in $^; do \
			$(LINT) $(LINTFLAGS) $$file; \
	done
	@rm -f ./$(SRCDIR)/*.c~ ./$(SRCDIR)/**/*.c~
	@rm -f ./$(TESTDIR)/$(SRCDIR)/*.c~ ./$(TESTDIR)/$(SRCDIR)/**/*.c~

production/debug: $(INPUT) build
	$(DEBUG) -s $(DFLAGS) $(BINDIR)/$(EXEC) $(SUBCOMMAND) $(INPUT) $(OUTPUT) 2>&1 | tee $(BINDIR)/.valgrind.rpt

production/debug/headless: $(INPUT) build
	$(DEBUG) -s $(DFLAGS) $(BINDIR)/$(EXEC) $(SUBCOMMAND) $(INPUT) $(OUTPUT) 2>&1 | tee $(BINDIR)/.valgrind.rpt | \
		grep -q "All heap blocks were freed -- no leaks are possible"; \
		if [ $$? -ne 0 ]; then \
			echo "Memory leaks detected!"; \
			exit 1; \
		fi;

debug: build/lib $(TESTDIR)/$(BINDIR)/$(TEST)
	@$(DEBUG) -s $(DFLAGS) $(TESTDIR)/$(BINDIR)/$(TEST) 2>&1 | tee $(TESTDIR)/$(BINDIR)/.valgrind.rpt

debug/headless: build/lib $(TESTDIR)/$(BINDIR)/$(TEST)
	@$(DEBUG) -s $(DFLAGS) $(TESTDIR)/$(BINDIR)/$(TEST) 2>&1 | tee $(TESTDIR)/$(BINDIR)/.valgrind.rpt | \
		grep -q "All heap blocks were freed -- no leaks are possible"; \
		if [ $$? -ne 0 ]; then \
			echo "Memory leaks detected!"; \
			exit 1; \
		fi;

build: build/lib $(BINDIR)/$(EXEC)

build/lib:
	@make -C $(LIBDIR)/jlib

$(BINDIR)/$(EXEC): $(OBJ)
	@mkdir -p $(BINDIR)
	@echo "Building $(BINDIR)/$(EXEC)..."
	@$(CC) -o $(BINDIR)/$(EXEC) $(OBJ) $(LDFLAGS)

tests: build/lib $(TESTDIR)/$(BINDIR)/$(TEST)
	@./$(TESTDIR)/$(BINDIR)/$(TEST)

$(TESTDIR)/$(BINDIR)/$(TEST): $(OBJTEST)
	@mkdir -p $(TESTDIR)/$(BINDIR)
	@$(CC) -o $(TESTDIR)/$(BINDIR)/$(TEST) $(OBJTEST) $(LDFLAGSTEST)

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	@$(CC) -o $@ -c $< $(CFLAGSTEST)

$(TESTDIR)/%/%.o: $(TESTDIR)/%/%.c
	@$(CC) -o $@ -c $< $(CFLAGSTEST)

coverage: CFLAGS=$(CFLAGSCOV)
coverage: LDFLAGS=$(LDFLAGSCOV)
coverage: CFLAGSTEST=$(CFLAGSCOV)
coverage: LDFLAGSTEST=$(LDFLAGSCOV)
coverage: coverage/init build tests
	@$(COVR) --root . --html --html-details -o $(COVDIR)/index.html \
		--exclude $(TESTDIR) \
		--exclude-unreachable-branches \
		--exclude-throw-branches \
		-f $(SRCDIR)

coverage/init:
	@mkdir -p $(COVDIR)

clean/all: clean
	@make -C $(LIBDIR)/jlib clean

clean: clean/objects clean/exec clean/docs clean/debug clean/coverage

clean/objects:
	@rm -f ./$(SRCDIR)/*.o
	@rm -f ./$(TESTDIR)/$(SRCDIR)/*.o

clean/exec:
	@rm -f ./$(BINDIR)/$(EXEC)
	@rm -f ./$(TESTDIR)/$(BINDIR)/$(TEST)

clean/docs:
	@rm -rf ./$(DOCSDIR)

clean/debug:
	@rm -f ./$(BINDIR)/*
	@rm -f ./$(BINDIR)/.valgrind.rpt
	@rm -f ./$(TESTDIR)/$(BINDIR)/.valgrind.rpt

clean/coverage:
	@rm -rf ./$(COVDIR)
	@find . -name "*.gcno" -type f -delete
	@find . -name "*.gcda" -type f -delete
	@find . -name "*.gcov" -type f -delete

install/debian:
	apt-get install libcunit1 libcunit1-doc libcunit1-dev valgrind gcovr -y