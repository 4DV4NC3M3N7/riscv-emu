

# The actual code
.section .text
.global _set_time_int
.type _set_time_int, @function              #<-Important

.global _get_time
.type _get_time, @function              #<-Important

.equ time, 0x10000010
.equ timecmp, 0x10000018

#Takes argument int n_cycles
_set_time_int:
    addi sp, sp, -8
    sw t0, 0(sp)
    sw t1, 4(sp)

    #load current time
    la t0, time
    lw t1, 0(t0)
    #offset loaded time by atgument
    add t1, t1, a0 
    #set timecmp
    la t0, timecmp
    sw t1, 0(t0)
    
    lw t0, 0(sp)
    lw t1, 4(sp)
    addi sp, sp, 8

    ret

_get_time:
    #push t0 into stack
    addi sp, sp, -4
    sw t0, 0(sp)

    #load time into argument registers
    la t0, time
    lw a0, 0(t0)
    lw a1, 4(t0)

    #pop t0 into stack
    lw t0, 0(sp)
    addi sp, sp, 4
    
    ret