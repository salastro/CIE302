#include "headers.h"

int clk;
int remainingtime;
int termqid;

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

    termqid = initMsgq(termkey);
    if (termqid == -1) {
        perror("Error in initializing termination queue");
        exit(-1);
    }
    termProc(getpid(), termqid);

    destroyClk(false);
    exit(0);
}
