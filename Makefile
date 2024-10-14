# ------------ Directory structure ------------
BINDIR=bin
CC=gcc
DOCSDIR=docs
INCLUDEDIR=include
LIBDIR=lib
SRCDIR=src
TESTDIR=tests
# ------------ Main configuration ------------
EXEC=huffman
# ------------ Documentation configuration ------------
DOCS=doxygen
DOCSCONFIG=Doxyfile
# ------------ Debug configuration ------------
DEBUG=valgrind
DFLAGS=--leak-check=full --show-leak-kinds=all --track-origins=yes
# ------------ Build configuration ------------
SRC=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJ=$(SRC:%.c=%.o)
CFLAGS=-Wall -pedantic -std=c99 -I$(INCLUDEDIR) -I$(LIBDIR)/jlib/include
LDFLAGS=-lm -lcunit -L./lib/jlib/bin/lib -ljlib
# ------------ Test configuration ------------
TEST=run
CFLAGSTEST=$(CFLAGS) -I$(TESTDIR)/$(INCLUDEDIR)
LDFLAGSTEST=$(LDFLAGS)
SRCTEST=$(wildcard $(TESTDIR)/$(SRCDIR)/*.c) $(wildcard $(TESTDIR)/$(SRCDIR)/**/*.c)
OBJTEST=$(SRCTEST:%.c=%.o)

# ---------------------------------
#               Targets            
# ---------------------------------

.PHONY: all docs debug debug/headless build tests clean clean/objects clean/exec clean/docs clean/debug install/debian changelog

all: build

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	@$(CC) -o $@ -c $< $(CFLAGS)

$(SRCDIR)/%/%.o: $(SRCDIR)/%/%.c
	@$(CC) -o $@ -c $< $(CFLAGS)

docs: $(DOCSDIR)
	mkdir -p $(DOCSDIR)
	$(DOCS) $(DOCSCONFIG)

debug: $(TESTDIR)/$(BINDIR)/$(TEST)
	@$(DEBUG) -s $(DFLAGS) $(TESTDIR)/$(BINDIR)/$(TEST) 2>&1 | tee $(TESTDIR)/$(BINDIR)/.valgrind.log

debug/headless: $(TESTDIR)/$(BINDIR)/$(TEST)
	@$(DEBUG) -s $(DFLAGS) $(TESTDIR)/$(BINDIR)/$(TEST) 2>&1 | tee $(TESTDIR)/$(BINDIR)/.valgrind.log | \
		grep -q "All heap blocks were freed -- no leaks are possible"; \
		if [ $$? -ne 0 ]; then \
			echo "Memory leaks detected!"; \
			exit 1; \
		fi;

build: $(BINDIR)/$(EXEC)

$(BINDIR)/$(EXEC): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/$(EXEC) $(OBJ) $(LDFLAGS)

tests: $(TESTDIR)/$(BINDIR)/$(TEST)
	@./$(TESTDIR)/$(BINDIR)/$(TEST)

$(TESTDIR)/$(BINDIR)/$(TEST): $(OBJTEST)
	@mkdir -p $(TESTDIR)/$(BINDIR)
	$(CC) -o $(TESTDIR)/$(BINDIR)/$(TEST) $(OBJTEST) $(LDFLAGSTEST)

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGSTEST)

$(TESTDIR)/%/%.o: $(TESTDIR)/%/%.c
	$(CC) -o $@ -c $< $(CFLAGSTEST)

clean: clean/objects clean/exec clean/docs clean/debug

clean/objects:
	@rm -f ./$(SRCDIR)/*.o
	@rm -f ./$(SRCDIR)/**/*.o
	@rm -f ./$(TESTDIR)/$(SRCDIR)/*.o
	@rm -f ./$(TESTDIR)/$(SRCDIR)/**/*.o

clean/exec:
	@rm -f ./$(TESTDIR)/$(BINDIR)/$(TEST)

clean/docs:
	@rm -rf ./$(DOCSDIR)

clean/debug:
	@rm -f ./$(BINDIR)/.valgrind.log

install/debian:
	apt-get install libcunit1 libcunit1-doc libcunit1-dev valgrind -y