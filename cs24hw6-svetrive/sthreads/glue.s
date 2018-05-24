#============================================================================
# Keep a pointer to the main scheduler context.  This variable should be
# initialized to %rsp, which is done in the __sthread_start routine.
#
        .data
        .align 8
scheduler_context:      .quad   0


#============================================================================
# __sthread_switch is the main entry point for the thread scheduler.
# It has three parts:
#
#    1. Save the context of the current thread on the stack.
#       The context includes all of the integer registers and RFLAGS.
#
#    2. Call __sthread_scheduler (the C scheduler function), passing the
#       context as an argument.  The scheduler stack *must* be restored by
#       setting %rsp to the scheduler_context before __sthread_scheduler is
#       called.
#
#    3. __sthread_scheduler will return the context of a new thread.
#       Restore the context, and return to the thread.
#
        .text
        .align 8
        .globl __sthread_switch
__sthread_switch:

        # Save the process state onto its stack
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
        pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15

        pushf


        # Call the high-level scheduler with the current context as an argument
        movq    %rsp, %rdi
        movq    scheduler_context, %rsp
        call    __sthread_scheduler

        # The scheduler will return a context to start.
        # Restore the context to resume the thread.
__sthread_restore:
        movq %rax, %rsp     # The returned value should be our stack pointer

        popf

        # Pop items off of stack into the register to restore

        popq  %r15
        popq  %r14
        popq  %r13
        popq  %r12
        popq  %r11
        popq  %r10
        popq  %r9
        popq  %r8
        popq  %rbp
        popq  %rdi
        popq  %rsi
        popq  %rdx
        popq  %rcx
        popq  %rbx
        popq  %rax

        ret


#============================================================================
# Initialize a process context, given:
#    1. the stack for the process
#    2. the function to start
#    3. its argument
# The context should be consistent with that saved in the __sthread_switch
# routine.
#
# A pointer to the newly initialized context is returned to the caller.
# (This is the thread's stack-pointer after its context has been set up.)
#
# This function is described in more detail in sthread.c.
#
#
        .align 8
        .globl __sthread_initialize_context
__sthread_initialize_context:

        movq %rdi, %rax      # Move return value (the stack pointer) into return
        subq $144, %rax      # Moving backward 144 bytes to get the start of the
                             # stack

        movq $__sthread_finish, 136(%rax)   # Address of the function at the top

        movq %rsi, 128(%rax)  # Return address of this function
        movq $0, 120(%rax)    # rax = 0
        movq $0, 112(%rax)    # rbx = 0
        movq $0, 104(%rax)    # rcx = 0
        movq $0, 96(%rax)     # rdx = 0
        movq $0, 88(%rax)     # rsi = 0

        movq %rdx, 80(%rax)   # Argument for function, rdi
        movq $0, 72(%rax)     # rbp = 0
        movq $0, 64(%rax)     # r8 = 0
        movq $0, 56(%rax)     # r9 = 0
        movq $0, 48(%rax)     # r10 = 0
        movq $0, 40(%rax)     # r11 = 0
        movq $0, 32(%rax)     # r12 = 0
        movq $0, 24(%rax)     # r13 = 0
        movq $0, 16(%rax)     # r14 = 0
        movq $0, 8(%rax)      # r15 = 0
        movq $0, (%rax)       # rflags = 0

        ret


#============================================================================
# The start routine initializes the scheduler_context variable, and calls
# the __sthread_scheduler with a NULL context.
#
# The scheduler will return a context to resume.
#
        .align 8
        .globl __sthread_start
__sthread_start:
        # Remember the context
        movq    %rsp, scheduler_context

        # Call the scheduler with no context
        movl    $0, %edi  # Also clears upper 4 bytes of %rdi
        call    __sthread_scheduler

        # Restore the context returned by the scheduler
        jmp     __sthread_restore
