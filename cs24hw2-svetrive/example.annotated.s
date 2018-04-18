.file	"example.c"
.text
.globl	ex
.type	ex, @function
ex:
.LFB0:
.cfi_startproc
pushq	%rbp
.cfi_def_cfa_offset 16
.cfi_offset 6, -16
movq	%rsp, %rbp
.cfi_def_cfa_register 6
movl	%edi, -20(%rbp)    # moving a into register
movl	%esi, -24(%rbp)    # moving b into register
movl	%edx, -28(%rbp)    # moving c into register
movl	%ecx, -32(%rbp)    # moving d into register
movl	-24(%rbp), %eax    # move b into register to compute
subl	-28(%rbp), %eax    # compute b - c
imull	-20(%rbp), %eax    # compute a * (b - c)
movl	%eax, %edx
movl	-32(%rbp), %eax
addl	%edx, %eax         # compute a * (b - c) + d
movl	%eax, -4(%rbp)
movl	-4(%rbp), %eax
popq	%rbp
.cfi_def_cfa 7, 8
ret
.cfi_endproc
.LFE0:
.size	ex, .-ex
.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
.section	.note.GNU-stack,"",@progbits
