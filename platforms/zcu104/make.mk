ARCH:=aarch64

boot_bin:=$(wrkdir_plat_imgs)/BOOT.BIN
prebuilt_images_src:=$(wrkdir_src)/zcu-firmware
prebuilt_images_repo:=https://github.com/Xilinx/soc-prebuilt-firmware.git
prebuilt_images_version:=xilinx_v2023.1
instuctions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
bao_uboot_image:=$(wrkdir_demo_imgs)/bao.img

include $(bao_demos)/platforms/uboot.mk
uboot_image:=$(wrkdir_plat_imgs)/u-boot.bin
uboot_zcu_defconfig:=xilinx_zynqmp_virt_defconfig
$(eval $(call build-uboot, $(uboot_image), $(uboot_zcu_defconfig)))

$(prebuilt_images_src): $(uboot_image)
	@git clone --depth 1 --branch $(prebuilt_images_version) $(prebuilt_images_repo) $@
	@cp $(wrkdir_plat_imgs)/u-boot.elf $@/$(PLATFORM)-zynqmp/

$(boot_bin): $(prebuilt_images_src)
	@cd $</$(PLATFORM)-zynqmp && bootgen -arch zynqmp -image bootgen.bif -w -o $@

$(bao_uboot_image): $(bao_image)
	@mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
		-e 0x200000 -d $(bao_image) $@
 
platform: $(bao_image) $(boot_bin) $(bao_uboot_image) 
	$(call print-instructions, $(instuctions), 1, false)
	$(call print-instructions, $(instuctions), 2, false)
	$(call print-instructions, $(instuctions), 3, true)
