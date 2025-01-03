#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

// Seperate the message queue structure from the process structure
// to avoid stack smashing
typedef struct process {
    int id;
    int arrival;
    int runtime;
    int priority;
    int memsize;
    int startAddress;
    bool isStopped;
    int remainning;
    double waitingTime;
    int pid;
} process_t;

typedef struct {
    long mtype;
    process_t process;
} msg_t;

int initMsgq(char key);
void sendMsg(process_t process, int msgqid);
void receiveMsg(int msgqid, process_t *process);
