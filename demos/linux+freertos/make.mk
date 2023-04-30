include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/freertos/make.mk

linux_image=$(wrkdir_demo_imgs)/linux.bin
linux_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux.dts
$(eval $(call build-linux, $(linux_image), $(linux_dts)))

freertos_image:=$(wrkdir_demo_imgs)/freertos.bin

ifeq ($(ARCH_PROFILE),armv8-r)
make_args:=MEM_BASE=0x10000000
fvpr_image_data:=$(freertos_image)@0x10000000 $(linux_image)@0x20000000
else
make_args:=STD_ADDR_SPACE=y
endif

$(eval $(call build-freertos, $(freertos_image), $(make_args)))

guest_images:=$(linux_image) $(freertos_image)
