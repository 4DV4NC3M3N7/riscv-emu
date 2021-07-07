
all:
	riscv64-unknown-elf-gcc -Wl,--print-memory-usage -T ../script.ld -lm -lgcc -static -march=rv32im -mabi=ilp32 -nostartfiles -ffreestanding -fpic -g -O0 rv32i_gen.s -o test_linked.o


clean:
	rm rv32i_gen.s
	rm test_linked.o