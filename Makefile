
ASM     = nasm
CC      = gcc
LD      = ld
CFLAGS  = -ffreestanding -O2 -Wall -Wextra -m32 -g -I$(SRC)
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib -g
QEMU    = qemu-system-i386

BUILD   = build
SRC     = src

OBJS = $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/rtc.o $(BUILD)/idt.o $(BUILD)/keyboard.o $(BUILD)/terminal.o $(BUILD)/graphics.o $(BUILD)/idt_asm.o

all: $(BUILD)/os.iso

$(BUILD)/os.iso: $(BUILD)/kernel.elf grub.cfg
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(BUILD)/kernel.elf $(BUILD)/iso/boot/
	cp grub.cfg $(BUILD)/iso/boot/grub/
	grub2-mkrescue -o $(BUILD)/os.iso $(BUILD)/iso 2>/dev/null || \
		xorriso -as mkisofs -R -J -o $(BUILD)/os.iso $(BUILD)/iso

$(BUILD)/kernel.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $(BUILD)/kernel.elf $(OBJS)

$(BUILD)/boot.o: $(SRC)/boot.s
	mkdir -p $(BUILD)
	$(ASM) -f elf32 $< -o $@

$(BUILD)/idt_asm.o: $(SRC)/idt.s
	mkdir -p $(BUILD)
	$(ASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: $(SRC)/kernel.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/rtc.o: $(SRC)/rtc.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/idt.o: $(SRC)/idt.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/keyboard.o: $(SRC)/keyboard.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/terminal.o: $(SRC)/terminal.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/graphics.o: $(SRC)/graphics.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	$(QEMU) -cdrom $(BUILD)/os.iso -m 512M -nographic

rebuild: clean all

clean:
	rm -rf $(BUILD)

.PHONY: all clean run rebuild
