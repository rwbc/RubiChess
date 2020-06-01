

ifeq ($(COMP),)
	COMP=gcc
endif

CXXFLAGS=-std=c++11 -Wall -O3 $(EXTRACXXFLAGS)

ifeq ($(COMP),gcc)
	CXX=g++
	CXXFLAGS += -pedantic -Wextra -Wshadow
	ifeq ($(shell uname -m),x86_64)
		#ARCHFLAGS=-msse3 -mpopcnt
	endif
	ifeq ($(shell uname -m),armv7l)
		ARCHFLAGS=-march=armv7-a
	endif
	LDFLAGS=-pthread  $(EXTRALDFLAGS)

	profile_make = gcc-profile-make
	profile_use = gcc-profile-use
endif

ifeq ($(COMP),clang)
	CXX=clang++
	CXXFLAGS += -pedantic -Wextra -Wshadow
	ifeq ($(shell uname -m),x86_64)
		#ARCHFLAGS=-msse3 -mpopcnt
	endif
	ifeq ($(shell uname -m),armv7l)
		ARCHFLAGS=-march=armv7-a
	endif
	LDFLAGS=-lpthread  -fuse-ld=lld $(EXTRALDFLAGS)

	profile_make = clang-profile-make
	profile_use = clang-profile-use
endif

DEPS = RubiChess.h
EXE = RubiChess
PGOBENCH1 = ./$(EXE) -profile
PROFDIR1 = OPT

GITVER = $(shell 2>/dev/null git show --name-only --abbrev-commit --date=format:%Y%m%d%H%M%S | grep -i "date" | grep -o -E '[0-9]+')
GITID = $(shell 2>/dev/null git show --name-only --abbrev-commit | grep -i -o -E "ommit[[:blank:]]+[0-9a-f]+" | grep -o -E '[0-9a-f]+')
ifneq ($(GITVER),)
    GITDEFINE = -D GITVER=\"$(GITVER)\"
endif
ifneq ($(GITID),)
    GITDEFINE += -D GITID=\"$(GITID)\"
endif

.PHONY: clean profile-build gcc-profile-make gcc-profile-use all

all: RubiChess

RubiChess:
	$(CXX) $(CXXFLAGS) $(EXTRACXXFLAGS) $(ARCHFLAGS) *.cpp $(LDFLAGS) $(EXTRALDFLAGS) $(GITDEFINE) -o $(EXE)

objclean:
	$(RM) $(EXE) *.o

profileclean:
	$(RM) -rf $(PROFDIR1)
	$(RM) *.clangprof-raw *.profdata

clean: objclean profileclean

gcc-profile-make:
	$(MAKE) EXTRACXXFLAGS='-fprofile-generate=$(PROFDIR1) -flto' EXTRALDFLAGS='-lgcov' $(EXE)

gcc-profile-use:
	$(MAKE) EXTRACXXFLAGS='-fprofile-use=$(PROFDIR1) -fno-peel-loops -fno-tracer -flto' EXTRALDFLAGS='-lgcov' $(EXE)

clang-profile-make:
	$(MAKE) ARCH=$(ARCH) COMP=$(COMP) EXTRACXXFLAGS='-fprofile-instr-generate=$(EXE).clangprof-raw -flto' EXTRALDFLAGS=' -fprofile-instr-generate=$(EXE).clangprof-raw' $(EXE)

clang-profile-use:
	llvm-profdata merge -output=$(EXE).profdata $(EXE).clangprof-raw
	$(MAKE) ARCH=$(ARCH) COMP=$(COMP) EXTRACXXFLAGS='-fprofile-instr-use=$(EXE).profdata -flto' EXTRALDFLAGS='-fprofile-instr-use=$(EXE).profdata' $(EXE)

profile-build: clean
	@echo "Building instrumented executable ..."
	$(MAKE) $(profile_make)
	@echo "Running benchmark for pgo-build ..."
	$(PGOBENCH1) > /dev/null
	@echo "Building optimized executable ..."
	$(MAKE) objclean
	$(MAKE) $(profile_use)
	@echo "Deleting profile data ..."
	$(MAKE) profileclean
