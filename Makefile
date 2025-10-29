# Copyright (c) 2025 mcpeaps_HD
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

CC = cc
WORKDIR = .

DEBUG = 1

# If windows use clang
ifeq ($(OS),Windows_NT)
		CC = clang
endif

.PHONY: all run build_rel build_debug  nob clean

all: run

# Build nob tool first, then use it to build and run
ifeq ($(DEBUG), 1)
RUN_TARGET = build_debug
else
RUN_TARGET = build_rel
endif

run: $(RUN_TARGET)
	cd $(WORKDIR) && ./nob -r

# Just build with nob
build_debug: nob
		cd $(WORKDIR) && ./nob -b -d

# Just build with nob
build_rel: nob $(WORKDIR)/src/main.cpp $(WORKDIR)/src/constants.hpp $(WORKDIR)/src/nord.hpp $(WORKDIR)/thirdparty/flag.h
		cd $(WORKDIR) && ./nob -b

# Build the nob tool
nob: $(WORKDIR)/nob.c $(WORKDIR)/nob.h $(WORKDIR)/flag.h
		$(CC) -o $(WORKDIR)/nob $(WORKDIR)/nob.c

# Clean build artifacts
clean: nob
		cd $(WORKDIR) && ./nob --clean && rm -f ./nob.old
