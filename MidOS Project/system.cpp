#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include "system.h"
using namespace std;

// Executes program using file name.
void execute_program(int pid) {
    current = pid;
    pcbTable[pid].state = RUNNING;
    contextSwitch++;
    for (int i = 0; i < quantum; i++) {
        int virtualAddress = pcbTable[pid].pc;
        int page = virtualAddress / PAGESIZE;
        if (pageTable[pid][page].valid) {
            pcbTable[pid].pc++;
        }
        else {
            pcbTable[pid].pc++;
            pcbTable[pid].numPageFaults++;
        }
        check_events();
        if (pcbTable[pid].state == BLOCKED || pcbTable[pid].state == TERMINATED) {
            break;
        }
    }
    if (pcbTable[pid].state == RUNNING) {
        pcbTable[pid].state = READY;
        readyQueue[pcbTable[pid].priority].push(pid);
    }
}

// Gets instruction from from file based on an address. Takes address as parameter
//		and jumps to the instruction at that address.
int findInstructionIndex(int addr) 
{
	for (int i = 0; i < pointer; i++) 
	{
		if (instructions[i].addr == addr) 
		{
			return i - 1;
		}
	}
	return 0;
}

// Executes a single instruction from a file. Instructions are detected by opcode.
void executeInstruction(int** registers, int& ip) {
	sem_wait(&critical);
	int instructionCode = instructions[ip].opcode;
	int paramA = instructions[ip].paramA;
	int paramB = instructions[ip].paramB;
	int address = instructions[ip].addr;
	switch (instructionCode)
	{
		// INCR: Increments value of register x by 1
		case 1:
		{
			registers[paramA][0]++;
			cout << "INCR function called" << endl;
		}
		break;
		// ADDI: Increase value of register x by literal y
		case 2:
		{
			registers[paramA][0] = registers[paramA][0] + paramB;
			cout << "ADDI function called" << endl;
		}
		break;
		// ADDR: Store sum of register x and register y in register x
		case 3:
		{
			registers[paramA][0] = registers[paramA][0] + registers[paramB][0];
			cout << "ADDR function called" << endl;
		}
		break;
		// PUSHR: Push contents of register x onto stack. Decrement SP by 4
		case 4:
		{
			customStack.push(registers[paramA][0]);
			cout << "PUSHR function called" << endl;
		}
		break;
		// PUSHI: Push literal x onto stack. Decrement SP by 4
		case 5:
		{
			customStack.push(paramA);
			cout << "PUSHI function called" << endl;
		}
		break;
		// MOVI: Store literal y in register x
		case 6:
		{
			registers[paramA][0] = paramB;
			cout << "MOVI function called" << endl;
		}
		break;
		// MOVR: Store value of register y in register x
		case 7:
		{
			registers[paramA][0] = registers[paramB][0];
			cout << "MOVR function called" << endl;
		}
		break;
		// MOVMR: rx,ry - rx <- [ry] 
		case 8:
		{
			registers[paramA][0] = registers[paramB][0];
			cout << "MOVMR function called" << endl;
		}
		break;
		// MOVRM: rx,ry - [rx] <- ry
		case 9:
		{
			registers[paramA][0] = registers[paramB][0];
			cout << "MOVRM function called" << endl;
		}
		break;
		// MOVMM: rx,ry [rx] <- [ry]
		case 10:
		{
			registers[paramA][0] = registers[paramB][0];
			cout << "MOVMM function called" << endl;
		}
		break;
		// PRINTR: Display contents of register x
		case 11:
		{
			cout << registers[paramA][0] << endl;
			cout << "PRINTR function called" << endl;
		}
		break;
		// PRINTMM: Display contents of memory whose address is in register x
		case 12:
		{
			cout << &registers[paramA][0] << endl;
			cout << "PRINTM function called" << endl;
		}
		break;
		// JMP: Control transfers to instruction whose address is register x bytes relative 
		//		to current instruction. Register x may be negative
		case 13:
		{
			ip = findInstructionIndex(address + 5 + registers[paramA][0]);
			cout << "JMP function called" << endl;
		}
		break;
		// CMPI: Subtract literal y from register x. If register x < literal y, set sign flag. 
		//		 If register x > literal y, clear sign flag. Else set zero flag
		case 14:
		{
			if (registers[paramA][0] < paramB) 
			{
				registers[SF][0] = SET_FLAG;
			}
			else if (registers[paramA][0] > paramB) 
			{
				registers[SF][0] = CLEAR_FLAG;
			}
			else 
			{
				registers[ZF][0] = SET_FLAG;
			}
			cout << "CMPI function called" << endl;
		}
		break;
		// CMPR: Subtract register y from register x. If register x < register y, set sign flag. 
		//		 If register x > register y, clear sign flag. Else set zero flag
		case 15:
		{
			if (registers[paramA][0] < registers[paramB][0]) 
			{
				registers[SF][0] = SET_FLAG;
			}
			else if (registers[paramA][0] > registers[paramB][0]) 
			{
				registers[SF][0] = CLEAR_FLAG;
			}
			else 
			{
				registers[ZF][0] = SET_FLAG;
			}
			cout << "CMPR function called" << endl;
		}
		break;
		// JLT: If sign flag set, jump to instruction whose offset is register x bytes from current instruction
		case 16:
		{
			if (registers[SF][0] == SET_FLAG) 
			{
				ip = findInstructionIndex(address + 5 + registers[paramA][0]);
				cout << "JLT function called" << endl;
			}
		}
		break;
		// JGT: If sign flag clear, jump to instruction whose offset is register x bytes from current instruction
		case 17:
		{
			if (registers[SF][0] == CLEAR_FLAG) 
			{
				ip = findInstructionIndex(address + 5 + registers[paramA][0]);
				cout << "JGT function called" << endl;
			}
		}
		break;
		// JE: If 0 flag clear, jump to instruction whose offset is register x bytes from current instruction
		case 18:
		{
			if (registers[ZF][0] == CLEAR_FLAG) 
			{
				ip = findInstructionIndex(address + 5 + registers[paramA][0]);
				cout << "JE function called" << endl;
			}
		}
		break;
		// CALL: Call procedure at offset register x bytes from current instruction. Address of next instruction 
		//		 to execute after return is pushed on stack
		case 19:
		{
			ip = findInstructionIndex(address + 5 + registers[paramA][0]);
			customStack.push(address + 5);
			cout << "CALL function called" << endl;
		}
		break;
		// CALLM: Call procedure at offset [register x] bytes from current instruction. Address of next instruction 
		//		  to execute after return is pushed on stack
		case 20:
		{
			ip = findInstructionIndex(address + 5 + registers[paramA][0]);
			customStack.push(address + 5);
			cout << "CALLM function called" << endl;
		}
		break;
		// RET: Pop return address from stack and transfer control to this instruction
		case 21:
		{
			cout << "RET function called" << endl;
			if (!customStack.empty()) 
			{
				ip = findInstructionIndex(customStack.top());
				customStack.pop();
			}
		}
		break;
		// ALLOC: Allocate memory of size = register x bytes and return address of new memory in register y. 
		//		  If it fails, register y = 0
		case 22:
		{
			registers[paramB] = new int[paramA];
			cout << "ALLOC function called" << endl;
		}
		break;
		// ACQUIRELOCK: Acquire OS lock whose number is provided in register x. If lock invalid, 
		//              instruction is no-op
		case 23:
		{
			static vector<mutex> locks;
			if (paramA >= 0 && paramA < locks.size()) 
			{
				locks[paramA].lock();
			}
			cout << "ACQUIRELOCK function called" << endl;
		}
		break;
		// RELEASELOCK: release OS lock whose # is provided in register x. If lock not held by current process,
		//				instruction is no-op
		case 24:
		{
			static vector<mutex> locks;
			if (paramA >= 0 && paramA < locks.size()) 
			{
				locks[paramA].unlock();
			}
			cout << "RELEASELOCK function called" << endl;
		}
		break;
		// SLEEP: Sleep # of clock cycles as indicated in register x. Another process or idle process must 
		//        be scheduled at this point. If sleep is 0, process sleeps infinitely
		case 25:
		{
			cout << "Sleeping for " << registers[paramA][0] << " seconds..." << endl;
			int sleep_duration = registers[paramA][0];
			sleep(sleep_duration);
		}
		break;
		// SETPRIORITY: Set priority of current process to value in register x
		case 26:
		{
			priority = registers[paramA][0];
			cout << "SETPRIORITY function called" << endl;
		}
		break;
		// EXIT: Opcode executed by process to exit and be unloaded. Another process or idle process must 
		//       now be scheduled
		case 27:
		{
			cout << "EXIT function called" << endl;
		}
		break;
		// FREEMEMORY: Free memory allocated whose address is in register x
		case 28:
		{
			int* mem = &paramA;
			cout << "FREEMEMORY function called" << endl;
		}
		break;
		// MAPSHAREDMEM: Map shared memory region identified by register x and return start address in register y
		case 29:
		{
			registers[paramB] = registers[paramA];
			cout << "MAPSHAREDMEM function called" << endl;
		}
		break;
		// SIGNALEVENT: Signal event indicated by value in register x
		case 30:
		{
			static mutex mtx;
			static condition_variable cv;
			static int eventState[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }; // Initialize events to 1
			// An event is set to 1 to signal it's ready and notify_all unblocks all threads waiting.
			if (paramA >= 0 && paramA < 10) 
			{
				unique_lock<mutex> lock(mtx);
				eventState[paramA] = 1;
				cv.notify_all();
			}
			cout << "SIGNALEVENT function called" << endl;
		}
		break;
		// WAITEVENT: Wait for event in register x to be triggered. This results in context switches happening
		case 31:
		{
			static mutex mtx;
			static condition_variable cv;
			static int eventState[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }; // Initialize events to 1
			if (paramA >= 0 && paramA < 10) 
			{
				unique_lock<mutex> mlock(mtx);
				// While event's state = 0, process is locked until another process signals
				while (eventState[paramA] == 0) 
				{
					cv.wait(mlock);
				}
				// Event state set to 1 after notify_all unblocks threads.
				eventState[paramA] = 1;
			}
			cout << "WAITEVENT function called" << endl;
		}
		break;
		// INPUT: Read next 32 bit value into register x
		case 32:
		{
			cin >> instructions[ip].paramA;
			cout << "INPUT function called" << endl;
		}
		break;
		// MEMORYCLEAR: Set bytes starting at address register x of length register y bytes to 0
		case 33:
		{
			cout << "MEMORYCLEAR function called" << endl;
		}
		break;
		// TERMINATEPROCESS: Terminate process whose ID is in register x
		case 34:
		{
			cout << "TERMINATEPROCESS function called" << endl;
		}
		break;
		// POPR: Pop contents at top of stack into register x which is the operand. SP decremented by 4
		case 35:
		{
			registers[paramA][0] = customStack.top();
			customStack.pop();
			cout << "POPR function called" << endl;
		}
		break;
		// POPM: Pop contents at top of stack into memory operand whose address is in register which is the
		//       operand. SP dcremented by 4
		case 36:
		{
			registers[paramA][0] = customStack.top();
			customStack.pop();
			cout << "POPM function called" << endl;
		}
		break;
		// Unknown instruction. Shouldn't reach this.
		default:
		{
			cout << "UNKNOWN OPCODE " << instructionCode << endl;
		}
		break;
	}
	ip++;
	sem_post(&critical);
}

// Translates logical memory to physical memory.
int translation(int pid, int address) {
    int page = address / PAGESIZE;
    int offset = address % PAGESIZE;
    int frame = pageTable[pid][page].frame;
    int physical_address = frame * PAGESIZE + offset;
    return physical_address;
}

// Used at the end of a process to display process ID, program counter, and page table
void print_statistics(int pid) {
    if (pid < 0 || pid >= TOTALPROC) {
        cout << "Invalid process ID: " << pid << endl;
        return;
    }
    PCB pcb = pcbTable[pid];
    cout << "Process ID: " << pid << endl;
    cout << "Program Counter: " << pcb.pc << endl;
    cout << "Page Table:" << endl;
    for (int i = 0; i < TOTALFRAMES; i++) {
        int page = i / TOTALPAGES;
        if (pcb.pageTable[page] == i) {
            cout << "Page " << page << ", Frame " << i << ": Present" << endl;
        }
        else {
            cout << "Page " << page << ", Frame " << i << ": Not present" << endl;
        }
    }
}

// Priority scheduler. Priority for a process is set with the SETPRIORITY instruction.
void schedule_process() 
{
	queue<int> readyQueue;
	for (int i = 0; i < TOTALPROC; i++) // Checks PCB Table based on number of processes
	{
		if (pcbTable[i].state == READY) // Push process to back of queue if ready to execute
		{
			readyQueue.push(i);
		}
	}
	while (!readyQueue.empty()) 
	{
		int pid = readyQueue.front();
		readyQueue.pop();
		current = pid;
		int remainingTime = quantum; // Remaining time set to 10
		// When a process is being executed, counts down time it has left before being sent back
		//		into the ready queue
		while (remainingTime > 0 && pcbTable[pid].state != BLOCKED) 
		{
			cycles++;
			remainingTime--;
		}
		contextSwitch++;
	}
	for (int i = 0; i < TOTALPROC; i++) // Terminates processes when finished
	{
		pcbTable[i].state = TERMINATED;
	}
}

// Idle processing. Checks for process interruptions after each run
void check_interrupt() 
{
	// A numbered process will be marked as ready and pushed into ready queue
	if (current != -1) 
	{
		pcbTable[current].state = READY;
		readyQueue[pcbTable[current].priority].push(current);
	}
	check_events(); // Checks for events like that may interrupt a process
	schedule_process(); // Schedules a process that's ready
}

// Checks for events that are ready to execute
void check_events() 
{
	for (int i = 0; i < TOTALEVENTS; i++) 
	{
		Event *e = eventTable[i];
		while (e != nullptr) 
		{
			if (cycles >= e->time) 
			{
				e->process->state = READY;
				readyQueue[i].push(e->process->pid);
				eventTable[i] = e->next;
				delete e;
				e = eventTable[i];
			}
			else 
			{
				break;
			}
		}
	}
}

// Gets a file name and parses file, dividing it into separate instructions to execute one by one.
void* runFile(void* param) {
	char* filename = (char*)param;
	int** registers = new int* [16]; 
//this sets the current address and instruction pointer
	int ip = 0;                      
	instructions[0].addr = 0;
	int currentAddress = 0;
//for each register this allocates the memory
	for (int i = 0; i < 16; i++) {
		registers[i] = new int();
	}
//opens file and makes sure it opens successfully or not and throws error messge if not
	ifstream inputFile(filename);
	if (!inputFile.is_open()) {
		cerr << "Failed to open file" << endl;
		pthread_exit(NULL);
	}
//parse instructions by going throught file line by line and stores the instruction
	string input;
	while (!inputFile.eof()) {
		int opcode = 0, paramA = 0, paramB = 0;
		getline(inputFile, input);
		vector<string> command;
		stringstream ss(input);
		string word;
		int counter = 0;
		while (ss >> word && word[0] != ';') {
			size_t pos = word.find(',');
			if (pos != string::npos) {
				word.erase(pos, 1);
			}
			pos = word.find('$');
			if (pos != string::npos) {
				word.erase(pos, 1);
			}
			command.push_back(word);
			if (counter == 0) {
				opcode = atoi(word.c_str());
				currentAddress++;
			}
			else if (counter == 1) {
				pos = word.find('r');
				if (pos == string::npos) {
					paramA = atoi(word.c_str());
				}
				else {
					word.erase(pos, 1);
					paramA = atoi(word.c_str());
				}
				currentAddress += 4;
			}
			else if (counter == 2) {
				pos = word.find('r');
				if (pos == string::npos) {
					paramB = atoi(word.c_str());
				}
				else {
					word.erase(pos, 1);
					paramB = atoi(word.c_str());
				}
				currentAddress += 4;
			}
			counter++;
		}
		instructions[pointer].opcode = opcode;
		instructions[pointer].paramA = paramA;
		instructions[pointer].paramB = paramB;
		pointer++;
		instructions[pointer].addr = currentAddress;
	}
	int i = 0;
	while (i < pointer) { // Instructions go from 0 to pointer
		executeInstruction(registers, ip);
		i = ip;
	}
	pthread_exit(NULL);
}
