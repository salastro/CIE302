#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Msgq.h"

// Define a structure for the queue node
typedef struct Node {
    process_t process;
    int priority;
    struct Node *next;
} Node;

void initializePQ(Node **head);
bool isEmptyPQ(Node **head);
void push(Node **head, process_t process, int priority);
process_t pop(Node **head);
void printPQ(Node **head);
