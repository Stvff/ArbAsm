# Adapted from:
#  "A Simple Makefile Tutorial" https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

IDIR =.
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=.
LDIR =.

LIBS=-lm

_DEPS = arbnum.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = ArbAsmCli.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ArbAsmCli: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
