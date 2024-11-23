build:
	gcc process_generator.c DS/Msgq.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c DS/Msgq.c DS/PQueue.c -o scheduler.out
	gcc process.c DS/Msgq.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./test_generator.out
	./process_generator.out processes.txt
