#include "Msgq.h"

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
