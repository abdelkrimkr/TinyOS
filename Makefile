
ASM     = nasm
CC      = gcc
LD      = ld
CFLAGS  = -ffreestanding -O2 -Wall -Wextra -m32 -g -I$(SRC)
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib -g
QEMU    = qemu-system-i386

BUILD   = build
SRC     = src

OBJS = $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/rtc.o $(BUILD)/idt.o $(BUILD)/keyboard.o $(BUILD)/terminal.o $(BUILD)/graphics.o $(BUILD)/log.o $(BUILD)/idt_asm.o

all: $(BUILD)/os.iso

$(BUILD)/os.iso: $(BUILD)/kernel.elf grub.cfg
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(BUILD)/kernel.elf $(BUILD)/iso/boot/
	cp grub.cfg $(BUILD)/iso/boot/grub/
	grub-mkrescue --isohybrid-gpt-basdat -o $(BUILD)/os.iso $(BUILD)/iso
	@isoinfo -d -i $(BUILD)/os.iso

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

$(BUILD)/log.o: $(SRC)/log.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	$(QEMU) -cdrom $(BUILD)/os.iso -m 512M -nographic

rebuild: clean all

clean:
	rm -rf $(BUILD)

.PHONY: all clean run rebuild
