
#include "memory.h"
#include <math.h>
#include <ncurses.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include "../debug.h"

MEMORY::MEMORY(int initial_size, uint32_t min_address, uint32_t max_address)
{
    this->max_address = max_address;
    this->min_address = min_address;
    
    //Calculate the size of the mask
    mask = (int)log2(MEM_BYTES);
    //Allocate memory
    //if(vector_mem.size() > initial_size)
    //{
    //    if(vector_mem.max_size() >= initial_size)
    //    {
    //        vector_mem.reserve(initial_size);
    //    }
    //}
    //Initalize memory
    //for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end(); ++it)
    //{
    //    it->memory = new std::vector<int8_t>;
    //}
    //Initalize memory regions into MEM_BYTES sections
    //size_t section = 0;
    //for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end(); ++it)
    //{
    //    it->memory_region = section;
    //    section = section + MEM_BYTES;
    //}
}

//Initialize memory at address with data on buffer
bool MEMORY::initalize(uint32_t addr, char* buff, size_t buff_size)
{
    if(buff)
    {
        if((buff_size + addr) >= max_address) return false; //Cannot initialize
        printf("Trying to initialize at Address %08x, buff_size %08x\n", addr, buff_size);
        
        for(int i = addr; i < buff_size;i++)
        {
            write(buff[i], i);
        }
        return true;
    }
    else
    {
        return false;
    }
    

}

MEMORY::~MEMORY()
{
    //Delete initialized vectors
    for(size_t i = 0;i < vector_mem.size();i++)
    {
        if(vector_mem[i].memory)
        {
            std::cout << "Deleting Memory" << i << "\n";
            delete vector_mem[i].memory;
        }
    }
}


uint8_t MEMORY::read(uint32_t addr)
{
    //std::cout << "Memory: Read -> " << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr << std::endl;
    /*
    int addr = 0b00000000111111110000111100000000;
    
    int mask = log2(4096);
    
    int section = addr >> mask;
    
    int new_address = ((int)(pow(2, mask) - 1) & addr);
    
    cout<< "mask: " << mask << endl;
    cout<< "section: " << std::bitset<32> (section) << endl;
    cout<< "new_address: " << std::bitset<32> (new_address) << endl;
    */
    //return data from memory

    //Calculate the size of the mask
    int section = (addr >> mask);
    int new_address = ((int)(pow(2, mask) - 1) & addr);

    if(last_used)
    {
        if(last_used->memory_region == section)
        {
            return last_used->memory->operator[](new_address);
        }
    }
    //mvprintw(32, 30, "section: %08x", section);
    //mvprintw(33, 30, "new_address: %08x", new_address);

    //if(section > vector_mem.max_size()) return 0x00;

    //return vector_mem.at(section).memory_region[];
    //MEM_BLOCK* block = &vector_mem[section];
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(34, 30, "Data: %08x", it->memory->at(new_address) & 0xff);
            #ifdef __DEBUG__
            printf("Memory: Read: D -> 0x%08x, A -> 0x%02x\n", it->memory->at(new_address), addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            return it->memory->at(new_address);
        }
    }

    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new std::vector<int8_t>(MEM_BYTES);
    vector_mem.push_back(new_block);

    last_used = &new_block;
    return new_block.memory->at(new_address);

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(34, 30, "Data: %08x", it->memory->at(new_address));
            #ifdef __DEBUG__
            printf("Memory: Read: D -> 0x%08x, A -> 0x%02x\n", it->memory->at(new_address), addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            return it->memory->at(new_address);
        }
    }
    //If still nothing
    #ifdef __DEBUG__
    printf("Memory: Read: D -> 0x%08x, A -> 0x%02x\n", 0x00, addr);
    #endif
    //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", 0x00, 0x00, addr);
    return 0x00;

}

void MEMORY::write(uint8_t data, uint32_t addr)
{
    //std::cout << "Memory: Write -> " << "0x" << std::hex << std::setw(8) << std::setfill('0') << ()data << " A-> " << addr << std::endl;
    #ifdef __DEBUG__
    printf("Memory: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(addr == 0x74) printf("Memory: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    int section = (addr >> mask);
    int new_address = ((int)(pow(2, mask) - 1) & addr);

    if(last_used)
    {
        if(last_used->memory_region == section)
        {
            last_used->memory->operator[](new_address) = data;
        }
    }
    //Search for the page
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(32, 30, "Data: %08x", it->memory->at(new_address));
            //it->memory[new_address].at(new_address) = 23;
            last_used = &(*it);
            it->memory->operator[](new_address) = data;
            return;
        }
    }


    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new std::vector<int8_t>(MEM_BYTES);
    vector_mem.push_back(new_block);

    last_used = &new_block;
    new_block.memory->operator[](new_address) = data;

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(32, 30, "Data: %08x", it->memory->at(new_address));
            //it->memory[new_address].at(new_address) = 23;
            last_used = &(*it);
            it->memory->operator[](new_address) = data;

            return;
        }
    }

}