ARCH?=aarch64
ARCH_PROFILE:=armv8-r
ARCH_SUB:=$(ARCH)

include $(bao_demos)/platforms/fvp.mk

platform: $(bao_image)

fvp_args:= \
	-C cache_state_modelled=0 \
	-C bp.refcounter.use_real_time=1 \
	-C bp.exclusive_monitor.monitor_access_level=1 \
	-C cci400.force_on_from_start=1 \
	-C gic_distributor.has-two-security-states=0 \
	-C cluster0.gicv3.cpuintf-mmap-access-level=2 \
	-C cluster0.gicv3.SRE-EL2-enable-RAO=1 \
	-C bp.smsc_91c111.enabled=true -C bp.hostbridge.userNetworking=true \
	-C bp.hostbridge.userNetSubnet=192.168.42.0/24 -C bp.hostbridge.userNetPorts=127.0.0.1:5555=22

ifeq ($(ARCH_SUB),aarch64)
fvp_args+= -C cluster0.has_aarch64=1 \
	-C cluster0.VMSA_supported=1 \
	-C cluster0.gicv3.SRE-enable-action-on-mmap=2 \
	-C cluster0.gicv3.extended-interrupt-range-support=1
endif

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
run: fvp platform
ifeq ($(ARCH),aarch64)
	$(call print-instructions, $(instructions), 1, false)
endif
	$(fvp_cmd) $(fvp_args) --data=$(bao_image)@0x0 \
		$(foreach image, $(fvpr_image_data), --data=$(image))
