CFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lpthread -g `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

all: terminal.o core.o memory.o device.o bus.o display.o ext_mem.o timer.o elf.o elf_tables.o graphics.o registers.o console.o log.o riscv_emu

terminal.o: terminal/terminal.cpp terminal/terminal.h debug.h
	g++ terminal/terminal.cpp -c

core.o: core/core.cpp core/core.h core/inset.h debug.h
	g++ core/core.cpp -c

memory.o: memory/memory.cpp memory/memory.h debug.h
	g++ memory/memory.cpp -c

bus.o: bus/bus.cpp bus/bus.h debug.h
	g++ bus/bus.cpp -c

device.o: device/device.cpp device/device.h debug.h
	g++ device/device.cpp -c

display.o: display/display.cpp display/display.h debug.h
	g++ display/display.cpp $(CFLAGS) -c

ext_mem.o: ext_mem/ext_mem.cpp ext_mem/ext_mem.h
	g++ ext_mem/ext_mem.cpp -c

timer.o: timer/timer.h timer/timer.cpp
	g++ timer/timer.cpp -c

elf.o: elf/elf.h elf/elf.cpp
	g++ elf/elf.cpp elf/elf_tables.cpp -c

elf_tables.o:
	g++ elf/elf_tables.cpp -c

graphics.o: graphics/graphics.cpp graphics/graphics.h
	g++ graphics/graphics.cpp -c

registers.o: graphics/registers.cpp graphics/registers.h
	g++ graphics/registers.cpp $(CFLAGS) -c

console.o: graphics/console.h graphics/console.cpp
	g++ graphics/console.cpp $(CFLAGS) -c

log.o: graphics/log.h graphics/log.cpp
	g++ graphics/log.cpp $(CFLAGS) -c

riscv_emu: main.cpp terminal.o core.o memory.o device.o bus.o display.o ext_mem.o timer.o debug.h elf.o elf_tables.o graphics.o registers.o console.o log.o
	g++ main.cpp terminal.o memory.o core.o bus.o device.o display.o ext_mem.o timer.o elf.o elf_tables.o graphics.o registers.o  console.o log.o $(CFLAGS) -o riscv_emu


clean:
	rm *.o
	rm ROM.bin
	rm riscv_emu

ROM.bin: script.ld test.c test.s crt0.s timer.s 
	#riscv64-unknown-elf-as test.s  -march=rv32i -o test.o
	#riscv64-unknown-elf-ld -L="/mnt/c/Users/camin/Documents/opt/riscv/lib/gcc/riscv64-unknown-elf/10.2.0/rv32i/ilp32" -b elf32-littleriscv -m elf32lriscv -T script.ld test.o crt0.o -o test_linked.o
	riscv64-unknown-elf-gcc -Wl,--print-memory-usage -T script.ld -lm -lgcc -static -march=rv32im -mabi=ilp32 -nostartfiles -ffreestanding -fpic -g crt0.s timer.s -O0 test.c -o test_linked.o
	riscv64-unknown-elf-objcopy -F elf32-littleriscv test_linked.o -O binary ROM.bin
	python3 generate_rom.py > ROM.hex

#xxd ROM.bin
test: ROM.bin riscv_emu
	#riscv64-unknown-elf-objdump -ds test_linked.o
	./riscv_emu test_linked.o