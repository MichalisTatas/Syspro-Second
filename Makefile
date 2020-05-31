CC = gcc
CFLAGS = -g3 -Wall -Wextra
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source
PDIR = pipes

EXECUTABLE = diseaseAggregator

_DEPS = pipes.h diseaseAggregator.h workers.h list.h patient.h hashTable.h avlTree.h queriesHandling.h queriesAnswering.h queue.h binaryHeap.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o pipes.o workers.o diseaseAggregator.o list.o patient.o hashTable.o avlTree.o queriesHandling.o queriesAnswering.o queue.o binaryHeap.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -w 4 -b 20 -i ./bashScript/input_dir/

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE) -w 4 -b 20 -i ./bashScript/input_dir/

clean:
	rm -f $(ODIR)/*.o
	rm diseaseAggregator
	rm -rf $(PDIR)/*