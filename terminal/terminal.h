#ifndef __TERMINAL_H__
#define __TERMINAL_H__


#include "../device/device.h"
#include <queue>

//Memory Map
//0x00000000 Data O
//0x00000004 Data I
//0x00000008 Size

class TERMINAL: public device
{

public:
    TERMINAL(uint32_t min_address, uint32_t max_address);
    uint8_t read(uint32_t addr);
    void write(uint8_t data, uint32_t addr);

    //External thread handles input and output
    std::queue<char> input_buffer;
private:

};

#endif