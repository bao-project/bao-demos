include $(bao_demos)/guests/linux/make.mk

linux1_image=$(wrkdir_demo_imgs)/linux1.bin
linux1_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux1.dts
$(eval $(call build-linux, $(linux1_image), $(linux1_dts)))

linux2_image=$(wrkdir_demo_imgs)/linux2.bin
linux2_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux2.dts
$(eval $(call build-linux, $(linux2_image), $(linux2_dts)))

guest_images:=$(linux1_image) $(linux2_image)