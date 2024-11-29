#include "headers.h"

int clk;
int msqid;
Node *head;
process_t running = {
    .id = -1,
    .arrival = -1,
    .runtime = -1,
    .priority = -1,
    .isStopped = -1,
    .isStopped = false,
    .isRunning = false,
    .pid = -1,
};
process_t incoming;
bool last = false;

int startProcess(process_t * proc);
int stopProcess(process_t * proc);
int continueProcess(process_t * proc);
void handleProcTerm(int signum);
void handleLastProc(int signum);
void runSJF();
void runPHPF();
void runRR(int quantum);

int startProcess(process_t * proc) {
    printf("Process ID %d starting at time %d\n", proc->id, getClk());
    proc->pid = fork();
    if (proc->pid == -1) {
        perror("Error in forking process");
        exit(-1);
    }
    if (proc->pid == 0) {
        char runtime[10];
        sprintf(runtime, "%d", proc->runtime);
        char id[10];
        sprintf(id, "%d", proc->id);
        char *args[] = {"./process.out", id, runtime, NULL};
        int proc_exec = execvp(args[0], args);
        if (proc_exec == -1) {
            perror("Error in executing process");
            exit(-1);
        }
        exit(0);
    }
    return proc->pid;
}

int stopProcess(process_t * proc) {
    pid_t pid = proc->pid;
    int status = kill(pid, SIGSTOP);
    if (status == -1) {
        perror("Error in stopping process");
        exit(-1);
    }
    proc->isStopped = true;
    printf("Process ID %d stopped at time %d\n", running.id, getClk());
    return status;
}

int continueProcess(process_t * proc) {
    pid_t pid = proc->pid;
    int status = kill(pid, SIGCONT);
    if (status == -1) {
        perror("Error in continuing process");
        exit(-1);
    }
    proc->isStopped = false;
    printf("Process ID %d continued at time %d\n", running.id, getClk());
    return status;
}

void handleProcTerm(int signum) {
    running.isRunning = false;
    printf("Process ID %d terminated at time %d\n", running.id, getClk());
}

void handleLastProc(int signum) {
    last = true;
    printf("Received last process signal at time %d\n", getClk());
}

// Shortest Job First
void runSJF() {
    // Loop until there are no more processes
    while (true) {
        if (!last) {
            // Receive a new process
            incoming = receiveMsg(msqid);
            if (incoming.id != -1) {
                // Push the new process into the priority queue
                printf("Received process with ID %d and runtime %d\n",
                       incoming.id, incoming.runtime);
                push(&head, incoming, incoming.runtime);
            }
        }
        if (!isEmptyPQ(&head) && !running.isRunning) {
            // Pop the process with the shortest runtime
            running = pop(&head);
            running.pid = startProcess(&running);
            if (running.pid == -1)
                return;
            running.isRunning = true;
        }
        else if (isEmptyPQ(&head) && last && !running.isRunning) {
            printf("No more processes to run\n");
            return;
        }
    }
}

// Preemptive Highest Priority First
void runPHPF() {
    // Loop until there are no more processes
    while (true) {
        if (!last) {
            // Receive a new process
            incoming = receiveMsg(msqid);
            if (incoming.id != -1) {
                // Push the new process into the priority queue
                printf("Received process with ID %d and priority %d\n",
                       incoming.id, incoming.priority);
                push(&head, incoming, incoming.priority);
                if (running.isRunning && head->priority < running.priority) {
                    stopProcess(&running);
                    push(&head, running, running.priority);
                    running.isRunning = false;
                }
            }
        }
        if (!isEmptyPQ(&head) && !running.isRunning) {
            // Pop the process with the shortest runtime
            running = pop(&head);
            // Check if the process is new or continued
            if (!running.isStopped) {
                running.pid = startProcess(&running);
                if (running.pid == -1)
                    return;
            } else {
                continueProcess(&running);
            }
            running.isRunning = true;
        }
        else if (isEmptyPQ(&head) && last && !running.isRunning) {
            printf("No more processes to run\n");
            return;
        }
    }
}

// Round Robin
void runRR(int quantum) {
    int remainingQuantum = quantum;
    // Loop until there are no more processes
    while (true) {
        if (!last) {
            // Receive a new process
            incoming = receiveMsg(msqid);
            if (incoming.id != -1) {
                // Push the new process into the priority queue
                printf("Received process with ID %d\n", incoming.id);
                push(&head, incoming, 0);
            }
        }
        if (!isEmptyPQ(&head) && !running.isRunning) {
            running = pop(&head);
            if (!running.isStopped) {
                int status = startProcess(&running);
                if (status == -1)
                    return;
            } else {
                continueProcess(&running);
            }
            remainingQuantum = quantum;
            running.isRunning = true;
        }
        else if (running.isRunning) {
            remainingQuantum--;
            tickClk();
            if (remainingQuantum <= 0 && !isEmptyPQ(&head)) {
                stopProcess(&running);
                push(&head, running, 0);
                running.isRunning = false;
            }
        }
        else if (isEmptyPQ(&head) && last && !running.isRunning) {
            printf("No more processes to run\n");
            return;
        }
    }
}


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

    // Initialize the priority queue
    initializePQ(&head);

    // Set the signal handler
    // signal(SIGCHLD, handleTermination);
    signal(SIGUSR1, handleProcTerm);
    signal(SIGUSR2, handleLastProc);

    switch (algorithm) {
        case SJF:
            runSJF();
            break;
        case PHPF:
            runPHPF();
            break;
        case RR:
            runRR(quantum);
            break;
        default:
            perror("Invalid algorithm");
            exit(-1);
    }
    destroyClk(false);
}
