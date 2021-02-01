	.file	"test.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	__modsi3
	.align	2
	.globl	itoa
	.type	itoa, @function
itoa:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	li	a5,48
	sb	a5,-21(s0)
	sw	zero,-20(s0)
.L4:
	lw	a5,-36(s0)
	li	a1,10
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	sw	a5,-28(s0)
	lw	a5,-28(s0)
	andi	a3,a5,0xff
	lw	a5,-20(s0)
	lw	a4,-40(s0)
	add	a5,a4,a5
	lbu	a4,-21(s0)
	add	a4,a3,a4
	andi	a4,a4,0xff
	sb	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
	lw	a4,-20(s0)
	lw	a5,-44(s0)
	bgt	a4,a5,.L5
	lw	a4,-28(s0)
	li	a5,1
	beq	a4,a5,.L4
	lw	a5,-28(s0)
	beq	a5,zero,.L4
	j	.L6
.L5:
	nop
.L6:
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	itoa, .-itoa
	.align	2
	.globl	print
	.type	print, @function
print:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	li	a5,-559038464
	addi	a5,a5,-273
	sw	a5,-24(s0)
	lw	a5,-36(s0)
	sw	a5,-20(s0)
	j	.L8
.L9:
	lw	a5,-20(s0)
	lbu	a4,0(a5)
	lw	a5,-24(s0)
	sb	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L8:
	lw	a5,-20(s0)
	lbu	a5,0(a5)
	bne	a5,zero,.L9
	nop
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	print, .-print
	.align	2
	.globl	print_fibbo
	.type	print_fibbo, @function
print_fibbo:
	addi	sp,sp,-80
	sw	ra,76(sp)
	sw	s0,72(sp)
	addi	s0,sp,80
	sw	a0,-68(s0)
	li	a5,1
	sw	a5,-20(s0)
	sw	zero,-24(s0)
	sw	zero,-28(s0)
	j	.L12
.L13:
	lw	a5,-20(s0)
	sw	a5,-32(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-32(s0)
	sw	a5,-24(s0)
	addi	a5,s0,-52
	li	a2,10
	mv	a1,a5
	lw	a0,-24(s0)
	call	itoa
	addi	a5,s0,-52
	mv	a0,a5
	call	print
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L12:
	lw	a4,-28(s0)
	lw	a5,-68(s0)
	ble	a4,a5,.L13
	nop
	nop
	lw	ra,76(sp)
	lw	s0,72(sp)
	addi	sp,sp,80
	jr	ra
	.size	print_fibbo, .-print_fibbo
	.section	.rodata
	.align	2
.LC0:
	.string	"Hello RISCV!\n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	lui	a5,%hi(.LC0)
	addi	a5,a5,%lo(.LC0)
	sw	a5,-20(s0)
	lw	a0,-20(s0)
	call	print
	li	a5,65536
	addi	a5,a5,-1
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 10.2.0"
