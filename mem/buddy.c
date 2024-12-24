#include "buddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to create a new memory block
static MemoryBlock *createMemoryBlock(int startAddress, int size,
                                      int processID) {
    MemoryBlock *block = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    block->startAddress = startAddress;
    block->size = size;
    block->processID = processID;
    block->left = NULL;
    block->right = NULL;
    return block;
}

// Initialize the memory system
void initMemory(int totalSize, MemoryBlock *root) {
    root = createMemoryBlock(0, totalSize, -1);
}

// Allocate memory for a process
int allocateMemory(int processID, int size, MemoryBlock *root) {
    // Check for invalid inputs
    if (!root)
        return -1; // No memory available
    if (size <= 0)
        return -1; // Invalid size

    // If the block is free and large enough
    if (root->processID == -1 && root->size >= size) {
        // Check if splitting is needed
        if (root->size == size) {
            root->processID = processID;
            return root->startAddress;
        }

        // Split the block
        if (!root->left && !root->right) {
            int halfSize = root->size / 2;
            // Check if the block can be split
            // If the block is too small, allocate in the current block
            if (halfSize < size) {
                root->processID = processID;
                return root->startAddress;
            }
            root->left = createMemoryBlock(root->startAddress, halfSize, -1);
            root->right =
                createMemoryBlock(root->startAddress + halfSize, halfSize, -1);
        }

        // Allocate memory in the left or right subtree
        int address = allocateMemory(processID, size, root->left);
        if (address != -1)
            return address;
        return allocateMemory(processID, size, root->right);
    }

    return -1; // Not enough memory
}

// Deallocate memory for a process
void deallocateMemory(int startAddress, MemoryBlock *root) {
    // Check for invalid inputs
    if (!root)
        return;

    // If the block is found, deallocate it
    if (root->startAddress == startAddress) {
        root->processID = -1;
        return;
    }

    // Recursively search for the block
    deallocateMemory(startAddress, root->left);
    deallocateMemory(startAddress, root->right);
}

// Log memory operations
void logMemory(const char *operation, int processID, int startAddress,
               int size) {
    FILE *outFile = fopen("memory.log", "a");
    fprintf(outFile, "%s: Process %d, Address %d, Size %d\n", operation,
            processID, startAddress, size);
    fclose(outFile);
}

// Cleanup memory tree
void cleanupMemory(MemoryBlock *root) {
    if (!root)
        return;

    // Recursively cleanup the subtrees
    cleanupMemory(root->left);
    cleanupMemory(root->right);

    // Free the current node
    free(root);
}

// Debugging function to print the memory layout
void printMemory(MemoryBlock *root, int level, bool isLeft) {
    if (!root)
        return;

    // Print the current node
    for (int i = 0; i < level; i++)
        printf("  ");
    if (isLeft)
        printf("L:");
    else
        printf("R:");
    printf("[%d, %d] - %d\n", root->startAddress, root->startAddress + root->size,
           root->processID);

    // Recursively print the left and right subtrees
    printMemory(root->left, level + 1, true);
    printMemory(root->right, level + 1, false);
}
