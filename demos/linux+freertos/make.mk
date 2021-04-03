include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/freertos/make.mk

linux_image=$(wrkdir_demo_imgs)/linux.bin
linux_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux.dts
$(eval $(call build-linux, $(linux_image), $(linux_dts)))

freertos_image:=$(wrkdir_demo_imgs)/freertos.bin
$(eval $(call build-freertos, $(freertos_image)))

guest_images:=$(linux_image) $(freertos_image)