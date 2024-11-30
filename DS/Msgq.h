#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

// bool and long causes stack smashing
// TODO: Fix the issue
typedef struct {
    // long mtype;
    int id;
    int arrival;
    int runtime;
    int priority;
    // bool isRunning;
    // bool isStopped;
    int isRunning;
    int isStopped;
    int remainning;
    double waitingTime;
    pid_t pid;

} process_t;

int initMsgq(char key);
void sendMsg(process_t process, int msgqid);
process_t receiveMsg(int msgqid);
