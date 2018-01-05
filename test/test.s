	.file	"test.cpp"
	.text
	.align 2
	.globl	_ZN1A5helloEv
	.type	_ZN1A5helloEv, @function
_ZN1A5helloEv:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	call	foo
	movl	$123, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	_ZN1A5helloEv, .-_ZN1A5helloEv
	.ident	"GCC: (GNU) 5.3.1 20160406 (Red Hat 5.3.1-6)"
	.section	.note.GNU-stack,"",@progbits
