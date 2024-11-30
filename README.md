# Process Scheduling System

This repository contains a **Process Scheduling System** implemented in C. The project provides a simulation of process scheduling using various algorithms like **Shortest Job First (SJF)**, **Preemptive Highest Priority First (PHPF)**, and **Round Robin (RR)**. It includes the process generator, scheduler, and individual process modules that communicate using shared memory and message queues.

---

## Features
1. **Dynamic Scheduling Algorithms**:
   - Shortest Job First (SJF)
   - Preemptive Highest Priority First (PHPF)
   - Round Robin (RR) with user-defined time quantum.
2. **Process Communication**:
   - Utilizes IPC mechanisms like shared memory and message queues.
3. **Resource Management**:
   - Graceful resource cleanup during interruptions or errors.
4. **Performance Metrics**:
   - Logs include CPU utilization, average waiting time, average WTA, and WTA standard deviation.

---

## Project Structure

### **1. `process_generator.c`**
- Reads input processes from a file.
- Dynamically allocates memory for process data.
- Selects the scheduling algorithm based on user input.
- Spawns `scheduler` and `clock` as child processes.
- Sends processes to the `scheduler` at their respective arrival times.

### **2. `scheduler.c`**
- Receives processes from the generator via message queues.
- Manages processes based on the selected scheduling algorithm:
  - **SJF**: Prioritizes the shortest runtime.
  - **PHPF**: Prioritizes the highest priority value.
  - **RR**: Distributes time slices to processes using a round-robin queue.
- Tracks and logs process execution states and metrics.

### **3. `process.c`**
- Represents an individual process.
- Simulates execution behavior by consuming runtime.
- Sends termination signals back to the scheduler upon completion.

### **4. `headers.h`**
- Defines shared structures, utility functions, and macros used across modules.
- Includes inter-process communication mechanisms.

### **5. `DS/Msgq.h`**
- Implements message queue utilities for sending and receiving process data.

---

## Setup and Usage

### Prerequisites
- GCC compiler
- Linux operating system (for IPC and signal handling)

### Compilation
```bash
make
```

### Running the Program
1. **Generate input data** (see below for format).
2. **Start the simulation**:
   ```bash
   ./process_generator processes.txt
   ```

OR

- **Run the program using the provided Makefile**:
   ```bash
   make run
   ```

3. **Follow the prompts** to select a scheduling algorithm and configure parameters (if applicable).

---

## Input File Format
The input file should have the following format:

```
ID    Arrival    Runtime    Priority
1     0          5          3
2     1          3          1
3     2          8          2
```

- **ID**: Process ID.
- **Arrival**: Time when the process becomes ready.
- **Runtime**: Total execution time required.
- **Priority**: Process priority (lower value = higher priority for PHPF).

---

## Logs and Output
1. **`scheduler.log`**:
   - Tracks process states (`started`, `stopped`, `resumed`, and `finished`) and metrics like turnaround time (TA) and weighted turnaround time (WTA).
   - Example:
     ```
     At time 3 process 1 started arr 0 total 5 remain 5 wait 3
     At time 6 process 2 finished arr 1 total 3 remain 0 wait 2 TA 5 WTA 1.67
     ```

2. **`scheduler.perf`**:
   - Summarizes performance metrics:
     ```
     CPU utilization = 85.00%
     Avg WTA = 1.67
     Avg Waiting = 2.33
     Std WTA = 0.47
     ```

---

## Key Functions

### **Process Generator**
- `countProcesses(const char *filename)`: Counts the number of processes in the input file.
- `readProcessData(const char *filename, int num_rows, process_t *processes)`: Reads and parses process data into a structure.
- `clearResources(int signum)`: Cleans up IPC resources.

### **Scheduler**
- `runSJF()`, `runPHPF()`, `runRR(int quantum)`: Implements scheduling algorithms.
- `startProcess(process_t *proc)`, `stopProcess(process_t *proc)`, `continueProcess(process_t *proc)`: Manages process state transitions.

### **Process**
- `simProc()`: Simulates process execution and reports progress.

---

## Future Improvements
- Enhance error handling and logging.
- Support additional scheduling algorithms.
- Provide visualization for process scheduling and execution.

---

## License
This project is open-source and distributed under no license. Feel free to modify and use the code as needed.
