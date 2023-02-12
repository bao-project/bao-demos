include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/zephyr/make.mk

linux_image=$(wrkdir_demo_imgs)/linux.bin
linux_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux.dts
$(eval $(call build-linux, $(linux_image), $(linux_dts)))

zephyr_image:=$(wrkdir_demo_imgs)/zephyr.bin
$(eval $(call build-zephyr, $(zephyr_image)))

guest_images:=$(linux_image) $(zephyr_image)
