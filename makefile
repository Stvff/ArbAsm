# Adapted by https://www.github.com/TechnicJelle from:
#  "A Simple Makefile Tutorial" https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc

C_FILES = aasm.c
OUTPUT = aasm

MICRO_TARGET=~/.config/micro/syntax/

RED = \033[0;31m
NC = \033[0m # No Color

$(OUTPUT): FORCE
	$(CC) -o $@ $(C_FILES) -lm

build: $(OUTPUT)

run: $(OUTPUT)
	./$^

clean: FORCE
	rm $(OUTPUT)

FORCE:

test: $(OUTPUT)
	./aasm -P scripts/bigtest/ scripts/bigtest/main.aa > scripts/bigtest/output.txt
	@diff scripts/bigtest/expectedoutput.txt scripts/bigtest/output.txt --ignore-trailing-space --side-by-side --suppress-common-lines | tee diff.txt
	@if [ `wc -l < diff.txt` -gt 1 ]; then \
		echo -e "$(RED)Differences were found!$(NC)"; \
		exit 2; \
	else \
		echo "No differences were found!"; \
	fi
	@rm scripts/bigtest/output.txt diff.txt

formicro: $(OUTPUT)
	mkdir -p $(MICRO_TARGET)
	cp .github/arbasm.yaml $(MICRO_TARGET)

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

help:
	@echo "All make commands that you can run:"
	@echo " make             | Compiles the program for your current platform (Tested on Linux & Windows (Using MSYS2))"
	@echo " make aasm        | Does the same"
	@echo " make build       | Does the same"
	@echo " make run         | Compiles and runs the program"
	@echo " make clean       | Cleans up the build targets"
	@echo " make formicro    | Compiles and sets up some files to add support for ArbAsm in the Micro editor"
	@echo " make forwindows  | Compiles the program for Windows using mingw. (Linux only)"
