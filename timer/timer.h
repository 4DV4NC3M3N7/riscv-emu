#ifndef __TIMER_H__
#define __TIMER_H__

#include "../device/device.h"
#include <mutex>
#include <atomic>


//Timer memory MAP
//0x00000000 TIME L
//0x00000004 TIME H
//0x00000008 CMP L
//0x0000000c CMP H


class TIMER: public device
{

public:
    TIMER(uint32_t min_address, uint32_t max_address);
    uint8_t read(uint32_t addr);
    uint32_t read32(uint32_t addr);
    void write(uint8_t data, uint32_t addr);
    void write32(uint32_t data, uint32_t addr);
    void timer_handle();
    std::atomic<bool> interrupt;
    std::mutex interrupt_lock;
    struct
    {
        union
        {
            uint8_t time[sizeof(uint64_t)];
            uint8_t timecmp[sizeof(uint64_t)];
        } raw;
        
        std::atomic<uint64_t> time;
        std::atomic<uint64_t> timecmp;
    }timers;
private:
    bool once;
};


#endif