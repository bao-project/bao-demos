ARCH:=aarch64

nxp_tools:=$(wrkdir_src)/nxp-tools
wrkdirs+=$(nxp_tools)

# ------------------------------------------------------------
# DDR FW (manual download from NXP)
# ------------------------------------------------------------
ddr_fw:=$(nxp_tools)/s32g3_linuxbsp_46.0_binaries/s32g399ardb3/ddr_fw_ecc_on.bin

$(ddr_fw):
	@echo "DDR firmware not found at $(ddr_fw)"
	@echo "Please download the BSP from NXP's official website:"
	@echo "  https://nxp.flexnetoperations.com/control/frse/download?agree=Accept&element=7739171"
	@echo "After logging in, download:"
	@echo "  s32g3_linuxbsp_46.0.0_binaries.tgz"
	@echo "and place it in: $(nxp_tools)"
	@echo "Then press 'c' to continue..."
	@read -p "Press 'c' to continue once the file is in place: " choice; \
	if [ "$$choice" != "c" ]; then \
	    echo "Aborting build."; exit 1; \
	fi
	@echo "Unpacking DDR firmware..."
	@mkdir -p $(nxp_tools)
	@tar -xzf $(nxp_tools)/s32g3_linuxbsp_46.0.0_binaries.tgz -C $(nxp_tools)
	@echo "DDR firmware ready at $(ddr_fw)"

# ------------------------------------------------------------
# U-Boot build (override repo/version)
# ------------------------------------------------------------
uboot_repo:=https://github.com/nxp-auto-linux/u-boot
uboot_version:=bsp46.0-2022.04
uboot_image:=$(wrkdir_demo_imgs)/u-boot.bin
uboot_nodtb_image:=$(wrkdir_demo_imgs)/u-boot-nodtb.bin
uboot_defconfig:=s32g399ardb3_defconfig
uboot_cfg_frag:="CONFIG_AUTOBOOT=n\n"
include $(bao_demos)/platforms/uboot.mk
$(eval $(call build-uboot, $(uboot_image), $(uboot_defconfig), $(uboot_cfg_frag)))

# ------------------------------------------------------------
# ATF build (override repo/version)
# ------------------------------------------------------------
atf_repo:=https://github.com/nxp-auto-linux/arm-trusted-firmware
atf_version:=bsp46.0-2.10.25
atf_image_bl2:=$(wrkdir_demo_imgs)/bl2_w_dtb.s32
atf_image_fip:=$(wrkdir_demo_imgs)/fip.bin
atf_plat:=s32g399ardb3
atf_args:=BL33=$(uboot_nodtb_image)
atf_args+=BL33DIR=$(uboot_src)
atf_args+=DDR_FW_BIN_PATH=$(ddr_fw)
ATF_BUILD_MODE:=full
include $(bao_demos)/platforms/atf.mk
$(eval $(call build-atf, $(atf_image_bl2), $(atf_image_fip), $(atf_plat), $(atf_args)))

# ------------------------------------------------------------
# Linux build (override repo/version)
# ------------------------------------------------------------
linux_repo:=https://github.com/nxp-auto-linux/linux.git
linux_version:=bsp46.0-6.6.99-rt

# ------------------------------------------------------------
# Bao build
# ------------------------------------------------------------
$(bao_src):
	git clone --branch $(bao_version) $(bao_repo) $(bao_src)
	cd $(bao_src) && git apply $(bao_demos)/platforms/$(PLATFORM)/0001-feat-armv8-add-configurable-SMC-passthrough.patch

BAO_SRC_DEFINED = 1

# ------------------------------------------------------------
# Build instructions
# ------------------------------------------------------------
instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md

platform: $(bao_image) $(ddr_fw) $(uboot_image) $(atf_image_bl2) $(atf_image_fip)
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)
