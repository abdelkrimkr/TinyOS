CC = gcc
AS = as
LD = ld

CFLAGS = -ffreestanding -m64 -mno-red-zone -fno-builtin -fno-stack-protector -nostdlib -Wall -Wextra
# ASFLAGS = --64

SRC_DIR = src
BUILD_DIR = build

KERNEL = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/os.img

C_SOURCES = $(wildcard $(SRC_DIR)/kernel/*.c)
ASM_SOURCES = $(wildcard $(SRC_DIR)/boot/*.S)
OBJ = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) $(ASM_SOURCES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

# Tool paths (relative to root)
BUSYBOX = ./busybox
MCOPY = ./mcopy
LIMINE = ./limine/limine
LIMINE_SYS = ./limine/limine-bios.sys

all: $(KERNEL)

image: $(IMAGE)

$(IMAGE): $(KERNEL) limine.cfg
	# Create a 64MB blank image
	dd if=/dev/zero of=$@ bs=1M count=64
	# Format as FAT32
	$(BUSYBOX) mkfs.vfat -F 32 $@
	# Copy kernel, config, and limine-bios.sys
	$(MCOPY) -i $@ $(KERNEL) ::/kernel.bin
	$(MCOPY) -i $@ limine.cfg ::/limine.cfg
	$(MCOPY) -i $@ $(LIMINE_SYS) ::/limine-bios.sys
	# Install Limine bootloader
	$(LIMINE) bios-install --force-mbr $@

$(KERNEL): $(OBJ)
	$(LD) -n -o $@ -T linker.ld $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: $(IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE) -nographic

.PHONY: all clean run image
