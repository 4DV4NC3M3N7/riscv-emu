#include "memory_map.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Font/consolas_font.h"

extern void _set_time_int(int cycles);
extern uint64_t _get_time();

volatile char* terminal_buff_size = (char*)TERMINAL_SIZE;
volatile char* terminal_i = (char*)TERMINAL_I_DATA;

typedef struct
{
  char r, g, b, a;
} pixel_t;

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
void blink_cursor();

void trap_handler(uint32_t mepc, uint32_t mcause, uint32_t mtval)
{
  print("Currently on trap handler! :)\n");
  char buffer [50];
  sprintf(buffer, "mepc: 0x%08x\nmtval: 0x%08x\nmcause: 0x%08x\n", mepc, mtval, mcause);
  print(buffer);
}

uint32_t cnt;
void timer_interrupt_handler()
{
  char buffer [50];
  sprintf(buffer, "Currently on timer handler %d\n", cnt);
  cnt++;
  //print(buffer);
  //*((int*)TIMER_CMP_L_ADDR) = 0xffffffff;
  blink_cursor();
  
  _set_time_int(0x04000);
}

volatile int* frame_buffer = (int*)DISPLAY_L_ADDR; 

void draw_point(int x, int y)
{
    frame_buffer[y*500+x] = 0xffffffff;
}

void draw_point_color(int x, int y, pixel_t color)
{
    //char r = (rand() % 0xff);
    //char g = (rand() % 0xff);
    //char b = (rand() % 0xff);
    //char q = (rand() % 0xff);
    //frame_buffer[y*500+x] = (r | (g << 8) | (b << 16) | (q << 24));
    frame_buffer[y*500+x] = (color.r | (color.g << 8) | (color.b << 16) | (color.a << 24));
}

void draw_point_rand_color(int x, int y)
{
    char r = (rand() % 0xff);
    char g = (rand() % 0xff);
    char b = (rand() % 0xff);
    char q = (rand() % 0xff);
    frame_buffer[y*500+x] = (r | (g << 8) | (b << 16) | (q << 24));
    //frame_buffer[y*500+x] = (color.r | (color.g << 8) | (color.b << 16) | (color.a << 24));
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

int abs(int x)
{
  if(x < 0)
  {
    return -x;
  }
  else
  {
    return x;
  }
}

float sign(float x)
{
  if(x < 0.0f)
  {
    return -1;
  }
  else
  {
    return 1;
  }
}

int sign_fixed(int x)
{
  if(x < 0)
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
  a1 = (a1 << (int64_t)15);
  b1 = (b1);
  //char buffer [50];
  //sprintf(buffer, "a: %ld, b: %ld, (a1/b1): %lld\n", a/32768, b/32768, (a1/b1));
  //print(buffer);
  //sprintf(buffer, "(a1/b1): %ld\n", (int32_t)(a1/b1));
  //print(buffer);
  return ((int64_t)(a1/b1));
}
void draw_line_fixed_steep(int x1, int y1, int x2, int y2)
{
    if(y1 > y2)
    {
      int temp = x2;
      x2 = x1;
      x1 = temp;
      temp = y2;
      y2 = y1;
      y1 = temp;
    }
    int deltay = ((y2 - y1) << 15);
    int deltax = ((x2 - x1) << 15);

    if(deltay != 0)
    {
      int deltaerr = _div_fixed(deltax, deltay);    // Assume deltay != 0 (line is not vertical),
      //char buffer [50];
      //sprintf(buffer, "deltaerr: %ld\n", deltaerr);
      //print(buffer);
      if(deltaerr < 0) deltaerr = -deltaerr;
            // note that this division needs to be done in a wax that preserves the fractional part
      int error = 0; // No error at start
      
      int x = (x1 << 15);
      for(int y = (y1 << 15); y <= (y2 << 15);y += (1 << 15))
      {
          draw_point((x >> 15), (y >> 15));
          error = error + deltaerr;
          if(error >= (1 << 14))
          {
              x = x + sign_fixed(deltax);
              error = error - (1 << 15);
          }
      } 
    }
}
void draw_line_fixed_steep_color(int x1, int y1, int x2, int y2, pixel_t color)
{
    if(y1 > y2)
    {
      int temp = x2;
      x2 = x1;
      x1 = temp;
      temp = y2;
      y2 = y1;
      y1 = temp;
    }
    int deltay = ((y2 - y1) << 15);
    int deltax = ((x2 - x1) << 15);

    if(deltay != 0)
    {
      int deltaerr = _div_fixed(deltax, deltay);    // Assume deltay != 0 (line is not vertical),
      //char buffer [50];
      //sprintf(buffer, "deltaerr: %ld\n", deltaerr);
      //print(buffer);
      if(deltaerr < 0) deltaerr = -deltaerr;
            // note that this division needs to be done in a wax that preserves the fractional part
      int error = 0; // No error at start
      
      int x = (x1 << 15);
      for(int y = (y1 << 15); y <= (y2 << 15);y += (1 << 15))
      {
          draw_point_color((x >> 15), (y >> 15), color);
          error = error + deltaerr;
          if(error >= (1 << 14))
          {
              x = x + sign_fixed(deltax);
              error = error - (1 << 15);
          }
      } 
    }
}

void draw_line_fixed(int x1, int y1, int x2, int y2)
{
    if(x1 > x2)
    {
      int temp = x2;
      x2 = x1;
      x1 = temp;
      temp = y2;
      y2 = y1;
      y1 = temp;
    }
    int deltax = ((x2 - x1) << 15);
    int deltay = ((y2 - y1) << 15);
    if(abs(deltax) <= abs(deltay)) draw_line_fixed_steep(x1, y1, x2, y2);

    int deltaerr = _div_fixed(deltay, deltax);    // Assume deltax != 0 (line is not vertical),
    //char buffer [50];
    //sprintf(buffer, "deltaerr: %ld\n", deltaerr);
    //print(buffer);
    
    if(deltaerr < 0) deltaerr = -deltaerr;
          // note that this division needs to be done in a way that preserves the fractional part
    else
    {
      int error = 0; // No error at start
      

      int y = (y1 << 15);
      for(int x = (x1 << 15); x <= (x2 << 15);x += (1 << 15))
      {
          draw_point((x >> 15), (y >> 15));
          error = error + deltaerr;
          if(error >= (1 << 14))
          {
              y = y + sign_fixed(deltay);
              error = error - (1 << 15);
          }
      } 
    }
}

void draw_line_fixed_color(int x1, int y1, int x2, int y2, pixel_t color)
{
    if(x1 > x2)
    {
      int temp = x2;
      x2 = x1;
      x1 = temp;
      temp = y2;
      y2 = y1;
      y1 = temp;
    }
    int deltax = ((x2 - x1) << 15);
    int deltay = ((y2 - y1) << 15);
    if(abs(deltax) <= abs(deltay)) draw_line_fixed_steep_color(x1, y1, x2, y2, color);

    int deltaerr = _div_fixed(deltay, deltax);    // Assume deltax != 0 (line is not vertical),
    //char buffer [50];
    //sprintf(buffer, "deltaerr: %ld\n", deltaerr);
    //print(buffer);
    
    if(deltaerr < 0) deltaerr = -deltaerr;
          // note that this division needs to be done in a way that preserves the fractional part
    else
    {
      int error = 0; // No error at start
      

      int y = (y1 << 15);
      for(int x = (x1 << 15); x <= (x2 << 15);x += (1 << 15))
      {
          draw_point_color((x >> 15), (y >> 15), color);
          error = error + deltaerr;
          if(error >= (1 << 14))
          {
              y = y + sign_fixed(deltay);
              error = error - (1 << 15);
          }
      } 
    }
}



void draw_line(int x1, int y1, int x2, int y2)
{
    float deltax = (x2 - x1);
    float deltay = (y2 - y1);

    float deltaerr = (deltay / deltax);    // Assume deltax != 0 (line is not vertical),
    if(deltaerr < 0) deltaerr = -deltaerr;
          // note that this division needs to be done in a way that preserves the fractional part
    float error = 0.0f; // No error at start
    
    int y = y1;
    for(int x = x1; x <= x2;x++)
    {
        draw_point(x, y);
        
        error = error + deltaerr;
        
        if(error >= 0.5f)
        {
            y = y + sign(deltay);
            error = error - 1.0f;
        }
    } 
}

pixel_t white = {0xff, 0xff, 0xff, 0xff};
pixel_t black = {0x00, 0x00, 0x00, 0x00};

//displays character(ch) in frame buffer position x, y
void draw_char(char ch, int px, int py)
{
  int offset = ch - ' ';

  for(int y = 0;y < 11;y++)
  {
    for(int x = 0; x < 1;x++)
    {
      char data = consolas_8pt[lut[offset]+((y)+x)];
      for(int shamt = 8; shamt > 0;shamt--)
      {
        if(((data >> shamt) & 1) == 1) //shamt - Shift amount
        {
          draw_point_color(px+((8-shamt)+(x*7)), py+y, white);
        }
        else
        {
          draw_point_color(px+((8-shamt)+(x*7)), py+y, black);
        }
      }
    }
  }
}

int count_y = 0;
int count_x = 0;
const int char_width = 7;
const int char_height = 11;
const int n_cols = 500/char_height;
const int n_row = (500/char_width)-1;

char ch_buffer [((500/7)-1)*(500/11)];

void blink_cursor()
{
  const int cursor_width = 2;
  static int state;
  for(int i = 0;i < char_height;i++)
  {
    for(int x = 0;x < cursor_width;x++)
    {
      if(state == 1)
      {
        draw_point_color((((count_x+1)*char_width)+2)+x, (count_y*char_height)+i, black);
      }
      else
      {
        draw_point_color((((count_x+1)*char_width)+2)+x, (count_y*char_height)+i, white);
      }
    }
  }
  if(state == 1) state = 0;
  else state = 1;
  //clear_screen();
}

void clear_screen()
{
    //memset((char*)DISPLAY_L_ADDR, 0xff, 500*500*4);
    memset((char*)DISPLAY_L_ADDR, 1, 500*500*4);
}

void draw_str(char* string)
{
  char* ptr = string;
  while(*ptr != '\0')
  {
    if(*ptr == '\b')
    {
      if(count_x > 0)
      {
        count_x -= 1;
      }
      //print("Backspace detected\n");
    }
    else if(*ptr == '\n')
    {
      count_x = 0;
      count_y++;
      draw_char(' ', count_x*char_width, count_y*char_height);
    }
    else if(*ptr == '\r')
    {
      count_x = 0;
    }
    else
    {
      count_x ++;
      //draw_char(' ', count_x*char_width, count_y*char_height);
      draw_char(*ptr, count_x*char_width, count_y*char_height);
      if(count_x >= n_row)
      {
        count_y++;
        count_x = 0;
      }
      if(count_y >= n_cols)
      {
        count_y = 0;
        //memcpy(ch_buffer, ch_buffer+n_row, sizeof(ch_buffer)-n_row);
      }
    }
    
    ptr = ptr + 1;
  }
}
char banner[] =                                                                     
" _   _             _ _       _____                \n\
| | | |           (_) |     /  __ \\               \n\
| | | | __ _ _ __  _| | __ _| /  \\/ ___  _ __ ___  \n\
| | | |/ _` | '_ \\| | |/ _` | |    / _ \\| '__/ _ \\ \n\
\\ \\_/ / (_| | | | | | | (_| | \\__/\\ (_) | | |  __/ \n\
 \\___/ \\__,_|_| |_|_|_|\\__,_|\\____/\\___/|_|  \\___| \n";

uint64_t last = 0;

int main()
{
  char buffer [50];

  print("In main, before trap handler!\n");
  asm(".word 0xaeaeaeae");
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
    
    memset((char*)EXT_L_ADDR, 'B', 2);

    last = _get_time();

    draw_str(banner);
    print(banner);

    draw_str("By: Benjamin Herrera Navarro\n");
    draw_str("Date: 1/16/2021\n");
    draw_str("8:04AM\n");
    print("By: Benjamin Herrera Navarro\n");
    print("Date: 1/16/2021\n");
    print("8:04AM\n");

    while(0)
    {
      sprintf(buffer, "Time: 0x%016llx -> Up time: %016llu\r", (uint64_t)((_get_time()-last)/0x04000),  (uint64_t)_get_time());
      draw_str(buffer);
      print(buffer);
    }


    pixel_t color;
    color.r = (rand() % 0xff);
    color.g = (rand() % 0xff);
    color.b = (rand() % 0xff);
    color.a = (rand() % 0xff);

      float i = 0;
      while(true){
      
        ////Read contents of the external_memory
        //for(char *i = (char*)EXT_L_ADDR;i <= (EXT_L_ADDR+10);i++)
        //{
        //  *((char*)TERMINAL_ADDR_L) = *i;
        //}
        //color.r = (rand() % 0xff);
        //color.g = (rand() % 0xff);
        //color.b = (rand() % 0xff);
        //color.a = (rand() % 0xff);
            //asm("ecall");
            //sprintf(buffer, "0x%08x -> 0x%08x 0x%08x:0x%08x\r", &i, i, (&i)+(i), rand());
        //for(int x = 0;x <= 0xff;x++);
        if(*terminal_buff_size > 0)
        {
            sprintf(buffer, "%c", *terminal_i);
            print(buffer);
            draw_str(buffer);
        }
            i = i + 1;
        //draw_line_fixed_color(rand() % 500, rand() % 500, rand() % 500, rand() % 500, color); 
      }
    //print("Waiting for interrupt\n");
  return 0xfffff;
}
