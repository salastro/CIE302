CFLAGS = -g -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -fstack-protector-all -march=native -ftree-vectorize -ffast-math
SRC = process_generator.c clk.c scheduler.c process.c test_generator.c buddy.c
DS_SRC = DS/Msgq.c DS/PQueue.c
OBJ = $(SRC:.c=.o)
DS_OBJ = $(DS_SRC:.c=.o)

all: build

build: process_generator.out clk.out scheduler.out process.out test_generator.out

process_generator.out: process_generator.o DS/Msgq.o
	gcc $(CFLAGS) $^ -o $@

clk.out: clk.o
	gcc $(CFLAGS) $^ -o $@

scheduler.out: scheduler.o DS/Msgq.o DS/PQueue.o buddy.o
	gcc $(CFLAGS) $^ -o $@ -lm

process.out: process.o DS/Msgq.o
	gcc $(CFLAGS) $^ -o $@

test_generator.out: test_generator.o
	gcc $(CFLAGS) $^ -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.out *.o DS/*.o processes.txt scheduler.log scheduler.perf memory.log

run: build
	./test_generator.out
	./process_generator.out processes.txt
