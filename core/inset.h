#ifndef __INSET_H__
#define __INSET_H__

/*
imm[11:0] rs1 funct3 rd opcode I-type
*/

#define I_ENCODING      0b0010011
#define R_ENCODING      0b0110011

#define LUI_OP          0b0110111
#define AUIPC_OP        0b0010111

#define SUB_FUNC7       0b0100000
#define SRA_FUNC7       0b0100000

#define JAL_OP          0b1101111
#define JALR_OP         0b1100111

#define ADD_SUB_FUNC3   0b000
#define SLL_FUNC3       0b001
#define SLT_FUNC3       0b010
#define SLTU_FUNC3      0b011
#define XOR_FUNC3       0b100
#define SRL_SRA_FUNC3   0b101
#define OR_FUNC3        0b110
#define AND_FUNC3       0b111


#define BRANCH          0b1100011

#define BEQ_FUNC3       0b000
#define BNE_FUNC3       0b001
#define BLT_FUNC3       0b100
#define BGE_FUNC3       0b101
#define BLTU_FUNC3      0b110
#define BGEU_FUNC3      0b111

#define LOAD            0b0000011

#define LB_FUNC3        0b000 
#define LH_FUNC3        0b001 
#define LW_FUNC3        0b010 
#define LBU_FUNC3       0b100 
#define LHU_FUNC3       0b101 

#define STORE           0b0100011

#define SB_FUNC3        0b000 
#define SH_FUNC3        0b001 
#define SW_FUNC3        0b010 

#define OPCODE_DECODE(data) (data & 0x7f)
#define RD_DECODE(data) ((data >> 0x7) & 0x1f)
#define FUNC3_DECODE(data) ((data >> 12) & 0b111)
#define RS1_DECODE(data) ((data >> 0xf) & 0x1f)
#define IMM12_DECODE(data) ((data >> 20) & 0xfff)
#define SHAMNT_DECODE(data) ((data >> 0x14) & 0x1f)
#define IMM7_DECODE(data) ((data >> 0x19) & 0x7f)
#define IMM20_DECODE(data) ((data >> 0xc) & 0xfffff)
#define RS2_DECODE(data) ((data >> 0x14) & 0x1f)
#define FUNC7_DECODE(data) ((data >> 0x19) & 0x7f)
#define UJ_IMM_DECODE(data) (((data >> 11) & 0x100000) | ((data >> 20) & 0x7fe) | ((data >> 9) & 0x800) | (data & 0xff000))
//(((data & 0x80000000) >> 11) | ((data & 0x7FE00000) >> 20) | ((data & 0x100000) >> 9) | (data & 0xff000))
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define B_IMM12_DECODE(data) ((data << 4) & 0x800) | ((data >> 7) & 0x1e) | ((data >> 19) & 0x1000) | ((data >> 20) & 0x7e0)

#define S_IMM12_DECODE(data) (((data >> 7) & 0x1f) | ((data >> 20) & 0xFE0))

#define ADDI_FUNC3          0b000
#define SLTI_FUNC3          0b010
#define SLTIU_FUNC3         0b011
#define XORI_FUNC3          0b100
#define ORI_FUNC3           0b110
#define ANDI_FUNC3          0b111

#define SLLI_FUNC3          0b001
#define SRLI_FUNC3          0b101
#define SRAI_IMM4H          0b0100000

#define SYSTEM              0b1110011

#define PRIV                0b000

#define ECALL               0b000000000000
#define EBREAK              0b000000000001

#define CSRRW               0b001
#define CSRRS               0b010
#define CSRRC               0b011
#define CSRRWI              0b101
#define CSRRSI              0b110
#define CSRRCI              0b111

#define URET                0b000000000010
#define SRET                0b000100000010
#define MRET                0b001100000010

#define M_MODE              3
#define S_MODE              1
#define U_MODE              0

#define IMM_PRIV(data)      ((((data >> 20) & 0xfff) >> 8) & 0b11)
#define IMM_ACCESS(data)    ((((data >> 20) & 0xfff) >> 10) & 0b11)

#define EVENT_LOAD          1
#define EVENT_STORE         2
#define EVENT_UNLGND_ACCS   3
#define EVENT_ALU_INS       4
#define EVENT_TRAP          5
#define EVENT_INTERRUPT     6
#define EVENT_UBRANCH       7
#define EVENT_CBRANCH       8
#define EVENT_BRANCHS       9

#define CAUSE_INS_ADDRESS_MISALIGNED                0
#define CAUSE_INS_ACCESS_FAULT                      1
#define CAUSE_ILLEGAL_INS                           2
#define CAUSE_BREAKPOINT4_LOADADDRESS_MISALIGNED    3
#define CAUSE_LOADACCESS_FAULT                      5
#define CAUSE_STORE_AMO_ADDRESS_MISALIGNED          6
#define CAUSE_STORE_AMO_ACCESS_FAULT                7
#define CAUSE_ENV_CALL_U_MODE                       8
#define CAUSE_ENV_CALL_S_MODE                       9
#define CAUSE_ENV_CALL_M_MODE                       11
#define CAUSE_INSTRUCTIONPAGE_FAULT                 12
#define CAUSE_LOADPAGE_FAULT                        13
#define CAUSE_STORE_AMO_PAGE_FAULT                  15

#define CAUSE_U_MODE_SOFT_INT                       0
#define CAUSE_S_MODE_SOFT_INT                       1
#define CAUSE_M_MODE_SOFT_INT                       3
#define CAUSE_U_MODE_TIMER_INT                      4
#define CAUSE_S_MODE_TIMER_INT                      5
#define CAUSE_M_MODE_TIMER_INT                      7
#define CAUSE_U_MODE_EXTERNAL_INT                   8
#define CAUSE_S_MODE_EXTERNAL_INT                   9
#define CAUSE_M_MODE_EXTERNAL_INT                   11

#define SET_CAUSE_INTERRUPT(cause) ((1 << 32) | cause)

//CSR >:(
#define CSR_MSTATUS                                 0x300
#define CSR_MIE                                     0x304
#define CSR_MIP                                     0x344

#define CSR_MTVEC                                   0x305  
#define CSR_MEPC                                    0x341 
#define CSR_MCAUSE                                  0x342 
#define CSR_MTVAL                                   0x343 

//Physical Memory Protection CSRs
#define pmpcfg0 0x3A0   //Physical memory protection configuration.
#define pmpcfg1 0x3A1   //Physical memory protection configuration, RV32 only.
#define pmpcfg2 0x3A2   //Physical memory protection configuration.
#define pmpcfg3 0x3A3   //Physical memory protection configuration, RV32 only.
#define pmpaddr0 0x3B0  //Physical memory protection address register.
#define pmpaddr1 0x3B1  //Physical memory protection address register.
#define pmpaddr2 0x3B2  //Physical memory protection address register.
#define pmpaddr3 0x3B3  //Physical memory protection address register.
#define pmpaddr4 0x3B4  //Physical memory protection address register.
#define pmpaddr5 0x3B5  //Physical memory protection address register.
#define pmpaddr6 0x3B6  //Physical memory protection address register.
#define pmpaddr7 0x3B7  //Physical memory protection address register.
#define pmpaddr8 0x3B8  //Physical memory protection address register.
#define pmpaddr9 0x3B9  //Physical memory protection address register.
#define pmpaddr10 0x3BA //Physical memory protection address register.
#define pmpaddr11 0x3BB //Physical memory protection address register.
#define pmpaddr12 0x3BC //Physical memory protection address register.
#define pmpaddr13 0x3BD //Physical memory protection address register.
#define pmpaddr14 0x3BE //Physical memory protection address register.
#define pmpaddr15 0x3BF //Physical memory protection address register

//Macros to Access the configuration registers
#define GET_PMPCFG_R(x) (x & 0b1)
#define GET_PMPCFG_W(x) ((x >> 1 &) 0x1)
#define GET_PMPCFG_X(x) ((x >> 2) & 0x1)
#define GET_PMPCFG_L(x) ((x >> 3) & 0b11)
#define GET_PMPCFG_A(x) ((x >> 7) & 0x1)

#define PMPCFG_A_OFF 0   //OFF Null region (disabled)
#define PMPCFG_A_TOR 1   //TOR Top of range
#define PMPCFG_A_NA4 2   //NA4 Naturally aligned four-byte region
#define PMPCFG_A_NAPOT 3 //NAPOT Naturally aligned power-of-two region, ≥8 bytes

#define DIRECT_MODE                                 0x00
#define VECTORED_MODE                               0x01


#define M_EXTENSION                                 0b0000001


#define MUL                                         0b000
#define MULH                                        0b001
#define MULHSU                                      0b010
#define MULHU                                       0b011
#define DIV                                         0b100
#define DIVU                                        0b101
#define REM                                         0b110
#define REMU                                        0b111

#endif