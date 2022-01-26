# C compiler
CC = gcc
CFLAGS = -Wall -I$(IDIR)
LIBS = -lpthread

# directory containing *.h
IDIR = include
# directory to store *.obj
ODIR = obj
# directory containing source code
SRC = src


_DEPS = router.h message.h neighbour.h load_config.h queue.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o load_config.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

router: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
