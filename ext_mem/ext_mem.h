#ifndef _EXT_MEM_H_
#define _EXT_MEM_H_

#include "../device/device.h"
#include <stdint.h>

///Block device

//external memory mapped device
class EXT_MEM : public device
{
    public:
        EXT_MEM(uint32_t min_address, uint32_t max_address);
        ~EXT_MEM();
        uint8_t read(uint32_t addr);
        void write(uint8_t data, uint32_t addr);
    private:
        //Block device memory
        char* mem_buffer;
        size_t max_used;
};













#endif