include $(bao_demos)/guests/nuttx/make.mk
include $(bao_demos)/guests/baremetal/make.mk

nuttx_image:=$(wrkdir_demo_imgs)/nuttx.bin
$(eval $(call build-nuttx, $(nuttx_image)))

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin
baremetal_args:=APP_SRC_DIR=$(bao_demos)/demos/nuttx+baremetal/baremetal-app

ifeq ($(ARCH_PROFILE),armv8-r)
baremetal_args+=MEM_BASE=0x00CB0000
baremetal_args+=MEM_SIZE=0x00020000
baremetal_args+=SHMEM_BASE=0x00CD0000 SHMEM_SIZE=0x20000
else
baremetal_args+=STD_ADDR_SPACE=y
baremetal_args+=SHMEM_BASE=0xf0000000 SHMEM_SIZE=0x200000
endif
$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(nuttx_image) $(baremetal_image)
