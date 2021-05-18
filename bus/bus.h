#ifndef __BUS_H__
#define __BUS_H__

#include <stdint.h>
#include <vector>
#include "../device/device.h"

class BUS
{
    public:
        void add(device* device);
        void write(uint8_t data, uint32_t address);
        void write32(uint32_t data, uint32_t address);
        void write16(uint16_t data, uint32_t address);
        uint8_t read(uint32_t address);
        uint16_t read16(uint32_t address);
        uint32_t read32(uint32_t address);

    private:
        std::vector<device*> devices;
};

#endif