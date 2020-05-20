CC = gcc
CFLAGS = -g3 -Wall
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source
PDIR = pipes

EXECUTABLE = diseaseMonitor

_DEPS = pipes.h diseaseAggregator.h workers.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o pipes.o workers.o diseaseAggregator.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -w 5 -b 5 -i input

valgrind:
	valgrind --leak-check=full ./$(EXECUTABLE) -w 5 -b 5 -i input

clean:
	rm -f $(ODIR)/*.o
	rm -f $(BDIR)/$(EXECUTABLE)
	rm -rf $(PDIR)/*