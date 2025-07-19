ARCH?=aarch64
ARCH_PROFILE:=armv8-a
ARCH_SUB:=$(ARCH)

include $(bao_demos)/platforms/fvp.mk
include $(bao_demos)/platforms/atf.mk

ifeq ($(ARCH_SUB),aarch32)

fvp_args+= -C cluster0.cpu0.CONFIG64=0 \
	-C cluster0.cpu1.CONFIG64=0 \
	-C cluster0.cpu2.CONFIG64=0 \
	-C cluster0.cpu3.CONFIG64=0
atf_flags+=AARCH32_SP=sp_min

bl33_code:=$(bao_demos)/platforms/fvp-a/bl33.S
atf_bl33:=$(wrkdir_plat_imgs)/bl33.bin

$(atf_bl33): $(bl33_code)
	$(CROSS_COMPILE)as $< -o bl33.o
	$(CROSS_COMPILE)objcopy -O binary bl33.o $@
	rm bl33.o

else

include $(bao_demos)/platforms/uboot.mk
uboot_defconfig:=vexpress_aemv8a_semi_defconfig
uboot_image:=$(wrkdir_plat_imgs)/u-boot.bin

uboot_cfg_frag:="CONFIG_AUTOBOOT=n\n"
$(eval $(call build-uboot, $(uboot_image), $(uboot_defconfig), $(uboot_cfg_frag)))

atf_bl33:=$(uboot_image)

endif

atf_plat:=fvp
atf_targets:=bl1 fip
atf_flags+=QEMU_USE_GIC_DRIVER=QEMU_GICV3 ARCH=$(ARCH) BL33:=$(atf_bl33)
atf_targets_path:=$(atf_src)/build/$(atf_plat)/release
atf_fip:=$(wrkdir_plat_imgs)/fip.bin
atf_bl1:=$(wrkdir_plat_imgs)/bl1.bin

$(atf_fip) $(atf_bl1): $(atf_bl33) $(atf_src)
	$(MAKE) -C $(atf_src) PLAT=$(atf_plat) $(atf_targets) $(atf_flags)
	$(foreach target, $(atf_targets), \
		cp $(atf_targets_path)/$(target).bin $(wrkdir_plat_imgs)/$(target).bin;)

platform: $(bao_image) $(atf_fip) $(atf_bl1)

# cache_state_modelled=0 is not fully necessarybut speeds up simulation
# significantly
fvp_args+= -C cluster0.NUM_CORES=4 \
	-C cache_state_modelled=0 \
	-C bp.refcounter.use_real_time=1 \
	-C bp.exclusive_monitor.monitor_access_level=1 \
	-C cluster0.supports_multi_threading=0 \
	-C cluster0.mpidr_layout=0 \
	-C cluster1.NUM_CORES=0 \
	-C pctl.startup=0.0.0.0 \
	-C pctl.Affinity-shifted=0 \
	-C pctl.CPU-affinities='0.0.0.0, 0.0.0.1, 0.0.0.2, 0.0.0.3' \
	-C gic_distributor.CPU-affinities='0.0.0.0, 0.0.0.1, 0.0.0.2, 0.0.0.3' \
	-C gic_distributor.reg-base-per-redistributor='0.0.0.0=0x2f100000,0.0.0.1=0x2f120000,0.0.0.2=0x2f140000,0.0.0.3=0x2f160000' \
	-C bp.smsc_91c111.enabled=true -C bp.hostbridge.userNetworking=true \
	-C bp.hostbridge.userNetSubnet=192.168.42.0/24 -C bp.hostbridge.userNetPorts=127.0.0.1:5555=192.168.42.15:22

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
run: fvp platform
ifeq ($(ARCH),aarch64)
	$(call print-instructions, $(instructions), 1, false)
endif
	@$(fvp_cmd) $(fvp_args) \
		--data=$(atf_bl1)@0x0  \
		--data=$(atf_fip)@0x08000000 \
		--data=$(bao_image)@0x90000000

.PHONY: run
