CC=gcc
CFLAGS=-std=gnu99 -O3 -pg
LDFLAGS=-lm -lgmp -lpthread
SRC:=$(wildcard *.c)

evaluate: CFLAGS+=-DMAX_N=$(MAX_N)
evaluate: OBJ:=evaluate.o characters.o count_cycle_types.o cycle_types.o multiplicities.o
evaluate: evaluate.o characters.o count_cycle_types.o cycle_types.o multiplicities.o
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$@ $(LDFLAGS)

cluster: CFLAGS+=-DMAX_N=$(MAX_N)
cluster: evaluate_from_file.o character_table.o characters.o count_cycle_types.o \
			cycle_types.o multiplicities.o
	mkdir -p bin
	$(CC) $(CFLAGS) character_table.o characters.o cycle_types.o -o \
		bin/character_table $(LDFLAGS)
	$(CC) $(CFLAGS) evaluate_from_file.o characters.o count_cycle_types.o \
		cycle_types.o multiplicities.o -o bin/evaluate_from_file $(LDFLAGS)

.PHONY: check
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
	rmdir bin

.PHONY: depend
depend:
	./remove_depends.sh
	gcc -MM $(SRC) >> Makefile

# Dependencies listed by gcc -MM
characters.o: characters.c characters.h cycle_types.h
count_cycle_types.o: count_cycle_types.c count_cycle_types.h \
 cycle_types.h
evaluate.o: evaluate.c cycle_types.h count_cycle_types.h characters.h \
 multiplicities.h
multiplicities.o: multiplicities.c multiplicities.h cycle_types.h
character_table.o: character_table.c characters.h cycle_types.h
evaluate_from_file.o: evaluate_from_file.c cycle_types.h multiplicities.h
cycle_types.o: cycle_types.c cycle_types.h
