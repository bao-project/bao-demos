include $(bao_demos)/guests/baremetal/make.mk

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin

app_src_dir:=$(bao_demos)/demos/baremetal/src
baremetal_args:=APP_SRC_DIR=$(app_src_dir)

ifneq ($(findstring $(PLATFORM), fvp-r fvp-r-aarch32),)
baremetal_args+=MEM_BASE=0x10000000
fvpr_image_data:=$(baremetal_image)@0x10000000
endif

ifneq ($(findstring $(PLATFORM), mps3-an536),)
baremetal_args+=MEM_BASE=0x32200000
qemu_loader_data:=$(baremetal_image)@0x32200000
endif


$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(baremetal_image)
