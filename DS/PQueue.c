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
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (isEmptyPQ(head) || priority < (*head)->process.priority) {
        newNode->next = *head;
        *head = newNode;
    } else {
        Node *current = *head;
        while (current->next != NULL && current->next->process.priority <= priority) {
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
        printf("Process with ID %d and priority %d\n", current->process.id, current->process.priority);
        current = current->next;
    }
}

