# Operating systems: Three Easy Pieces

# Introduction 

The three easy pieces are virtualization, concurrency and persistence. 

The OS is a body of software that allows you to run multiple programs, allows programs to share memory, and allows programs to interact with other devices. 

The OS must run non-stop; when it fails, all applications running on the system fail as well. The OS is itself a software program. The OS controls the hardware in your system. 

## Virtualization

The OS takes a Processor, Memory, or a Disk for example and transforms it into a more general "virtual" easy-to-use form of itself. The OS is therefore a "virtual machine." The OS also virtualizes the CPU, which means that, for running programs, there seem to be an endless number of CPUs, whereas in reality, the OS manages how CPU resources are allocated between running processes. 

The OS ensures that each running process has its own private virtual address space, which the OS somehow maps onto the physical memory of the machine. A memory reference within one running program does not affect the address space of other processes, or the OS itself. Physical memory is a shared resource mangaed by the OS. 

## Concurrency

When there are many concurrently executing threads within the same memory space, how can we build a correctly working program?

## Persistence

A hard drive or solid state drive are common repositories for long-lived data. The software in the operating system that usually manages the disk is called the file system. Files are shared across processes. 

## What's missing in this book

There is a lot of netowrking code in the OS that is not covered in this book. Instead you should take a class on networking. Same with graphics devices, which are not covered. 

# Part one: Virtualization

A Process is a running program. The Program itself is lifeless: it just sits there on the disk, a bunch of instructions. It is the OS that takes those bytes and gets them running. The OS virtualizes the CPU to provide teh illusion that there are nearly an endless supply of CPUs. The Process never needs to wonder, is a CPU available?

A context switch is when the OS stops running one program and starts running another on a given CPU. Time sharing is a technique used by the OS to share a resource across Processes. Space sharing is when a resource is divided up, such as a disk. 

A Process can be described by its Machine State: the memory addresses its writing to, its Program Counter register (PC - aka the Instruction Pointer), etc. The contents of other CPU registers. Also the stack and frame pointers, which are used to manage the stack for function params, local variables and return addresses (these are CPU registers). 

How are programs tranformed into processes? How does an OS get a program running? First it loads its code and static data into memory; into the address space of the process. Programs initially reside on Disk in some kind of executable format. 

In early OS, loading was done "eagerly", meaning all at once before running the program. Modern OSes perform the process "lazily", meaning by loading pieces of code or data into memory once as they are needed during program execution. 

The OS must also allocate memory for the program's run-time stack (stack for local vars, function params and return addresses...). The OS initializes the stack with arguments (on the stack). The OS also allocates memory for the program's heap. The OS then uses a specialized "mechanism" (low-level hardware instruction) to "jump" to the given program's main() routine on the process' stack. Thus transferring CPU control over to the process. 

Process states:
Running: a processor is executing the process' stack's instructions one by one
Ready: a Process is ready (code and data loaded, stack set up...) but OS has chosen not to "jump" to its main() 
Blocked: a Process has performed an operation that makes it not ready to run until some other event takes place. A Process might be blocked when performing I/O ops like reading from a disk or waiting for a packet from a network. 

The context switch: when a process is blocked, the OS runs a different process that is "ready." To do this, it first saves the state of the blocked process' registers, so that the physical registers can be restored with this state later when the process is restarted. 

When a process completes successfully, the OS can clean up any relevant data structures that referred to the now completed process. 

## Process API

fork, exec, wait and kill are key apis for a OS Process. Each Process has a name which is a number called the Process ID (PID). Process control is available in the form of signals. 

`fork()` in c will create a child process. The CPU scheduler determines which process runs at a given moment in time. Use `wait()` to ensure that the child process has finished. `wait()` maks the program output deterministic because it forces the CPU scheduler to run one program before another at least.  

### The Shell

For example, bash or zsh. The shell is just a user program. It shows you a prompt and then waits for you to type something into it. You then type a command (i.e., the name of an executable program, plus any arguments) into it; in most cases, the shell then figures out where in the file system the executable resides, calls fork() to create a new child process to run the command, calls some variant of exec() to run the command, and then waits for the command to complete by calling wait(). When the child completes, the shell returns from wait() and prints out a prompt again, ready for your next command. The separation of `fork()` and `exec()` allows the shell to do a whole bunch of useful things easily. 

With the `pipe()` system call, the output of one process is connected to an in-kernel pipe (i.e queue), and the input of another process is connected to that same pipe. Thus the output of one process is used as input to the next. 

```
# find all instances of the string "OS" in this file and pipe that to "word count"
unix> grep -o OS ./README.md | wc -l
26
```

Shells provide a lot of convenience for working with OS processes. For example, you can send a SIGINT (interrupt) to kill a process with the key combination ctrl-c. The shell allows this for convenience. The "signals" subsystem provides a means to external events to processes. Incuding ways to receive and process those signals withinin individual processes. A process can use the `signal()` system call to "catch" signals.

See `man ps` and `man top` as some useful tools. `top` displays currently running system processes.

# Mechanism: Limited Direct Execution

High-level creation of a process without concern for content switching between active process (sharing the CPU):

OS Actions

1. Create entry for soon-to-be process in the process list.
2. Allocate memory for the program code (instructions), heap, and static data.  
3. Load program into memory.
4. Set up stack with argv and argc pushed to the top
5. Clear memory registers
6. Execute call main()

Program Actions

1. Run main()
2. Execute Return from main()

OS Actions

7. Free memory of Process.
8. Remove Process from the process list.

This includes direct execution of the process. The program instructions are running natively directly on the CPU. How does the OS then restrict certain operations? The advantage of direct execution is that it is very fast. But then the OS loses control over the hardware resources. So this is not allowed.

The hardware typically provides two modes of execution. In user mode, the process is restricted from accessing certain resources, like the disk or memory. In kernel mode, the process is unrestricted. The CPU explicitly provides these two modes via the trap table. User code in user mode uses a system call to trap into the kernel to request operating system services. 

The trap instruction saves the register state, changes the hardware status to kernel mode and jumps into the OS to a pre-specified destination provided by the trap table.

To execute a system call, a prcocess must execute a special trap instruction that simultaneously jumps into kernel code and raises the privelage level to kernel mode. To return to user code, there is a return-from-trap instruction. The OS provides a trap table to the hardware, so the hardware knows where to jump given a certain trap instruction. A trap event is assigned a trap handler for the hardware to jump to. The hardware trapping mechanism ensures all calls are routed through the OS itself. In other words, it's clear that x86 systems are "assembled" with the OS in mind.

An example is the `exit()` system call when a process is finished. This "traps into the OS" where the OS can clean up the process' memory, remove it from the process list, etc. 

During a context switch, the OS will execute some low-level assembly code to save the general purpose registers, PC, and the kernel stack pointer of the currently running process, and then restore those same resources for the soon-to-be-running process. 

A timer-interrupt gives the OS the ability to run again on a CPU even if processes act in a non-cooperative fashion.

See https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf

See more on schd_setaffinity: https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html
See more on the `taskset` command: https://man7.org/linux/man-pages/man1/taskset.1.html

An "affinity" in Linux refers to a given process' "attachment"/"affinity" for one or more CPUs. As a user, you can set the "affinity" that the Linux Scheduler assigns to your process. Although otherwise, the scheduler does this itself based on the demands on the computer's resources. 

`lscpu` will show you data on your system's CPUs: https://www.cyberciti.biz/faq/check-how-many-cpus-are-there-in-linux-system/

# Chapter 7: Scheduling

Scheduling jobs to run on the computer's CPUS.

Turnaround time is defined as the time at which the job (process) completes minus the time at which the job started.

An example of a general scheduling principle is "Shortest Job First" (SJF), meaning the scheduler tries to schedule the job with the fastest potential turnaround time. Once that one is complete, it aims to run the next shortest and so forth. 

Shortest-Time-To-Completion builds on SFJ by allowing processes to be preempted. Thus, if Job A arrives first, but then Job b arrives second and will take much less time to complete, job A can be preempted and stopped so that Job B can run. This is also known as Preemptive Shortest Job First (PSJF). In PSJF, any time a new job arrives in the system, the scheduler determines which job will take the least time and schedules that one. 

The round-robin scheduling algorithm runs a job for a time slice and then switches to the next job in the queue. It usually switches as a multiple of the timer interrupt period. For example, if the timer interrupt is every 10 ms, then maybe the "switch" is every 30 ms. Response time (Time of first run - Time of job arrival to queue) is the key metric for Round Robin.

Running process build up a lot of state. When they run, they build up state in CPU caches, TLBs, branch predictors and other on-chip hardware. 

RR can be bad for the turnaround time metric however. RR is "fair" in that it evenly distributes the CPU among active processes on a small time scale. 

# Scheduling: The Multi-Level Feedback Queue

How can the scheduler learn as the system runs the characteristics of the currently running processes? 

MLFQ has a number of distinct queues, each with a different priority level. All jobs in a given queue therefore are assigned the same priority. If there are multiple jobs in the same queue, the scheduler applies Round Robin scheduling to them. 

MLFQ varies the priority of a given job based on its observed behavior. If, for ex, the job is constantly blocking as it waits for IO from the keyboard, this means that it is an "important"/ used process by the user, so it should have a high priority. 

The algorithm might work like this: When a job first arrives, it is placed in the highest priority queue. If it frequently relinquishes the CPU in favor of IO work, it remains a high priority job as it is clearly being used. If it just hogs the CPU (uses the CPU during an entire RR time slice...), then its priority is downgraded. 

Note, another word for the "time slice" is "quantum."  

Technically a user could ensure their process systematically issues an I/O call towards the end of a time slice to keep its priority up. This would subvert the scheduler. The scheduler can get around this by adding a rule: After some time period, move all jobs in the system to the topmost queue. 

# Lottery Scheduling 

Tickets represent a given processes' share of computer resources. 

The goal of the Linux Completely Fair Scheduler (CFS) is to fairly divide the CPU resources among all competing processes.

The concept of niceness ranges from -20 to +19 for a process. A positive nice value marks a process as lower priority for the scheduler. 

See Summation symbol: https://mathinsight.org/definition/summation_symbol

p. 14/14 Homework (simulation)

