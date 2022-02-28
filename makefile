# Adapted by https://www.github.com/TechnicJelle from:
#  "A Simple Makefile Tutorial" https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc

C_FILES = aasm.c
OUTPUT = aasm

MICRO_TARGET=~/.config/micro/syntax/

RED = \033[0;31m
NC = \033[0m # No Color

$(OUTPUT): $(C_FILES)
	$(CC) -o $@ $^ -lm

windows:
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
	cp scripts/arbasm.yaml $(MICRO_TARGET)

run: $(OUTPUT)
	./$^
