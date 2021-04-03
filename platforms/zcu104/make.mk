ARCH:=aarch64

boot_bin:=$(wrkdir_plat_imgs)/BOOT.BIN
prebuilt_images_tar:=$(wrkdir_src)/2020.2-$(PLATFORM)-release.tar.xz
instuctions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
bao_uboot_image:=$(wrkdir_demo_imgs)/bao.img

$(prebuilt_images_tar):
	$(call print-instructions, $(instuctions), 1, false)

$(boot_bin): $(prebuilt_images_tar)
	@tar xfvm $< -C $(dir $@) --wildcards "*BOOT.BIN" --strip-components=1

$(bao_uboot_image): $(bao_image)
	@mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
		-e 0x200000 -d $(bao_image) $@
 
platform: $(bao_image) $(boot_bin) $(bao_uboot_image) 
	$(call print-instructions, $(instuctions), 2, false)
	$(call print-instructions, $(instuctions), 3, false)
	$(call print-instructions, $(instuctions), 4, true)