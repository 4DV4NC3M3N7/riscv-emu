#include <iostream>
#include "timer.h"
#include "../debug.h"
#include "../memory_map.h"
#include <chrono>
#include <thread>

TIMER::TIMER(uint32_t min_address, uint32_t max_address)
{
    this->min_address = min_address;
    this->max_address = max_address;
}

uint8_t TIMER::read(uint32_t addr)
{
    switch ((addr & 0xf))
    {
        case 0x00000000 ... 0x00000007:
            return timers.raw.time[addr-min_address];
        break;
        case 0x00000008 ... 0x0000000f: 
            return timers.raw.timecmp[(addr-min_address)-8];
        break;
    default: return 0x00;
        break;
    }
}

uint32_t TIMER::read32(uint32_t addr)
{
    switch ((addr & 0xf))
    {
        case 0x00000000 ... 0x00000003:
            return (timers.time&0xffffffff);
        break;
        case 0x00000004 ... 0x00000007:
            return ((timers.time >> 32) & 0xffffffff);
        break;
        case 0x00000008 ... 0x0000000b: 
            return (timers.timecmp&0xffffffff);
        break;
        case 0x0000000c ... 0x0000000f: 
            return ((timers.timecmp >> 32) & 0xffffffff);
        break;
    default: return 0x00;
        break;
    }
}

void TIMER::write(uint8_t data, uint32_t addr)
{
    switch ((addr-min_address))
    {
        case 0x00000000 ... 0x00000007:
            timers.raw.time[addr-min_address] = data;
        break;
        case 0x00000008 ... 0x0000000f: 
            timers.raw.timecmp[(addr-min_address)-8] = data;
        break;
    default:
        break;
    }
}

void TIMER::write32(uint32_t data, uint32_t addr)
{
    switch ((addr & 0xf))
    {
        case 0x00000000 ... 0x00000003:
        break;
        case 0x00000004 ... 0x00000007:
        break;
        case 0x00000008 ... 0x0000000b: 
            timers.timecmp = ((timers.timecmp & 0xffffffff00000000) | data);
        break;
        case 0x0000000c ... 0x0000000f: 
            timers.timecmp = ((timers.timecmp & 0x00000000ffffffff) | ((uint64_t)data << 32));
        break;
    default:
        break;
    }
}

void TIMER::timer_handle()
{
    timers.timecmp;
    timers.time++;
}