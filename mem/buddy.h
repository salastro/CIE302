#pragma once
#include <stdbool.h>

// Define the memory block structure
// This structure represents a block of memory in the memory tree
typedef struct MemoryBlock {
    int startAddress;
    int size;
    int processID;
    struct MemoryBlock *left;
    struct MemoryBlock *right;
} MemoryBlock;

// Initialize the memory system
void initMemory(int totalSize, MemoryBlock *root);

// Allocate memory for a process
// Buddy System Algorithm
int allocateMemory(int processID, int size, MemoryBlock *root);

// Deallocate memory for a process
// Buddy System Algorithm
void deallocateMemory(int startAddress, MemoryBlock *root);

// Log memory operations
void logMemory(const char *operation, int processID, int startAddress, int size);

// Cleanup memory tree
void cleanupMemory(MemoryBlock *root);

// Debugging function to print the memory layout
void printMemory(MemoryBlock *root, int level, bool isLeft);
