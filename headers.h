#include <stdio.h>
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
#include <stdbool.h>
#include "DS/Msgq.h"
#include "DS/PQueue.h"
#include <math.h>

#define SHKEY 300

#define SJF 1
#define PHPF 2
#define RR 3


///===============================
//don't mess with these variable//
int * shmaddr;                  //
char msgkey = 'P';             //
int msgqid;                     //
//================================

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

// Wait for one clock second
void waitClk() {
    int clk = getClk();
    while (clk == getClk());
}


// Clear all resources in case of interruption
void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msgqid, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    exit(0);
}
