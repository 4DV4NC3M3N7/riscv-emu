
#include <iostream>
#include <fstream>
#include "ext_mem.h"
#include "../debug.h"
#include <queue>

EXT_MEM::EXT_MEM(uint32_t min_address, uint32_t max_address)
{
    this->min_address = min_address;
    this->max_address = max_address;

    //Open file
    std::fstream file;
    file.open("external_memory.bin", std::fstream::in | std::fstream::binary);

    if(file.is_open())
    {
        //Get file size
        file.seekg(file.end);
        size_t lenght = file.tellg();
        file.seekg(file.beg);

        //Read file into buffer
        mem_buffer = new char[(max_address - min_address)];
        file.read(mem_buffer, lenght);
        max_used = (lenght+min_address);
        file.close();
    }
    else
    {
        mem_buffer = nullptr;
    }
    
}

EXT_MEM::~EXT_MEM()
{
    std::fstream file;
    file.open("external_memory.bin", std::fstream::in | std::fstream::out | std::fstream::binary);
    if(mem_buffer)
    {
        if(file.is_open())
        {
            file.seekg(file.end);
            size_t size = file.tellg();
            file.seekg(file.beg);
            //Write buffer back
            if(max_used > size) file.write(mem_buffer, ((max_used-min_address)));
            else file.write(mem_buffer, size);
            file.close();
        }
        delete mem_buffer;
        //Write contents back

    }
}

uint8_t EXT_MEM::read(uint32_t addr)
{
    #ifdef __DEBUG__
        printf("EXT_MEM: Read: A -> 0x%02x\n", addr);
    #endif
    
    return mem_buffer[addr-min_address];
}

void EXT_MEM::write(uint8_t data, uint32_t addr)
{
    #ifdef __DEBUG__
        printf("EXT_MEM: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(max_used < addr) max_used = addr;
    //printf("%c", data);
    mem_buffer[addr-min_address] = data;
    //std::cout.flush();
}