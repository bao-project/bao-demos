
ifeq ($(BAO_BASE_ADDR),)
$(error BAO_BASE_ADDR and BAO_IMG need to be defined)
endif

trampline_dir:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
trampline_elf:=$(OUTPUT_DIR)/trampline.elf
TRAMPLINE_IMG?=$(OUTPUT_DIR)/trampline.bin

all: $(TRAMPLINE_IMG)

$(trampline_elf): linker.ld trmp.S
	cd $(trampline_dir) && $(CROSS_COMPILE)gcc \
		-Wl,-build-id=none -nostdlib \
		-DBAO_BASE_ADDR=$(BAO_BASE_ADDR) \
		trmp.S -o $@

$(TRAMPLINE_IMG): $(trampline_elf)
	cd $(trampline_dir) && $(CROSS_COMPILE)objcopy -O binary $< $@
