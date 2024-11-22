#include "headers.h"

int clk;
int remainingtime;
int termqid;

// Handle the command line arguments
void parseArgs(int argc, char *argv[]) {
    remainingtime = atoi(argv[1]);
    if (remainingtime <= 0) {
        perror("Invalid remaining time");
        exit(-1);
    }
}

// Simulating CPU-bound behavior
void simulateProcess() {
    while (remainingtime > 0) {
        // Consume the remaining time only if the clock has ticked
        clk = getClk();
        while (clk == getClk());
        remainingtime--;
    }
}

// Notify the scheduler upon completion
void notifyScheduler() {
    termqid = initMsgq(termkey);
    if (termqid == -1) {
        perror("Error in initializing termination queue");
        exit(-1);
    }
    termination_t term;
    term.mtype = 1;
    term.process_id = getpid();
    int send_val = msgsnd(termqid, &term, sizeof(term), 0);
    if (send_val == -1) {
        perror("Error in sending the termination message");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    initClk();
    simulateProcess();
    notifyScheduler();
    destroyClk(false);
    exit(0);
}
