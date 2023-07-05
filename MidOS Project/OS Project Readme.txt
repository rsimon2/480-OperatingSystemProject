Team OS's MidOS project
Rick Simon
Jessica Landmeier

Running Program:
	- A Makefile is included with the project. When you type "make" in the terminal, the 
	  project will compile it's 3 parts, system.h, system.cpp, main.cpp.
	  
	- The run command in the makefile is included for testing purposes. Filenames for
	  sample programs may need to be changed in order to work properly if run using the 
	  "make run" command.
	  
Program features:
	- This OS incorporates all of the base features requested in the project description.
	
	- Process scheduling based on priority.
		- A priority scheduler has been incorporated using a ready queue and PCB Table.
		
		- The program checks a process in the PCB table using a for loop based on the 
		  number of processes to check for status code READY (set to 1).
			- If ready, it will push the process to the end (back) of the ready queue
			  for execution.
		
		- If the queue is not empty, a process is executed for the amount of time
		  quantum (set to 10), which decrements 1 for every clock cycle.
		
		- Scheduler is called every time the OS checks for an interrupt after a newly 
		  ready process is pushed into the queue.
		  
		- Priority is set by the SETPRIORITY instruction code based on the value in a 
		  register given when the instruction in executed.
			- SETPRIORITY is instruction code 26.
	
	- Mandatory sleep function.
		- Sleep function exists in the instruction codes as case 25 in the switch-case.
		
		- Function sets the sleep duration based on the number of clock cycles in the
		  register parameter given. If sleep is 0, the process will sleep infinitely.
		  
	- Process scheduling is based on time quantum expiration
		- In the schedule_process() function, if the ready queue is not empty, a timer is 
		  set for time quantum (10). The process gets that amount of time to execute. If
		  it doesn't finish in that amount of time, it is sent back into the ready queue
		  and another process can execute.
	
	- Output services.
		- Each instruction code has an output to tell what instruction was called.
		
		- The PRINTR and PRINTM are instructions that can be called using instruction
		  codes 11 and 12 respectively.
			- PRINTR prints the contents of the register given with the instruction.
			
			- PRINTM prints the contents of memory whose address is in a given register.
	
	- Mutual Exclusion Facilities.
		- Mutex locks are acquired and released using the ACQUIRELOCK and RELEASELOCK
		  instructions (instruction codes 23 and 24 respectively).
		  
		- Mutex locks are used in the SIGNALEVENT and WAITEVENT instructions (instruction
		  codes 30 and 31 respectively).
		  
		- A critical section semaphore is used to make sure multiple threads can't access
		  the critical section at the same time.
		  
		- WAITEVENT locks events that have a state of zero until another event signals.
	
	- Idle process.
		- The check_interrupt() function is responsible for idle processing.
		
		- check_interrupt() checks after each run if a process has any events or calls
		  interrupts.
			- Waits for events or interrupts to finish, then moves process back to READY.
	
	- Exit.
		- schedule_process() handles process exiting.
		
		- Process is moved to TERMINATED state so OS knows to exit process from memory.
			- Happens in execute_program(). When process is TERMINATED, the process will 
			  break.