# Adapted by https://www.github.com/TechnicJelle from:
#  "A Simple Makefile Tutorial" https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc

C_FILES = aasm.c
H_FILES = arbnum_global.h arbnum.h arbnum_stdlib.h
OUTPUT = aasm

MICRO_TARGET=~/.config/micro/syntax/

RED = \033[0;31m
NC = \033[0m # No Color

$(OUTPUT): $(C_FILES) $(H_FILES)
	$(CC) -o $@ $(C_FILES) -lm

build: $(OUTPUT)

forwindows:
	@if x86_64-w64-mingw32-gcc -o $(OUTPUT).exe $(C_FILES); then \
		echo "Compilation succeeded!"; \
	else \
		echo -e "$(RED)Could not use mingw to compile an executable for Windows.$(NC)"; \
		if apt -v > /dev/null ; then \
			echo -e "$(RED)Install it by doing:\n # apt install gcc-mingw-w64-x86-64$(NC)"; \
		else \
			if pacman -V > /dev/null; then \
				echo -e "$(RED)Install it by doing:\n # pacman -S mingw-w64-gcc$(NC)"; \
			fi \
		fi \
	fi

formicro: $(OUTPUT)
	mkdir -p $(MICRO_TARGET)
	cp .github/arbasm.yaml $(MICRO_TARGET)

run: $(OUTPUT)
	./$^

help:
	@echo "All make commands that you can run:"
	@echo " make             | Compiles the program for your current platform (Tested on Linux & Windows (Using MSYS2))"
	@echo " make build       | Does the same"
	@echo " make forwindows  | Compiles the program for Windows using mingw. (Linux only)"
	@echo " make formicro    | Compiles and sets up some files to add support for ArbAsm in the Micro editor"
	@echo " make run         | Compiles and runs the program"
