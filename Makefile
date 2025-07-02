# NeoNovaOS Complete Build System

# Tools
ASM = nasm
CC = gcc
LD = ld
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Icore -Ikernel64/include -Idrivers/unified_driver_framework -Icore/arch/x86_64 -Icore/jit -Icore/power_manager -Icore/resource_manager -Icore/app_runtime -Icore/dev_tools -Icore/driver_manager -Iutils -Icloud -Iiot -Icompatibility -Igui -Inetwork -Igaming -Ikernel32/include
LDFLAGS = -T boot/linker32.ld -nostdlib -z max-page-size=0x1000

# Directories
SRC_DIRS = core core/arch/arm core/arch/riscv core/arch/photonic core/arch/x86_64 core/jit core/power_manager core/resource_manager core/app_runtime core/dev_tools core/driver_manager drivers/unified_driver_framework drivers/input drivers/framebuffer kernel64 gui gui/desktop gui/widgets network gaming cloud iot compatibility/virtualization compatibility/android compatibility/linux compatibility/windows compatibility/macos utils security

# Find all C source files
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:.c=.o)

# Bootloader
BOOT_OBJ = boot/boot32.o
KERNEL_LD = boot/linker32.ld

# Output
KERNEL_BIN = kernel64/NeoNovaOS.bin
ISO = NeoNovaOS.iso

all: $(KERNEL_BIN)

$(BOOT_OBJ): boot/boot32.asm
	$(ASM) -f elf32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(BOOT_OBJ) $(OBJS) $(KERNEL_LD)
	$(LD) -m elf_i386 -T $(KERNEL_LD) -o $@ $(BOOT_OBJ) $(OBJS)

iso: $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/NeoNovaOS.bin
	echo 'set timeout=0' > isodir/boot/grub/grub.cfg
	echo 'menuentry "NeoNovaOS" { multiboot /boot/NeoNovaOS.bin }' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir

run: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -f $(OBJS) $(BOOT_OBJ) $(KERNEL_BIN)
	rm -rf isodir $(ISO)

.PHONY: all clean iso run
