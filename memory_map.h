#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__





#define MEMORY_L_ADDR       0x00000000
#define MEMORY_H_ADDR       0x0fffffff

#define TERMINAL_ADDR_L     0x10000000
#define TERMINAL_O_DATA     0x10000000
#define TERMINAL_I_DATA     0x10000004
#define TERMINAL_SIZE       0x10000008
#define TERMINAL_ADDR_H     0x1000000c

#define TIMER_L_ADDR        0x10000010
#define TIMER_TIME_L_ADDR   0x10000010
#define TIMER_TIME_H_ADDR   0x10000014
#define TIMER_CMP_L_ADDR    0x10000018
#define TIMER_CMP_H_ADDR    0x1000001c
#define TIMER_H_ADDR        0x1000001f

#define DISPLAY_HEIGHT      500
#define DISPLAY_WIDTH       500

#define DISPLAY_L_ADDR      0x20000000
//1750000 size                BASE         SIZE
#define DISPLAY_H_ADDR      (0x20000000 + ((DISPLAY_HEIGHT*DISPLAY_WIDTH)*4))

#define EXT_L_ADDR      0x30000000
#define EXT_H_ADDR      0x40000000



#endif