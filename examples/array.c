int main()
{
    int array[1000];
    int i;
    for (i = 0; i < 1000; i++)
        array[i] = 0;
}

/**
 * See page 11 of vm paging chapter
 * https://pages.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf
 *
 * Rough x86 translation (just for main()):
 *
 * 1024 movl $0x0,(%edi,%eax,4) -> edi register holds base address of the array, and eax register holds the array index. It is an array of integers, so each array entry is 4 bytes long. So each getting to the next index means adding 4 bytes to the current index address.
 * 1028 incl %eax               -> Increment the eax register
 * 1032 cmpl $0x03e8,%eax       -> 0x03e8 == 1000 in decimal. Compare 1000 to the value in the eax register. Pushes 0 (false) or 1 (true) to the stack.
 * 1036 jne 0x1024              -> "jump not equal to address 0x1024", or set PC (program counter register) to this instruction address.
 *
 * In particular, what will change as the loop continues to run
 * beyond these first five iterations? Which new memory locations will be
 * accessed?
 *
 * On every loop, the same instructions are fetched from the Code block of memory (movl, incl, cmpl, and jne), so these will not
 * change from the first loop to the last.
 * For the array, each "index" address (address of i integer in the array) can be derived from translating the array's base virtual address to the
 * physical address of the base of the array, then adding (i * 4) to the base address. So Page Table accessing actually is always the same
 * For every loop, since the base address of the array and the instructions are unchanging.
 *
 * This memory trace exercise demonstrates that the page table accesses for every loop are actually unchanging. There is an opportunity to
 * chache these memory lookups for every loop iteration. The new memory locations accessed are in other words predictable ->
 * Array Base Address + (i * 4)
 */