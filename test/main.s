	.file	"main.cpp"
	.section	.text._ZN1AC2Ev,"axG",@progbits,_ZN1AC5Ev,comdat
	.align 2
	.weak	_ZN1AC2Ev
	.type	_ZN1AC2Ev, @function
_ZN1AC2Ev:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	nop
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	_ZN1AC2Ev, .-_ZN1AC2Ev
	.weak	_ZN1AC1Ev
	.set	_ZN1AC1Ev,_ZN1AC2Ev
	.section	.text._ZNK1A6getVarEv,"axG",@progbits,_ZNK1A6getVarEv,comdat
	.align 2
	.weak	_ZNK1A6getVarEv
	.type	_ZNK1A6getVarEv, @function
_ZNK1A6getVarEv:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	sete	%al
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	_ZNK1A6getVarEv, .-_ZNK1A6getVarEv
	.section	.rodata
.LC0:
	.string	"Printf MACRO1"
	.text
	.globl	_Z10macrofunc1v
	.type	_Z10macrofunc1v, @function
_Z10macrofunc1v:
.LFB19:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC0
	call	puts
	addl	$16, %esp
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE19:
	.size	_Z10macrofunc1v, .-_Z10macrofunc1v
	.section	.rodata
.LC1:
	.string	"##name_str\r"
	.text
	.type	_ZL5func1v, @function
_ZL5func1v:
.LFB20:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC1
	call	puts
	addl	$16, %esp
	movl	$1, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE20:
	.size	_ZL5func1v, .-_ZL5func1v
	.type	_ZL5func2v, @function
_ZL5func2v:
.LFB21:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC1
	call	puts
	addl	$16, %esp
	movl	$1, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE21:
	.size	_ZL5func2v, .-_ZL5func2v
	.type	_ZL5func3v, @function
_ZL5func3v:
.LFB22:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC1
	call	puts
	addl	$16, %esp
	movl	$1, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE22:
	.size	_ZL5func3v, .-_ZL5func3v
	.section	.rodata
.LC2:
	.string	"a->getVar() = %d\n\r"
.LC3:
	.string	"b = %d\n\r"
.LC4:
	.string	"HELLO IS NOT DEFINED"
.LC5:
	.string	"SSE_NOT_DEFINED"
	.text
	.globl	main
	.type	main, @function
main:
.LFB23:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x78,0x6
	.cfi_escape 0x10,0x3,0x2,0x75,0x7c
	subl	$16, %esp
	subl	$12, %esp
	pushl	$4
	call	_Znwj
	addl	$16, %esp
	movl	%eax, %ebx
	subl	$12, %esp
	pushl	%ebx
	call	_ZN1AC1Ev
	addl	$16, %esp
	movl	%ebx, -12(%ebp)
	subl	$12, %esp
	pushl	-12(%ebp)
	call	_ZN1A5helloEv
	addl	$16, %esp
	call	_ZL5func1v
	call	_ZL5func2v
	call	_ZL5func3v
	subl	$12, %esp
	pushl	-12(%ebp)
	call	_ZNK1A6getVarEv
	addl	$16, %esp
	testb	%al, %al
	je	.L12
	movl	$1, %eax
	jmp	.L13
.L12:
	movl	$0, %eax
.L13:
	subl	$8, %esp
	pushl	%eax
	pushl	$.LC2
	call	printf
	addl	$16, %esp
	movl	$1, -16(%ebp)
	subl	$8, %esp
	pushl	-16(%ebp)
	pushl	$.LC3
	call	printf
	addl	$16, %esp
	subl	$12, %esp
	pushl	$.LC4
	call	puts
	addl	$16, %esp
	subl	$12, %esp
	pushl	$.LC5
	call	puts
	addl	$16, %esp
	movl	$1, %eax
	leal	-8(%ebp), %esp
	popl	%ecx
	.cfi_restore 1
	.cfi_def_cfa 1, 0
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE23:
	.size	main, .-main
	.globl	_Z7my_funci
	.type	_Z7my_funci, @function
_Z7my_funci:
.LFB24:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	nop
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE24:
	.size	_Z7my_funci, .-_Z7my_funci
	.ident	"GCC: (GNU) 5.3.1 20160406 (Red Hat 5.3.1-6)"
	.section	.note.GNU-stack,"",@progbits
