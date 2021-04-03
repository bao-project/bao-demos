ARCH:=aarch64

nxp_tools:=$(wrkdir_src)/nxp-tools
wrkdirs+=$(nxp_tools)

ifeq ($(ALWAYS_ASK),)
noask:=1
endif

scfw:=$(nxp_tools)/imx-sc-firmware-1.5.0/mx8qm-mek-scfw-tcm.bin
scfw_link:=https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/imx-sc-firmware-1.5.0.bin
$(scfw):
	wget -P $(nxp_tools) $(scfw_link)
	cd $(nxp_tools) && chmod +x imx-sc-firmware-1.5.0.bin
	cd $(nxp_tools) && noask=$(noask) ./imx-sc-firmware-1.5.0.bin

seco:=$(nxp_tools)/imx-seco-3.6.3/firmware/seco/mx8qmb0-ahab-container.img
seco_link:=https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/imx-seco-3.6.3.bin
$(seco):
	wget -P $(nxp_tools) $(seco_link)
	cd $(nxp_tools) && chmod +x imx-seco-3.6.3.bin
	cd $(nxp_tools) && noask=$(noask) ./imx-seco-3.6.3.bin

include $(bao_demos)/platforms/uboot.mk
uboot_image:=$(wrkdir_demo_imgs)/u-boot.bin
uboot_defconfig:=imx8qm_mek_defconfig
$(eval $(call build-uboot, $(uboot_image), $(uboot_defconfig)))

include $(bao_demos)/platforms/atf.mk
atf_image:=$(wrkdir_demo_imgs)/bl31.bin
atf_plat:=imx8qm
$(eval $(call build-atf, $(atf_image), $(atf_plat)))

nxp_mkimage:=$(nxp_tools)/imx-mkimage
nxp_mkimage_repo:=https://source.codeaurora.org/external/imx/imx-mkimage
nxp_mkimage_vesion:=rel_imx_5.4.24_2.1.0
$(nxp_mkimage):
	git clone --depth 1 --branch $(nxp_mkimage_vesion) $(nxp_mkimage_repo)\
		$(nxp_mkimage)

flash_image:=$(wrkdir_demo_imgs)/flash.bin
$(flash_image): $(nxp_mkimage) $(scfw) $(seco) $(atf_image) $(uboot_image)
	cp $(scfw) $(nxp_mkimage)/iMX8QM/scfw_tcm.bin
	cp $(seco) $(nxp_mkimage)/iMX8QM
	cp $(atf_image) $(nxp_mkimage)/iMX8QM
	cp $(uboot_image) $(nxp_mkimage)/iMX8QM
	cd $(nxp_mkimage) && $(MAKE) SOC=iMX8QM flash_b0
	cp $(nxp_mkimage)/iMX8QM/flash.bin $@

# override default linux repo and version
linux_repo:=https://source.codeaurora.org/external/imx/linux-imx
linux_version:=rel_imx_5.4.24_2.1.0

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
platform: $(bao_image) $(flash_image)
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)
