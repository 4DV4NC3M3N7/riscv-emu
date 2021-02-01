#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "../device/device.h"
#include <SFML/Graphics.hpp>
#include <thread>


typedef struct
{
    uint8_t r, g, b, a;

} pixel_t;

class DISPLAY: public device
{
    public:
        DISPLAY(uint32_t min_address, uint32_t max_address);
        DISPLAY(uint32_t min_address, uint32_t max_address, uint16_t height, uint16_t width);
        ~DISPLAY();
        uint8_t read(uint32_t addr);
        void write(uint8_t data, uint32_t addr);
        uint16_t height, width;
        sf::Uint8* frame_buffer;
};


#endif