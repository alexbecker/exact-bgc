CC=gcc
CFLAGS=-std=gnu99
LDFLAGS=-lm -lgmp -lpthread
SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c=.o)

evaluate: CFLAGS+=-DMAX_N=$(MAX_N)
evaluate: $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$@ $(LDFLAGS)

check: CFLAGS+=-DMAX_N=5
check: 
	mkdir -p bin
	$(CC) $(CFLAGS) -DTEST_CYCLE_TYPES cycle_types.c -o bin/cycle_types
	$(CC) $(CFLAGS) -DTEST_CCT cycle_types.c count_cycle_types.c \
		-o bin/count_cycle_types $(LDFLAGS)
	$(CC) $(CFLAGS) -DTEST_CHARACTERS cycle_types.c count_cycle_types.c \
		characters.c -o bin/characters $(LDFLAGS)
	bin/cycle_types
	bin/count_cycle_types
	bin/characters

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
