include $(bao_demos)/guests/baremetal/make.mk

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin

app_src_dir:=$(bao_demos)/demos/baremetal/src
baremetal_args:=APP_SRC_DIR=$(app_src_dir)

ifeq ($(ARCH_PROFILE),armv8-r)
baremetal_args+=MEM_BASE=0x10000000
fvpr_image_data:=$(baremetal_image)@0x10000000
endif

$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(baremetal_image)
