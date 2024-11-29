CFLAGS = -g -Wall -Wextra -std=c99 -pedantic -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -fstack-protector-all -march=native -ftree-vectorize -ffast-math

build:
	gcc $(CFLAGS) process_generator.c DS/Msgq.c -o process_generator.out
	gcc $(CFLAGS) clk.c -o clk.out
	gcc $(CFLAGS) scheduler.c DS/Msgq.c DS/PQueue.c -o scheduler.out
	gcc $(CFLAGS) process.c DS/Msgq.c -o process.out
	gcc $(CFLAGS) test_generator.c -o test_generator.out

clean:
	rm -f *.out processes.txt

all: clean build

run:
	./test_generator.out
	./process_generator.out processes.txt
