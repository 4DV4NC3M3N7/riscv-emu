#include "../../memory_map.h"
#include "primitives.h"
#include <math.h>

int floorSqrt(int n) 
{ 
    // Base cases 
    if (n == 0 || n == 1) 
    return n; 
    
    int i = 1, result = 1; 
    while (result <= n) 
    { 
      i++; 
      result = i * i; 
    } 
    return i - 1; 
} 
   

void init_display()
{
    
}
void delete_display()
{

}

void draw_pixel(display_t* display, int x, int y)
{

}

void draw_line(display_t* display, int x, int y, int x2, int y2)
{
    //y = mx + b
    //Get size of the line first
    int slope = ((y2-y)/(x2-x));
    int size = sqrt(((y2-y)*(y2-y))+((x2-x)*(x2-x)));
    int point_y;
    int point_x;
    for(int i = 0; i <= size; i++)
    {
        point_y = (slope*i);
        int screenX = i + (display->width/2);
        int screenY = (display->height/2) - point_y;
        *(display->buffer + ((screenX * screenY)*4) + 0x0) = 0xff; 
        *(display->buffer + ((screenX * screenY)*4) + 0x1) = 0xff; 
        *(display->buffer + ((screenX * screenY)*4) + 0x2) = 0xff; 
        *(display->buffer + ((screenX * screenY)*4) + 0x3) = 0xff; 
    }
}
void draw_square()
{

}
void draw_triangle()
{

}
void draw_circle()
{

}