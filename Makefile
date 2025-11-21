CC = gcc
AS = as
LD = ld

CFLAGS = -ffreestanding -m64 -mno-red-zone -fno-builtin -fno-stack-protector -nostdlib -Wall -Wextra
# ASFLAGS = --64

SRC_DIR = src
BUILD_DIR = build

KERNEL = $(BUILD_DIR)/kernel.bin

C_SOURCES = $(wildcard $(SRC_DIR)/kernel/*.c)
ASM_SOURCES = $(wildcard $(SRC_DIR)/boot/*.S)
OBJ = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) $(ASM_SOURCES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

all: $(KERNEL)

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

run: $(KERNEL)
	qemu-system-x86_64 -kernel $(KERNEL) -nographic

.PHONY: all clean run
