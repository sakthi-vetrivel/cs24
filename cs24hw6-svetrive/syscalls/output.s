# data is stored in rdi
# size is stored in rsi
.globl output

output:

  movq %rsi, %rdx
  movq %rdi, %rsi
  movl $1, %edi

  movl $1, %eax

  syscall

  ret
