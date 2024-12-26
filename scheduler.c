#include "headers.h"
#include "buddy.h"
#include <string.h>

int clk;
int msqid;
Node *head;
process_t running = {
    .id = -1,
    .arrival = -1,
    .runtime = -1,
    .priority = -1,
    .isStopped = -1,
    .remainning = -1,
    .waitingTime = -1,
    .isStopped = false,
    .pid = -1,
    .startAddress = -1,  // Add memory related information
    .memsize = -1,    // Add memory related information
};
process_t incoming;
bool last = false;
bool isRunning = false;

//=============================================================
int totalProcesses = 0;
int totalWaitingTime = 0;
float totalWTA = 0.0;
float WTAValues[100];
int refStart;
int refWait;
float CPUutilization=0.0;
int totalTimeUsed=0;
//==============================================================

//===================== Buddy System Variables =======================
#define MEMORY_SIZE 1024
struct buddy *memory_manager;
//===================================================================

int startProcess(process_t * proc);
int stopProcess(process_t * proc);
int continueProcess(process_t * proc);
void handleProcTerm(int signum);
void handleLastProc(int signum);
void runSJF();
void runPHPF();
void runRR(int quantum);
void writePerFile();
void WritelogFile(const char *state, process_t *proc);
void logMemoryEvent(const char *action, process_t proc);

int startProcess(process_t * proc) {
    printf("Process ID %d starting at time %d\n", proc->id, getClk());
    refStart=getClk();
    proc->waitingTime=(double)getClk()-(double)proc->arrival;
    totalTimeUsed= totalTimeUsed+proc->runtime;
    //proc->remainning=(proc->remainning)-(getClk()-refStart);

    // Attempt to allocate memory for the process
    int mem_offset = buddy_alloc(memory_manager, proc->memsize);
    if (mem_offset == -1) {
        printf("Error: Could not allocate memory for process %d\n", proc->id);
        return -1;
    }
    proc->startAddress = mem_offset;

    logMemoryEvent("allocated", *proc);

    proc->pid = fork();
    if (proc->pid == -1) {
        perror("Error in forking process");
        buddy_free(memory_manager,mem_offset);
        return -1;
    }
    if (proc->pid == 0) {
        char runtime[10];
        refStart=getClk();
        sprintf(runtime, "%d", proc->runtime);
        char id[10];
        sprintf(id, "%d", proc->id);
        char *args[] = {"./process.out", id, runtime, NULL};
        int proc_exec = execvp(args[0], args);
        if (proc_exec == -1) {
            perror("Error in executing process");
            buddy_free(memory_manager,mem_offset);
            exit(-1);
        }

        exit(0);
    }

    WritelogFile("started",proc);


    return proc->pid;
}

int stopProcess(process_t * proc) {
    pid_t pid = proc->pid;
    proc->remainning=proc->remainning-(getClk()-refStart);
    refWait=getClk();
    int status = kill(pid, SIGSTOP);
    if (status == -1) {
        perror("Error in stopping process");
        exit(-1);
    }
    proc->isStopped = true;

    printf("Process ID %d stopped at time %d\n", running.id, getClk());

    WritelogFile("stopped",proc);
    return status;
}

int continueProcess(process_t * proc) {
    pid_t pid = proc->pid;
    refStart=getClk();
    proc->waitingTime=proc->waitingTime+(getClk()-refWait);
    int status = kill(pid, SIGCONT);
    if (status == -1) {
        perror("Error in continuing process");
        exit(-1);
    }
    proc->isStopped = false;
    printf("Process ID %d continued at time %d\n", running.id, getClk());

    WritelogFile("resumed",proc);

    proc->remainning=(proc->remainning)-(getClk()-refStart);

    return status;
}

void handleProcTerm(int signum) {
    isRunning = false;
    printf("Process ID %d terminated at time %d\n", running.id, getClk());
    running.remainning=0;

    // Free memory upon process termination
    if (running.startAddress != -1) {
        logMemoryEvent("freed", running);
        buddy_free(memory_manager, running.startAddress);

        running.startAddress = -1;  // Reset memory start
    }


    //===================================================
    // Calculate TA and WTA
    int TA = getClk() - running.arrival;
    float WTA = (float)TA / running.runtime;

    totalProcesses++;
    totalWaitingTime += getClk() - running.arrival - running.runtime;
    totalWTA += WTA;
    WTAValues[totalProcesses - 1] = WTA;
    // Log the finished process
    FILE *outFile = fopen("scheduler.log", "a");
    fprintf(outFile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n",
            getClk(), running.id, running.arrival, running.runtime,
            getClk() - running.arrival - running.runtime, TA, WTA);
    fclose(outFile);
}

void handleLastProc(int signum) {
    last = true;
    printf("Received last process signal at time %d\n", getClk());
}

void WritelogFile(const char *state, process_t *proc) {
    FILE *outFile = fopen("scheduler.log", "a");
    if (outFile == NULL) {
        perror("Error opening scheduler log file");
        return;
    }
    fprintf(outFile, "At time %d process %d %s arr %d total %d remain %d wait %.2lf\n",
            getClk(), proc->id, state, proc->arrival, proc->runtime, proc->remainning,
            proc->waitingTime);
    fclose(outFile);
}

void logMemoryEvent(const char *action, process_t proc) {
    FILE *memoryLog = fopen("memory.log", "a");
    if (memoryLog == NULL) {
        perror("Error opening memory log file");
        return;
    }
    if (strcmp(action,"allocated")==0)
        fprintf(memoryLog, "At time %d allocated %d bytes for process %d from %d to %d\n",
                getClk(), proc.memsize, proc.id, proc.startAddress,proc.startAddress + buddy_size(memory_manager, proc.startAddress) - 1);
    else
        fprintf(memoryLog, "At time %d freed %d bytes from process %d from %d to %d\n",
                getClk(), proc.memsize, proc.id, proc.startAddress,proc.startAddress + buddy_size(memory_manager, proc.startAddress) - 1);
    fclose(memoryLog);
}


void writePerFile() {

    float avgWTA = totalWTA / totalProcesses;
    float avgWaitingTime = (float)totalWaitingTime / totalProcesses;

    // Calculate std of WTA
    float variance = 0.0;
    double somevar;
    for (int i = 0; i < totalProcesses; i++) {
        somevar = WTAValues[i] - avgWTA;
        variance += pow(somevar, 2);
    }
    variance /= totalProcesses;
    float stdWTA = sqrt(variance);
    CPUutilization=((float)totalTimeUsed/getClk())*100;

    FILE *perfFile = fopen("scheduler.perf", "w");
    fprintf(perfFile, "CPU utilization = %.2f %%\n",CPUutilization);
    fprintf(perfFile, "Avg WTA = %.2f\n", avgWTA);
    fprintf(perfFile, "Avg Waiting = %.2f\n", avgWaitingTime);
    fprintf(perfFile, "Std WTA = %.2f\n", stdWTA);
    fclose(perfFile);
}

// Shortest Job First
void runSJF() {
    // Loop until there are no more processes
    while (true) {
        if (!last) {
            // Receive a new process
            receiveMsg(msqid, &incoming);
            if (incoming.id != -1) {
                // Push the new process into the priority queue
                printf("Received process with ID %d and runtime %d\n",
                       incoming.id, incoming.runtime);
                push(&head, incoming, incoming.runtime);
            }
        }
        if (!isEmptyPQ(&head) && !isRunning) {
            // Pop the process with the shortest runtime
            running = pop(&head);
            //check the memory allocation
            if(startProcess(&running)==-1)
            {
                push(&head, running, running.runtime);
                waitClk();
                continue;
            }

            if (running.pid == -1)
                return;
            isRunning = true;
        }
        else if (isEmptyPQ(&head) && last && !isRunning) {
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
            receiveMsg(msqid, &incoming);
            if (incoming.id != -1) {
                // Push the new process into the priority queue
                printf("Received process with ID %d and priority %d\n",
                       incoming.id, incoming.priority);
                push(&head, incoming, incoming.priority);
                if (isRunning && head->priority < running.priority) {
                    stopProcess(&running);
                    push(&head, running, running.priority);
                    isRunning = false;
                }
            }
        }
        if (!isEmptyPQ(&head) && !isRunning) {
            // Pop the process with the shortest runtime
            running = pop(&head);
            // Check if the process is new or continued
            if (!running.isStopped) {
                if (startProcess(&running) == -1)
                {
                    push(&head, running, running.priority);
                    waitClk();
                    continue;
                }

                if (running.pid == -1)
                    return;
            } else {
                continueProcess(&running);
            }
            isRunning = true;
        }
        else if (isEmptyPQ(&head) && last && !isRunning) {
            printf("No more processes to run\n");
            return;
        }
    }
}

// Round Robin
void runRR(int quantum) {
    int remainingQuantum = quantum;
    while (true) {
        receiveMsg(msqid, &incoming);
        if (incoming.id != -1) {
            printf("Received process with ID %d\n", incoming.id);
            push(&head, incoming, 0);
        }
        if (!isEmptyPQ(&head) && !isRunning) {
            running = pop(&head);
            if (!running.isStopped) {
                if (startProcess(&running) == -1)
                {
                    push(&head, running, 0);
                    waitClk();
                    continue;
                }
            } else {
                continueProcess(&running);
            }
            remainingQuantum = quantum;
            isRunning = true;
        } 
        else if (isRunning) {
            remainingQuantum--;
            running.remainning--;
            if (running.remainning <= 0) {
                continue;
            } 
            else if (remainingQuantum <= -1 && !isEmptyPQ(&head)) {
                stopProcess(&running);
                push(&head, running, 0);
                isRunning = false;
            }
            waitClk();
        } 
        else if (isEmptyPQ(&head) && last && !isRunning) {
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
    signal(SIGINT, clearResources);


    // Initialize memory manager
    memory_manager = buddy_new(MEMORY_SIZE);
    if (!memory_manager) {
        fprintf(stderr, "Failed to initialize buddy system\n");
        exit(-1);
    }

    // Log the header of the memory log
    FILE *memoryLog = fopen("memory.log", "w");
    if (memoryLog == NULL) {
        perror("Error opening memory log file");
    }
    fprintf(memoryLog, "#At time x allocated y bytes for process z from i to j\n");
    fclose(memoryLog);


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
    writePerFile();
    buddy_destory(memory_manager);
    destroyClk(false);
}
