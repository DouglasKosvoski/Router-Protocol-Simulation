IDIR = include
CC = gcc
CFLAGS = -Wall -I$(IDIR)
ODIR = obj
SRC = src
LIBS = -lpthread


_DEPS = router.h message.h neighbour.h load_config.h
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
