# Yggdrasil C++20 - NovaC-style Makefile
# This archive contains declarations only. Every .cpp is intentionally empty.
# "all" builds the static archive; build will not link before implementations.

CXX ?= g++
AR ?= ar
RANLIB ?= ranlib
INSTALL ?= install
DOXYGEN ?= doxygen

CPPFLAGS ?= -Isrc/include
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -O2 -pthread
LDFLAGS ?=
LDLIBS ?= -lssl -lcrypto -lsodium -lsqlite3 -pthread

LIB_SRC := $(shell find src/core -name '*.cpp' -type f | sort)
LIB_OBJ := $(patsubst src/%.cpp,build/obj/%.o,$(LIB_SRC))
LIB_DIR := lib
LIB_NAME := libYggdrasil.a
LIB := $(LIB_DIR)/$(LIB_NAME)

PREFIX ?= /usr/local
DESTDIR ?=
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib

all: $(LIB)

$(LIB): $(LIB_OBJ)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^
	$(RANLIB) $@

build/obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(LIB_OBJ:.o=.d)

# Verify that the public API is parseable; this does not check linking.
check-headers:
	@printf '#include "Yggdrasil.hpp"\nint main() { return 0; }\n' \
	  | $(CXX) $(CPPFLAGS) $(CXXFLAGS) -x c++ -fsyntax-only -

# Application binaries are intentionally not part of "all" until code exists.
build: $(LIB)
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) build/yggd/main.cpp $(LIB) $(LDFLAGS) $(LDLIBS) -o bin/yggd
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) build/yggctl/main.cpp $(LIB) $(LDFLAGS) $(LDLIBS) -o bin/yggctl

# Doxygen configuration is supplied through Makefile; docs/ sources stay empty.
doc:
	@command -v $(DOXYGEN) >/dev/null 2>&1 || { echo 'Doxygen is required'; exit 1; }
	@printf '%s\n' 'PROJECT_NAME = Yggdrasil' \
	  'OUTPUT_DIRECTORY = docs/generated' 'INPUT = src/include' \
	  'FILE_PATTERNS = *.hpp' 'RECURSIVE = YES' \
	  'EXTRACT_ALL = YES' 'GENERATE_HTML = YES' 'GENERATE_LATEX = NO' \
	  'QUIET = YES' | $(DOXYGEN) -

install: $(LIB)
	$(INSTALL) -d $(DESTDIR)$(INCLUDEDIR)
	cp -R src/include/. $(DESTDIR)$(INCLUDEDIR)/
	$(INSTALL) -d $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m 0644 $(LIB) $(DESTDIR)$(LIBDIR)/$(LIB_NAME)

uninstall:
	rm -f $(DESTDIR)$(INCLUDEDIR)/Yggdrasil.hpp
	rm -rf $(DESTDIR)$(INCLUDEDIR)/ygg
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_NAME)

doc-clean:
	rm -rf docs/generated

clean:
	rm -rf build/obj lib bin

.PHONY: all check-headers build doc install uninstall doc-clean clean
