#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef short bool;
#define true 1
#define false 1

#define SHKEY 300


///===============================
//don't mess with these variable//
int * shmaddr;                  //
char msqkey = 'P';              //
char termkey = 'T';             //
//================================


typedef struct 
{
    int id;
    int arrival;
    int runtime;
    int priority;
    long mtype;
} process_t;

// Initialize the message queue
int initMsgq(char key)
{
    key_t msgqkey = ftok(".", key);
    int msgqid = msgget(msgqkey, 0666 | IPC_CREAT);

    if (!~msgqid)
    {
        perror("Error in creating message queue");
        exit(-1);
    }
    return msgqid;
}

// Send a message to the message queue
void sendMsg(process_t process, int msgqid)
{
    int send_val = msgsnd(msgqid, &process, sizeof(process), 0);
    if (send_val == -1)
    {
        perror("Error in sending the process");
        exit(-1);
    }
}

// Receive a message from the message queue
process_t receiveMsg(int msgqid)
{
    process_t process;
    int receive_val = msgrcv(msgqid, &process, sizeof(process), 0, 0);
    if (receive_val == -1)
    {
        perror("Error in receiving the process");
        exit(-1);
    }
    return process;
}

int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
