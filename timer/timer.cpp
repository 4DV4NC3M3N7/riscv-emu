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
    interrupt_lock = new std::mutex;
}

uint8_t TIMER::read(uint32_t addr)
{
    #ifdef __DEBUG__
        printf("Timer: Read: D -> 0x%08x, A -> 0x%02x\n", time[(addr & 0xf)], addr);
        printf("Timer: Read: D -> 0x%08x, A -> 0x%02x\n", timecmp[(addr & 0xf)], addr);
    #endif
    switch ((addr & 0xf))
    {
        case 0x00000000 ... 0x00000007:
            return time[addr-min_address];
        break;
        case 0x00000008 ... 0x0000000f: 
            return timecmp[(addr-min_address)-8];
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
            time[addr-min_address] = data;
        break;
        case 0x00000008 ... 0x0000000f: 
            timecmp[(addr-min_address)-8] = data;
        break;
    default:
        break;
    }
    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(2000ms);
}

void TIMER::timer_handle()
{
    //Iterate a little bit to get a good operating frequency
    uint64_t time_64 = 0;
    uint64_t timecmp_64 = 0;
    int x = 0;
    for(int i = 0;i <= 64;i = i + 8)
    {
        time_64 |= time[x] << i;
        timecmp_64 |= timecmp[x] << i;
        x++;
    }
    time_64++;
    if(timecmp_64 <= time_64)
    {
        if(interrupt_lock->try_lock())
        {
            interrupt = true;
            interrupt_lock->unlock();
        }
    }
    else
    {
        if(interrupt_lock->try_lock())
        {
            interrupt = false;
            interrupt_lock->unlock();
        }
    }
    
    x = 0;
    for(int i = 0;i <= 64;i = i + 8)
    {
        timecmp[x] = ((timecmp_64 >> i) & 0xff);
        time[x] = ((time_64 >> i) & 0xff);
        x++;
    }
    //printf("time %d\n", time_64);
    //printf("timecmp %08x\n", (uint64_t)timecmp_64);
    
    //std::cout.flush();
}