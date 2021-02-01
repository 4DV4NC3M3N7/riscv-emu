#ifndef __CLIC_H__
#define __CLIC_H__

#include <vector>
#include <atomic>
#include <queue>

/*
M-mode CLIC memory map
  Offset
  ### 0x0008-0x07FF reserved ###
  ### 0x0800-0x0FFF custom ###
  0x0000 1B RW cliccfg
  0x0004 4B R clicinfo
  0x0040 4B RW clicinttrig[0]
  0x0044 4B RW clicinttrig[1]
  0x0048 4B RW clicinttrig[2]
  ...
  0x00B4 4B RW clicinttrig[29]
  0x00B8 4B RW clicinttrig[30]
  0x00BC 4B RW clicinttrig[31]
  0x1000+4*i 1B/input R or RW clicintip[i]
  0x1001+4*i 1B/input RW clicintie[i]
  0x1002+4*i 1B/input RW clicintattr[i]
  0x1003+4*i 1B/input RW clicintctl[i]
  ...
  0x4FFC 1B/input R or RW clicintip[4095]
  0x4FFD 1B/input RW clicintie[4095]
  0x4FFE 1B/input RW clicintattr[4095]
  0x4FFF 1B/input RW clicintctl[4095]
*/


class CLIC
{
private:
    /* data */
public:
    CLIC(/* args */);
    ~CLIC();
};








#endif