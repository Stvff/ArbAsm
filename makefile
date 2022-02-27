# Adapted by https://www.github.com/TechnicJelle from:
#  "A Simple Makefile Tutorial" https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc

C_FILES = aasm.c
OUTPUT = aasm

MICRO_TARGET=~/.config/micro/syntax/

$(OUTPUT): $(C_FILES)
	$(CC) -o $@ $^ -lm

formicro:
	mkdir -p $(MICRO_TARGET)
	cp scripts/arbasm.yaml $(MICRO_TARGET)

run: $(OUTPUT)
	./$^
