

# The actual code
.section .text
.global _get_cycles
.type _get_cycles, @function              #<-Important

#return uint64_t
_get_cycles:
    csrr a0, minstret
    csrr a1, minstreth
    ret
