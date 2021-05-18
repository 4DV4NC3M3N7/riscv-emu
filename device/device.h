#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>
#include <iostream>

class device
{
public:
    device(/* args */);
    ~device();

    virtual void write(uint8_t data, uint32_t address);
    virtual void write32(uint32_t data, uint32_t address);
    virtual void write16(uint16_t data, uint32_t address);
    virtual uint8_t read(uint32_t address);
    virtual uint16_t read16(uint32_t address);
    virtual uint32_t read32(uint32_t address);
    virtual void print();
    virtual void handle();
    /* data */
    int32_t max_address;
    int32_t min_address;
    bool threaded = false;
    private:

};


#endif