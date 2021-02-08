# riscv-emu
RISCV RV32I emulator written in C++ and using SMFL for handling the window. The emulator support the RV32I ISA extention and the M-Mode Privilege mode with timer interrupt support. Memory Protection Register are not implemented.

## Dependecies
```
sudo apt-get update
sudo apt-get install g++
sudo apt-get install libsfml-dev
```
## RISC-V Toolchain
RISCV Toolchain in order to compiler the tests
```
sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
./configure --prefix=/opt/riscv --enable-multilib
make -j$(nproc)
```


## Compile & Test
```
  make test
```

<div>
  <p align="center">
    <img src="img/img1.PNG" width="400" height="400"> </img>    
    <img src="img/img2.PNG" width="400" height="400"> </img>    
  </p>
</div>

### RISCV Code
The code that the emulator will be executing is on crt0.s, timer.s and test.c which includes the main function and exeption handler written in C.

### Known Issues.
The timer interrupts are not working correctly and if enabled the emulator will glitch on a infinite loop. 
