#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

typedef struct {
    int id;
    int arrival;
    int runtime;
    int priority;
    int remainingTime;
    bool running;
    pid_t pid;
    // long mtype;
} process_t;

typedef struct {
    long mtype;
    int pid;
} termination_t;

int initMsgq(char key);
void sendMsg(process_t process, int msgqid);
process_t receiveMsg(int msgqid);
void termProc(int pid, int termqid);
