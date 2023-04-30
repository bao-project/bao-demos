include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/zephyr/make.mk

linux_image=$(wrkdir_demo_imgs)/linux.bin
linux_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux.dts
$(eval $(call build-linux, $(linux_image), $(linux_dts)))

zephyr_image:=$(wrkdir_demo_imgs)/zephyr.bin
$(eval $(call build-zephyr, $(zephyr_image)))

ifeq ($(ARCH_PROFILE),armv8-r)
fvpr_image_data:=$(linux_image)@0x28000000 $(zephyr_image)@0x24000000
endif

guest_images:=$(linux_image) $(zephyr_image)
