#Test for the jal instruction



.global _start
.section .text

_start:

    jal print
    
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

.L1:

    jal .L2

    nop
    nop
    nop
    nop
    nop
    nop
    nop

.L2:
    jal .L1

