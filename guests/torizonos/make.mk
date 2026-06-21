

SHELL := /bin/bash

toradex_dir=$(wrkdir_src)/toradex
toradex_tools=$(toradex_dir)/tools
torizonos_dir=$(toradex_dir)/torizon
torizonos_version?=7.1.0
torizonos_tar=$(wrkdir_demo_imgs)/torizonos.tar
# Extract the directory of the current Makefile
MAKEFILE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILE_DIR)/$(PLATFORM)/make.mk

uboot_repo:=https://github.com/u-boot/u-boot.git
uboot_version:=v2024.10
uboot_src:=$(toradex_dir)/u-boot
$(uboot_src):
	@git clone --depth 1 --branch $(uboot_version) $(uboot_repo) $(uboot_src)

$(wrkdir_demo_imgs)/u-boot.bin: $(uboot_src)
	@cp $(MAKEFILE_DIR)/$(PLATFORM)/uboot-patch.diff $(uboot_src)/ && cd $(uboot_src) && git apply uboot-patch.diff
	@$(MAKE) -C $(uboot_src) $(UBOOT_DEFCONFIG)
	@$(MAKE) -C $(uboot_src) -j$(nproc) u-boot.bin
	@cp $(uboot_src)/u-boot.bin $(wrkdir_demo_imgs)

environment+=BAO_DEMOS_UBOOT_PATCH=$(MAKEFILE_DIR)/$(PLATFORM)/uboot-patch.diff
environment+=BAO_DEMOS_TORADEX_DIR=$(toradex_dir)
environment+=BAO_DEMOS_TORADEX_TOOLS=$(toradex_tools)
environment+=BAO_DEMOS_TORIZONOS_DIR=$(torizonos_dir)

$(toradex-easy-installer):
	@mkdir -p $(toradex_tools)
	@[ -f $(toradex_tools)/$(toradex-easy-installer-version).zip ] || \
		wget -P $(toradex_tools) $(toradex-easy-installer-link)
	@cd $(toradex_tools) && unzip -n $(toradex-easy-installer-version).zip
	
ifeq ($(ALWAYS_ASK),)
noask:=1
endif

$(torizonos_tar):
	@mkdir -p $(torizonos_dir)
	@cd $(torizonos_dir) && \
		repo init -u git://git.toradex.cn/toradex-manifest.git -b refs/tags/7.1.0 -m torizon/default.xml && \
		repo sync --no-clone-bundle && \
		MACHINE=$(TORIZONOS_MACHINE) EULA=$(noask) source setup-environment && \
		[ -d ../layers/meta-bao ] || git clone https://github.com/bao-project/meta-bao ../layers/meta-bao -b bao-demo-$(DEMO) && \
		bitbake-layers add-layer ../layers/meta-bao/ && \
		bitbake torizon-docker
	@cp -u $(torizonos_dir)/build-torizon/deploy/images/$(TORIZONOS_MACHINE)/torizon-docker-$(TORIZONOS_MACHINE)-*$(torizonos_version)*.tar \
		$(torizonos_tar)

# configure the torizonos image
environment+=BAO_DEMOS_TORIZONOS_TAR=$(torizonos_tar)
environment+=BAO_DEMOS_TORIZONOS_EXTRACTED=$(basename $(torizonos_tar))


.PHONY: torizonos_images
torizonos_images: $(wrkdir_demo_imgs)/u-boot.bin $(torizonos_tar) $(toradex-easy-installer)

