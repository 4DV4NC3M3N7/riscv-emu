#include "memory_map.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

extern void _set_time_int(int cycles);

void print(char* string)
{
  //TERMINAL_ADDR_L is just a macro defined on "memory_map.h"
  volatile char* terminal_base_addr = (char*)TERMINAL_ADDR_L;
  char* ptr = string;
  while(*ptr != '\0')
  {
    *terminal_base_addr = *ptr;
    ptr = ptr + 1;
  }
  return;
}

void trap_handler(uint32_t mepc, uint32_t mcause, uint32_t mtval)
{
  print("Currently on trap handler! :)\n");
  char buffer [50];
  sprintf(buffer, "mepc: 0x%08x\nmtval: 0x%08x\nmcause: 0x%08x\n", mepc, mtval, mcause);
  print(buffer);
}

void timer_interrupt_handler()
{
  print("Currently on timer handler\n");
  //*((int*)TIMER_CMP_L_ADDR) = 0xffffffff;
  _set_time_int(0x01000);
}

volatile int* frame_buffer = (int*)DISPLAY_L_ADDR; 

void draw_point(int x, int y)
{
    frame_buffer[y*500+x] = 0xffffffff;
}

void plot_point(int x, int y)
{
  draw_point((DISPLAY_WIDTH/2)+x, (DISPLAY_HEIGHT/2)+y);
}

int int_sqrt(int i)
{
  if (i == 0 || i == 1) return i;
  int counter = 1, sqroot=1, output;
  while (sqroot <= i)
  {
  counter++;
  sqroot = counter*counter;
  }
  return (counter - 1);
}


void plot_line(int x1, int y1, int x2, int y2)
{
  //Get lenght
  int slope = (y2 - y1)/(x2 - x1);
  //int leght = int_sqrt(((x2-x1)*(x2-x1))+((y2 - y1)*(y2 - y1)));
  int min;
  int max;
  if(x1 < x2)
  {
    for(int i = x1; i < x2;i++) plot_point(i, slope*i);
  }
  else if(x1 > x2)
  {
    for(int i = x2; i < x1;i++) plot_point(i, slope*i);
  }
}

int sign(int x)
{
  if(((x >> 31) & 1))
  {
    return ((-1) << 15);
  }
  else
  {
    return (1 << 15);
  }
}

int _div_fixed(int a, int b)
{
  int64_t a1 = a;
  int64_t b1 = b;
  a1 = (a1 << 15);
  b1 = (b1 << 15);
  return ((int64_t)(a1/b1) >> 15);
}

void draw_line(int x1, int y1, int x2, int y2)
{
    int deltax = (x2 - x1) << 15;
    int deltay = (y2 - y1) << 15;

    int deltaerr = _div_fixed(deltay, deltax);    // Assume deltax != 0 (line is not vertical),
    if(deltaerr < 0) deltaerr = -deltaerr;
          // note that this division needs to be done in a way that preserves the fractional part
    int error = 0; // No error at start
    
    int y = y1;
    for(int x = x1; x <= x2;x++)
    {
        draw_point((x >> 15), (y >> 15));
        error = error + deltaerr;
        if(error >= (1 << 14))
        {
            y = y + sign(deltay);
            error = error - (1 << 15);
        }
    } 
}

int main()
{
  print("In main, before trap handler!\n");
  asm(".word 0xaeaeaeae");
  //asm("ecall");
  print("Back from the trap handler!!!\n");
  //while(true) 
  //{
  //  for(int i = 0;i <= (DISPLAY_H_ADDR-DISPLAY_L_ADDR);i++)
  //  {
  //    *((char*)DISPLAY_L_ADDR + i) = rand()%0xff;
  //  }
  //}
  //_set_time_int(0x010);
    //memset((char*)DISPLAY_L_ADDR, 0, DISPLAY_H_ADDR);
    //for(int i = (0-DISPLAY_WIDTH);i <= DISPLAY_WIDTH;i++) plot_point(i, (s)*i+4);
    //plot_line(50, 50, 0, 0);
    draw_line(0, 0, 50, 30);
    
    //print("Waiting for interrupt\n");
  return 0xfffff;
}
