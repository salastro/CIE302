#include "headers.h"

int clk;
int id;
int remainingtime;

// Simulating CPU-bound process behavior
void simProc() {
    while (remainingtime > 0) {
        // Consume the remaining time only if the clock has ticked
        clk = getClk();
        while (clk == getClk());
        printf("Process ID %d PID %d, parent %d, time %d\n", id, getpid(), getppid(), getClk());
        remainingtime--;
    }
}

int main(int argc, char *argv[]) {
    id = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    if (remainingtime <= 0) {
        perror("Invalid remaining time");
        exit(-1);
    }

    initClk();
    simProc();

    // Send a termination message to the scheduler Using default SIGCHLD signal
    // Does not need to be explicitly handled

    destroyClk(false);
    exit(0);
}
