uboot_repo?=https://github.com/u-boot/u-boot.git
uboot_version?=v2025.10
uboot_src:=$(wrkdir_src)/u-boot-$(uboot_version)

$(uboot_src):
	git clone --depth 1 --branch $(uboot_version) $(uboot_repo) $(uboot_src)
ifneq (,$(filter $(PLATFORM),zcu102 zcu104))
	git -C $(uboot_src) apply $(bao_demos)/platforms/$(PLATFORM)/u-boot.patch
endif

# $1: target u-boot.bin path; $2: defconfig; $3: .config fragment to append;
# $4 (optional): extra make arguments for both invocations (e.g. a
# CROSS_COMPILE= override for boards that cannot build with the default
# bare-metal toolchain).
define build-uboot
$(strip $1): $(uboot_src)
	$(MAKE) -C $(uboot_src) $(strip $4) $(strip $2)
	echo $(strip $3) >> $(uboot_src)/.config
	$(MAKE) -C $(uboot_src) $(strip $4) -j$(nproc)
	cp $(uboot_src)/u-boot.bin $$@
	[ -f $(uboot_src)/u-boot.elf ] && cp $(uboot_src)/u-boot.elf $(wrkdir_plat_imgs)/u-boot.elf 2>/dev/null || true
	[ -f $(uboot_src)/u-boot-nodtb.bin ] && cp $(uboot_src)/u-boot-nodtb.bin $(wrkdir_plat_imgs)/$(DEMO)/u-boot-nodtb.bin 2>/dev/null || true
endef

u-boot: $(wrkdir_plat_imgs)/u-boot.bin
