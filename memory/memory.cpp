
#include "memory.h"
#include <math.h>
#include <ncurses.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include "../debug.h"
#include "../terminal_colors.h"

MEMORY::MEMORY(int initial_size, uint32_t min_address, uint32_t max_address)
{
    this->max_address = max_address;
    this->min_address = min_address;
    
    //Calculate the size of the mask
    mask = (int)log2(MEM_BYTES);

    memory_block = new uint32_t[max_address-min_address*4];
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
            if((buff[i]&0xff) != (read(i)& 0xff))
            {
                printf("Error: Incorrect data read %x, expected %x\n", read(i)&0xff, buff[i]&0xff);
            }
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
    return (memory_block[addr>>2] >> ((addr&0b11)<<3)) & 0xff;

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
            printf("Memory: Read: D -> 0x%08x, A -> 0x%02x\n", ((uint8_t*)(it->memory))[new_address], addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            return it->memory[new_address];
        }
    }

    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new uint8_t[MEM_BYTES];
    vector_mem.push_back(new_block);

    last_used = &new_block;
    return new_block.memory[new_address];

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(34, 30, "Data: %08x", it->memory->at(new_address));
            #ifdef __DEBUG__
            printf("Memory: Read: D -> 0x%08x, A -> 0x%02x\n", ((uint8_t*)(it->memory))[new_address], addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            return it->memory[new_address];
        }
    }
    //If still nothing
    #ifdef __DEBUG__
    printf("Memory: Read: D -> 0x%08x, A -> 0x%02x\n", 0x00, addr);
    #endif
    //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", 0x00, 0x00, addr);
    return 0x00;

}

uint16_t MEMORY::read16(uint32_t addr)
{
    return (memory_block[addr>>2] >> (((addr&0b1000)>>3)<<4)) & 0xffff;
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
            return ((uint16_t*)last_used->memory)[new_address>>1];
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
            printf("Memory: Read16: D -> 0x%08x, A -> 0x%02x\n", ((uint16_t*)it->memory)[new_address], addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            return ((uint16_t*)(it->memory))[new_address];
        }
    }

    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new uint8_t[MEM_BYTES];
    vector_mem.push_back(new_block);

    last_used = &new_block;
    return (uint16_t)(new_block.memory)[new_address>>1];

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(34, 30, "Data: %08x", it->memory->at(new_address));
            #ifdef __DEBUG__
            printf("Memory: Read16: D -> 0x%08x, A -> 0x%02x\n", ((uint16_t*)it->memory)[new_address], addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            return ((uint16_t*)it->memory)[new_address];
        }
    }
    //If still nothing
    #ifdef __DEBUG__
    printf("Memory: Read16: D -> 0x%08x, A -> 0x%02x\n", 0x00, addr);
    #endif
    //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", 0x00, 0x00, addr);
    return 0x00;

}

void MEMORY::write(uint8_t data, uint32_t addr)
{
    //printf("mask 0x%08x : %d\n", ~(0xff << ((addr&0b11)<<3)), addr&0b11);
    memory_block[addr>>2] = (memory_block[addr>>2] & ~(0xff << ((addr&0b11)<<3))| (data << ((addr&0b11)<<3)));
    return;
    //std::cout << "Memory: Write -> " << "0x" << std::hex << std::setw(8) << std::setfill('0') << ()data << " A-> " << addr << std::endl;
    #ifdef __DEBUG__
    printf("Memory: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    //if(addr == 0x74) printf("Memory: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    int section = (addr >> mask);
    int new_address = ((int)(pow(2, mask) - 1) & addr);

    if(last_used)
    {
        if(last_used->memory_region == section)
        {
            last_used->memory[new_address] = data;
            return;
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
            it->memory[new_address] = data;
            return;
        }
    }


    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new uint8_t[MEM_BYTES];
    vector_mem.push_back(new_block);

    last_used = &new_block;
    new_block.memory[new_address] = data;
    return;

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(32, 30, "Data: %08x", it->memory->at(new_address));
            //it->memory[new_address].at(new_address) = 23;
            last_used = &(*it);
            it->memory[new_address] = data;

            return;
        }
    }

}

void MEMORY::write16(uint16_t data, uint32_t addr)
{
    if((addr & 0b11) != 0)
    {
        //printf("Unaligned access at address 0x%08x\n", addr);
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[addr>>2]);
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[(addr>>2)+1]);
        //printf("Unaligned by %d\n", addr&0b11);
        //printf("shifting by %d\n", (0b11 - addr&0b11));
        //uint32_t part1 = memory_block[addr>>2] | memory_block[addr>>2] >> ((addr&0b11)<<3);
        
        memory_block[addr>>2] = ((memory_block[addr>>2] & ~(0xffffffff << ((addr&0b11)<<3))) | (data << ((addr&0b11)<<3)));
        memory_block[(addr>>2)+1] = ((memory_block[(addr>>2)+1] & ~(0xffffffff >> ((0b100-addr&0b11)<<3))) | data >> ((0b100-addr&0b11)<<3));
        
        //memory_block[(addr>>2)+1] = ((memory_block[(addr>>2)+1] & ~(0xffffffff << ((addr&0b11)<<3))) | (data << ((addr&0b11)<<3)));

        //printf("mask of 0x%08x\n",  ~(0xffffffff >> ((0b100-addr&0b11)<<3)));
        ////printf("part1 is 0x%08x\n", memory_block[addr>>2] & ~(0xffffffff << ((addr&0b11)<<3)));
        //printf("shifting by %d\n", ((0b1000 - addr&0b11)<<3));
        ////printf("result is 0x%08x\n", part1 | (memory_block[(addr>>2)+1] << ((0b1000 - addr&0b11)<<3)));
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[addr>>2]);
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[(addr>>2)+1]);
        return;
        //part1 | (memory_block[(addr>>2)+1] << ((0b1000 - addr&0b11)<<3));
        //exit(1);
    }
    else
    {
        memory_block[addr>>2] = data;
        return;
        /* code */
    }
    return;
    //std::cout << "Memory: Write -> " << "0x" << std::hex << std::setw(8) << std::setfill('0') << ()data << " A-> " << addr << std::endl;
    #ifdef __DEBUG__
    printf("Memory: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    if(addr == 0x74) printf("Memory: Write16: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    int section = (addr >> mask);
    int new_address = ((int)(pow(2, mask) - 1) & addr);

    if(last_used)
    {
        if(last_used->memory_region == section)
        {
            ((uint16_t*)last_used->memory)[new_address>>1] = data;
            return;
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
            ((uint16_t*)it->memory)[new_address>>1] = data;
            return;
        }
    }


    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new uint8_t[MEM_BYTES];
    vector_mem.push_back(new_block);

    last_used = &new_block;
    ((uint16_t*)new_block.memory)[new_address>>1] = data;
    return;

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(32, 30, "Data: %08x", it->memory->at(new_address));
            //it->memory[new_address].at(new_address) = 23;
            last_used = &(*it);
            ((uint16_t*)it->memory)[new_address>>1] = data;
            return;
        }
    }

}


void MEMORY::write32(uint32_t data, uint32_t addr) //read 32bits
{
    if((addr & 0b11) != 0)
    {
        //printf("Unaligned access at address 0x%08x\n", addr);
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[addr>>2]);
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[(addr>>2)+1]);
        //printf("Unaligned by %d\n", addr&0b11);
        //printf("shifting by %d\n", (0b11 - addr&0b11));
        //uint32_t part1 = memory_block[addr>>2] | memory_block[addr>>2] >> ((addr&0b11)<<3);
        
        memory_block[addr>>2] = ((memory_block[addr>>2] & ~(0xffffffff << ((addr&0b11)<<3))) | (data << ((addr&0b11)<<3)));
        memory_block[(addr>>2)+1] = ((memory_block[(addr>>2)+1] & ~(0xffffffff >> ((0b100-addr&0b11)<<3))) | data >> ((0b100-addr&0b11)<<3));
        
        //memory_block[(addr>>2)+1] = ((memory_block[(addr>>2)+1] & ~(0xffffffff << ((addr&0b11)<<3))) | (data << ((addr&0b11)<<3)));

        //printf("mask of 0x%08x\n",  ~(0xffffffff >> ((0b100-addr&0b11)<<3)));
        ////printf("part1 is 0x%08x\n", memory_block[addr>>2] & ~(0xffffffff << ((addr&0b11)<<3)));
        //printf("shifting by %d\n", ((0b1000 - addr&0b11)<<3));
        ////printf("result is 0x%08x\n", part1 | (memory_block[(addr>>2)+1] << ((0b1000 - addr&0b11)<<3)));
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[addr>>2]);
        //printf("Data at address 0x%08x: 0x%08x\n", addr, memory_block[(addr>>2)+1]);
        return;
        //part1 | (memory_block[(addr>>2)+1] << ((0b1000 - addr&0b11)<<3));
        //exit(1);
    }
    else
    {
        memory_block[addr>>2] = data;
        return;
        /* code */
    }
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
            *(uint32_t*)last_used->memory[new_address] = data;
            return;
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
            printf("Memory: Write32: D -> 0x%08x, A -> 0x%02x\n", it->memory[new_address], addr);
            #endif
            //printf("Memory: Write: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            *(uint32_t*)(it->memory)[new_address] = data;
            return;
        }
    }

    //The section was not found, allocate it
    MEM_BLOCK new_block;
    new_block.memory_region = section;
    new_block.memory = new uint8_t[MEM_BYTES];
    vector_mem.push_back(new_block);

    last_used = &new_block;
    new_block.memory[new_address] = data;
    return;

    //Now search again
    for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
    {
        if(it->memory_region == section)
        {
            //mvprintw(34, 30, "Data: %08x", it->memory->at(new_address));
            #ifdef __DEBUG__
            printf("Memory: Read32: D -> 0x%08x, A -> 0x%02x\n", it->memory[new_address], addr);
            #endif
            //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
            last_used = &(*it);
            *(uint32_t*)it->memory[new_address] = data;
            return;
        }
    }
    //If still nothing
    #ifdef __DEBUG__
    printf("Error: Memory: Write32: D -> 0x%08x, A -> 0x%02x\n", 0x00, addr);
    #endif
}

uint32_t MEMORY::read32(uint32_t addr) //read 32bits
{
    //printf("Memory: Read32: D -> 0x%08x, A -> 0x%02x\n", ((uint32_t*)memory_block)[addr], addr);
    if((addr & 0b11) != 0)
    {
        uint32_t part1 = memory_block[addr>>2] >> ((addr&0b11)<<3);
        return part1 | (memory_block[(addr>>2)+1] << ((0b1000 - addr&0b11)<<3));
    }
    else
    {
        return memory_block[addr>>2];
        /* code */
    }
    
    
    std::cout << "Memory: Read -> " << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr << std::endl;
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
    int new_address = ((int)(pow(2, mask) - 1) & addr >> 2);
    if(((((int)(pow(2, mask) - 1) & addr)) & 0b11) != 0)
    {
        printf("Memory: Error: Section Unaligned\n");
        uint8_t bytes [4];
        bytes [0] = read(addr + 0x00);
        bytes [1] = read(addr + 0x01);
        bytes [2] = read(addr + 0x02);
        bytes [3] = read(addr + 0x03);
        return (uint32_t)(bytes [0] | (bytes [1] << 8) | (bytes [2] << 16) | (bytes [3] << 24));
    }
    else
    {
        
        if(last_used)
        {
            if(last_used->memory_region == section)
            {
                #ifdef __DEBUG__
                printf("Memory: Read32: D -> 0x%08x, A -> 0x%02x\n", last_used->memory[new_address], addr);
                #endif
                printf("Returned 0\n");
                return last_used->memory[new_address];
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
                printf("Memory: Read32: D -> 0x%08x, A -> 0x%02x\n", it->memory[new_address], addr);
                #endif
                printf("Returned 1\n");
                //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
                last_used = &(*it);
                return it->memory[new_address];
            }
        }

        //The section was not found, allocate it
        printf("Section not found Memory: A -> 0x%02x\n", addr);

        MEM_BLOCK new_block;
        new_block.memory_region = section;
        new_block.memory = new uint8_t[MEM_BYTES];
        vector_mem.push_back(new_block);

        last_used = &new_block;
                printf("Returned 2\n");
        return new_block.memory[new_address];

        //Now search again
        for(std::vector<MEM_BLOCK>::iterator it = vector_mem.begin();it != vector_mem.end();++it)
        {
            if(it->memory_region == section)
            {
                //mvprintw(34, 30, "Data: %08x", it->memory->at(new_address));
                #ifdef __DEBUG__
                printf("Memory: Read32: D -> 0x%08x, A -> 0x%02x\n", it->memory[new_address], addr);
                #endif
                //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", it->memory->at(new_address), it->memory->at(new_address), addr);
                last_used = &(*it);
                printf("Returned 3\n");
                return it->memory[new_address];
            }\
        }
        //If still nothing
        #ifdef __DEBUG__
        printf("Memory: Read32: D -> 0x%08x, A -> 0x%02x\n", 0x00, addr);
        #endif
        //printf("Memory: Read: D -> 0x%08x, C -> %c, A -> 0x%02x\n", 0x00, 0x00, addr);
        return 0x00;
    }
}

void MEMORY::test()
{
    std::cout << "--------------------------------------------------------\n";
    std::cout << "----------------------TESTING MEMORY--------------------\n";
    std::cout << "--------------------------------------------------------\n";

    std::cout << "Writting 50MB 8bit data...";
    for(unsigned int i = 0;i < 50000000;i++)
    {
        write((i&0xff), i);
    }
    std::cout << ANSI_COLOR_GREEN " OK" ANSI_COLOR_RESET "\n";
    std::cout << "Verifying 50MB 8bit data...";
    bool integrity = true;
    for(unsigned int i = 0;i < 50000000;i++)
    {
        if(read(i) != (i&0xff))
        {
            integrity = false;
            std::cout << ANSI_COLOR_RED " ERROR" ANSI_COLOR_RESET "\n";
            printf(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "wrong data at address: 0x%08x\n", i);
            printf("\t\texpected 0x%02x but got 0x%02x\n", (i&0xff), read(i));
            if((i & 0b11) != 0)
            {
                printf("unaligned: " ANSI_COLOR_YELLOW "YES\n" ANSI_COLOR_RESET);

            }
            //exit(1);
            break;
        }
    }
    if(integrity)
    {
        std::cout << ANSI_COLOR_GREEN " OK" ANSI_COLOR_RESET "\n";
    }
    std::cout << "Clearing 50MB 8bit data...";
    for(unsigned int i = 0;i < 50000000;i++)
    {
        write((i&0xff), 0);
    }
    std::cout << ANSI_COLOR_GREEN " OK" ANSI_COLOR_RESET "\n";

    std::cout << "Writting 50MB 16bit data...";
    for(unsigned int i = 0;i < 50000000;i++)
    {
        write16((i&0xffff), i);
    }
    for(unsigned int i = 0;i < 50000000;i++)
    {
        if(read16(i) != (i&0xffff))
        {
            integrity = false;
            std::cout << ANSI_COLOR_RED " ERROR" ANSI_COLOR_RESET "\n";
            printf(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "wrong data at address: 0x%08x\n", i);
            printf("\t\texpected 0x%04x but got 0x%04x\n", (i&0xffff), read16(i));
            if((i & 0b11) != 0)
            {
                printf("unaligned: " ANSI_COLOR_YELLOW "YES\n" ANSI_COLOR_RESET);
            }
            break;
        }
    }
    if(integrity)
    {
        std::cout << ANSI_COLOR_GREEN " OK" ANSI_COLOR_RESET "\n";
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "-------------------FINISH TESTING MEMORY----------------\n";
    std::cout << "--------------------------------------------------------\n";
    //exit(0);
}