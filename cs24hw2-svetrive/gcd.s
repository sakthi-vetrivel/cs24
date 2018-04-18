# The gcd(n1, n2) function recursively calculates the GCD of two integers.
# n1 stored in edi
# n2 stored in esi
#
.globl gcd
gcd:
        cmpl    $0, %esi          # Sets zero flag if b equals zero
        jne     gcd_continue      # Compute gcd if nonzero
        movl    %edi, %eax        # Otherwise return a
        jmp     gcd_done          # return

gcd_continue:
        movl    %edi, %eax      # Compute a mod b for arg in recursive call
        cltd                    # convert long to double-long for computation
        idivl   %esi            # divide n1 by n2, remainder stored in edx
        movl    %esi, %edi      # b is our new n1 in the recursive call
        movl    %edx, %esi      # a mod b is the our new n2 in the recursive call
        call    gcd             # Make recursive call; answer will be in eax

gcd_done:
        ret                     # all done!
