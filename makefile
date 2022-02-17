# Adapted by https://www.github.com/TechnicJelle from:
#  "A Simple Makefile Tutorial" https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc

C_FILES = ArbAsmCli.c
OUTPUT = ArbAsmCli

$(OUTPUT): $(C_FILES)
	$(CC) -o $@ $^ -lm

run: $(OUTPUT)
	./$^
