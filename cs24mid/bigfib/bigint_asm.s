# The int __add_bigint(uint64_t *a, uint64_t *b, uint8_t size) function adds
# two large integers a and b that are represented as arrays of size "size" of
# 8-byte quadwords. The function returns 0 if there is overflow detected,
# otherwise, it returns 1.
# a stored in rdi
# b stored in rsi
# size stored in dl

.globl __add_bigint

__add_bigint:
  xor      %rcx, %rcx               # clear counter register
  xor      %r13, %r13               # clear register to store size as we decr
  mov      $0, %cl                  # set counter to 0
  mov      %dl, %r13b               # counter to decrement size
  cmp      %dl, %cl                 # compare counter value to size
  clc                               # clear carry flag
  je       done                     # if size == 0, return


add_loop:
  xor     %r11, %r11                # clear registers for math later
  xor     %r10, %r10
  mov     (%rdi, %rcx), %r10        # %r10 stores a[i]
  mov     (%rsi, %rcx), %r11        # %r11 stores b[i]
  adc     %r11, %r10                # add r10 to r11 with carry
  mov     %r10, (%rsi, %rcx)        # move result of addition into b[i]
  inc     %cl                       # increment i
  dec     %r13b                     # decrement size for zero flag to end loop
  je      done                      # move to final operations

done:
  jc      fail                      # if the carry flag is on, then overflow
  mov     $1, %eax                  # otherwise, return 1
  ret

fail:
  mov     $0, %eax                  # carry flag on, return 0
  ret
