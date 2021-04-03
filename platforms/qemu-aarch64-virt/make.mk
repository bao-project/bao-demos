ARCH:=aarch64

include $(bao_demos)/platforms/qemu.mk

include $(bao_demos)/platforms/uboot.mk
uboot_defconfig:=qemu_arm64_defconfig
uboot_cfg_frag:="CONFIG_SYS_TEXT_BASE=0x60000000\nCONFIG_TFABOOT=y\n"
uboot_image:=$(wrkdir_plat_imgs)/u-boot.bin
$(eval $(call build-uboot, $(uboot_image), $(uboot_defconfig), $(uboot_cfg_frag)))

atf_plat:=qemu
atf_targets:=bl1 fip 
atf_flags+=BL33=$(wrkdir_plat_imgs)/u-boot.bin
atf_flags+=QEMU_USE_GIC_DRIVER=QEMU_GICV3
include $(bao_demos)/platforms/atf.mk

atf-fip:=$(wrkdir_plat_imgs)/flash.bin
$(atf-fip): $(uboot_image) $(atf_src)
	$(MAKE) -C $(atf_src) PLAT=$(atf_plat) $(atf_targets) $(atf_flags)
	dd if=$(atf_src)/build/qemu/release/bl1.bin of=$(atf-fip)
	dd if=$(atf_src)/build/qemu/release/fip.bin of=$(atf-fip) seek=64 bs=4096 conv=notrunc


platform: $(bao_image) $(atf-fip) 

instuctions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
run: qemu platform
	$(call print-instructions, $(instuctions), 1, false)
	@$(qemu_cmd) -nographic\
		-M virt,secure=on,virtualization=on,gic-version=3 \
		-cpu cortex-a53 -smp 4 -m 4G\
		-bios $(atf-fip)\
		-device loader,file="$(bao_image)",addr=0x50000000,force-raw=on\
		-device virtio-net-device,netdev=net0\
		-netdev user,id=net0,net=192.168.42.0/24,hostfwd=tcp:127.0.0.1:5555-:22\
		-device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3
	

.PHONY: run