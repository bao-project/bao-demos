
ifeq ($(and $(IMAGE), $(DTB), $(TARGET), $(ARCH)),)
ifneq ($(MAKECMDGOALS), clean)
 $(error Linux image (IMAGE) and/or device tree (DTB) and/or target name \
 	(TARGET) and/or architecture (ARCH) not specified)
endif
endif

ARCH?=aarch64
ifeq ($(ARCH), aarch64)
CROSS_COMPILE?=aarch64-none-elf-
OPTIONS=-mcmodel=large 
else ifeq ($(ARCH), riscv)
CROSS_COMPILE?=riscv64-unknown-elf-
OPTIONS=-mcmodel=medany
else
$(error unkown architecture $(ARCH))
endif

LLOADER_ASM:=$(ARCH).S
LLOADER_LD:=$(ARCH).ld
TARGET_ELF:=$(basename $(TARGET)).elf

all: $(TARGET)

clean:
	-rm $(TARGET_ELF) $(TARGET)

.PHONY: all clean
	
$(TARGET): $(TARGET_ELF)
	$(CROSS_COMPILE)objcopy -S -O binary $(TARGET_ELF) $(TARGET)

$(TARGET_ELF): $(LLOADER_ASM) $(IMAGE) $(DTB) $(LLOADER_LD)
	$(CROSS_COMPILE)gcc -Wl,-build-id=none -nostdlib -T $(LLOADER_LD)\
		-o $@ $(OPTIONS) $(LLOADER_ASM) -I. -D IMAGE=$(IMAGE) -D DTB=$(DTB)
