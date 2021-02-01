#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

typedef struct
{
    int width;
    int height;
    volatile char* buffer;
} display_t;

extern void init_display();
extern void delete_display();
void draw_line(display_t* display, int x, int y, int x2, int y2);
extern void draw_square();
extern void draw_triangle();
extern void draw_circle();

#endif