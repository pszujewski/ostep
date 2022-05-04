# Operating systems: Three Easy Pieces

# Introduction

The three easy pieces are virtualization, concurrency and persistence.

The OS is a body of software that allows you to run multiple programs, allows programs to share memory, and allows programs to interact with other devices.

The OS must run non-stop; when it fails, all applications running on the system fail as well. The OS is itself a software program. The OS controls the hardware in your system.

General note

See Summation symbol: https://mathinsight.org/definition/summation_symbol

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

A Process is a running program. The Program itself is lifeless: it just sits there on the disk, a bunch of instructions. It is the OS that takes those bytes and gets them running. The OS virtualizes the CPU to provide the illusion that there are nearly an endless supply of CPUs. The Process never needs to wonder, is a CPU available?

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

# The Abstraction: Address Spaces

As time-sharing between processes became an issue (allowing multiple programs to reside concurrently in memory and switching between them by simply saving their registers' and PC state....), protectionbecame an important issue. You don't want one process to overwrite the memory of another.

The OS solves this by abstracting away memory into the "Address Space", which is the running program's "view" of memory.

The program while it is running uses a stack to keep track of where it is in the function call chain as well as to allocate local variables and pass parameters and return values to and from routines. The heap is used for dynamically allocated user-managed memory. So there is the code, stack and heap for a given process.

The virtual memory system is responsible for providing the illusion of a large, sparse, private address space to programs, which hold all their instructions and data therein.

If you print out a pointer memory address in a C program, you are seeing the virtual memory address. Try this out to view an example

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("location of code : %p\n", main);
    printf("location of heap : %p\n", malloc(100e6));
    int x = 3;
    printf("location of stack: %p\n", &x);
    return x;
}

/*
Results might look like this:

location of code : 0x1095afe50
location of heap : 0x1096008c0
location of stack: 0x7fff691aea64
*/

```

## Homework

Using free, this system has ~12GB (12,573MB) of total memory, of which ~4GB is "Swap" memory. There is 11880MB of "free"/unused memory.

When you run memory-user.c and allocate 1 MB of stack based memory to an array of ints, the "used" memory increases about 1 - 3 MB. When I stop the process (which runs in an infinite loop), the "used" memory decreases by about 1 - 3 MB. Memory usage varies over the process' runtime for some reason.

I get a segmentation fault when trying to allocate an array that is any larger than 2MB.

By default, pmap prints one line for each mapping within the address space of the target process:

6949: ./a.out 1 30
Address Kbytes RSS Dirty Mode Mapping
000055c931e00000 4 4 0 r-x-- a.out // file-backed mapping
000055c931e00000 0 0 0 r-x-- a.out
000055c932000000 4 4 4 r---- a.out
000055c932000000 0 0 0 r---- a.out
000055c932001000 4 4 4 rw--- a.out
000055c932001000 0 0 0 rw--- a.out
000055c932183000 132 4 4 rw--- [ anon ] // anonymous mapping (not file-backed)
000055c932183000 0 0 0 rw--- [ anon ]
00007fee8dee9000 1948 1280 0 r-x-- libc-2.27.so // file-backed mapping
00007fee8dee9000 0 0 0 r-x-- libc-2.27.so
00007fee8e0d0000 2048 0 0 ----- libc-2.27.so
00007fee8e0d0000 0 0 0 ----- libc-2.27.so
00007fee8e2d0000 16 16 16 r---- libc-2.27.so
00007fee8e2d0000 0 0 0 r---- libc-2.27.so
00007fee8e2d4000 8 8 8 rw--- libc-2.27.so
00007fee8e2d4000 0 0 0 rw--- libc-2.27.so
00007fee8e2d6000 16 12 12 rw--- [ anon ]
00007fee8e2d6000 0 0 0 rw--- [ anon ]
00007fee8e2da000 164 152 0 r-x-- ld-2.27.so
00007fee8e2da000 0 0 0 r-x-- ld-2.27.so
00007fee8e4f8000 8 8 8 rw--- [ anon ]
00007fee8e4f8000 0 0 0 rw--- [ anon ]
00007fee8e503000 4 4 4 r---- ld-2.27.so
00007fee8e503000 0 0 0 r---- ld-2.27.so
00007fee8e504000 4 4 4 rw--- ld-2.27.so
00007fee8e504000 0 0 0 rw--- ld-2.27.so
00007fee8e505000 4 4 4 rw--- [ anon ]
00007fee8e505000 0 0 0 rw--- [ anon ]
00007ffec2e05000 3920 3920 3920 rw--- [ stack ] // mapped to the "stack"
00007ffec2e05000 0 0 0 rw--- [ stack ]
00007ffec31f4000 16 0 0 r---- [ anon ]
00007ffec31f4000 0 0 0 r---- [ anon ]
00007ffec31f8000 4 4 0 r-x-- [ anon ]
00007ffec31f8000 0 0 0 r-x-- [ anon ]

---

total kB 8304 5428 3988

`pmap` displays each separate mapping of the process. A mapping is a range of contiguous pages having the same backend (anonymous or file) and the same access modes. `pmap` provides the size of the mappings instead of the ranges of addresses. Sum the size to the address to get the range of the addresses in the virtual address space.

Using pmap, I see a section labeled [RSS](https://en.wikipedia.org/wiki/Resident_set_size), which stands for Rsident set size, and is the portion of memory occupied by a process that is held in main memory (RAM). The rest of the occupied memory resides in the swap space of file system. Use of resident set size by a process is the total meory consumption of a proces. Some mappings are only partially mapped in physical memory.

Each map is associated with a set of modes:

r: if set, the map is readable
w: if set, the map is writable
x: if set, the map contains executable code
s: if set, the map is shared (right column in our previous classification). You can notice that pmap only has the s flag, while the kernel exposes two different flags for shared (s) and private (p) memory.
R: if set, the map has no swap space reserved (MAP_NORESERVE flag of mmap), this means that we can get a segmentation fault by accessing that memory if it has not already been mapped to physical memory and the system is out of physical memory.

It shows three categories of memory: anon, stack and file-backed. pmap does not "keep the heap mark." `anon` refers to the fact that the mapping does not have a "disk-based backend." Also, "Memory not relating to any named object or file within the file system is reported as `anon`. The pmap command displays common names for certain known anonymous memory mappings like 'stack'." If the common name for the mapping is unknown, pmap displays `anon` as the mapping name. (https://docs.oracle.com/cd/E19683-01/817-3936/6mjgdbveg/index.html)

Each memory mapping "maps" to one of the 3 logical segments of a process' address space: **code** (static code that defines the program), the **stack** (for runtime data processing; i.e setting of local variables or function arguments), and finally the **heap** (long-running dynamically allocated memory).

Mappings with no modes help ensure buffers so that pointers don't accidentally travers into a region of memory that they aren't supposed to.

For more detail see: https://techtalk.intersec.com/2013/07/memory-part-2-understanding-process-memory/

Also notable, is that two processes that are backed by the same file can share certain memory "page" mappings. In other words, the operating system is smart enough to know that it doesn't need to reload all the executable code and static data into virtual memory when it is already there to run an already existing process. This works because these memory mappings have modes that are only "r" or "x" and never "w". If a user program can never "write" to a certain mapping, then there is no reason that this mapping cannot be shared between processes to save memory. The exact size of a memory page can vary between systems.

# Memory

Two types of memory in a C/Unix program: Stack and Heap. Stack memory is managed implicitly by the compiler for the programmer. So it is also known as automatic memory. All allocations and deallocations of heap memory are explicitly managed by the programmer however. Heap memory is long-term memory. Here's an example:

```c
void func()
{
    int *x = (int *) malloc(sizeof(int));
}
```

Not allocating enough memory is called a buffer overflow:

```c
char *src = "hello";
char *dst = (char *) malloc(strlen(src)); // too small!
strcpy(dst, src);
```

A memory leak occurs when you allocate memory from the heap and then forget to `free()` it. In long running applications (such as the OS itself), this is a huge problem, as slowly leaking memory will lead you to eventually run out of memory. Memory leaks can occur even when working with a modern language that includes a garbage collector (if a reference to unused memory hangs around the GC won't clean it up).

There are really two levels of memory management in a system. The first level is performed by the OS, which hands out memory to processes when they run and then takes it back when they are finished. The second level is within each process, for example allocating and deallocating memory. Thus, even if you fail to call `free`, the OS will reclaim the memory pages allocated for your process once it's done. Thus for short-lived programs, leaking memory does not cause problems usually but it is considered poor form.

`malloc()` and `free()` are C standard library calls that manage space within your virtual address space, they are built on top of system calls like `brk()` which is used to change the location of the end of the heap (aka the Program's "break").

You can also obtain memory by using the `mmap()` call, which creates an "anonymous" memory region with your program - a region which is not associated with a particular file but rather with swap space. This memory can also be treated like a heap and managed as such. `man mmap` for more info.

# Linux objdump utility

Use the `objdump` utility to get info on an executable object file. For example: `objdump -i ./a.out`. See `objdump --help` for more options. There are many.

Remember "x86-64 machine code is the native language of the processors in most desktop and laptop computers. x86-64 assembly language is a human-readable version of this machine code." [Source](http://cs.brown.edu/courses/csci1260/spring-2021/lectures/x86-64-assembly-language-reference.html)

You can disassemble to x86-64 assembly code using this command: `objdump -d ./a.out -M x86-64`. Add `-S` to add source code to the assembly code as well. Where "./a.out" is an executable file.

# Address Translation

With address translation, the OS can control each and every memory access from a process, ensuring the accesses stay within the bounds of the address space for the process.

Hardware often provides a means to translate a virtual memory address into a real "physical" memory address. The OS works to maintain the illusion that each running process has it's own enclosed memory address space. In reality, programs often share memory blocks as the OS context switches between processes.

The CPU does this via the _base_ register. For example, if the OS decides to load a given process starting at physical address 32KB, it will therefore set the base register to 32KB. As the process runs, any memory address is converted at runtime by the hardware with this formula: `physical address = virtual address + base register value`. A base register is used to transform virtual addresses into physical addresses.

Because the relocation of the address happens at runtime, the technique is referred to as dynamic relocation. There is also a bounds register that ensures the generated physical address is valid for the process (i.e user processes should not be able to access memory addresses within the bounds of the actuall OS memory space). These registers are a part of the CPU's memory management unit (MMU).

The OS as startup issues a "privileged instruction" (an instruction that can only be run in the CPUs kernel mode) to set the base and bounds registers. Th ebase and bounds registers are set for each running process. So when a context switch occurs, the state of these registers is saved along with the state of all other CPU registers.

The OS must provide _exception handlers_, or functions that run when the CPU throws an exception. The OS installs these handlers at boot time via privileged instructions. For example if the process tries to access memory at runtime that is outside of its bounds, the CPU will "throw" a specific exception that will trigger an OS exception handler (residing in the OS' static 'code' wher it 'lives' in memory). In this case, the OS will likely terminate the offending process and return an error code. If no errors occur, the process runs "directly" on the CPU in user mode. Address translation extends the concept of limited direct execution, because the process does not need to know the state of the base and bounds registers.

# Segmentation

What if we had a base and bounds per logical **segment** of the address space? A segment is a contiguous portion of the address space of a particular length. There are 3 logically different segments: code, stack,and heap. Segmentation allows us to place each segment in a different part of physical memory. The hardware supports segmentation with 3 pairs of base and bounds registers for each logical segment.

The segmentation fault occurs from a memory access on a segmented machine to an illegal address.

When allocating memory, the general problem that arises is that physical memory quickly becomes full of little holes of free space, making it difficult to allocate new segments, or to grow existing. This problem is known as external fragmentation. No matter how smart the algorithm, external fragmentation will always exist, and thus a good algorithm simply seeks to minimize it.

But Segmentation helps build a more effective virtualiztion of memory that simply allocating single blocks of memory for the stack, code and heap with a set amount of "free" space that these 3 segments could grow into. Segmentation supports sparse address spaces, which avoids the potential (likely) waste of memory between logical segments of the address space.

## Homework

Link: https://pages.cs.wisc.edu/~remzi/OSTEP/vm-segmentation.pdf

Example taken from running:
`./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -c`

### Question 1 - 2

#### FOR THE STACK (SEGMENT 1):

Negative direction (grows negatively):

```
nbase1 = base1 + len1
paddr = nbase1 + (vaddr - asize) // physical address is "nbase1" + difference of virtual address and address space size.
```

Example:

```
len1 = 59
base1 = 453
nbase1 = base1 + len1 = 512
vaddr = 0x00000061 (decimal: 97) -> 1 100001  // msb indicates "stack segent" and the rest is offset, which = 33 here.
asize = 128                                   // address space size
paddr = nbase1 + (vaddr - asize)
```

Example:
481 = 512 + (97 - 128)

To check if it's a INVALID address for the segment:

`if paddr < base1 // INVALID if true`

For Heap:

`vaddr >= len0 // INVALID if true`

b) `./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -s 1`

ARG seed 1
ARG address space size 128
ARG phys mem size 512

Segment register information:

Segment 0 base (grows positive) : 0x00000000 (decimal 0)
Segment 0 limit : 20
Segment 0 Highest legal virtual address: 19

Segment 1 base (grows negative) : 0x00000200 (decimal 512)
Segment 1 limit : 36
Segment 1 Lowest legal virtual address: 92 (512 + (92 - 128) = 476)

What are the lowest and highest illegal addresses in this entire address space?

lowest illegal: vaddr = 20, paddr = 20
highest illegal: vaddr = 91, paddr = 475

SEG1 base1 = 476 (512 - 36)

VA 0: 0x0000006c (decimal: 108) (binary: 1101100) --> (SEG1: 512 + (108 - 128) = 492: VALID PHYSICAL ADDRESS)
VA 1: 0x00000061 (decimal: 97) (binary: 1100001) --> (SEG1: 512 + (97 - 128) = 481: VALID PHYSICAL ADDRESS)
VA 2: 0x00000020 (decimal: 32) (binary: 0100000) --> (SEG0: SEG VIOLATION)
VA 3: 0x0000003f (decimal: 63) (binary: 0111111) --> (SEG0: SEG VIOLATION)
VA 4: 0x00000039 (decimal: 57) (binary: 0111001) --> (SEG0: SEG VIOLATION)

Finally, how would you run segmentation.py with the -A flag to test if you are right?

This should yield two virtual address traces for vaddresses 19 and 92.

`./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -s 1 -A 19,92 -c` The results are:

```
Virtual Address Trace
  VA  0: 0x00000013 (decimal:   19) --> VALID in SEG0: 0x00000013 (decimal:   19)
  VA  1: 0x0000005c (decimal:   92) --> VALID in SEG1: 0x000001dc (decimal:  476)
```

Then if you run it instead for virtual addresses 20 and 91, you get:

```
Virtual Address Trace
  VA  0: 0x00000014 (decimal:   20) --> SEGMENTATION VIOLATION (SEG0)
  VA  1: 0x0000005b (decimal:   91) --> SEGMENTATION VIOLATION (SEG1)
```

c) ./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2

ARG seed 2
ARG address space size 128
ARG phys mem size 512

SEG1 base1 = 492 (512 - 20)

Segment register information:

Segment 0 base (grows positive) : 0x00000000 (decimal 0)
Segment 0 limit : 20
Segment 0 Highest legal virtual address: 19

Segment 1 base (grows negative) : 0x00000200 (decimal 512)
Segment 1 limit : 20
Segment 1 Lowest legal virtual address: 108 (512 + (108 - 128) = 492)

What are the lowest and highest illegal addresses in this entire address space?

Lowest illegal: 20
Highest illegal: 107

Virtual Address Trace
VA 0: 0x0000007a (decimal: 122) --> VALID in SEG1: 0x000001fa (decimal: 506)
VA 1: 0x00000079 (decimal: 121) --> VALID in SEG1: 0x000001f9 (decimal: 505)
VA 2: 0x00000007 (decimal: 7) --> VALID in SEG0: 0x00000007 (decimal: 7)
VA 3: 0x0000000a (decimal: 10) --> VALID in SEG0: 0x0000000a (decimal: 10)
VA 4: 0x0000006a (decimal: 106) --> SEGMENTATION VIOLATION (SEG1)

### Question 3

Run this `./segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15`

SEG0 last valid paddr = 0x00000036 (vaddr = 0x00000001 (decimal: 1))
SEG1 first valid paddr = 0x00000026 (vaddr = 0x0000000e (decimal: 14))

To check if it's a INVALID address for the segment:

`if paddr < base1 // INVALID if true`

For Heap:

`vaddr >= len0 // INVALID if true`

-b (Base0 register) = 53
-l (Len0 register) = 2

-B (Base1 register) = 13
-L (Len1 register) = 2

### Question 4 and 5

4. Assume we want to generate a problem where roughly 90% of the
   randomly-generated virtual addresses are valid (not segmentation
   violations). How should you configure the simulator to do so?
   Which parameters are important to getting this outcome?

A: The only thing I can come up with is to make the segments have a length that is close to 98% of the total address space size. For example, by setting the limit registers as follows:

`./segmentation.py -a 500 -p 1g -n 10 -A -1 -c --l0=245 --l1=245`

Here 490 is 98% of 500.

5. Can you run the simulator such that no virtual addresses are valid?
   How?

A: Make it so that the limit registers for Segment 1 and 2 are both set to 0. Thus, the segments have no length and can't contain any addresses. For example, run:

`./segmentation.py -a 128 -p 512 -n 10 -A -1 -c --l0=0 --l1=0`

# Free-Space Management

Conceptually, the free-list is the list of unused virtual memory in the heap.

It's easy when the space you are managing is divided into fixed-size units, but difficult when those units are of a variable size. This arises, for example, with the OS, which manages physical memory by implementing segmentation to virtualize memory resources.

External fragmentation: the free space gets chopped up into little pieces of different sizes and is thus fragmented; subsequent requests for memory may fail because there is no single contiguous space that can satisfy the request, even if the total amount of free space exceeds the size of the request.

`void *malloc(size t size)` takes `size`, which is the number of bytes requested by the application.
`malloc` manages the `heap`, and the generic data structure used to manage free space in the heap is some kind of "free list".

`man mmap`

For a real world example, see the `glibc` [allocator](https://www.gnu.org/software/libc/manual/html_node/The-GNU-Allocator.html)

## Homework

**1) Run `./malloc.py -n 10 -H 0 -p BEST -s 0`, which means 10 random operations (-n 10) with each allocation requiring 0 bytes for the header (-H 0).**

```
seed 0
size 100 (addr 1000 - 1099)
baseAddr 1000
headerSize 0 // No bytes to allocate for the header
alignment -1 // no alignment, so allocated blocks can be exactly what's requested.
policy BEST
listOrder ADDRSORT
coalesce False
numOps 10
range 10
percentAlloc 50
allocList
compute False

ptr[0] = Alloc(3) returned 1000 (searched 1 element)
Free List [ Size 1 ]: [ addr:1003 sz:97 ]

Free(ptr[0])
returned 0
Free List [ Size 2 ]: [ addr:1000 sz:3 ] [ addr:1003 sz:97 ]

ptr[1] = Alloc(5) returned 1003 (searched 2 elements)
Free List [ Size 2 ]: [ addr:1000 sz:3 ] [ addr:1008 sz:92 ]

Free(ptr[1])
returned 0
Free List [ Size 3 ]: [ addr:1000 sz:3 ] [ addr:1003 sz:5 ] [ addr:1008 sz:92 ]

ptr[2] = Alloc(8) returned 1008 (searched 3 elements)
Free List [ Size 3 ]: [ addr:1000 sz:3 ] [ addr:1003 sz:5 ] [ addr:1016 sz:84 ]

Free(ptr[2])
returned 0
Free List [ Size 4 ]: [ addr:1000 sz:3 ] [ addr:1003 sz:5 ] [ addr:1008 sz:8 ] [ addr:1016 sz:84 ]

ptr[3] = Alloc(8) returned 1008 (searched 4 elements)
Free List [ Size 3 ]: [ addr:1000 sz:3 ] [ addr:1003 sz:5 ] [ addr:1016 sz:84 ]

Free(ptr[3])
returned 0
List [ Size 4 ]: [ addr:1000 sz:3 ] [ addr:1003 sz:5 ] [ addr:1008 sz:8 ] [ addr:1016 sz:84 ]

ptr[4] = Alloc(2) returned 1000 (searched 4 elements)
Free List [ addr:1002 sz:1 ] [ addr:1003 sz:5 ] [ addr:1008 sz:8 ] [ addr:1016 sz:84 ]

ptr[5] = Alloc(7) returned 1008 (searched 4 elements)
Free List [ addr:1002 sz:1 ] [ addr:1003 sz:5 ] [ addr:1015 sz:1 ] [ addr:1016 sz:84 ]
```

**2) How are the results different when using a WORST fit policy to search the free list (-p WORST)? What changes?**

Run `./malloc.py -n 10 -H 0 -p WORST -s 0`

By the end of the last Alloc operation, the free list has a larger number of "free" nodes in it that if you had used the "BEST" policy. But with the "BEST" policy it seems like External Fragmentation is more likely, since it's easier for the free units of memory to end up chopped up into increasingly smaller pieces.

**3) What about when using FIRST fit (-p FIRST)? What speeds up when you use first fit?**

Searching through the "free" nodes in the list speeds up with the First fit policy. When the policy finds a node that "fits" that node is used immediately rather than searching the entire list to find the "best" fit.

**4)Use the different free list orderings (-l ADDRSORT, -l SIZESORT+, -l SIZESORT-) to see how the policies and the list orderings interact.**

Sorting the nodes has no impact on BEST, since all the nodes are traversed regardless.
The default seems to "ADDRSORT", where the list is sorted by addresses from least to highest. So setting `-l ADDRSORT` has no impact from the default.
Sorting from largest node size to smallest when using the FIRST policy means that the largest block is always used for memory allocation until its chopped up into small enough pieces. The final state of the largest node with SIZESORT- and FIRST is: `[ addr:1033 sz:67 ]` Whereas with ADDRSORT it is: `[ addr:1016 sz:84 ]`

**5) Coalescing of a free list can be quite important. Increase the number of random allocations (say to -n 1000). What happens to larger allocation requests over time? Run with and without coalescing (i.e., without and with the -C flag). What differences in outcome do you see? How big is the free list over time in each case? Does the ordering of the list matter in this case?**

Run `./malloc.py -n 1000 -H 0 -p (BEST|WORST|FIRST) -s 0`

BEST with coalescing:

The final operation is

ptr[514] = Alloc(2) returned 1000 (searched 1 elements)
Free List [ Size 1 ]: [ addr:1002 sz:98 ]

BEST without coalescing results in a large free list of 31 small nodes. Eventually most Alloc calls return -1 because none of the nodes are large enough to fulfill the request.

FIRST with coalescing and sorted by ADDRSORT gives this for the final operation

ptr[516] = Alloc(10) returned 1000 (searched 1 elements)
Free List [ Size 1 ]: [ addr:1010 sz:90 ]

This is very similar to BEST. However, if you sort from largest size to smallest in the Free List you end up with a lot of External Fragmentation (small nodes in the list) similar to not using coalescing. So ordering the list does matter a lot when using FIRST.

FIRST without coalescing also results in many small sized nodes in the free list and Alloc calls that are rejected.

**6) What happens when you change the percent allocated fraction -P to higher than 50? What happens to allocations as it nears 100? What about as the percent nears 0?**

The more Allocs you have, the greater the chance you will run out of memory and get returns of -1 for your allocs. If no memory is freed, you will run out of space. If you have very few allocs and a lot of frees (i.e -P 05), you will see a lot of external fragmentation for BEST WORST and FIRST unless you enable free node coalescing.

# Introduction to Paging

Paging is dividing memory into fixed-size chunks in virtual memory. Each fixed-size unit is called a **page**. Pages help avoid the problem of segmentation.

A processes virtual address space is not contiguous in memory even if it "appears" that way to a process. Virtual memory pages are assigned to a process by the OS. The OS keeps a "free list" of unused (free) pages. Physical memory is divided into fixed-size **page frames** into which a process' virtual memory page can be "placed."

The page table is a per process data structure that stores address translations for each virtual page in the process' address space, thus letting us know where each page resides in physical memory.

The virtual address for a process is split into two components: the **virtual page number (vpn)** and the **offset** within the page. If a process' virtual address space is (very very small) only 64 bytes, then a virtual address would be 6 bits (`2^6 = 64 bytes`), and represented as follows:

```
| Va5 | Va4 | Va3 | Va2 | Va1 | Va0 |

// Va5 is the highest order bit and va0 the lowest order bit

// The two highest order bits indicate the VPN and the rest indicate the offset within the page.
```

The OS must translate the vpn into the address of the physical page frame.

When a process is first started up, the virtual address space of the process is 4 distinct sections: `code`, `heap` (at one "end"), `stack`, and `unused`. In physical memory, pages are allocated to these sections are not necessarily contiguous. If a process ever tries to access an invalid address, a trap into the OS is triggered that terminates the process.

It is possible to "swap" a page to disk, which is a strategy the OS uses to free up physical memory.

The page table data structure is composed of a page table entry (PTE). In x86 architecture, a 32-bit PTE has a few key bits that convey information about the page. For example, a read/write bit conveys if the page can be read and or written to. A user/ supervisor bit conveys if a user-mode process can access the memory page. There are a few bits that determine how hardware caching works, there is a "present" bit that conveys if the page has been "swapped" to disk or resides in memory currently. An "accessed bit" to convey if the page has been used recently, a dirty bit, and also the Page Frame Number (PFN) itself which might be many bits long based on the total number of addresses avaialble in the system.

## Homework

Steps to translate a virtual address to a physical address

1. Calculate total number of pages in the address space.
   Total = Address space size / Page size
   Then solve 2^x == Total, where x is the number of bits required to represent PTE index in the VA.
   Example for a 16K address space:
   (16 \* 1024) / (1024 \* 4) === 4
   2^x = 4 -> x == 2, so 2 bits are required for the PTE index.

2. Calculate the total size of a virtual address in the address space
   2^x = Total space of address space, where x is number of bits in a virtual address.
   Do the same for the physical address space
   For example:
   2^14 == 16384 (16K or 16 \* 1024), so 14 bit VAs in a 16K address space.
   2^15 == 32k (physical memory addresses), so 15 bits are required for an address.

3. Translate a virtual address to binary and retrieve PTE
   0x00003229 -> 11 001000101001 -> idx === 3
   idx | offset

4. Match idx to PTE index and get PTE
   Page Table (from entry 0 down to the max size)
   [ 0] 0x8000000c
   [ 1] 0x00000000
   [ 2] 0x00000000
   [ 3] 0x80000006

5. Validate PTE
   0x80000006 from index 3 (11) -> 10000000000000000000000000000110
   Highest order bit is "1", so it's valid

6. Use lowest order bits to obtain physical address
   0110 | 001000101001 -> (decimal 25129)
   PFN | offset

# Translation Lookaside Buffers

Paging as the core mechanism to support virtual memory requires a lot of mapping information and therefore extra use of memory and memory lookups for each virtual address. The hardware helps the OS speed up the VA to PA translation.

A translation lookaside buffer (TLB) is a part of the chip's memory-management unit and is a hardware cache for popular virtual-to-physical address translations. A better name would therefore be an "address-translation cache."

In modern RISC (reduced-instruction set) computers, the system expects the OS (software) to manage the TLB. if the hardware raises a "TLB MISS" when translating a virtual address, the current instruction stream is paused, the privilege level is raised to kernel mode, and the CPU jumps to a "TLB MISS" trap handler to handle the exception, i.e translate the virtual address using the OS' data structures for the Page Table, and save the translation in the TLB. Once the "Return from trap" is issued, the hardware retries the TLB access.

The TLB contains virtual-to-physical translations that are only valid for the currently running process, so when context switching, the os must be careful to ensure that the next process does not accidentally use translations from some previously run process.

Some hardware systems flush the TLB (set all entries to 0) on a context switch. Others support an address space identifier field in the TLB.

See `man mmap`.

## Homework

projects/vm-tlb

See https://man7.org/linux/man-pages/man2/getpagesize.2.html
`sysconf(_SC_PAGESIZE)`
