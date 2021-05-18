#include "device.h"

device::device(/* args */)
{
    
}

device::~device()
{
}

void device::write(uint8_t data, uint32_t address)
{

}

void device::write16(uint16_t data, uint32_t address)
{
    printf("WRITE16 not implemented for device at address 0x%08x\n", address);
}

void device::write32(uint32_t data, uint32_t address)
{
    printf("WRITE32 not implemented for device at address 0x%08x\n", address);
}

uint8_t device::read(uint32_t address)
{
    return 0x00;
}

uint16_t device::read16(uint32_t address)
{
    return 0x00;
}

uint32_t device::read32(uint32_t address)
{
    printf("READ32 not implemented for device at address 0x%08x\n", address);
    return 0x00;
}

void device::print()
{
    std::cout << "max_address -> " << max_address << std::endl;
    std::cout << "min_address -> " << min_address << std::endl;
}

void device::handle()
{
    
}