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
    #ifdef __DEBUG__
        //printf("Timer: Read: D -> 0x%08x, A -> 0x%02x\n", timecmp[(addr & 0xf)], addr);
        //printf("Timer: Read: D -> 0x%08x, A -> 0x%02x\n", timecmp[(addr & 0xf)], addr);
    #endif
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
    #ifdef __DEBUG__
        //printf("Timer: Read: D -> 0x%08x, A -> 0x%02x\n", timecmp[(addr & 0xf)], addr);
    #endif

    switch ((addr & 0xf))
    {
        case 0x00000000 ... 0x00000003:
            printf("Timer32: Read: D -> 0x%08x, A -> 0x%02x\n", (timers.time&0xffffffff), addr);
            return (timers.time&0xffffffff);
        break;
        case 0x00000004 ... 0x00000007:
            printf("Timer32: Read: D -> 0x%08x, A -> 0x%02x\n", ((timers.time >> 32) & 0xffffffff), addr);
            return ((timers.time >> 32) & 0xffffffff);
        break;
        case 0x00000008 ... 0x0000000b: 
            printf("Timer32: Read: D -> 0x%08x, A -> 0x%02x\n", (timers.timecmp&0xffffffff), addr);
            return (timers.timecmp&0xffffffff);
        break;
        case 0x0000000c ... 0x0000000f: 
            printf("Timer32: Read: D -> 0x%08x, A -> 0x%02x\n", ((timers.timecmp >> 32) & 0xffffffff), addr);
            return ((timers.timecmp >> 32) & 0xffffffff);
        break;
    default: return 0x00;
        break;
    }
}

void TIMER::write(uint8_t data, uint32_t addr)
{
    #ifdef __DEBUG__
        printf("Timer: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    
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
    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(2000ms);
}

void TIMER::write32(uint32_t data, uint32_t addr)
{
    #ifdef __DEBUG__
        printf("Timer32: Write: D -> 0x%08x, A -> 0x%02x\n", data, addr);
    #endif
    
    switch ((addr & 0xf))
    {
        case 0x00000000 ... 0x00000003:
            //timers.time = ((timers.time & 0xffffffff00000000) | data);
        break;
        case 0x00000004 ... 0x00000007:
            //timers.time = ((timers.time & 0x00000000ffffffff) | (data << 32));
        break;
        case 0x00000008 ... 0x0000000b: 
            timers.timecmp = ((timers.timecmp & 0xffffffff00000000) | data);
        break;
        case 0x0000000c ... 0x0000000f: 
            //printf("Timer: Write: D -> 0x%08llx, A -> 0x%02x\n", ((uint64_t)(timers.timecmp & 0x00000000ffffffff) | ((uint64_t)data << 32)), addr);
            timers.timecmp = ((timers.timecmp & 0x00000000ffffffff) | ((uint64_t)data << 32));
        break;
    default:
        break;
    }
    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(2000ms);
}

void TIMER::timer_handle()
{
    timers.timecmp;
    timers.time++;
    
    if((uint64_t)timers.timecmp <= (uint64_t)timers.time)
    {
        //printf("timecmp 0x%016llx time 0x%016llx\n", (uint64_t)timers.timecmp, (uint64_t)timers.time);
        //printf("Setting to true\n");
        //printf("timecmp 0x%016x\n", timers.timecmp);
        //printf("time 0x%016x\n", timers.time);
        //interrupt_lock.lock();
        interrupt.store(true, std::memory_order_release);
        //interrupt_lock.unlock();

    }
    else
    {
        //printf("Setting to false\n");
        interrupt.store(false, std::memory_order_release);
    }
}