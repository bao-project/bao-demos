include $(bao_demos)/guests/zephyr/make.mk
include $(bao_demos)/guests/baremetal/make.mk

zephyr_image:=$(wrkdir_demo_imgs)/zephyr.bin
$(eval $(call build-zephyr, $(zephyr_image)))

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin
baremetal_args:=APP_SRC_DIR=$(bao_demos)/demos/zephyr+baremetal/baremetal-app

ifeq ($(ARCH_PROFILE),armv8-r)
baremetal_args+=MEM_BASE=0x10000000
fvpr_image_data:=$(baremetal_image)@0x10000000 $(zephyr_image)@0x24000000
baremetal_args+=SHMEM_BASE=0x70000000 SHMEM_SIZE=0x200000
else
baremetal_args+=STD_ADDR_SPACE=y
baremetal_args+=SHMEM_BASE=0xf0000000 SHMEM_SIZE=0x200000
endif
$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(zephyr_image) $(baremetal_image)
