linux_repo?=https://github.com/torvalds/linux.git
linux_version?=v5.11
linux_src:=$(wrkdir_src)/linux-$(linux_version)
linux_cfg_frag:=$(wildcard $(bao_demos)/guests/linux/configs/base.config\
	$(bao_demos)/guests/linux/configs/$(ARCH).config\
	$(bao_demos)/guests/linux/configs/$(PLATFORM).config)
linux_patches:=$(wildcard $(bao_demos)/guests/linux/patches/$(linux_version)/*.patch)

$(linux_src):
	git clone --depth 1 --branch $(linux_version) $(linux_repo) $(linux_src)
	git -C $(linux_src) apply $(linux_patches)

buildroot_repo:=https://github.com/buildroot/buildroot.git
buildroot_version:=2022.02.2
buildroot_src:=$(wrkdir_src)/buildroot-$(ARCH)-$(linux_version)
buildroot_defcfg:=$(bao_demos)/guests/linux/buildroot/$(ARCH).config

$(buildroot_src):
	git clone --depth 1 --branch $(buildroot_version) $(buildroot_repo)\
		$(buildroot_src)

buildroot_image:=$(buildroot_src)/output/images/Image-$(PLATFORM)
export LINUX_OVERRIDE_SRCDIR=$(linux_src) 
export BAO_DEMOS_LINUX_CFG_FRAG=$(linux_cfg_frag)

linux $(buildroot_image): $(linux_patches) $(linux_cfg_frag) $(buildroot_defcfg) | $(linux_src) $(buildroot_src) 
	$(MAKE) -C $(buildroot_src) defconfig BR2_DEFCONFIG=$(buildroot_defcfg)
	$(MAKE) -C $(buildroot_src) linux-reconfigure all
	mv $(buildroot_src)/output/images/Image $(buildroot_image)

lloader_dir:=$(bao_demos)/guests/linux/lloader

define build-linux
$(wrkdir_demo_imgs)/$(basename $(notdir $2)).dtb: $(strip $2)
	dtc $$< > $$@
$(strip $1): $(buildroot_image) $(wrkdir_demo_imgs)/$(basename $(notdir $2)).dtb
	$(MAKE) -C $(lloader_dir) ARCH=$(ARCH) IMAGE=$(buildroot_image)\
		DTB=$(wrkdir_demo_imgs)/$(basename $(notdir $2)).dtb TARGET=$(basename $$@)
endef
