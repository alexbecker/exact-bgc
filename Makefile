CC=gcc
CFLAGS=-std=gnu99 -DMAX_N=$(MAX_N)
LDFLAGS=-lm -lgmp -lpthread
SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c=.o)

evaluate: $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o bin/*

.PHONY: depend
depend:
	./remove_depends.sh
	gcc -MM $(SRC) >> Makefile

# Dependencies listed by gcc -MM
characters.o: characters.c characters.h cycle_types.h
count_cycle_types.o: count_cycle_types.c count_cycle_types.h \
 cycle_types.h
cycle_types.o: cycle_types.c cycle_types.h
evaluate.o: evaluate.c cycle_types.h count_cycle_types.h characters.h
