
#include "memory.h"
#include <math.h>
#include <ncurses.h>
#include <iostream>
#include <iomanip>
#include <map>
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

MEMORY::~MEMORY()
{
    //Delete initialized vectors
    virtual_ram.~map();
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

    if(last_section == section) if(last_used) return last_used->at(new_address);

    //Find if exists
    it = virtual_ram.find(section);
    if(it != virtual_ram.end() && (it->second))
    {
            last_used = it->second;
            last_section = section;
            return it->second->at(new_address);
    }
    else
    {
        //Create element and return data
        std::vector<uint8_t>* ptr = new std::vector<uint8_t>();
        virtual_ram.emplace(section, ptr);
        last_used = ptr;
        last_section = section;
        return 0;
    } 
}

void MEMORY::write(uint8_t data, uint32_t addr)
{
    //std::cout << "Memory: Write -> " << "0x" << std::hex << std::setw(8) << std::setfill('0') << ()data << " A-> " << addr << std::endl;
    #ifdef __DEBUG__
    printf("Memory: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    int section = (addr >> mask);
    int new_address = ((int)(pow(2, mask) - 1) & addr);


    //Check cached
    if(last_section == section) if(last_used) last_used->operator[](new_address) = data;
    //Find if exists
    it = virtual_ram.find(section);
    if((it != virtual_ram.end()) && (it->second))
    {
        last_used = it->second;
        last_section = section;
        it->second->operator[](new_address) = data;
    }
    else
    {
        //Create element and return data
        std::vector<uint8_t>* ptr = new std::vector<uint8_t>(MEM_BYTES);
        virtual_ram.emplace(section, ptr);
        ptr->operator[](new_address) = data;
        last_used = ptr;
        last_section = section;
    } 

}

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <vector>
#include <map>
#include "../device/device.h"

//4KB for each block allocated
#define MEM_BYTES (4096)

typedef struct
{
    /* data */
    std::vector<int8_t>* memory;
    size_t memory_region;
} MEM_BLOCK;


class MEMORY : public device
{
    public:
        MEMORY(int initial_size, uint32_t min_address, uint32_t max_address);
        uint8_t read(uint32_t addr);
        void write(uint8_t data, uint32_t addr);
        ~MEMORY();
    private:
        /* data */
        std::vector<MEM_BLOCK> vector_mem; 
        std::map<size_t, std::vector<uint8_t>*> virtual_ram;
        std::map<size_t, std::vector<uint8_t>*>::iterator it;
        std::vector<uint8_t>* last_used;
        size_t last_section;
        int mask;
};


#endif