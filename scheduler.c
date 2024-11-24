#include "headers.h"

int clk;
int msqid;
process_t running = {
    .id = -1,
    .arrival = -1,
    .runtime = -1,
    .priority = -1,
    .remainingTime = -1,
    .running = false,
    .pid = -1,
};


pid_t startProcess(process_t process) {
    process.pid = fork();
    if (process.pid == -1) {
        perror("Error in forking process");
        exit(-1);
    }
    if (process.pid == 0) {
        char remainingTime[10];
        sprintf(remainingTime, "%d", process.runtime);
        char id[10];
        sprintf(id, "%d", process.id);
        char *args[] = {"./process.out", id, remainingTime, NULL};
        int proc_exec = execvp(args[0], args);
        if (proc_exec == -1) {
            perror("Error in executing process");
            exit(-1);
        }
        process.running = true;
        exit(0);
    }
    return process.pid;
}

int stopProcess(int pid) {
    int status = kill(pid, SIGSTOP);
    if (status == -1) {
        perror("Error in stopping process");
        exit(-1);
    }
    return status;
}

int continueProcess(int pid) {
    int status = kill(pid, SIGCONT);
    if (status == -1) {
        perror("Error in continuing process");
        exit(-1);
    }
    return status;
}

void handleTermination(int signum) {
    running.running = false;
    printf("Process ID %d terminated at time %d\n", running.id, getClk());
}

void sjf();
void phpf();
void rr();

// Shortest Job First
void sjf() {
    // Initialize the priority queue
    Node *head;
    initializePQ(&head);
    process_t incoming;
    pid_t pid;
    bool last = false;

    // Loop until there are no more processes
    while (true) {
        if (!last) {
            // Receive a new process
            incoming = receiveMsg(msqid);
            if (incoming.id == -1)
                last = true;
            // Push the new process into the priority queue
            printf("Received process with ID %d and runtime %d\n", incoming.id, incoming.runtime);
            push(&head, incoming, incoming.runtime);
        }
        if (!isEmptyPQ(&head) && !running.running) {
            // Pop the process with the shortest runtime
            running = pop(&head);
            if (running.id == -1)
                return;
            running.running = true;
            pid = startProcess(running);
        }
    }
}

// Preemptive Highest Priority First
void phpf() {}

// Round Robin
void rr() {}


int main(int argc, char * argv[])
{
    // Initialize the clock
    printf("Scheduler started PID %d\n", getpid());
    initClk();

    // Arguments
    int algorithm = atoi(argv[1]);
    int quantum = atoi(argv[2]);

    // Initialize the message queue
    msqid = initMsgq(msgkey);

    // Set the signal handler
    signal(SIGCHLD, handleTermination);

    switch (algorithm) {
        case SJF:
            sjf();
            break;
        case PHPF:
            break;
        case RR:
            break;
        default:
            perror("Invalid algorithm");
            exit(-1);
    }
    destroyClk(false);
}
