CC = gcc
CFLAGS = -g3 -Wall
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source
PDIR = pipes

EXECUTABLE = diseaseAggregator

_DEPS = pipes.h diseaseAggregator.h workers.h dataStructures.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o pipes.o workers.o diseaseAggregator.o dataStructures.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -w 20 -b 20 -i input

valgrind:
	valgrind --leak-check=full ./$(EXECUTABLE) -w 20 -b 20 -i input

clean:
	rm -f $(ODIR)/*.o
	rm diseaseAggregator
	rm -rf $(PDIR)/*