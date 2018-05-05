.globl my_setjmp
.globl my_longjmp

# This function saves the execution state of the program by saving the
# state of the callee-save registers, the stack pointer, and the caller's
# return address into a jump buffer (passed in as an argument) in %rdi. After
# saving this execution state, the function returns 0.

my_setjmp:

   movq    %rsp,    (%rdi)      # save stack pointer at the start of the buffer
   movq    (%rsp),  %rcx        # save caller's return address
   movq    %rcx,    8(%rdi)     # save return address in buffer
   movq    %rbx,    16(%rdi)    # save callee save registers
   movq    %rbp,    24(%rdi)
   movq    %r12,    32(%rdi)
   movq    %r13,    40(%rdi)
   movq    %r14,    48(%rdi)
   movq    %r15,    56(%rdi)

   xorq    %rax,    %rax         # return value 0
   ret


# This function restores the saved execution state of the program by restoring
# the state of the callee-save registers, the stack pointer, and the caller's
# return address from a jump buffer (passed in as an argument) in %rdi. After
# restoring this execution state, it returns the passed in value if not zero.
# If zero, returns 1.

my_longjmp:

  movq    (%rdi),      %rsp     # restoring stack-pointer
  movq    8(%rdi),     %rcx     # restoring caller return address
  movq    %rcx,       (%rsp)
  movq    16(%rdi),    %rbx     # restoring callee-save registers
  movq    24(%rdi),    %rbp
  movq    32(%rdi),    %r12
  movq    40(%rdi),    %r13
  movq    48(%rdi),    %r14
  movq    56(%rdi),    %r15

  movl    %esi,    %eax          # getting integer arguments to return
  movl    $1,      %ecx          # moving one in register to potentially return
  test    %eax,    %eax          # check if the arguments is nonzero
  cmovz   %ecx,    %eax          # if 0, move 1 in register to return
  ret
