#include "headers.h"

int clk;
int remainingtime;

// Simulating CPU-bound process behavior
void simProc() {
    while (remainingtime > 0) {
        // Consume the remaining time only if the clock has ticked
        clk = getClk();
        while (clk == getClk());
        printf("Process with PID %d at time %d\n", getpid(), getClk());
        remainingtime--;
    }
}

int main(int argc, char *argv[]) {
    printf("Starting process with PID %d\n", getpid());
    remainingtime = atoi(argv[0]);
    if (remainingtime <= 0) {
        printf("Invalid remaining time %d\n", remainingtime);
        exit(-1);
    }

    initClk();
    simProc();

    // Send a termination message to the scheduler Using default SIGCHLD signal
    // Does not need to be explicitly handled

    destroyClk(false);
    exit(0);
}
