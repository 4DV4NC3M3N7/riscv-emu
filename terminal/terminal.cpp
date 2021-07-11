#include <iostream>
#include "terminal.h"
#include "../debug.h"
#include <queue>

TERMINAL::TERMINAL(uint32_t min_address, uint32_t max_address)
{
    this->min_address = min_address;
    this->max_address = max_address;
}

uint8_t TERMINAL::read(uint32_t addr)
{
    //printf("Terminal: Read: A -> 0x%02x\n", addr);
    //std::cout.flush();
    if((addr & 0xf) < 0x8 && ((addr & 0xf) >= 0x4))
    {
        if(!input_buffer.empty())
        {
            char data =  input_buffer.front();
            input_buffer.pop();
            return data;
        }
        else
        {
            return 0x00;
        }
        
        //std::cout << "Input Buffer Data read\n";
    }
    else if((addr & 0xf) >= 0x8)
    {
        //std::cout << "Input Buffer Size read\n";
        return (uint8_t)input_buffer.size();
    }
    return 0x00;
}

void TERMINAL::write(uint8_t data, uint32_t addr)
{
    #ifdef __DEBUG__
    printf("Terminal: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(graphical)
    {
        output_buffer.push(data);
    }
    else
    {
        printf("%c", data);
        std::cout.flush();
    }
}