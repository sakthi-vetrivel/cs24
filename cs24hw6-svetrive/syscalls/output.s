# int64_t output(void *data, uint64_t size);
#
# This function uses a system call to write() to output the data that is input.
# This function returns the number of bytes read, or an error message
#
# Input:
#  data (the data to write) is stored in rdi
#  size (the size of the data to write) is stored in rsi
#
#

.globl output

output:

  movq %rsi, %rdx       # Move third argument (number of bytes to read)
  movq %rdi, %rsi       # Move pointer to data (2nd argument)
  movl $1, %edi         # Move first argument (file descriptor)

  movl $1, %eax         # Move return value

  syscall               # Make function call

  ret
