/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 */
 /*The naive implementations would all contain if statements to compute the results. Since these implementations avoid that, we avoid conditional jumps in the assembly code, which would have been very costly. Instead, all of these functions simply just use x86-64 operations for this computation, making all of these operations much faster. 

        .text

/*====================================================================
 * int f1(int x, int y) returns min(x,y)
 */
.globl f1
f1:
        movl    %edi, %edx  # move x into edx
        movl    %esi, %eax  # move y into eax
        cmpl    %edx, %eax  # compare x and y
        cmovg   %edx, %eax  # if y > x, then move x into eax
        ret


/*====================================================================
 * int f2(int x) returns abs(x)
 */
.globl f2
f2:
        movl    %edi, %eax  # eax contains x
        movl    %eax, %edx  # copy x to edx
        sarl    $31, %edx   # shift right to get sign bit
        xorl    %edx, %eax  # if eax is negative, then inverts all bits
        subl    %edx, %eax  # subtract will add one to inverted bits, so
                            # this uses twos complement to calculate the abs(x)
        ret


/*====================================================================
 * int f3(int x) returns sign(x)
 */
.globl f3
f3:
        movl    %edi, %edx    # edx contains x
        movl    %edx, %eax    # copy x to eax as well
        sarl    $31, %eax     # shift to get sign bit
        testl   %edx, %edx    # set flag based on whether its positive or neg
        movl    $1, %edx      # set edx to 1 if positive
        cmovg   %edx, %eax    # move into result register
        ret
