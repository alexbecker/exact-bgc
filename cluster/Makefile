CC=gcc
CFLAGS=-std=gnu99 -g
LDFLAGS=-lm -lgmp -lpthread
SRC=$(wildcard *.c)

cluster: CFLAGS+=-DMAX_N=$(MAX_N)
cluster: evaluate_from_file.o character_table.o ../characters.o \
			../count_cycle_types.o ../cycle_types.o ../multiplicities.o
	mkdir -p bin
	$(CC) $(CFLAGS) character_table.o ../characters.o ../cycle_types.o -o \
		bin/character_table $(LDFLAGS)
	$(CC) $(CFLAGS) evaluate_from_file.o ../characters.o ../cycle_types.o \
		../count_cycle_types.o ../multiplicities.o -o bin/evaluate_from_file \
		$(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o bin

.PHONY: depend
depend:
	../remove_depends.sh
	gcc -MM $(SRC) >> Makefile

# Dependencies listed by gcc -MM
character_table.o: character_table.c ../characters.h ../cycle_types.h \
 ../cycle_types.h
evaluate_from_file.o: evaluate_from_file.c ../cycle_types.h \
 ../count_cycle_types.h ../cycle_types.h ../multiplicities.h
