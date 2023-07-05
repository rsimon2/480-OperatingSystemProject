#ifndef SYSTEM_H
#define SYSTEM_H

//This includes system libraries and defines constants, structs, and typedefs

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

#define SET_FLAG        1
#define CLEAR_FLAG      0
#define SF              11          // Sign flag
#define ZF              12          // Zero flag
#define MEMSIZE         1024 * 32
#define PAGESIZE        4 * 1024 
#define FRAMESIZE       4 * 1024 
#define PAGE_TABLE_SIZE 256
#define TOTALPROC       16
#define TOTALEVENTS     10
#define UNINITIALIZED   0
#define READY           1
#define RUNNING         2
#define BLOCKED         3
#define TERMINATED      4
#define SLEEPING        5
#define TOTALFRAMES     MEMSIZE / PAGESIZE
#define TOTALPAGES      MEMSIZE / PAGESIZE

// this is for the mutex object, this is used to control the access to shared resources so it 
//      will add a wait queue if more then one thread is trying to access the Mutex at the same time

typedef struct Mutex {
    int id;
    queue<int> waitQueue;
    int owner;
}Mutex;

typedef struct Instruction // Applies parameters to instructions
{
    int opcode;
    int paramA;
    int paramB;
    int addr;
} Instruction;

//this is for a node in a page table

typedef struct NODE {
    bool valid;
    int frame;
    bool dirty;
    bool flag;
}NODE;

typedef struct PCB { // Stores process information
    int pid;
    int priority;
    int state;
    int pc;
    int registers[8];
    int pageTable[TOTALFRAMES];
    int time_of_death;
    int numPageFaults;
    int base_address;
}PCB;

typedef struct Event { // Relates to processes that can start different events
    int id;
    bool signaled;
    queue<int> waitQueue;
    int time;
    PCB* process;
    Event* next;
}Event;

// Global variables. Inlined for use in multiple files
inline char diskTable[MEMSIZE]; // Array based on size of memory
inline int current = -1; // Current process running
inline int cycles = 0; // Clock cycles for instruction execution
inline int priority = 0; // Priority of a process
inline stack<int> customStack;
inline Event* eventTable[TOTALEVENTS]; // Array based on number of processes
inline PCB pcbTable[TOTALPROC]; // Array based on number of processes
inline NODE pageTable[TOTALPROC][PAGE_TABLE_SIZE]; // 2D array based on size of page table
inline Instruction *instructions = new Instruction[100];
inline int pointer = 0; // Number of processes to run
inline sem_t critical; // For things that must be executed one at a time
inline Mutex customLock[10]; // Lock to be activated and deactivated
inline Event customEvent[10]; // Events to be executed during runtime
inline int quantum = 10; // Time quantum a process is limited to running before it is swapped
inline int contextSwitch = 0; // Number of process saved to resume later if needed
inline int frameTable[TOTALFRAMES];
inline int freeFrames = TOTALFRAMES;
inline queue<int> readyQueue[32];

void execute_program(int pid);
int findInstructionIndex(int addr);
void executeInstruction(int** registers, int& ip);
void* runFile(void* param);
int translation(int pid, int address);
void print_statistics(int pid);
void schedule_process();
void check_interrupt();
void check_events();

#endif