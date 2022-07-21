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

By default, pmap prints one line for each mapping within the address space of the target process. By default `pmap` shows four columns: the virtual address of the mapping, its size, the protection bits of the regios, and the source of the mapping.

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

Using pmap, I see a section labeled [RSS](https://en.wikipedia.org/wiki/Resident_set_size), which stands for Rsident set size, and is the portion of memory occupied by a process that is held in main memory (RAM). The rest of the occupied memory resides in the swap space of file system. Use of resident set size by a process is the total memory consumption of a proces. Some mappings are only partially mapped in physical memory.

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

Another simple example:

```
0000000000400000 372K r-x-- tcsh
00000000019d5000 1780K rw--- [anon ]
00007f4e7cf06000 1792K r-x-- libc-2.23.so
00007f4e7d2d0000 36K r-x-- libcrypt-2.23.so
00007f4e7d508000 148K r-x-- libtinfo.so.5.9
00007f4e7d731000 152K r-x-- ld-2.23.so
00007f4e7d932000 16K rw--- [stack ]

```

As you can see from this output, the code from the tcsh binary, as well as code from libc, libcrypt, libtinfo, and code from the dynamic linker itself (ld.so) are all mapped into the address space. Also present are two anonymous regions, the heap (the second entry, labeled anon) and the stack (labeled stack). Memory-mapped files provide a straightforward and efficient way for the OS to construct a modern address space.

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

Computer Memory is referenced by a process on each instructin fetch, as well as explicit loads and stores.

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

# Advanced Page Tables

A Multi-level page table means that the OS does not need to maintain a large contiguous block of memory for the Page Table, since the VPN is the index into the Page Table from which the Page Table Entry (PTE) can be retrieved. Instead, given a new data structure called the Page Table Directory, the OS can dynamically allocate additional memory to a page table that is free from the constraint of needing to be contiguous.

For example, in a 16kB address space with 64 byte pages, there are a total of 256 pages (2^8). Therefore a contiguous Page Table would require 256 entries. However, using a multi-level approach, those could themselves be broken up into 16 pages 265 / 64 = 16. The Page Directory holds therefore 16 entries, each one referencing a "page" of the Page Table. As for example the heap grows, more memory is required by the process and more memory can be allocated to the Page Table rather than needing to allocate the entire page table up front at once (as with the linear page table model).

# Beyond Physical Memory: Mechanisms

To support large virtual address spaces for processes, the OS needs to stash away portions of the adress space outside of physical memory. The OS uses a hard disk drive, or SSD drive to accomplish this. Thus, memory pages can be "swapped" in and out of physical memory, less used pages can be stash on disk. Programs don't need to know if a given piece of data is currently stored by the OS on disk or in physical memory. Both are valid in the virtual address space.

`swap space` is the region on disk that is reserved by the OS for moving memory pages in and out of physical memory. Thus the OS reads and writes to this space in page-sized units. The OS needs to remember the `disk address` for pages in swap space.

The `present bit` in a PTE entry will identify whether that page is swapped to disk (0) or in physical memory already (1). The act of accessing a page that is not in physical memory is commonly referred to as a `page fault`. The hardware invokes the OS page-fault handler to determine how to access the page.

If the present bit is 0 in the page table entry (PTE), then the bits in the PTE that normally are used for such data as locating the PFN of the page in memory, are instead used to identfy its location in the swap space on disk. An I/O fetch must occur to retrieve the page from disk. The OS must identify a Phyisical Page Frame in which the swapped page can be loaded into. Then, the PTE must be updated with the PFN, and a present bit of 1 (now in memory). THEN the page fault handler can finally retry the original memory access instruction, which will generate a TLB miss... but at this point the retrieved PTE refers to a Page in memory (present bit is 1), so this PTE can be added to the TLB with its PFN... phew (See page 6, figure 21.2 - Page fault control algorithm)

## Homework

`man vmstat` -> virtual memory usage reporter.

`man /proc` to learn about the pseudo-filesystem for reading from kernel data structures.

`cat /proc/meminfo` to read system memory info.

3. Re: Evaluating the impact of using swap memory in virtual memory.
   Allocating ~ 4GB to a single user process (using `mem.c` -> `./mem 4000`) means the system has close to 3 GB of "free" memory left, because the system has 8GB in total and some resources are allocated to the OS and its data structures and kernel processes.
   No data is swapped in/out with just this one user process running.
   If I allocate 7GB using `mem`, vmstat reports 327088kB swpd and ~126188kB free memory. Without running `mem` it reported 7043392kB free. 7043392 - 126188 = ~6.9GB, which is close to 7 with the remander being filled in by ~0.2GB of swap memory (these values vary every second).
   Interestingly, after killing `./mem 7000`,the OS "slowly" frees up memory written to "swpd" space on disk.

4. Performance: When ~ 7, 8GB, or > 8GB is allocated in `mem`, vmstat reports the cpu `wa` (Time spent waiting for IO) as skyrocketing. The firt loop in `mem` takes much longer:

```
# allocate 4GB
> ./mem 4000

loop 0 in 1573.15 ms (bandwidth: 2542.66 MB/s)
loop 1 in 714.48 ms (bandwidth: 5598.52 MB/s)
```

```
# allocate 12GB
> ./mem 12000

loop 0 in 19379.70 ms (bandwidth: 464.40 MB/s)
loop 1 in 76164.79 ms (bandwidth: 118.16 MB/s)
loop 2 in 79432.61 ms (bandwidth: 113.30 MB/s)
loop 3 in 73266.04 ms (bandwidth: 122.84 MB/s)
```

`vmstat` reports ~2GB of swap memory in use in the second case, and cpu I/O "wait" times of ~98 units of time (I can't figure out what the unit is for the "cpu" time section of reporting in vmstat). No swap memory is used in the first case. Using a large amount of swap memory clearly incurs a significant performace hit, as the CPU is blocked.

Using swap memory in loop 0 in the second case, the bandwidth reported is 454.40 MB/s. Bandwidth gives you a sense of how fast the system you're using can move through data. Using no swap memory, the reported bandwidth was 2542.66 MB/s, which is 559% faster.

5. View a summary of swap location using `swapon`:

```
> swapon -s

Filename                                Type            Size    Used    Priority
/swap/file                              file            2097152 234804  -2
```

This is different than I would have expected. The swap location is repored as being a "file" instead of some sort of hardware device. I guess this device is "virtualized" behind the file abstraction. `swapon` reports "Used" as very high because I ran `mem` with 9GB and so swap memory was used and is slowly reclaimed over time by the OS.

# Beyond Physical Memory: Policies

The _Replacement Policy_ of the OS dictates which pages are evicted when memory pressure is high.
The average memory access time (AMAT) is measured by computer architects to mesure hardware cache performance. The AMAT can be calculated as follows:

```
AMAT = Tm + (Pmiss * Td)

// Tm == Cost of accessing main memory
// Pmiss == The probability of not finding data in the cache
// Td == Cost of accessing data pages on disk.
```

A compulsory miss for a memory page (i.e page not in memory) is when a "miss" occurs simply because the page hasn't been loaded into main memory yet (it's the first time it is referenced). Sometimes when evaluating the memory cost of a certain cache policy, it makes sense to disregard this first miss from your analysis because there's nothing you can do about it, and instead focus on the cache misses that occur due to you policy of when to clear or replace all or parts of the cache.

The Least-Frequently-Used (LFU) policy replaces the least frequently used page when an eviction must take place, and the Least-Recently-Used (LRU) policy replaces teh least recently used page. This are stateful policies that track the "use" of memory pages over time to help in deciding which pages to relpace in the cache with new pages.

# Complete VM Systems

Linux runs effectively on systems as small and underpowered as phones to the most scalable multicore systems found in modern datacenters. Thus, its VM system must be flexible enough to run successfully in all of those scenarios.

The kernel is mapped onto each user process virtual address space using a "system space" within the user process virtual address space. In this way, the OS, its trap table (i.e), its heap, data structures, etc appear as a (protected) "library" accessible to each process. The CPU maintains "S" (system) base and bounds registers. On a context switch, the OS does not swap out the references in the S registers because these should always point to the kernel. As a result the "same" kernel structures are mapped into each user address space.

OS implementation depends on hardware support: "The OS does not want user applicaions reading or writing OS data or code. Thus, the hardware must support different protection levels for pages to enable this. The VAX (old OS example...) did so by specifying in protection bits in the page table, what privelage level the CPU must be at in order to access a particular page. Thus system data and code are set to a higher level of protection than user data and code; an attempted access to such information from user code will generate a trap into the OS, and a likely termination of the offending process."

In the passage above, note that the particular implementation regarding how OS code and data structures are protected from random accesses from user-level applications depends on hardware implementations. For example, the CPU must support "setting" a user-level or kernel-level bit. The OS manages the page table in this example. Each PTE denotes the protection level of each page. The hardware must also support a "trap table", and the OS must know this. The OS provides the trap table to the hardware. The hardware must also support a page table, otherwise virtual memory cannot be supported.

Resident Set Size (RSS) denotes the maximum number of pages a process can keep in memory. To examine the address space of a linux process, again, use `pmap`.

## The Linux Virtual Memory System

The book focuses on Linux for Intel x86 architecture. While Linux runs on many different processor architectures, Linux on x86 is the most dominant. "32-bit Linux" is Linux with a 32-bit address space for each process.

the x86 architecture provides a hardware managed, multi-level page-table structure, with one page table per process; the OS simply sets up mappings in its memory, points a priveleged register at the start of the page directory, and the hardware handles the rest. On context switches, the OS makes sure the hardware page directory registers are set appropriately.

The standard page size is 4KB (4096 bytes) in Linux for x86 64-bit architecture, where "64-bit" refers to the size of a process' virtual address space. But Linux also supports "huge pages", which can be as large as 2MB or even 1GB. One advantage of _huge pages_ is that the OS can maintain smaller page tables (less memory pressure in the kernel). But this is not the main benefit. The main benefit is in performace, as fewer PTEs means fewer TLB misses need to be serviced. Applications can spend fewer CPU cycles servicing TLB misses with larger pages.

Linux applications can explicitly request memory allocations with large pages using `mmap()` or `shmget()` calls.

The Linux page cache keeps "popular"/ often used pages in memory from these primary sources:

1. memory-mapped files -> file data, and metadata from devices (usually accessed by directing `read()` and `write()` calls to the file system).
2. anonymous memory -> heap and stack pages that comprise each process. Called "anonymous" because there is no named file underneath of it, but rather just swap space.

These entities are kept in a page cache hash table, allowing for quick lookup when said data is needed.

https://spectreattack.com/

# Concurrency

Threads have their own PCs (program counters) from which instructions are fetched. A multi-threaded program has more than one poin of execution. Each thread is like a separate process, except they all share the same address space and thus can access the same data. The state of a thread is similar to that of a process: each thread has a PC and it's own registers state. Context switches between threads can take place. Each thread must have its own stack.

Why threads? Multi-threaded programs take advantage of parallelism, or the task of splitting up work in a process across two or more CPUs to allow work to be done in parallel. You also might want to avoid progress due to slow I/O.

To avoid race conditions in mutli-threaded programming, we want to ensure our code adheres to the property of multual exclusion, which guarantees that accesses to a shared variable will not be executed concurrently by two or more threads.

## Key terms

Critical section: code that accesses a shared resource, like a variable.
Race condition: if mutliple threads of execution enter the critical section at roughly the same time, shared data might be accessed in unexpected and inconsistent ways.
Indeterminate program has race conditions. Thus the outcome is not deterministic, which we usually want in computer systems.
The hardware should provide mutual exclusion primitives to guarantee that only a single thread ever enters a critical section at once, thus avoiding races, and keepingthings deterministic.

Homework:

1. `./x86.py -p loop.s -t 1 -i 100 -R dx`

Notes: `-t` specifies the number of threads. `-i 100` specifies the interrupt frequency with the unit being "number of instructions to execute before a thread context switch." `-R` notes the registers to "track" in the output results.

Regsiter `%dx` starts at `0` (this is the default), and after the run it will be -1.

2. `./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx`

Here the `dx` register is initialized to 3 for 2 threads. `%dx` should, for each thread go from 3 to -1. There is not a race in this code because the critical section is alwys run in its entirety by each thread. Therefore the outcome is deterministic.

Note: `jgte` will effectively "jump" if the _second_ value provided to the previous `test` is greater than or equal to the first value provided to `test`.

3. `./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx`.

Even though this config increases the number of interrupts and makes it so the threads are inerrupted as they change the value held in `%dx`, there is still no race condition, since the threads are not changing a shared global variable located at a certain memory address. Instead they are changing the state of their isolated `%dx` registers, which are isolated and specific to each thread.

4. `./x86.py -p looping-race-nolock.s -t 1 -M 2000`

Note: this will report the state of memory address `2000`.

The `value` at memory address 2000 is `0` because it is not initialized to a different value in the config.

5. `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000`

The two threads load a global (shared) variable in their `ax` register respectively, but there is no race condition because the interrupt config default to `50` instructions before forcing a switch here. So each thread runs to completion. Each thread loops three times, because the `bx` registers for each thread are initialized to `3`, and the state of each `bx` is isolated and not shared between threads. The value at memory address 2000 starts at `0` and after each thread runs, it ends up at `6`.

6. `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0`

Where is the critical section in `looping-race-nolock.s`? The critical section is the assembly code/ block of code that accesses a shared variable (shared across threads),for example:

```
mov 2000, %ax  # get 'value' at address 2000
add $1, %ax    # increment it
mov %ax, 2000  # store it back
```

Here,the code loads the value at memory address `2000` into the thread's `%ax` register, thus accessing and modifying a global variable.

Does the timing of the interrupt matter? Yes, the timing matters a lot. For example, here:

```
 M 2000

 ------ Interrupt ------  ------ Interrupt ------
    3   1004 mov 2000, %ax
    3   1005 add $1, %ax
    3   ------ Interrupt ------  ------ Interrupt ------
    3                            1004 mov 2000, %ax
    3                            1005 add $1, %ax
    4                            1006 mov %ax, 2000
    4                            1007 sub  $1, %bx
    4                            1008 test $0, %bx
    4                            1009 jgt .top
    4   ------ Interrupt ------  ------ Interrupt ------
    3   1006 mov %ax, 2000
```

Thread 0 is "interrupted" in the middle of the critical seciton. It has loaded variable `2000` into its `ax` register. When it starts running aagin, it ends up resetting the state of variable `2000` back to 3 simply because of when it's last interrupt occurred. This is a non-deterministic outcome.

7. `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1`

For which interval setting is the "correct" answer provided? The correct answer is provided when one thread does not overwrite the work of another in a global shared variable. So in this case setting `-i 3` allows that, but `-i 1` or `2` does not.

9. `./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000`

This creates two threads. How is the value at location `2000` being used by the threads? Memory location `2000` is a global shared variable that the threads both reference. The first thread has its `%ax` register value set to `1`, which causes it to "jump" to `.signaller`, in which the constant `1` is `mov`-ed to memory location `2000`. When the second thread starts, it reads `2000` into a register, and if its value is equal to `1`, the program "halts." How should this code behave? The code is set up such that one thread must place `1` into memory location `2000` for another thread in order for the second thread to exit the loop in the `.waiter` block.

10. Switch the inputs: `./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000`

What is thread 0 doing? Thread 0 has its `ax` register set to `0`, which will push it into the `.waiter` block. Since `2000` is set to `0` still it is effectively in an infinite loop until the interrupt occurs and the second thread begins after a thread context switch. The second thread has ann initial state of `1` for its `%ax` register, which means it will update `2000` to hold `1` and then exit. Finally, when Thread 0 starts up again in its loop, the next time it loads `2000`, the logic in `.waiter` will reach the `halt`. Thus the length of time the program takes in total is entirely dependent on when an interrupt occurs.

# Thread API

`pthread_create(...)`, `pthread_join(...)`

Homework

1. `valgrind --tool=helgrind ./main-race`. How does `helgrind` report the race condition and what other information does it give you?
   `helgrind` is a thread error detector. The race condition is reported as follows:

```
==1373== Possible data race during write of size 4 at 0x309014 by thread #1
==1373== Locks held: none
==1373==    at 0x10880F: main (main-race.c:15)
==1373==
==1373== This conflicts with a previous write of size 4 by thread #2
==1373== Locks held: none
==1373==    at 0x10879B: worker (main-race.c:8)
==1373==    by 0x4C38C26: ??? (in /usr/lib/valgrind/vgpreload_helgrind-amd64-linux.so)
==1373==    by 0x4E4B6DA: start_thread (pthread_create.c:463)
==1373==    by 0x518471E: clone (clone.S:95)
==1373==  Address 0x309014 is 0 bytes inside data symbol "balance"
```

The main thread at line 15 reads (and then modifies) a `int` global variable (size 4 bytes). The created thread is reported as having modified this same variable.

2. What happens when you remove one of the offending lines of code? `helgrind` reports no errors. What happens when you put "locks" around just one of the offending lines of code?

```c
void *worker(void *arg)
{
    pthread_mutex_t lock;
    Pthread_mutex_lock(&lock);
    balance++; // unprotected access
    Pthread_mutex_unlock(&lock);
    return NULL;
}
```

`helgrind` still reports the same error. The problem is that there are no possible "locks" from thread #1, so that gurantees that this code would never be "lock"-ed out of running. You must wrap every access of a global variable with "locks". By correctly locking (and initializing locks) for all global variable accesses, `helgrind` reports 0 possible errors.

3. `helgrid` reports "Observed (incorrect) order" in lock acquisition. I don't fully understand the error as reported by `helgrind`, but it seems to read as, once a lock order is observed, subsequent "lock" requests mst adhere to that same order.

# Locks

Locks "wrap" critical sections to ensure the code therein executes as a single atomic instruction. Example:

```
lock_t mutex;
lock(&mutex);
balance = balance + 1; // critical section, global var
unlock(&mutex);
```

A lock variable holds the state of the lock at any instant in time. It is either available, free, or acquired. By putting locks around some section of code, the programmer guanrantees that no more than a single thread can be active within that code. This brings some scheduling control back to the programmer. POSIX calls "locks" "mutex" for "mutual exclusion."

Today, system designers build hardware that explicitly supports locks. Often, these are known as the _test-and-set_ or _atomic-exchange instruction_. In x86, this instruction is called `xchg`. This works because the "test" and "set" of the lock is a single atomic operation.

The Compare-and-swap instruction, or Compare-on-exchange (as it is called on x86) single instruction. Similar to test-and-set.

Implementing a `lock` depends on an operating system primitive `yield()`, which a thread can ncall when it wants to give up the CPU and let another thread run. The yielding thread essentially deschedules itself.
Some OS (i.e Solaris) provide `park()` and `unpark(threadid)` to put a calling thread to sleep, and then later wake it up.

# Locked Data Structures

See ostep-homework/threads-locks-usage

# Condition Variables

https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf
