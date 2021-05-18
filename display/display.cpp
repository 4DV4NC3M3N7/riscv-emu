#include <iostream>
#include "display.h"
#include "../debug.h"
#include <string.h>
#include <SFML/Graphics.hpp>
#include <thread>

//void DISPLAY::window_handler()
//{
//
//    //while (window->isOpen())
//    //{
//    //    std::cout << "Running main window thread\n";
//    //    sf::Event event;
//    //    while (window->pollEvent(event))
//    //    {
//    //        if (event.type == sf::Event::Closed)
//    //            window->close();
//    //    }
////
//    //    window->clear();
//    //    //window->draw(shape);
//    //    //Draw frame buffer, and update.
////
//    //    //window->display();
//    //}
//}
//
//void DISPLAY::runThread() 
//{ 
//    std::cout << "Starting Thread\n\n\n\n\n\n";
//    m_thread = std::thread(&DISPLAY::window_handler, this);
//    m_thread.join();
//};

/* Run thread save
void DISPLAY::runThread() 
{ 

    std::cout << "Starting Thread\n\n\n\n\n\n";
    m_thread = std::thread([=]{
        buffer_texture = new sf::Texture();
        buffer_texture->create(this->width, this->height);
        //printf("Height: %d, Width: %d\n", this->height, this->width);
        //for(size_t i = 0; i < sizeof(pixel_t)*(this->height*this->width);i++)
        //{
        //    *(frame_buffer + i) = 0xff;
        //}

        printf("*Pixel 1 %x, %x, %x, %x\n", *(frame_buffer), *(frame_buffer + 1), *(frame_buffer + 2), *(frame_buffer + 3));

        sf::RenderWindow window(sf::VideoMode(this->width, this->height), "Riscv Emu!");

        while (window.isOpen())
        {
            //std::cout << "Running main window thread\n";
            sf::Event event;
            while (window.pollEvent(event))
            {
                //std::cout << "Pull Event >:(\n";
                if (event.type == sf::Event::Closed)
                {
                    std::cout << "Closing Window >:(\n";
                    window.close();
                }
            }

            //printf("Pixel 1 %x, %x, %x, %x\n", *(buffer_texture), *(buffer_texture + 1), *(buffer_texture + 2), *(buffer_texture + 3));

            //window.clear();
            //sf::CircleShape shape(100.f);
            //shape.setFillColor(sf::Color::Green);
            //window.draw(shape);
            //Draw frame buffer, and update.
            buffer_texture->update(frame_buffer);
            sf::Sprite sprite(*buffer_texture); // needed to draw the texture on screen

            //buffer_texture->update(window);
            window.draw(sprite);
            window.display();
        }
    });
    m_thread.join();
};
*/

DISPLAY::DISPLAY(uint32_t min_address, uint32_t max_address)
{
    this->min_address = min_address;
    this->max_address = max_address;
    this->height = 500;
    this->width = 500;
    frame_buffer = (sf::Uint8*)malloc(sizeof(pixel_t)*(height * width));
    if(!frame_buffer) std::cout << "Frame buffer allocation failed" << std::endl;
    else std::cout << "Frame buffer allocated" << std::endl;
}

DISPLAY::DISPLAY(uint32_t min_address, uint32_t max_address, uint16_t height, uint16_t width)
{
    this->min_address = min_address;
    this->max_address = max_address;
    this->height = height;
    this->width = width;
    frame_buffer = (sf::Uint8*)malloc(sizeof(pixel_t)*(height * width));
    if(!frame_buffer) std::cout << "Frame buffer allocation failed" << std::endl;
    else std::cout << "Frame buffer allocated" << std::endl;
}

DISPLAY::~DISPLAY()
{
    //window->close();
    //delete window;
    if(frame_buffer) delete frame_buffer;
}

uint8_t DISPLAY::read(uint32_t addr)
{
    return *(frame_buffer + (addr - min_address));
}
uint16_t DISPLAY::read16(uint32_t addr)
{
    return *(uint16_t*)(frame_buffer + ((addr>>1) - min_address));
}
uint32_t DISPLAY::read32(uint32_t addr)
{
    return *(uint32_t*)(frame_buffer + ((addr>>2) - min_address));
}

void DISPLAY::write(uint8_t data, uint32_t addr)
{
    #ifdef __DEBUG__
        printf("Display: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(addr <= max_address)
    {
        //Substract the offset to start accessing memory from index 0
        *(frame_buffer + (addr - min_address)) = data;
    }
}
void DISPLAY::write16(uint16_t data, uint32_t addr)
{
    #ifdef __DEBUG__
        printf("Display: Write16: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(addr <= max_address)
    {
        //Substract the offset to start accessing memory from index 0
        *(uint16_t*)(frame_buffer + (addr - min_address)) = data;
    }
}
void DISPLAY::write32(uint32_t data, uint32_t addr)
{
    #ifdef __DEBUG__
        printf("Display: Write32: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(addr <= max_address)
    {
        //Substract the offset to start accessing memory from index 0
        *(uint32_t*)(frame_buffer + (addr - min_address)) = data;
    }
}