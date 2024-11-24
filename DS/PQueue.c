#include "PQueue.h"

// Initialize the priority queue
void initializePQ(Node **head)
{
    *head = NULL;
}

// Check if the priority queue is empty
bool isEmptyPQ(Node **head)
{
    return *head == NULL;
}

// Push a process into the priority queue
void push(Node **head, process_t process, int priority)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Error in allocating memory");
        exit(-1);
    }
    newNode->process = process;
    newNode->priority = priority;
    newNode->next = NULL;

    // If the queue is empty or the new process has higher priority
    if (*head == NULL || priority < (*head)->priority) {
        newNode->next = *head;
        *head = newNode;
    } else {
        Node *current = *head;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Pop a process from the priority queue
process_t pop(Node **head)
{
    Node *temp = *head;
    process_t process = temp->process;
    *head = (*head)->next;
    free(temp);
    return process;
}

void printPQ(Node **head)
{
    Node *current = *head;
    while (current != NULL) {
        printf("ID: %d, Arrival: %d, Runtime: %d, Priority: %d\n", current->process.id, current->process.arrival, current->process.runtime, current->process.priority);
        current = current->next;
    }
}

