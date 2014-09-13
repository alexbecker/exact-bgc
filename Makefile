CC=gcc
CFLAGS=-std=gnu99 -O3
LDFLAGS=-lm -lgmp -lpthread
SRC:=$(wildcard *.c) $(wildcard cluster/*.c)

evaluate: CFLAGS+=-DMAX_N=$(MAX_N)
evaluate: OBJ:=evaluate.o characters.o count_cycle_types.o cycle_types.o multiplicities.o
evaluate: evaluate.o characters.o count_cycle_types.o cycle_types.o multiplicities.o
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$@ $(LDFLAGS)

.PHONY: cluster
cluster:
	$(MAKE) --directory=$@ MAX_N=$(MAX_N)

.PHONY: check
check: CFLAGS+=-DMAX_N=20
check: 
	mkdir -p bin
	$(CC) $(CFLAGS) -DTEST_CYCLE_TYPES cycle_types.c -o bin/cycle_types
	$(CC) $(CFLAGS) -DTEST_CCT cycle_types.c count_cycle_types.c \
		-o bin/count_cycle_types $(LDFLAGS)
	$(CC) $(CFLAGS) -DTEST_CHARACTERS cycle_types.c count_cycle_types.c \
		characters.c -o bin/characters $(LDFLAGS)
	bin/cycle_types 5
	bin/count_cycle_types
	bin/characters 5

.PHONY: clean
clean:
	rm -rf *.o bin
	$(MAKE) --directory=cluster $@

.PHONY: depend
depend:
	./remove_depends.sh
	gcc -MM $(SRC) >> Makefile
	$(MAKE) --directory=cluster $@

# Dependencies listed by gcc -MM
characters.o: characters.c characters.h cycle_types.h
count_cycle_types.o: count_cycle_types.c count_cycle_types.h \
 cycle_types.h
evaluate.o: evaluate.c cycle_types.h count_cycle_types.h characters.h \
 multiplicities.h
multiplicities.o: multiplicities.c multiplicities.h cycle_types.h
cycle_types.o: cycle_types.c cycle_types.h
character_table.o: cluster/character_table.c cluster/../characters.h \
 cluster/../cycle_types.h cluster/../cycle_types.h
evaluate_from_file.o: cluster/evaluate_from_file.c \
 cluster/../cycle_types.h cluster/../count_cycle_types.h \
 cluster/../cycle_types.h cluster/../multiplicities.h
