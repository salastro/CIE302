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
    msg_t msg;
    msg.mtype = 1;
    msg.process = process;
    int send_val = msgsnd(msgqid, &msg, sizeof(msg.process), IPC_NOWAIT);
    if (send_val == -1)
    {
        perror("Error in sending the process");
        exit(-1);
    }
}

// Receive a message from the message queue
void receiveMsg(int msgqid, process_t *process)
{
    msg_t msg;
    int receive_val = msgrcv(msgqid, &msg, sizeof(msg.process), 0, IPC_NOWAIT);
    if (receive_val == -1) {
        // perror("Error in receiving the process");
        process->id = -1;
    }
    else {
        *process = msg.process;
    }
}
