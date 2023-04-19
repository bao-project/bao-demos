ARCH:=aarch64

firmware_repo:=https://github.com/raspberrypi/firmware.git
firmware_version:=1.20230405
firmware_images:=$(wrkdir_plat_imgs)/firmware

$(firmware_images):
	git clone --depth 1 --branch $(firmware_version) $(firmware_repo) $@

include $(bao_demos)/platforms/uboot.mk
uboot_defconfig:=rpi_4_defconfig
uboot_image:=$(wrkdir_plat_imgs)/u-boot.bin
$(eval $(call build-uboot, $(uboot_image), $(uboot_defconfig)))

atf_image:=$(wrkdir_plat_imgs)/bl31.bin
atf_plat:=rpi4
include $(bao_demos)/platforms/atf.mk
$(eval $(call build-atf, $(atf_image), $(atf_plat)))

instuctions:=$(bao_demos)/platforms/$(PLATFORM)/README.md

platform: $(bao_image) $(uboot_image) $(atf_image)  $(firmware_images) 
	$(call print-instructions, $(instuctions), 1, false)
	$(call print-instructions, $(instuctions), 2, false)
	$(call print-instructions, $(instuctions), 3, true)
