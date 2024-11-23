#include "headers.h"

int clk;
int msqid;

pid_t startProcess(process_t process) {
    process.pid = fork();
    char remainingTime[5];
    sprintf(remainingTime, "%d", process.runtime);
    char *args[] = {"./process.out", remainingTime, NULL};
    int proc_exec = execvp(args[0], args);
    if (proc_exec == -1) {
        perror("Error in executing process");
        exit(-1);
    }
    return process.pid;
}

void stopProcess(int pid) {
    kill(pid, SIGSTOP);
}

void continueProcess(int pid) {
    kill(pid, SIGCONT);
}

void sjf();
void phpf();
void rr();

void sjf() {
    // Initialize the priority queue
    Node *head;
    initializePQ(&head);
    process_t process;
    process.id = 0;
    int pid;

    while (true) {
        // Receive a process from the message queue
        msgrcv(msqid, &process, sizeof(process), 0, !IPC_NOWAIT);

        // Push the process into the priority queue
        push(&head, process, process.runtime);

        // If the priority queue is not empty, start the process
        if (!isEmptyPQ(&head)) {
            process = pop(&head);
            startProcess(process);
        }
    }
}

int main(int argc, char * argv[])
{
    initClk();

    // Arguments
    int algorithm = atoi(argv[1]);
    int quantum = atoi(argv[2]);

    // Initialize the message queue
    msqid = initMsgq(msgkey);

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
