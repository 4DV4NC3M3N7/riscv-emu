/*
Author: Benjamin Herrera Navarro.
Date: 10/27/2020
10:32PM
*/
#ifndef __CORE_H__
#define __CORE_H__


#include <stdint.h>
#include <vector>
#include <atomic>
#include "../device/device.h"
#include "../memory/memory.h"
#include "../bus/bus.h"
#include "inset.h"
#include "../timer/timer.h"
#include "interrupt.h"
#include "../elf/elf.h"

#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

/*
Registers
x0 / zero
x1 - x31

Register ABI Name Description Saver
x0 zero Hard-wired zero —
x1 ra Return address Caller
x2 sp Stack pointer Callee
x3 gp Global pointer —
x4 tp Thread pointer —
x5 t0 Temporary/alternate link register Caller
x6–7 t1–2 Temporaries Caller
x8 s0/fp Saved register/frame pointer Callee
x9 s1 Saved register Callee
x10–11 a0–1 Function arguments/return values Caller
x12–17 a2–7 Function arguments Caller
x18–27 s2–11 Saved registers Callee
x28–31 t3–6 Temporaries Caller
f0–7 ft0–7 FP temporaries Caller
f8–9 fs0–1 FP saved registers Callee
f10–11 fa0–1 FP arguments/return values Caller
f12–17 fa2–7 FP arguments Caller
f18–27 fs2–11 FP saved registers Callee
f28–31 ft8–11 FP temporaries Caller

*/

typedef struct
{
    signed short int data : 12;
} int12_t;

//For tracing, u know...
typedef struct
{
    uint32_t rs1, rs2, rd;
    uint32_t opcode;
    std::string name;
} ins_trace_t;


typedef struct
{
    bool call_trace;
    bool stack_trace;
    bool exec_trace;
    bool device_access;
    bool fast_exec_trace;
} debug_options_t;

class CORE
{
    public:
    CORE(uint32_t startup_vector, BUS* bus, TIMER* timer);

    void execute();
    
    //return program counter and modyfies program counter internally
    uint32_t execute_trap(uint32_t mcause, uint32_t mtval);
    uint32_t execute_interrupt(uint32_t mcause, uint32_t mtval);

    void print_ccr();
    void print_regs();
    uint32_t get_reg(int);

    void attach_debug_symbols(std::vector<symbol32_t>* symbol_table);
    void debug_options(debug_options_t debug_options);
    
    void pmp_check(debug_options_t debug_options);

    uint8_t get_ins_opcode(uint8_t data);
    bool running;

    void event(int32_t event);
    void print_tracing();
    void trace(uint32_t data_fetch);

    void attach_interrupt(interrupt_t* sig);

    void print_status();

    private:
    BUS* bus;

    uint32_t REGS[32];
    uint32_t PC;

    //CSRs
    uint32_t CSR [4096];
    uint8_t mode;

    //Interrupts
    std::vector<interrupt_t*> interrupts;

    TIMER* timer;

    //Core tracing
    std::vector <ins_trace_t> trace_buffer;
    std::vector <std::string> str_trace_buffer;
    std::vector<symbol32_t>* symbol_table;
};



#endif