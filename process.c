#include "headers.h"

int clk;
int id;
int remainingtime;

// Simulating CPU-bound process behavior
void simProc() {
    while (remainingtime > 0) {
        // Consume the remaining time only if the clock has ticked
        remainingtime--;
        tickClk();
        printf("Process ID %d PID %d, parent %d, time %d\n", id, getpid(), getppid(), getClk());
    }
}

int main(int argc, char *argv[]) {
    id = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    if (remainingtime < 0) {
        perror("Invalid remaining time");
        exit(-1);
    }

    initClk();
    simProc();

    // Send a termination message to the scheduler when the process finishes
    // The scheduler will handle the termination of the process
    int status = kill(getppid(), SIGUSR1);
    if (status == -1) {
        perror("Error in sending termination signal");
        exit(-1);
    }

    destroyClk(false);
    exit(0);
}
