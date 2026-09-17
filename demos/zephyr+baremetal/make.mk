include $(bao_demos)/guests/zephyr/make.mk
include $(bao_demos)/guests/baremetal/make.mk

zephyr_image:=$(wrkdir_demo_imgs)/zephyr.bin
$(eval $(call build-zephyr, $(zephyr_image)))

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin
baremetal_args:=APP_SRC_DIR=$(bao_demos)/demos/zephyr+baremetal/baremetal-app

ifeq ($(ARCH_PROFILE),armv8-r)
ifneq ($(filter fvp-r fvp-r-aarch32,$(PLATFORM)),)
baremetal_args+=MEM_BASE=0x10000000
fvpr_image_data:=$(baremetal_image)@0x10000000 $(zephyr_image)@0x24000000
baremetal_args+=SHMEM_BASE=0x70000000 SHMEM_SIZE=0x200000
else ifneq ($(findstring $(PLATFORM), mps3-an536),)
baremetal_args+=SHMEM_BASE=0x70000000 MEM_BASE=0x32200000 SHMEM_SIZE=0x200000
qemu_loader_data:=$(baremetal_image)@0x32200000 $(zephyr_image)@0x40000000
else
baremetal_args+=SHMEM_BASE=0x31840000 SHMEM_SIZE=0x4000
endif
else ifeq ($(PLATFORM),tc4dx)
# Second of two hw-VMs sharing the chip (Zephyr owns core 0 and ASCLIN0).
baremetal_args+=TC4DX_ASCLIN=1
baremetal_args+=MASTER_CPU=1 PLAT_CPU_NUM=5
# VM_ID feeds plat.h's TIMER_IRQ_ID formula -- must be 2 here to land on
# this VM's own STM line (28) instead of Zephyr's (26, the VM_ID=1 default).
baremetal_args+=VM_ID=2
baremetal_args+=SHMEM_BASE=0x90280000 SHMEM_SIZE=0x10000
# tc4dx can't share an IPC-notify IRQ id between VMs (no per-VM virtual IRQ
# space) -- must match configs/tc4dx.c's second .ipcs entry.
baremetal_args+=SHMEM_IRQ_ID=1361
else
baremetal_args+=STD_ADDR_SPACE=y
baremetal_args+=SHMEM_BASE=0xf0000000 SHMEM_SIZE=0x200000
endif
$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(zephyr_image) $(baremetal_image)
