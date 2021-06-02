//Author: Benjamin Herrera Navarro
//Date: 10/27/2020 10:33PM
#include "core.h"
#include "../bus/bus.h"
#include "inset.h"
#include <bitset>
#include <iostream>
#include <algorithm>
#include <ios>
#include <stdlib.h>
#include "../debug.h"
#include "../terminal_colors.h"
#include <string.h>

//#define __DEBUG__

CORE::CORE(uint32_t startup_vector, BUS* bus, TIMER* timer)
{
    this->PC = startup_vector;
    this->bus = bus;
    this->timer = timer;
    this->symbol_table = nullptr;
    running = true;
    for(int i = 0;i <= 31;i++)
    {
        REGS[i] = 0x00000000;
    }
    //Machine mode
    mode = 3;

    //Read only CSRs setup
    CSR[0xF11] = 0xdeadbeef; //MRO mvendorid Vendor ID.
    CSR[0xF12] = 0xdeadbee2; //MRO marchid Architecture ID.
    CSR[0xF13] = 0xdeadbee3; //MRO mimpid Implementation ID.
    CSR[0xF14] = 0x00000000; //MRO mhartid Hardware thread ID.

    CSR[0x300] = 0x00000000; // MRW mstatus Machine status register.
    CSR[0x301] = 0b01000000000000000000000100000000; // MRW misa ISA and extensions
    CSR[0x302] = 0x00000000; // MRW medeleg Machine exception delegation register.
    CSR[0x303] = 0x00000000; // MRW mideleg Machine interrupt delegation register.
    CSR[0x304] = 0x00000000; // MRW mie Machine interrupt-enable register.
    CSR[0x305] = 0x00000000; // MRW mtvec Machine trap-handler base address.
    CSR[0x306] = 0x00000000; // MRW mcounteren Machine counter enable.

//0x323 MRW mhpmevent3 Machine performance-monitoring event selector.
//0x324 MRW mhpmevent4 Machine performance-monitoring event selector.
//                                  .
//                                  .
//                                  .
//0x33F MRW mhpmevent31 Machine performance-monitoring event selector.

    //Set event counters to 0
    for(int i = 0x323; i <= 0x33F;i++)
    {
        CSR[i] = 0x00000000;
    }
}

void CORE::event(int32_t event)
{
    //Increment all counters with the selected event
    for(int i = 0x323; i <= 0x33F;i++)
    {
        if(((CSR[0x320] >> (i - 0x320)) & 0x1)!= 1)
        {
            uint64_t temp_var = (CSR[(i-0x323)+0xB03] | ((uint64_t)CSR[(i-0x323)+0xB83] << 32));
            switch(CSR[i])
            {
                case EVENT_LOAD: temp_var++;
                case EVENT_STORE: temp_var++;
                case EVENT_UNLGND_ACCS: temp_var++;
                case EVENT_ALU_INS: temp_var++;
                case EVENT_TRAP: temp_var++;
                case EVENT_INTERRUPT: temp_var++;
                case EVENT_UBRANCH: temp_var++;
                case EVENT_CBRANCH: temp_var++;
                case EVENT_BRANCHS: temp_var++;
            }
            CSR[(i-0x323)+0xB03] = (temp_var & 0xffffffff);
            CSR[(i-0x323)+0xB83] = ((temp_var >> 32) & 0xffffffff);
        }
    }
}

void CORE::print_status()
{   
    std::cout << "MSTATUS: " << std::bitset<32>(CSR[CSR_MSTATUS]) << std::endl;
    int field_size [] = {1, 8, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::string mstatus_field_names [] = 
    {
        "SD", "WPRI", "TSR", "TW", "TVM", "MXR", "SUM",
        "MPRV", "XS[1:0]", "FS[1:0]", "MPP[1:0]", "WPRI",
        "SPP", "MPIE", "WPRI", "SPIE", "UPIE", "MIE", "WPRI", "SIE" ,"UIE"
    };

    std::cout << "|";
    for(auto field_name : mstatus_field_names)
    {
        std::cout << field_name;
        std::cout << "|";
    }
    std::cout << std::endl;
    int shamt = 32;
    std::cout << "|";
    bool unaligned = false;
    for(int i = 0;i < (sizeof(field_size)/sizeof(int));i++)
    {
        //std::cout << " " << shamt << " ";
        if(field_size[i] == 1)
        {
            if(unaligned)
            {
                shamt -= 1;
            }
            std::cout << std::bitset<1>((CSR[CSR_MSTATUS] >> shamt) & 0b1);
            for(int x = 0; x < mstatus_field_names[i].length()-1;x++) std::cout << " ";
            if(!unaligned)
            {
                shamt -= 1;
            }
            std::cout << "|";
        }
        else if(field_size[i] == 2)
        {
            unaligned = true;
            shamt -= 2;
            std::cout << std::bitset<2>((CSR[CSR_MSTATUS] >> shamt) & 0b11);
            for(int x = 0; x < mstatus_field_names[i].length()-2;x++) std::cout << " ";
            std::cout << "|";
        }
        else
        {
            unaligned = true;
            shamt -= (field_size[i]-1);
            printf("%x", CSR[CSR_MSTATUS >> shamt] & 0b11);
            shamt -= 1;
            for(int x = 0; x < mstatus_field_names[i].length()-1;x++) std::cout << " ";
            std::cout << "|";
        }
        
    }
    std::cout << std::endl;
}

void CORE::print_regs()
{
    std::string registers [] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "fp", "s1", "a0",
     "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
    for(int i = 0;i < 8;i++)
    {
        ////std::cout << "REG -> " << registers[i] << " 0x" << std::hex << REGS[i] << " 0b" << std::bitset<32>(REGS[i]) << std::endl;
        printf("r%02d: 0x%08x\tr%02d: 0x%08x\tr%02d: 0x%08x\tr%02d: 0x%08x\n", i, REGS[i], i+8, REGS[i+8], i+16, REGS[i+16], i+24, REGS[i+24]);
    }
    printf("PC: 0x%08x\n", PC);
    ////std::cout << "REG -> " << "PC" << " 0x" << std::hex << PC << " 0b" << std::bitset<32>(20) << std::endl;
}
void CORE::print_ccr()
{


}

uint32_t CORE::get_reg(int i)
{
    return REGS[i];
}


void CORE::trace(uint32_t data_fetch)
{
    int rd = RD_DECODE(data_fetch);
    int rs1 = RS1_DECODE(data_fetch);    
    int rs2 = RS2_DECODE(data_fetch);
    int zero = 0;
    int imm20 = ((IMM20_DECODE(data_fetch) << 12) & 0xfffff000);
    int ujimm = (int32_t)SIGNEX(UJ_IMM_DECODE(data_fetch), 20);
    int jimm12 = (int32_t)(SIGNEX(IMM12_DECODE(data_fetch), 11));
    int bimm = (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
    int imm12 = (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 7);
    int simm12 = (int32_t)SIGNEX(S_IMM12_DECODE(data_fetch), 11);
    int shamt = SHAMNT_DECODE(data_fetch);

    int* trace_args [][3] =
    {
        {&rd, &imm20, &imm20},
        {&rd, &imm20, &imm20},
        {&rd, &ujimm, &ujimm},
        {&rd, &rs1, &jimm12},
        {&rs1, &rs2, &bimm},
        {&rs1, &rs2, &bimm},
        {&rs1, &rs2, &bimm},
        {&rs1, &rs2, &bimm},
        {&rs1, &rs2, &bimm},
        {&rs1, &rs2, &bimm},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &simm12},
        {&rd, &rs1, &simm12},
        {&rd, &rs1, &simm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &shamt},
        {&rd, &rs1, &shamt},
        {&rd, &rs1, &shamt},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&zero, &zero, &zero},
        {&zero, &zero, &zero},
        {&zero, &zero, &zero},
        //Zicsr
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        {&rd, &rs1, &imm12},
        //M
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        {&rd, &rs1, &rs2},
        //Priv
        {&zero, &zero, &zero}
    };

    std::string opcode_table [][2] =
    {
        {"xxxxxxxxxxxxxxxxxxxxxxxxx0110111", "LUI rd:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxxxxxxxxxx0010111", "AUIPC rd:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxxxxxxxxxx1101111", "JAL rd:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx000xxxxx1100111", "JALR rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx000xxxxx1100011", "BEQ rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx001xxxxx1100011", "BNE rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx100xxxxx1100011", "BLT rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx101xxxxx1100011", "BGE rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx110xxxxx1100011", "BLTU rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx111xxxxx1100011", "BGEU rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx000xxxxx0000011", "LB rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx001xxxxx0000011", "LH rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx010xxxxx0000011", "LW rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx100xxxxx0000011", "LBU rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx101xxxxx0000011", "LHU rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx000xxxxx0100011", "SB rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx001xxxxx0100011", "SH rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx010xxxxx0100011", "SW rs1:%d, rs2:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx000xxxxx0010011", "ADDI rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx010xxxxx0010011", "SLTI rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx011xxxxx0010011", "SLTIU rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx100xxxxx0010011", "XORI rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx110xxxxx0010011", "ORI rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx111xxxxx0010011", "ANDI rd:%d, rs1:%d, imm:%d\n"},
        {"0000000xxxxxxxxxx001xxxxx0010011", "SLLI rd:%d, rs1:%d, shamt:%d\n"},
        {"0000000xxxxxxxxxx101xxxxx0010011", "SRLI rd:%d, rs1:%d, shamt:%d\n"},
        {"0100000xxxxxxxxxx101xxxxx0010011", "SRAI rd:%d, rs1:%d, shamt:%d\n"},
        {"0000000xxxxxxxxxx000xxxxx0110011", "ADD rd:%d, rs1:%d, rs2:%d\n"},
        {"0100000xxxxxxxxxx000xxxxx0110011", "SUB rd:%d, rs1:%d, rs2:%d\n"},
        {"0000000xxxxxxxxxx001xxxxx0110011", "SLL rd:%d, rs1:%d, rs2:%d\n"},
        {"0000000xxxxxxxxxx010xxxxx0110011", "SLT rd:%d, rs1:%d, rs2:%d\n"},
        {"0000000xxxxxxxxxx011xxxxx0110011", "SLTU rd:%d, rs1:%d, rs2:%d\n"},
        {"0000000xxxxxxxxxx100xxxxx0110011", "XOR rd:%d, rs1:%d, rs2:%d\n"},
        {"0000000xxxxxxxxxx101xxxxx0110011", "SRL rd:%d, rs1:%d, rs2:%d\n"},        
        {"0100000xxxxxxxxxx101xxxxx0110011", "SRA rd:%d, rs1:%d, rs2:%d\n"},
        {"0000000xxxxxxxxxx110xxxxx0110011", "OR rd:%d, rs1:%d, rs2:%d\n"},
        {"xxxxxxxxxxxxxxxxx000xxxxx0001111", "FENCE\n"},
        {"0000000xxxxxxxxxx111xxxxx0110011", "AND rd:%d, rs1:%d, rs2:%d\n"},
        {"00000000000000000000000001110011", "ECALL\n"},
        {"00000000000100000000000001110011", "EBREAK\n"},
        //Zicsr
        {"xxxxxxxxxxxxxxxxx001xxxxx1110011", "CSRRW rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx010xxxxx1110011", "CSRRS rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx011xxxxx1110011", "CSRRC rd:%d, rs1:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx101xxxxx1110011", "CSRRWI rd:%d, uimm:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx110xxxxx1110011", "CSRRSI rd:%d, uimm:%d, imm:%d\n"},
        {"xxxxxxxxxxxxxxxxx111xxxxx1110011", "CSRRCI rd:%d, uimm:%d, imm:%d\n"},
        //M Extension
        {"0000001xxxxxxxxxx000xxxxx0110011", "MUL rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx001xxxxx0110011", "MULH rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx010xxxxx0110011", "MULHSU rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx011xxxxx0110011", "MULHU rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx100xxxxx0110011", "DIV rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx101xxxxx0110011", "DIVU rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx110xxxxx0110011", "REM rd:%d, rs1:%d, rs2:%d\n"},
        {"0000001xxxxxxxxxx111xxxxx0110011", "REMU rd:%d, rs1:%d, rs2:%d\n"},

        //Priv
        {"00110000001000000000000001110011", "MRET\n"}

    };
    std::string data_fetch_bitset = std::bitset<32>(data_fetch).to_string();

    //Compare
    bool valid_opcode = false;
    for(int i = 0; i < (sizeof(opcode_table)/sizeof(opcode_table[0]));i++)
    {
        valid_opcode = true;
        //std::cout << "Testing: " << data_fetch_bitset << " Against: " << opcode_table[i][0] << std::endl;
        for(int x = 0;x < opcode_table[i][0].length();x++)
        {
            if(((opcode_table[i][0])[x] != 'x') && ((opcode_table[i][0])[x] != data_fetch_bitset[x]))
            {
                //printf("Failed at %d: Exepecting %c, got %c\n", x, (opcode_table[i][0])[x], data_fetch_bitset[x]);
                //printf("%c", (opcode_table[i][0])[x]);
                valid_opcode = false;
                break;
            }
        }
        if(valid_opcode == true)
        {
            //std::cout << opcode_table[i][1] << std::endl;
            std::string format;
            format.append("0x%08x: ");
            format.append((opcode_table[i][1]));
            char buffer [50];
            sprintf(buffer, format.c_str(), PC, *trace_args[i][0], *trace_args[i][1], *trace_args[i][2]);
            printf(buffer);
            str_trace_buffer.push_back(buffer);
            //while(true);
            break;
        }
    }
}

void CORE::print_tracing()
{
    for (std::vector<std::string>::iterator it = str_trace_buffer.begin(); it < str_trace_buffer.end(); it++)
    {
        std::cout << *it;
    }
}

uint32_t CORE::execute_trap(uint32_t mcause, uint32_t mtval)
{
    //Setup trap handling registers
    CSR[0x341] = PC; //Current PC
    CSR[0x342] = mcause;
    CSR[0x343] = mtval;



    //REMEMBER! mode = xPP, if on s_mode use sPP  <========
    
    //DELEG REGISTERS NOT SUPPORTED YET
    //Set MPP to mode
    CSR[CSR_MSTATUS] = (CSR[CSR_MSTATUS] | (mode << 11)); //Current mode
    //Set Machine mode as current mode
    mode = M_MODE;


    //Save older IE mode because traps need interrupts to be disabled
    switch (mode)
    {
    case M_MODE:
        ////Set xPIE to xIE
        CSR[CSR_MSTATUS] |= ((CSR[CSR_MSTATUS] & 0b1000) << 4);
        //Set xIE to 0
        CSR[CSR_MSTATUS] &= ~((uint32_t)0b1000);
        break;
    
    default:
        break;
    }

    
    //Jump to desired address
    //Jump to trap handler
    //No interrupt so its not vectored. Direct Jump.
    PC = CSR[0x305] & (~((uint32_t)0b11));
   // print_status();
}

uint32_t CORE::execute_interrupt(uint32_t mcause, uint32_t mtval)
{
    //Setup trap handling registers
    CSR[0x341] = PC; //Current PC
    CSR[0x342] = mcause;
    CSR[0x343] = mtval;

    //printf("mtvec: 0x%08x\n", CSR[0x341]);
    //printf("current PC: 0x%08x\n", PC);

    //REMEMBER! mode = xPP, if on s_mode use sPP  <========
    
    //DELEG REGISTERS NOT SUPPORTED YET
    //Set MPP to mode
    CSR[CSR_MSTATUS] = (CSR[CSR_MSTATUS] | (mode << 11)); //Current mode
    //Set Machine mode as current mode
    mode = M_MODE;


    //Save older IE mode because traps need interrupts to be disabled
    switch (mode)
    {
    case M_MODE:
        ////Set xPIE to xIE
        CSR[CSR_MSTATUS] |= ((CSR[CSR_MSTATUS] & 0b1000) << 4);
        //Set xIE to 0
        CSR[CSR_MSTATUS] &= ~((uint32_t)0b1000);
        //print_status();
        break;
    
    default:
        break;
    }

    
    //Jump to desired address
    //Jump to trap handler
    //No interrupt so its not vectored. Direct Jump.
    //If vectored, use vectored mode
    if((CSR[0x305] & 0b11) == VECTORED_MODE)
    {
        //std::cout << "Vectored mode selected\n\n\n";
        PC = ((CSR[0x305] & (~((uint32_t)0b11))) + (mcause * 4));
        //print_regs();
    }
    else
    {
        PC = CSR[0x305] & (~((uint32_t)0b11));
    }
    
    //print_status();
}

void CORE::attach_interrupt(interrupt_t* sig)
{
    interrupts.push_back(sig);
}

void CORE::execute()
{
    //Single cycle emulator so each call its a clock cycle
    //update CSRs
    uint64_t temp_var;
    //CSR[0xB80] //MRW mcycleh Upper 32 bits of mcycle, RV32I only.
    //MRW mcycle Machine cycle counter.
    temp_var = (CSR[0xB00] | ((uint64_t)CSR[0xB80] << 32));
    if((CSR[0x320] & 0b1) != 1) temp_var++; //mcountinhibit register. Which Controls the increment enable signal.
    CSR[0xB00] = (temp_var & 0xffffffff);
    CSR[0xB80] = ((temp_var >> 32) & 0xffffffff);
    temp_var = 0x00000000;
    temp_var = (CSR[0xB02] | ((uint64_t)CSR[0xB82] << 32)); 
    if((CSR[0x320] & 0b100) != 0b100) temp_var++; //mcountinhibit register. Which Controls the increment enable signal.
    CSR[0xB02] = (temp_var & 0xffffffff);
    CSR[0xB82] = ((temp_var >> 32) & 0xffffffff);
    //MRW minstret Machine instructions-retired counter.
    //MRW minstreth Upper 32 bits of minstret, RV32I only.

    //Setup interrupt pending bits
    //CPU core main timer
    if(timer->interrupt.load(std::memory_order_acq_rel))
    {
        CSR[CSR_MIP] |=  (1 << 7);
    }
    else
    {
        CSR[CSR_MIP] &=  ~(1 << 7);
    }
        
    //Do CSR operations before instruction execution
    //Do interrupts first
    //Interrupts are disabled for lower privilege levels ALWAYS!
    
    //Interrupts are taken if (Pending Interrupt), (Interrupt Enable)
    // and Global interrupt enable for an privilege level. 
    //Take interrupt of machine mode if ther is an interrupt
    //Check timer interrupt for machine mode

    //Global interrupt enabled for machine mode
    if(((CSR[CSR_MSTATUS] >> 3) & 0x1) == 0b1)
    {
        //Timer Interrupt enable bit on mie
        if(((CSR[CSR_MIE] >> 7) & 0x1) == 0x1)
        {
            //Timer interrupt pending bit is enabled
            if(((CSR[CSR_MIP] >> 7) & 0x1) == 0x1)
            {
                //Take timer interrupt
                //print_status();
                execute_interrupt(CAUSE_M_MODE_TIMER_INT, 0);
            }
        }
        else if(((CSR[CSR_MIE] >> 3) & 0x1) == 0x1) //Machine Software interrupt
        {
            //Machine software interrupt pending bit is enabled
            if(((CSR[CSR_MIP] >> 3) & 0x1) == 0x1)
            {
                //Take timer interrupt
                execute_interrupt(CAUSE_M_MODE_SOFT_INT, 0);
            }
        }
    }

    std::ios_base::fmtflags f( std::cout.flags() );
    //std::cout << "PC -> 0x" << std::hex << PC << std::endl;
    uint32_t data_fetch = bus->read32(PC);//(bus->read(PC) | (bus->read(PC + 1) << 8) | (bus->read(PC + 2) << 16) | (bus->read(PC + 3) << 24)); 
    #ifdef __TRACING__
        trace(data_fetch);
    #endif
    ////std::cout << "DATA FETCH: " << std::hex << data_fetch << std::endl;

    switch(OPCODE_DECODE(data_fetch))
    {
        case I_ENCODING:
            ////std::cout << "I ENCODING rd(0x" << std::hex << RD_DECODE(data_fetch) << ") " <<
            //"func3(0x" << std::hex << FUNC3_DECODE(data_fetch) << ") " << 
            //"rs1(0x" << std::hex << RS1_DECODE(data_fetch) << ") " <<
            //std::cout.flags( f );
            switch (FUNC3_DECODE(data_fetch))
            {
            case ADDI_FUNC3:
                /* code */
                //std::cout << "ADDI Executed" << std::endl;
                //std::cout << "{" << (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) << "}" << std::endl;
                REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11);
                PC += 0x4;
                break;
            
            case SLTI_FUNC3:
                //std::cout << "SLTI Executed" << std::endl;
                if((int32_t)REGS[RS1_DECODE(data_fetch)] < (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11))
                {
                    REGS[RD_DECODE(data_fetch)] = 1;
                }
                else
                {
                    REGS[RD_DECODE(data_fetch)] = 0;
                }
                PC += 0x4;
                break;
            case SLTIU_FUNC3:
                //std::cout << "SLTIU Executed" << std::endl;
                if((uint32_t)REGS[RS1_DECODE(data_fetch)] < (uint32_t)SIGNEX(IMM12_DECODE(data_fetch), 11))
                {
                    REGS[RD_DECODE(data_fetch)] = 1;
                }
                else
                {
                    REGS[RD_DECODE(data_fetch)] = 0;
                }
                PC += 0x4;
                break;

            case ANDI_FUNC3:
                /* code */
                //std::cout << "ANDI Executed" << std::endl;
                REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] & (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11);
                PC += 0x4;
                break;

            case ORI_FUNC3:
                /* code */
                //std::cout << "ORI Executed" << std::endl;
                REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] | (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11);
                PC += 0x4;
                break;                

            case XORI_FUNC3:
                /* code */
                //std::cout << "XORI Executed" << std::endl;
                REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] ^ (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11);
                PC += 0x4;
                break;                

            case SLLI_FUNC3:
                //std::cout << "SLLI Executed" << std::endl;
                REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] << SHAMNT_DECODE(data_fetch); 
                PC += 0x4;
                break;
            case SRLI_FUNC3:
                if(IMM7_DECODE(data_fetch) == SRAI_IMM4H)
                {
                    //std::cout << "SRAI Executed" << std::endl;
                    REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] >> (int32_t)SHAMNT_DECODE(data_fetch);
                    //if ((int32_t)REGS[RS1_DECODE(data_fetch)] < 0 && SHAMNT_DECODE(data_fetch) > 0)
                    //{
                    //}
                    //else
                    //{
                    //    REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] >> SHAMNT_DECODE(data_fetch);
                    //}
                }
                else
                {
                    /* code */
                    //std::cout << "SRLI Executed" << std::endl;
                    //printf("shamnt %d\n", SHAMNT_DECODE(data_fetch));
                    //std::cout << "C: " << (REGS[RS1_DECODE(data_fetch)] << SHAMNT_DECODE(data_fetch)) << " A: " << REGS[RS1_DECODE(data_fetch)] << " >> B: "  << SHAMNT_DECODE(data_fetch) << std::endl;
                    REGS[RD_DECODE(data_fetch)] = (unsigned)REGS[RS1_DECODE(data_fetch)] >> SHAMNT_DECODE(data_fetch);
                }
                PC += 0x4;
                break;
            default:
                break;
            }
            break;
        case LUI_OP:
            //std::cout << "LUI Executed" << std::endl;
            REGS[RD_DECODE(data_fetch)] = ((IMM20_DECODE(data_fetch) << 12) & 0xfffff000);
            PC += 0x4;
        break;
        case AUIPC_OP:
            //std::cout << "AUIPC Executed" << std::endl;
            //std::cout << "IMM20 -> " << ((IMM20_DECODE(data_fetch) << 12) & 0xfffff000) << std::endl;
            REGS[RD_DECODE(data_fetch)] = PC + ((IMM20_DECODE(data_fetch) << 12) & 0xfffff000);
            PC += 0x4;
        break;
        case R_ENCODING:
            if(FUNC7_DECODE(data_fetch) == M_EXTENSION)
            {
                switch (FUNC3_DECODE(data_fetch))
                {
                    case MUL:
                            REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] * (int32_t)REGS[RS2_DECODE(data_fetch)];
                    break;
                    case MULH:
                            REGS[RD_DECODE(data_fetch)] = ((int64_t)REGS[RS1_DECODE(data_fetch)] * (int64_t)REGS[RS2_DECODE(data_fetch)]) >> 32;
                    break;
                    case MULHSU:
                            REGS[RD_DECODE(data_fetch)] = ((int64_t)REGS[RS1_DECODE(data_fetch)] * (uint64_t)REGS[RS2_DECODE(data_fetch)]) >> 32;
                    break;
                    case MULHU:
                            REGS[RD_DECODE(data_fetch)] = ((uint64_t)REGS[RS1_DECODE(data_fetch)]) - ((uint64_t)REGS[RS2_DECODE(data_fetch)]);
                    break;
                    case DIV:
                            if(REGS[RS2_DECODE(data_fetch)] == 0)
                            {
                                REGS[RD_DECODE(data_fetch)] = -1;
                            }
                            else
                            {
                                if((REGS[RS2_DECODE(data_fetch)] == -1) && (REGS[RS1_DECODE(data_fetch)] == 0xffffffff))
                                {
                                    REGS[RD_DECODE(data_fetch)] = -4294967297;
                                }
                                else
                                {
                                    REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] / (int32_t)REGS[RS2_DECODE(data_fetch)];
                                }
                            }
                    break;
                    case DIVU:
                            if(REGS[RS2_DECODE(data_fetch)] == 0)
                            {
                                REGS[RD_DECODE(data_fetch)] = 0xffffffff;
                            }
                            else
                            {
                                REGS[RD_DECODE(data_fetch)] = (uint32_t)REGS[RS1_DECODE(data_fetch)] / (uint32_t)REGS[RS2_DECODE(data_fetch)];
                            }
                    break;
                    case REM:
                            if(REGS[RS2_DECODE(data_fetch)] == 0)
                            {
                                REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)];
                            }
                            else
                            {
                                if((REGS[RS2_DECODE(data_fetch)] == -1) && (REGS[RS1_DECODE(data_fetch)] == 0xffffffff))
                                {
                                    REGS[RD_DECODE(data_fetch)] = 0;
                                }
                                else
                                {
                                    REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] % (int32_t)REGS[RS2_DECODE(data_fetch)];
                                }
                            }
                    break;
                    case REMU:
                            if(REGS[RS2_DECODE(data_fetch)] == 0)
                            {
                                REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)];
                            }
                            else
                            {
                                REGS[RD_DECODE(data_fetch)] = (uint32_t)REGS[RS1_DECODE(data_fetch)] % (uint32_t)REGS[RS2_DECODE(data_fetch)];
                            }
                    
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (FUNC3_DECODE(data_fetch))
                {
                    case ADD_SUB_FUNC3:
                        if(FUNC7_DECODE(data_fetch) == SUB_FUNC7)
                        {
                            //std::cout << "SUB Executed" << std::endl;
                            REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] - REGS[RS2_DECODE(data_fetch)];
                        }
                        else
                        {   
                            //std::cout << "ADD Executed" << std::endl;
                            //printf("rs1: 0x%08d, rs2: 0x%08x");
                            REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] + REGS[RS2_DECODE(data_fetch)];
                        }
                        break;
                    case SLL_FUNC3:
                        //std::cout << "SLL Executed" << std::endl;
                        REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] << REGS[RS2_DECODE(data_fetch)];
                        break;
                    case SLT_FUNC3:
                        //std::cout << "SLT Executed" << std::endl;
                        REGS[RD_DECODE(data_fetch)] = ((int32_t)REGS[RS1_DECODE(data_fetch)] < (int32_t)REGS[RS2_DECODE(data_fetch)])? 1 : 0;
                        break;
                    case SLTU_FUNC3:
                        //std::cout << "SLT Executed" << std::endl;
                        if(RS1_DECODE(data_fetch) == 0x00)
                        {
                            REGS[RD_DECODE(data_fetch)]= 0;
                        }
                        else
                        {
                            REGS[RD_DECODE(data_fetch)] = ((uint32_t)REGS[RS1_DECODE(data_fetch)] < (uint32_t)REGS[RS2_DECODE(data_fetch)])? 1 : 0;                    
                        }

                        break;
                    case XOR_FUNC3:
                        //std::cout << "XOR Executed" << std::endl;
                        REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] ^ REGS[RS2_DECODE(data_fetch)];
                        break;
                    case SRL_SRA_FUNC3:
                        if(FUNC7_DECODE(data_fetch) == SRA_FUNC7)
                        {
                            //std::cout << "SRA Executed" << std::endl;
                            if ((int32_t)REGS[RS1_DECODE(data_fetch)] < 0 && (REGS[RS1_DECODE(data_fetch)] & 0x1f) > 0)
                            {
                                REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] >> REGS[RS2_DECODE(data_fetch)] | ~(~0U >> REGS[RS1_DECODE(data_fetch)]);
                            }
                            else
                            {
                                REGS[RD_DECODE(data_fetch)] = (int32_t)REGS[RS1_DECODE(data_fetch)] >> REGS[RS2_DECODE(data_fetch)];
                            }
                        }
                        else
                        {
                            //std::cout << "SRL Executed" << std::endl;
                            REGS[RD_DECODE(data_fetch)] = (uint32_t)REGS[RS1_DECODE(data_fetch)] >> (REGS[RS2_DECODE(data_fetch)] & 0x1f);
                        }
                        break;
                    case OR_FUNC3:
                        //std::cout << "OR Executed" << std::endl;
                        REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] | REGS[RS2_DECODE(data_fetch)];
                        break;
                    case AND_FUNC3:
                        //std::cout << "AND Executed" << std::endl;
                        //print_regs();
                        REGS[RD_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)] & REGS[RS2_DECODE(data_fetch)];
                        break;
                default:
                    break;
                }
            }
            PC += 0x4;
            break;
        case JAL_OP:
            {
                //std::cout << "JAL Executed" << std::endl;
                REGS[RD_DECODE(data_fetch)] = PC + 4;
                //symbol32_t* ref;
                PC += (int32_t)SIGNEX(UJ_IMM_DECODE(data_fetch), 20);
                //std::vector<symbol32_t>::iterator entry = std::find_if(symbol_table->begin(), symbol_table->end(), [&](const symbol32_t& symbol32)->bool{
                //    return (symbol32.value == PC)? true : false; });
                //if(entry != symbol_table->end())
                //{
                //    printf("Function called %s\n", entry->name.c_str());
                //}
            }
            break;
        case JALR_OP:
            {
                //std::cout << "JALR Executed" << std::endl;
                REGS[RD_DECODE(data_fetch)] = PC + 4;
                PC = REGS[RS1_DECODE(data_fetch)] + (int32_t)(SIGNEX(IMM12_DECODE(data_fetch), 11));
                //std::vector<symbol32_t>::iterator entry = std::find_if(symbol_table->begin(), symbol_table->end(), [&](const symbol32_t& symbol32)->bool{
                //    return (symbol32.value == PC)? true : false; });
                //if(entry != symbol_table->end())
                //{
                //    printf("Function called %s\n", entry->name.c_str());
                //}
            }
            break;
        case BRANCH:
            switch (FUNC3_DECODE(data_fetch))
            {
                case BEQ_FUNC3:
                    //std::cout << "BEQ Executed" << std::endl;
                    if((int32_t)REGS[RS1_DECODE(data_fetch)] == (int32_t)REGS[RS2_DECODE(data_fetch)])
                    {
                        PC += (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
                    }
                    else
                    {
                        PC += 4;
                    }
                    
                break;
                case BNE_FUNC3:
                    //std::cout << "BNE Executed" << std::endl;
                    if((int32_t)REGS[RS1_DECODE(data_fetch)] != (int32_t)REGS[RS2_DECODE(data_fetch)])
                    {
                    //std::cout << " A: " << std::bitset<32>((unsigned)REGS[RS1_DECODE(data_fetch)]) << " BNE B: "  << std::bitset<32>((int32_t)REGS[RS2_DECODE(data_fetch)]) << std::endl;

                        PC += (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
                    }
                    else
                    {
                        PC += 4;
                    }
                break;
                case BLT_FUNC3:
                    //std::cout << "BLT Executed" << std::endl;
                    if((int32_t)REGS[RS1_DECODE(data_fetch)] < (int32_t)REGS[RS2_DECODE(data_fetch)])
                    {
                        PC += (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
                    }
                    else
                    {
                        PC += 4;
                    }
                break;
                case BGE_FUNC3:
                    //std::cout << "BGE Executed" << std::endl;
                    if((int32_t)REGS[RS1_DECODE(data_fetch)] >= (int32_t)REGS[RS2_DECODE(data_fetch)])
                    {
                        PC += (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
                    }
                    else
                    {
                        PC += 4;
                    }
                break;
                case BLTU_FUNC3:
                    //std::cout << "BLTU Executed" << std::endl;
                    if((uint32_t)REGS[RS1_DECODE(data_fetch)] < (uint32_t)REGS[RS2_DECODE(data_fetch)])
                    {
                        PC += (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
                    }
                    else
                    {
                        PC += 4;
                    }
                break;
                case BGEU_FUNC3:
                    //std::cout << "BGEU Executed" << std::endl;
                    if((uint32_t)REGS[RS1_DECODE(data_fetch)] >= (uint32_t)REGS[RS2_DECODE(data_fetch)])
                    {
                        PC += (int32_t)SIGNEX(B_IMM12_DECODE(data_fetch), 12);
                    }
                    else
                    {
                        PC += 4;
                    }
                break;
            default:
                break;
            }
        break;    
        case LOAD:
            switch (FUNC3_DECODE(data_fetch))
            {
                case LB_FUNC3:
                {
                    //std::cout << "LB Executed" << std::endl;
                    REGS[RD_DECODE(data_fetch)] = (int32_t)SIGNEX(bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11)), 7);
                }
                break;
                case LH_FUNC3:
                {
                    //std::cout << "LH Executed" << std::endl;
                    uint8_t bytes [2];
                    bytes [0] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x0);
                    bytes [1] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x1);
                    int16_t data_16 = (bytes [0] | (bytes [1] << 8));
                    REGS[RD_DECODE(data_fetch)] = (int32_t)data_16;
                }
                break;
                case LW_FUNC3:
                {
                    //std::cout << "LW Executed" << std::endl;
                    //uint8_t bytes [4];
                    //bytes [0] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x00);
                    //bytes [1] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x01);
                    //bytes [2] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x02);
                    //bytes [3] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x03);
                    //REGS[RD_DECODE(data_fetch)] = (bytes [0] | (bytes [1] << 8) | (bytes [2] << 16) | (bytes [3] << 24));
                    REGS[RD_DECODE(data_fetch)] = bus->read32(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x00);
                    
                    //if((uint32_t)(bytes [0] | (bytes [1] << 8) | (bytes [2] << 16) | (bytes [3] << 24)) != 
                    //bus->read32(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11)))
                    //{
                    //    printf("Core: Read32 failed: Expected 0x%08x, Received 0x%08x\n",
                    //    (uint32_t)(bytes [0] | (bytes [1] << 8) | (bytes [2] << 16) | (bytes [3] << 24)), bus->read32(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11))
                    //    );
                    //    exit(1);
                    //}
                }
                break;
                case LBU_FUNC3:
                {
                    //std::cout << "LBU Executed" << std::endl;
                    REGS[RD_DECODE(data_fetch)] = bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11));
                }
                break;
                case LHU_FUNC3:
                {
                    //std::cout << "LHU Executed" << std::endl;
                    uint32_t bytes [2];
                    bytes [0] = (uint32_t)bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x0);
                    bytes [1] = (uint32_t)bus->read(REGS[RS1_DECODE(data_fetch)] + (int32_t)SIGNEX(IMM12_DECODE(data_fetch), 11) + 0x1);
                    REGS[RD_DECODE(data_fetch)] = (bytes [0] | (bytes [1] << 8)) & 0xffff;
                }
                break;
            default:
                break;
            }
            PC += 0x4;
        break;
        case STORE:
            switch (FUNC3_DECODE(data_fetch))
            {
                case SB_FUNC3:
                    //std::cout << "SB Executed" << std::endl;
                    bus->write((REGS[RS2_DECODE(data_fetch)] & 0xff), REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11));
                break;
                case SH_FUNC3:
                    //std::cout << "SH Executed" << std::endl;
                    bus->write((REGS[RS2_DECODE(data_fetch)] & 0x00ff) >> 0, (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)) + 0x0);
                    bus->write((REGS[RS2_DECODE(data_fetch)] & 0xff00) >> 8, (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)) + 0x1);
                break;
                case SW_FUNC3:
                    //std::cout << "SW Executed" << std::endl;
                    //bus->write((REGS[RS2_DECODE(data_fetch)] & 0x000000ff) >> 0,  (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)) + 0x0);
                    //bus->write((REGS[RS2_DECODE(data_fetch)] & 0x0000ff00) >> 8,  (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)) + 0x1);
                    //bus->write((REGS[RS2_DECODE(data_fetch)] & 0x00ff0000) >> 16, (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)) + 0x2);
                    //bus->write((REGS[RS2_DECODE(data_fetch)] & 0xff000000) >> 24, (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)) + 0x3);
                    bus->write32(REGS[RS2_DECODE(data_fetch)], (REGS[RS1_DECODE(data_fetch)] + SIGNEX(S_IMM12_DECODE(data_fetch), 11)));
                break;
            default:
                break;
            }
            PC += 0x4;
        break;
        //Enviroment Instructions
        case SYSTEM:
            switch (FUNC3_DECODE(data_fetch))
            {
                case PRIV:
                    switch (IMM12_DECODE(data_fetch))
                    {
                        case ECALL:
                            std::cout << "ECALL Executed" << std::endl;
                            print_regs();
                            //Trap to handler
                            if(mode == M_MODE) execute_trap(CAUSE_ENV_CALL_M_MODE, data_fetch);
                            else if(mode == S_MODE) execute_trap(CAUSE_ENV_CALL_S_MODE, data_fetch);
                            else if(mode ==  U_MODE) execute_trap(CAUSE_ENV_CALL_U_MODE, data_fetch);
                            //while(true);
                        break;
                        case EBREAK:
                            std::cout << "EBREAK Executed" << std::endl;
                            print_regs();
                            running = false;
                            
                            //std::cout << "executing ebreak while true loop" << std::endl;
                            //while(true)
                        break;
                        case URET:
                            std::cout << "Error: Illegal Instruction: User node not supported" << std::endl;
                        break;
                        case SRET:
                            std::cout << "Error: Illegal Instruction: Supervisor node not supported" << std::endl;
                        break;
                        case MRET:
                            PC = CSR[0x341];
                            //Set previous privilege level
                            mode = (CSR[CSR_MSTATUS] >> 11) & 0b11;
                            //Set xIE to xPIE
                            CSR[CSR_MSTATUS] |= ((CSR[CSR_MSTATUS] & 0b10000000) >> 4); 
                            //Set xPIE to 1
                            CSR[CSR_MSTATUS] |= 0b10000000;
                            //std::cout << "MRET EXECUTED" << std::endl;
                            //print_status();
                        break;
                        default:
                            std::cout << "Illegal Instruction" << std::endl;
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);

                        break;
                    }
                break;
                case CSRRW:
                    //Check Privilege
                    if(IMM_PRIV(data_fetch) <= mode)
                    {
                        //Check for illegal write to CSR
                        if((IMM_ACCESS(data_fetch) == 0b11)) //If read only and trying to write
                        {
                            std::cout << "Error: Read only register\n";
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                        }
                        else
                        {
                            if(RD_DECODE(data_fetch) != 0)
                            {
                                //Read CSR
                                REGS[RD_DECODE(data_fetch)] = CSR[IMM12_DECODE(data_fetch)];
                            }
                            //Write to CSR
                            CSR[IMM12_DECODE(data_fetch)] = REGS[RS1_DECODE(data_fetch)];
                            //printf("Writting to CSR 0x%08x -> CSR Value now: 0x%08x\n", IMM12_DECODE(data_fetch), CSR[IMM12_DECODE(data_fetch)]);
                            //while(true);
                            PC += 0x4;
                        }

                    }
                    else
                    {
                        std::cout << "Error: Access denied to higher privilege mode CSRRW\n";
                        execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                    }
                    std::cout << "IMM_ACCESS: " << std::bitset<2>(IMM_ACCESS(data_fetch)) << std::endl;
                    std::cout << "IMM: " << std::bitset<12>(IMM12_DECODE(data_fetch)) << std::endl;

                break;
                case CSRRS:
                    if(IMM_PRIV(data_fetch) <= mode)
                    {
                        //Check for illegal write to CSR
                        if((IMM_ACCESS(data_fetch) == 0b11) && (RS1_DECODE(data_fetch) != 0)) //If read only and trying to write
                        {
                            std::cout << "Error: Read only register\n";
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                        }
                        else
                        {
                            //Read CSR
                            REGS[RD_DECODE(data_fetch)] = CSR[IMM12_DECODE(data_fetch)];
                            //Write to CSR
                            CSR[IMM12_DECODE(data_fetch)] = (CSR[IMM12_DECODE(data_fetch)] | REGS[RS1_DECODE(data_fetch)]);
                            PC += 0x4;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Access denied to higher privilege mode\n";
                        execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                    }

                break;
                case CSRRC:
                    if(IMM_PRIV(data_fetch) <= mode)
                    {
                        //Check for illegal write to CSR
                        if((IMM_ACCESS(data_fetch) == 0b11) && (RS1_DECODE(data_fetch) != 0)) //If read only and trying to write
                        {
                            std::cout << "Error: Read only register\n";
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                        }
                        else
                        {
                            //Read CSR
                            REGS[RD_DECODE(data_fetch)] = CSR[IMM12_DECODE(data_fetch)];
                            //Write to CSR
                            CSR[IMM12_DECODE(data_fetch)] = ~REGS[RS1_DECODE(data_fetch)] & CSR[IMM12_DECODE(data_fetch)];
                            PC += 0x4;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Access denied to higher privilege mode\n";
                        //Trap to handler
                        execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                    }
                break;
                case CSRRWI:
                    //Check Privilege
                    if(IMM_PRIV(data_fetch) <= mode)
                    {
                        //Check for illegal write to CSR
                        if((IMM_ACCESS(data_fetch) == 0b11)) //If read only and trying to write
                        {
                            std::cout << "Error: Read only register\n";
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                        }
                        else
                        {
                            if(RD_DECODE(data_fetch) != 0)
                            {
                                //Read CSR
                                REGS[RD_DECODE(data_fetch)] = CSR[IMM12_DECODE(data_fetch)];
                            }
                            //Write to CSR
                            CSR[IMM12_DECODE(data_fetch)] = RS1_DECODE(data_fetch);
                            PC += 4;
                        }

                    }
                    else
                    {
                        std::cout << "Error: Access denied to higher privilege mode CSRRW\n";
                        //Trap to handler
                        execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                    }
                    std::cout << "IMM_ACCESS: " << std::bitset<2>(IMM_ACCESS(data_fetch)) << std::endl;
                    std::cout << "IMM: " << std::bitset<12>(IMM12_DECODE(data_fetch)) << std::endl;

                break;
                case CSRRSI:
                    if(IMM_PRIV(data_fetch) <= mode)
                    {
                        //Check for illegal write to CSR
                        if((IMM_ACCESS(data_fetch) == 0b11) && (RS1_DECODE(data_fetch) != 0)) //If read only and trying to write
                        {
                            std::cout << "Error: Read only register\n";
                            //Trap to handler
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                        }
                        else
                        {
                            //Read CSR
                            REGS[RD_DECODE(data_fetch)] = CSR[IMM12_DECODE(data_fetch)];
                            //Write to CSR
                            CSR[IMM12_DECODE(data_fetch)] = (CSR[IMM12_DECODE(data_fetch)] | RS1_DECODE(data_fetch));
                            PC += 4;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Access denied to higher privilege mode\n";
                        //Trap to handler
                        execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                    }

                break;
                case CSRRCI:
                    if(IMM_PRIV(data_fetch) <= mode)
                    {
                        //Check for illegal write to CSR
                        if((IMM_ACCESS(data_fetch) == 0b11) && (RS1_DECODE(data_fetch) != 0)) //If read only and trying to write
                        {
                            std::cout << "Error: Read only register\n";
                            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                        }
                        else
                        {
                            //Read CSR
                            REGS[RD_DECODE(data_fetch)] = CSR[IMM12_DECODE(data_fetch)];
                            //Write to CSR
                            CSR[IMM12_DECODE(data_fetch)] = (~RS1_DECODE(data_fetch) & CSR[IMM12_DECODE(data_fetch)]);
                            PC += 0x4;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Access denied to higher privilege mode\n";
                        execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
                    }

                break;
                default:

                    break;
            }
            //Basically ignore fense
            case 0b0001111:
                PC += 0x4;
            break;
        break;
        default:
            printf("Illegal Instruction 0x%08x at: 0x%08x\n", data_fetch, PC);
            //Trap to handler
            execute_trap(CAUSE_ILLEGAL_INS, data_fetch);
            //std::cout << "PC set to: " << PC << std::endl;
            //while(true);
        break;
    }
    REGS[0] = 0;
    if(data_fetch == 0)
    {
        running = false;
    }
    #ifdef __DEBUG__
    #endif
    //print_regs();
    ////std::cout << "DATA FETCH: " << std::bitset<16>(data_fetch) << std::endl;
    //Read from memory, decode and fetch extra bits
}

uint8_t CORE::get_ins_opcode(uint8_t data)
{
    return 0x000;
}

void CORE::attach_debug_symbols(std::vector<symbol32_t>* symbol_table)
{
    this->symbol_table = symbol_table;
}