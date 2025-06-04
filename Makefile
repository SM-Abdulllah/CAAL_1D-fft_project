# Makefile for RISC-V Vector FFT Project

# Compiler and tools
RISCV_PREFIX = riscv64-linux-gnu-
CC = $(RISCV_PREFIX)gcc
AS = $(RISCV_PREFIX)as
LD = $(RISCV_PREFIX)ld
OBJDUMP = $(RISCV_PREFIX)objdump

# QEMU for emulation
QEMU = qemu-riscv64-static

# Compiler flags
CFLAGS = -O2 -static -march=rv64gcv -mabi=lp64d
ASFLAGS = -march=rv64gcv -mabi=lp64d
LDFLAGS = -static

# Source files
C_SOURCES = fft.c test_fft.c
ASM_SOURCES = fft_vector.s
OBJECTS = fft.o test_fft.o fft_vector.o

# Targets
all: fft_c fft_test

# C-only version
fft_c: fft.c
	$(CC) $(CFLAGS) -o fft_c fft.c -lm

# Combined C+Assembly version
fft_test: test_fft.o fft_vector.o
	$(CC) $(LDFLAGS) -o fft_test test_fft.o fft_vector.o -lm

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Run C version
run-c: fft_c
	$(QEMU) ./fft_c

# Run assembly version
run-asm: fft_test
	$(QEMU) -cpu rv64,v=true,vlen=128 ./fft_test

# Run both versions
run: run-c run-asm

# Disassemble for debugging
disasm: fft_test
	$(OBJDUMP) -d fft_test > fft_test.dump
	@echo "Disassembly saved to fft_test.dump"

# Clean
clean:
	rm -f *.o fft_c fft_test *.dump

# Help
help:
	@echo "RISC-V Vector FFT Project"
	@echo "========================"
	@echo "make all      - Build all targets"
	@echo "make fft_c    - Build C-only version"
	@echo "make fft_test - Build C+Assembly version"
	@echo "make run-c    - Run C version"
	@echo "make run-asm  - Run assembly version"
	@echo "make run      - Run both versions"
	@echo "make disasm   - Disassemble for debugging"
	@echo "make clean    - Clean build files"

.PHONY: all run-c run-asm run disasm clean help
