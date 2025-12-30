uboot_repo:=https://github.com/u-boot/u-boot.git
uboot_version:=v2025.10
uboot_src:=$(wrkdir_src)/u-boot

$(uboot_src):
	git clone --depth 1 --branch $(uboot_version) $(uboot_repo) $(uboot_src)

define build-uboot
$(strip $1): $(uboot_src)
	$(MAKE) -C $(uboot_src) $(strip $2)
	echo $(strip $3) >> $(uboot_src)/.config
	$(MAKE) -C $(uboot_src) -j$(nproc) 
	cp $(uboot_src)/u-boot.bin $$@
endef

u-boot: $(wrkdir_plat_imgs)/u-boot.bin
