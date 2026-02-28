ARCH:=aarch64

ifndef CROSS_COMPILE32
 $(error No CROSS_COMPILE32 prefix defined)
endif

ifndef CROSS_COMPILE64
 $(error No CROSS_COMPILE64 prefix defined)
endif

atf_repo:=https://github.com/bao-project/arm-trusted-firmware.git
atf_version:=bao/demo
atf_src:=$(wrkdir_src)/arm-trusted-firmware

$(atf_src):
	git clone --branch $(atf_version) --depth 1 $(atf_repo) $@

atf_image:=$(wrkdir_plat_imgs)/bl31.bin
$(atf_image): $(atf_src)
	$(MAKE) -C $(atf_src) bl31 CROSS_COMPILE=$(CROSS_COMPILE64) PLAT=k3 SPD=opteed TARGET_BOARD=generic
	cp $(atf_src)/build/k3/generic/release/bl31.bin $@

optee_repo:=https://github.com/OP-TEE/optee_os.git
optee_version:=master
optee_src:=$(wrkdir_src)/optee_os

$(optee_src):
	git clone --branch $(optee_version) --depth 1 $(optee_repo) $@

optee_image:=$(wrkdir_plat_imgs)/tee-raw.bin
$(optee_image): $(optee_src)
	$(MAKE) -C $(optee_src) \
		CROSS_COMPILE=$(CROSS_COMPILE32) \
		CROSS_COMPILE64=$(CROSS_COMPILE64) \
		CFG_ARM64_core=y \
		PLATFORM=k3-j721e
	cp $(optee_src)/out/arm-plat-k3/core/tee-raw.bin $@

tifw_repo:=https://github.com/TexasInstruments/ti-linux-firmware.git
tifw_version:=ti-linux-firmware
tifw_src:=$(wrkdir_src)/ti-linux-firmware

$(tifw_src):
	git clone --branch $(tifw_version) --depth 1 $(tifw_repo) $@

uboot_repo:=https://github.com/u-boot/u-boot.git
uboot_version:=master
uboot_src:=$(wrkdir_src)/u-boot

$(uboot_src):
	git clone --branch $(uboot_version) --depth 1 $(uboot_repo) $@

uboot_image:=$(wrkdir_plat_imgs)/u-boot.img
$(uboot_image): $(uboot_src) $(tifw_src) $(atf_image) $(optee_image)
	$(MAKE) -C $(uboot_src) j721e_beagleboneai64_r5_defconfig
	$(MAKE) -C $(uboot_src) \
		CROSS_COMPILE=$(CROSS_COMPILE32) \
		BINMAN_INDIRS=$(tifw_src)
	cp $(uboot_src)/tiboot3-j721e-gp-evm.bin $(wrkdir_plat_imgs)/tiboot3.bin
	cp $(uboot_src)/sysfw-j721e-gp-evm.itb $(wrkdir_plat_imgs)/sysfw.itb
	$(MAKE) -C $(uboot_src) j721e_beagleboneai64_a72_defconfig
	$(MAKE) -C $(uboot_src) \
		CROSS_COMPILE=$(CROSS_COMPILE64) \
		BINMAN_INDIRS=$(tifw_src) \
		BL31=$(atf_image) \
		TEE=$(optee_image)
	cp $(uboot_src)/tispl.bin_unsigned $(wrkdir_plat_imgs)/tispl.bin
	cp $(uboot_src)/u-boot.img_unsigned $@

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md

platform: $(bao_image) $(atf_image) $(optee_image) $(uboot_image)
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)
