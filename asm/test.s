.section .init, "ax"
.global _start
_start:
    addi sp, sp, 0x8
    auipc gp,0x14
    lui	a5,0xdeadc