linux_repo?=https://github.com/torvalds/linux.git
linux_version?=v6.1
linux_src:=$(wrkdir_src)/linux-$(linux_version)
linux_cfg_frag:=$(wildcard $(bao_demos)/guests/linux/configs/base.config\
	$(bao_demos)/guests/linux/configs/$(ARCH).config\
	$(bao_demos)/guests/linux/configs/$(PLATFORM).config)
bao_linux_drivers_src:=$(wrkdir_src)/bao-linux-drivers
bao_linux_drivers_repo:=https://github.com/bao-project/bao-linux-drivers.git

ifeq ($(DEMO),virtio)
linux_cfg_frag+=$(bao_demos)/guests/linux/configs/virtio.config
endif

linux_patches:=$(wildcard $(bao_demos)/guests/linux/patches/$(linux_version)/*.patch)

$(linux_src):
	git clone --depth 1 --branch $(linux_version) $(linux_repo) $(linux_src)
	git -C $(linux_src) apply $(linux_patches)

$(bao_linux_drivers_src):
	git clone --depth 1 --branch linux-$(linux_version) $(bao_linux_drivers_repo) $(bao_linux_drivers_src)

buildroot_repo:=https://github.com/buildroot/buildroot.git
buildroot_version:=2022.11
buildroot_src:=$(wrkdir_src)/buildroot-$(ARCH)-$(linux_version)
buildroot_defcfg:=$(bao_demos)/guests/linux/buildroot/$(ARCH).config

export BAO_DEMOS_BUILDROOT_OVERLAY_DIR=$(buildroot_src)-overlay

$(buildroot_src):
	git clone --depth 1 --branch $(buildroot_version) $(buildroot_repo)\
		$(buildroot_src)
	mkdir -p $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/bin $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/etc

buildroot_image:=$(buildroot_src)/output/images/Image-$(PLATFORM)
export LINUX_OVERRIDE_SRCDIR=$(linux_src) 
export BAO_DEMOS_LINUX_CFG_FRAG=$(linux_cfg_frag)

linux $(buildroot_image): $(linux_patches) $(linux_cfg_frag) $(buildroot_defcfg) | $(linux_src) $(buildroot_src) 
	$(MAKE) -C $(buildroot_src) defconfig BR2_DEFCONFIG=$(buildroot_defcfg)
	$(MAKE) -C $(buildroot_src) linux-reconfigure all
	mv $(buildroot_src)/output/images/*Image $(buildroot_image)

lloader_dir:=$(bao_demos)/guests/linux/lloader

define build-linux
$(wrkdir_demo_imgs)/$(basename $(notdir $2)).dtb: $(strip $2)
	dtc $$< > $$@
$(strip $1): $(buildroot_image) $(wrkdir_demo_imgs)/$(basename $(notdir $2)).dtb
	$(MAKE) -C $(lloader_dir) ARCH=$(ARCH) IMAGE=$(buildroot_image)\
		DTB=$(wrkdir_demo_imgs)/$(basename $(notdir $2)).dtb TARGET=$$(basename $$@)
endef

ifeq ($(ARCH),aarch64)
bao_linux_driver_arch:=arm64
bao_linux_driver_cc:=aarch64-buildroot-linux-gnu-
else ifeq ($(ARCH),riscv64)
bao_linux_driver_arch:=riscv
bao_linux_driver_cc:=riscv64-buildroot-linux-gnu-
else
bao_linux_driver_arch:=arm32
bao_linux_driver_cc:=arm-buildroot-linux-gnueabi-
endif

linux_built_kernel_dir:=$(buildroot_src)/output/build/linux-custom

define build-linux-drivers
$(strip $1): $(bao_linux_drivers_src)
	$(MAKE) -C $(linux_built_kernel_dir) M=$(bao_linux_drivers_src)/$(basename $(notdir $1)) CROSS_COMPILE=$(buildroot_src)/output/host/bin/$(bao_linux_driver_cc) ARCH=$(bao_linux_driver_arch) modules
	cp $(bao_linux_drivers_src)/$(basename $(notdir $1))/$(basename $(notdir $1)).ko $$@
	cp $(bao_linux_drivers_src)/$(basename $(notdir $1))/$(basename $(notdir $1)).ko $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/bin
	mv $(buildroot_image) $(buildroot_image)-frontend
	$(MAKE) -C $(buildroot_src) linux-reconfigure all
	mv $(buildroot_src)/output/images/*Image $(buildroot_image)
	cp $(buildroot_image) $(buildroot_image)-backend
endef
