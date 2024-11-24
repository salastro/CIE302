#include "headers.h"
#include <limits.h>

int msgqid;

void clearResources(int);
int countProcesses(const char *filename);
void readProcessData(const char *filename, int num_rows, process_t *processes);
int getSchedulingAlgorithm(int *timeQuantum);
void testExtractedData(int num_rows, process_t *processes);

#define SJF 1
#define PHPF 2
#define RR 3

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);

    // Initialization
    // 1. Read the input files.
    // Get the number of rows in the file
    int num_rows = countProcesses(argv[1]);
    if (num_rows < 0) {
        return 1;
    }

    // Dynamically allocate arrays based on the number of rows
    process_t *processes = malloc(num_rows * sizeof(process_t));
    if (processes == NULL) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }

    // Read and store data into arrays
    readProcessData(argv[1], num_rows, processes);

    // Test the extracted data
    testExtractedData(num_rows, processes);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int timeQuantum = 0;
    int selectedAlgorithm = getSchedulingAlgorithm(&timeQuantum);

    // 3. Initiate and create the scheduler and clock processes.
    // 3.1. Initialize the message queue and termination queue
    msgqid = initMsgq(msgkey);
    if (msgqid == -1) {
        perror("Error in initializing message queue");
        exit(-1);
    }

    // 3.2. Fork the clock and scheduler processes
    pid_t clk_pid = fork();
    if (clk_pid == -1) {
        perror("Error in forking clock process");
        exit(-1);
    } else if (clk_pid == 0) {
        char *args[] = {"./clk.out", NULL};
        int clk_exec = execvp(args[0], args);
        if (clk_exec == -1) {
            perror("Error in executing clock process");
            exit(-1);
        }
    }

    // 3.3. Send the selected algorithm and its parameters to the scheduler
    pid_t scheduler_pid = fork();
    if (scheduler_pid == -1) {
        perror("Error in forking scheduler process");
        exit(-1);
    } else if (scheduler_pid == 0) {
        char algorithm[2];
        sprintf(algorithm, "%d", selectedAlgorithm);
        char quantum[5];
        sprintf(quantum, "%d", timeQuantum);
        char *args[] = {"./scheduler.out", algorithm, quantum, NULL};
        int scheduler_exec = execvp(args[0], args);
        if (scheduler_exec == -1) {
            perror("Error in executing scheduler process");
            exit(-1);
        }
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // (DONE in headers.h)
    // 6. Send the information to the scheduler at the appropriate time.
    int clk = getClk();
    int process_id = 0;
    while (process_id < num_rows) {
        // Wait until the arrival time of the next process
        while (getClk() < processes[process_id].arrival) {
            // Print the current time every second
            clk = getClk();
            while (clk == getClk());
            printf("Generator %d, time %d\n", getpid(), getClk());
        };

        // Send the process to the message queue
        sendMsg(processes[process_id], msgqid);
        printf("Sent process with ID %d to the message queue\n", processes[process_id].id);

        process_id++;
    }

    // Send a process with ID -1 to indicate the end of the processes
    process_t last_process = {
        .id = -1,
        .runtime = INT_MAX,
        .priority = INT_MAX,
    };
    sendMsg(last_process, msgqid);

    // Wait for the scheduler to finish
    waitpid(scheduler_pid, NULL, 0);
    printf("Scheduler has finished\n");

    // 7. Clear clock resources
    clearResources(0);

    // Free allocated memory
    free(processes);

    return 0;
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msgqid, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    exit(0);
}

// Function to count the number of Processes in the file
int countProcesses(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return -1;
    }

    int count = 0;
    char buffer[256];
    // Skip the header line
    fgets(buffer, sizeof(buffer), file);

    // Count the number of rows
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        count++;
    }

    fclose(file);
    return count;
}

// Function to read and store process data from the file
void readProcessData(const char *filename, int num_rows, process_t *processes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return;
    }

    char buffer[256];
    // Skip the header line
    fgets(buffer, sizeof(buffer), file);

    // Read and store values into the process_t array
    int count = 0;
    while (count < num_rows && fscanf(file, "%d %d %d %d", 
            &processes[count].id, 
            &processes[count].arrival, 
            &processes[count].runtime, 
            &processes[count].priority) == 4) {
        processes[count].remainingTime = processes[count].runtime;
        processes[count].running = false;
        // processes[count].mtype = 0;
        count++;
    }

    fclose(file);
}

// Function to ask the user for the algorithm and its parameters
int getSchedulingAlgorithm(int *timeQuantum) {
    int choice;

    // Keep asking for a valid input until the user provides one
    while (1) {
        printf("Select a scheduling algorithm:\n");
        printf("1. Short Job First (SJF)\n");
        printf("2. Preemptive Highest Priority First (PHPF)\n");
        printf("3. Round Robin (RR)\n");
        printf("Enter your choice (1/2/3): ");
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
            printf("Invalid choice. Please enter a number between 1 and 3.\n");
            while (getchar() != '\n');  // Clear invalid input from buffer
            continue; // Loop again to prompt for valid input
        }

        // Handle parameters based on the chosen algorithm
        if (choice == RR) {  // If Round Robin is selected
            while (1) {
                printf("Enter the time quantum for Round Robin: ");
                if (scanf("%d", timeQuantum) != 1 || *timeQuantum <= 0) {
                    printf("Invalid time quantum. Please enter a positive number.\n");
                    while (getchar() != '\n');  // Clear invalid input from buffer
                } else {
                    break;  // Exit the loop once valid input is entered
                }
            }
        }

        break; // Exit the loop if valid input is received (for both choice and time quantum)
    }

    return choice; // Return the selected algorithm
}

// Function to make sure that the data extracted right
void testExtractedData(int num_rows, process_t *processes)
{
    printf("Extracted data:\n");
    printf("ID    Arrival    Runtime    Priority\n");
    for (int i = 0; i < num_rows; i++) {
        printf("%d     %d         %d         %d\n", 
               processes[i].id, 
               processes[i].arrival, 
               processes[i].runtime, 
               processes[i].priority);
    }
}
