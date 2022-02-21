# C compiler
CC = gcc
CFLAGS = -I$(IDIR) -Wall
LIBS = -lpthread

# directory containing *.h
IDIR = include
# directory to store *.obj
ODIR = obj
# directory containing source code
SRC = src

_DEPS = main.h load_config.h message.h neighbour.h queue.h router.h routing_table.h
_OBJS = main.o load_config.o message.o neighbour.o queue.o router.o routing_table.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

router: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
