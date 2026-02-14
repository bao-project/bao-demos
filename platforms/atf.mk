atf_repo?=https://github.com/bao-project/arm-trusted-firmware.git
atf_version?=bao/demo-next
atf_src:=$(wrkdir_src)/arm-trusted-firmware-$(ARCH)-$(atf_version)

# Default build mode
ATF_BUILD_MODE ?= bl31

$(atf_src):
	git clone --depth 1 --branch $(atf_version) $(atf_repo) $(atf_src)

define build-atf
ifeq ($(ATF_BUILD_MODE),bl31)
$(strip $(1)): $(atf_src)
	$(MAKE) -C $(atf_src) bl31 PLAT=$(strip $(2)) $(strip $(3))
	cp $(atf_src)/build/$(strip $(2))/release/bl31.bin $$@
else
$(strip $(1)) $(strip $(2)): $(atf_src)
	$(MAKE) -C $(atf_src) PLAT=$(strip $(3)) $(strip $(4))
	cp $(atf_src)/build/$(strip $(3))/release/bl2_w_dtb.s32 $(strip $(1))
	cp $(atf_src)/build/$(strip $(3))/release/fip.bin $(strip $(2))
endif
endef
