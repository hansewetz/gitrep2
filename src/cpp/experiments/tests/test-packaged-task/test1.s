	.file	"test1.cc"
	.text
	.p2align 4,,15
	.type	_Z4foo1IyEvRT_.isra.4, @function
_Z4foo1IyEvRT_.isra.4:
.LFB1659:
	.cfi_startproc
	jmp	_Z3barv
	.cfi_endproc
.LFE1659:
	.size	_Z4foo1IyEvRT_.isra.4, .-_Z4foo1IyEvRT_.isra.4
	.p2align 4,,15
	.type	_Z3fooIyEvN5paramIT_E4typeE.isra.5, @function
_Z3fooIyEvN5paramIT_E4typeE.isra.5:
.LFB1660:
	.cfi_startproc
	jmp	_Z3barv
	.cfi_endproc
.LFE1660:
	.size	_Z3fooIyEvN5paramIT_E4typeE.isra.5, .-_Z3fooIyEvN5paramIT_E4typeE.isra.5
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC1:
	.string	"milli time passing integer by reference: "
	.align 8
.LC2:
	.string	"milli time passing integer by value: "
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC4:
	.string	"%faster: "
.LC5:
	.string	"%"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB1480:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movl	$1000000000, %ebx
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	call	_ZNSt6chrono12system_clock3nowEv
	movq	%rax, %rbp
	.p2align 4,,10
	.p2align 3
.L5:
	call	_Z4foo1IyEvRT_.isra.4
	subq	$1, %rbx
	jne	.L5
	call	_ZNSt6chrono12system_clock3nowEv
	subq	%rbp, %rax
	movl	$1000000000, %ebx
	cvtsi2sdq	%rax, %xmm3
	divsd	.LC0(%rip), %xmm3
	movsd	%xmm3, (%rsp)
	call	_ZNSt6chrono12system_clock3nowEv
	movq	%rax, %rbp
	.p2align 4,,10
	.p2align 3
.L7:
	call	_Z3fooIyEvN5paramIT_E4typeE.isra.5
	subq	$1, %rbx
	jne	.L7
	call	_ZNSt6chrono12system_clock3nowEv
	subq	%rbp, %rax
	movl	$.LC1, %esi
	movl	$_ZSt4cerr, %edi
	cvtsi2sdq	%rax, %xmm1
	divsd	.LC0(%rip), %xmm1
	movsd	%xmm1, 8(%rsp)
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movsd	(%rsp), %xmm0
	movq	%rax, %rdi
	call	_ZNSo9_M_insertIdEERSoT_
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	movl	$.LC2, %esi
	movl	$_ZSt4cerr, %edi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movsd	8(%rsp), %xmm1
	movq	%rax, %rdi
	movapd	%xmm1, %xmm0
	call	_ZNSo9_M_insertIdEERSoT_
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	movsd	(%rsp), %xmm2
	movl	$.LC4, %esi
	movsd	8(%rsp), %xmm1
	movl	$_ZSt4cerr, %edi
	movapd	%xmm2, %xmm0
	subsd	%xmm1, %xmm0
	mulsd	.LC3(%rip), %xmm0
	divsd	%xmm2, %xmm0
	movsd	%xmm0, (%rsp)
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movsd	(%rsp), %xmm0
	movq	%rax, %rdi
	call	_ZNSo9_M_insertIdEERSoT_
	movl	$.LC5, %esi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	addq	$24, %rsp
	.cfi_def_cfa_offset 24
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE1480:
	.size	main, .-main
	.p2align 4,,15
	.type	_GLOBAL__sub_I_main, @function
_GLOBAL__sub_I_main:
.LFB1654:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$_ZStL8__ioinit, %edi
	call	_ZNSt8ios_base4InitC1Ev
	movl	$__dso_handle, %edx
	movl	$_ZStL8__ioinit, %esi
	movl	$_ZNSt8ios_base4InitD1Ev, %edi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	__cxa_atexit
	.cfi_endproc
.LFE1654:
	.size	_GLOBAL__sub_I_main, .-_GLOBAL__sub_I_main
	.section	.ctors,"aw",@progbits
	.align 8
	.quad	_GLOBAL__sub_I_main
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	0
	.long	1083129856
	.align 8
.LC3:
	.long	0
	.long	1079574528
	.hidden	__dso_handle
	.ident	"GCC: (GNU) 4.8.0 20121104 (experimental)"
	.section	.note.GNU-stack,"",@progbits
