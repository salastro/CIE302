#include "headers.h"

void clearResources(int);
int countProcesses(const char *filename);
void readProcessData(const char *filename, int num_rows, int *id, int *arrival, int *runtime, int *priority);
int getSchedulingAlgorithm(int *timeQuantum);
void testExtractedData(int num_rows, int *id, int *arrival, int *runtime, int *priority);

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
    int *id = malloc(num_rows * sizeof(int));
    int *arrival = malloc(num_rows * sizeof(int));
    int *runtime = malloc(num_rows * sizeof(int));
    int *priority = malloc(num_rows * sizeof(int));

    if (id == NULL || arrival == NULL || runtime == NULL || priority == NULL) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }

    // Read and store data into arrays
    readProcessData(argv[1], num_rows, id, arrival, runtime, priority);

    // Test the extracted data
    testExtractedData(num_rows,id,arrival,runtime,priority);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
        int timeQuantum = 0;
        int selectedAlgorithm = getSchedulingAlgorithm(&timeQuantum);

    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.

    // 7. Clear clock resources
    destroyClk(true);

    // Free allocated memory
    free(id);
    free(arrival);
    free(runtime);
    free(priority);

    return 0;
}

void clearResources(int signum)
{
       // TODO Clears all resources in case of interruption
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
void readProcessData(const char *filename, int num_rows, int *id, int *arrival, int *runtime, int *priority) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return;
    }

    char buffer[256];
    // Skip the header line
    fgets(buffer, sizeof(buffer), file);

    // Read and store values into arrays
    int count = 0;
    while (fscanf(file, "%d %d %d %d", &id[count], &arrival[count], &runtime[count], &priority[count]) == 4) {
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
void testExtractedData(int num_rows, int *id, int *arrival, int *runtime, int *priority)
{
    printf("Extracted data:\n");
    printf("ID    Arrival    Runtime    Priority\n");
    for (int i = 0; i < num_rows; i++) {
        printf("%d     %d         %d         %d\n", id[i], arrival[i], runtime[i], priority[i]);
    }
}