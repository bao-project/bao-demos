include $(bao_demos)/guests/baremetal/make.mk

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin

app_src_dir:=$(bao_demos)/demos/baremetal/src
baremetal_args:=APP_SRC_DIR=$(app_src_dir)

ifeq ($(ARCH_PROFILE),armv8-r)
ifneq ($(filter fvp-r fvp-r-aarch32,$(PLATFORM)),)
baremetal_args+=MEM_BASE=0x10000000
fvpr_image_data:=$(baremetal_image)@0x10000000
else ifeq ($(PLATFORM),e3650)
baremetal_args+=MEM_BASE=0x00B00000 MEM_SIZE=0x00100000
# Intentionally use default values for S32Z270 
endif
endif

$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(baremetal_image)
