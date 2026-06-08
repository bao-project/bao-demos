ARCH:=riscv64

# Linux is SpacemiT's vendor kernel. The standard guests/linux/make.mk flow
# handles everything else: buildroot builds the kernel from this tree
# (LINUX_OVERRIDE_SRCDIR) on its arch defconfig plus the bao-demos fragments
# (configs/base.config + configs/k3-com260.config: SoC gate, CCU, pinctrl,
# AIA, gmac, 8250) and embeds its rootfs (dropbear + the bao-drivers ipc
# module + the S99bao init script) as the initramfs.
linux_repo    := https://github.com/spacemit-com/linux-6.18.git
linux_version := k3-br-v1.0.y

# This board's U-Boot has the legacy image format disabled, so Bao is delivered
# as a U-Boot FIT (bao.itb) plus a boot script (boot.scr). See README.md for the
# OpenSBI/U-Boot and SD-card steps (and ../../deploy in the workspace root).
bao_its  := $(platform_dir)/bao.its
bao_itb  := $(wrkdir_demo_imgs)/bao.itb
boot_scr := $(wrkdir_demo_imgs)/boot.scr

$(bao_itb): $(bao_image) $(bao_its)
	# mkimage resolves the FIT /incbin/ path relative to the .its file's own
	# directory, so place the .its next to bao.bin before packaging.
	cp $(bao_its) $(wrkdir_demo_imgs)/bao.its
	cd $(wrkdir_demo_imgs) && mkimage -f bao.its bao.itb

$(boot_scr): $(platform_dir)/boot.cmd $(platform_dir)/boot_scr.its
	# boot.scr must ALSO be a FIT (`source` rejects legacy `mkimage -T script`
	# images on this U-Boot). /incbin/ resolves next to the .its, so stage both.
	cp $(platform_dir)/boot.cmd $(platform_dir)/boot_scr.its $(wrkdir_demo_imgs)/
	cd $(wrkdir_demo_imgs) && mkimage -f boot_scr.its boot.scr

# --- Boot firmware: OpenSBI + U-Boot from SpacemiT's repos -------------------
# Reuse the stock opensbi.mk/uboot.mk fetch infrastructure, pointed at the
# SpacemiT repos. Both build with the Linux-targeting RV64
# toolchain already required for RISC-V demos (OPENSBI_CROSS_COMPILE). Only the
# K3-specific packaging is local: OpenSBI as fw_dynamic.itb from k3_defconfig
# (not the generic FW_PAYLOAD flow) and U-Boot as u-boot.itb + the default env.
# Pin the k3-br-v1.0.0 TAGS, not the k3-br-v1.0.y branch: the branch moves and
# its tip already broke once (SPL grew past the FSBL's 0x72f00 size limit).
OPENSBI_CROSS_COMPILE    ?= riscv64-unknown-linux-gnu-
override opensbi_repo    := https://github.com/spacemit-com/opensbi.git
override opensbi_version := k3-br-v1.0.0
include $(bao_demos)/platforms/opensbi.mk

uboot_repo    := https://github.com/spacemit-com/uboot-2022.10.git
uboot_version := k3-br-v1.0.0
include $(bao_demos)/platforms/uboot.mk

opensbi_itb := $(wrkdir_plat_imgs)/fw_dynamic.itb
uboot_bin   := $(wrkdir_plat_imgs)/u-boot.bin
uboot_itb   := $(wrkdir_plat_imgs)/u-boot.itb
uboot_env   := $(wrkdir_plat_imgs)/u-boot-env-default.bin
fsbl_bin    := $(wrkdir_plat_imgs)/FSBL.bin
bootinfo    := $(wrkdir_plat_imgs)/bootinfo_block.bin

# OpenSBI is loaded by the FSBL as fw_dynamic (SpacemiT wraps it as an itb).
$(eval $(call build-opensbi-dynamic, $(opensbi_itb), k3_defconfig))

# U-Boot cannot build with the default bare-metal toolchain (its EFI apps need
# a linker with shared-object support), so force the linux-gnu one.
$(eval $(call build-uboot, $(uboot_bin), k3_defconfig,, \
	ARCH=riscv CROSS_COMPILE=$(OPENSBI_CROSS_COMPILE)))

# The same U-Boot build also produces everything else the K3 boot flow needs:
# the FIT (u-boot.itb), the default env image, the FSBL (the signed SPL the
# BootROM loads) and the bootinfo block that points the BootROM at it.
$(uboot_itb) $(uboot_env) $(fsbl_bin) $(bootinfo): $(uboot_bin)
	cp $(uboot_src)/u-boot.itb $(uboot_itb)
	cp $(uboot_src)/u-boot-env-default.bin $(uboot_env)
	cp $(uboot_src)/FSBL.bin $(fsbl_bin)
	cp $(uboot_src)/bootinfo_block.bin $(bootinfo)

opensbi: $(opensbi_itb)

# The vendor env's bootcmd (run autoboot -> mmc_boot) only knows how to boot
# grub or a raw Linux kernel; it never sources boot.scr. Swap bootcmd for one
# that does, so a freshly flashed card boots Bao unattended. The env image is
# 4-byte CRC32 + NUL-separated vars + 0xff padding; mkenvimage rebuilds it.
uboot_env_bao := $(wrkdir_plat_imgs)/u-boot-env-bao.bin

$(uboot_env_bao): $(uboot_env)
	tail -c +5 $< | tr '\0' '\n' | grep -aE '^[[:alnum:]_]+=' | \
		sed '/^bootcmd=/d' > $@.txt
	printf '%s\n' 'bootcmd=mmc dev 0; if part number mmc 0 bootfs bootpart && load mmc 0:$${bootpart} 0x140000000 boot.scr; then source 0x140000000; else run autoboot; fi' >> $@.txt
	mkenvimage -s 0x4000 -p 0xff -o $@ $@.txt

# --- Flashable SD-card image -------------------------------------------------
# The K3 boot chain (BootROM -> FSBL -> OpenSBI -> U-Boot -> Bao) finds the early
# stages by FIXED byte offset, not via the partition table, so the firmware is
# dd'd into raw gaps before the single GPT 'bootfs' partition (which holds
# bao.itb + boot.scr). The offsets below are the vendor K3 layout - do NOT move
# the firmware ones (bootinfo points the BootROM at FSBL@1536K). These exact
# steps also work BY HAND on a real card: same sgdisk/dd with of=/dev/sdX (the
# disk, not a partition) under sudo - see the README.
sdcard_img := $(wrkdir_demo_imgs)/sdcard.img

$(sdcard_img): $(bao_itb) $(boot_scr) $(opensbi_itb) $(uboot_itb) $(uboot_env_bao) \
		$(fsbl_bin) $(bootinfo)
	# 1) bootfs: a 256 MiB ext4 holding bao.itb + boot.scr (+ u-boot.itb).
	rm -rf $(@D)/bootfs-root && mkdir -p $(@D)/bootfs-root
	cp $(bao_itb) $(boot_scr) $(uboot_itb) $(@D)/bootfs-root/
	mke2fs -q -t ext4 -L bootfs -d $(@D)/bootfs-root $(@D)/bootfs.ext4 256M
	# 2) empty 270 MiB image + GPT with one 'bootfs' partition at 12 MiB / 256 MiB.
	rm -f $@ && truncate -s 270M $@
	sgdisk --clear -n 1:24576:548863 -c 1:bootfs $@ >/dev/null
	# 3) place each blob at its offset (seek is in KiB because bs=1024):
	dd if=$(uboot_env_bao) of=$@ bs=1024 seek=640   conv=notrunc status=none # env      @640K
	dd if=$(bootinfo)      of=$@ bs=1024 seek=1024  conv=notrunc status=none # bootinfo @1M
	dd if=$(fsbl_bin)      of=$@ bs=1024 seek=1536  conv=notrunc status=none # fsbl     @1536K
	dd if=$(opensbi_itb)                     of=$@ bs=1024 seek=7168  conv=notrunc status=none # opensbi  @7M
	dd if=$(uboot_itb)                       of=$@ bs=1024 seek=8192  conv=notrunc status=none # uboot    @8M
	dd if=$(@D)/bootfs.ext4                  of=$@ bs=1024 seek=12288 conv=notrunc status=none # bootfs   @12M

sdcard: $(sdcard_img)
	@echo "SD image ready: $(sdcard_img)"
	@echo "Flash: sudo dd if=$(sdcard_img) of=/dev/sdX bs=4M status=progress conv=fsync; sync"

# Building the platform (the default `all` goal) now produces the flashable
# sdcard.img directly: $(sdcard_img) pulls in bao.itb + boot.scr + OpenSBI +
# U-Boot, so a single `make ... PLATFORM=k3-com260 DEMO=<demo>` yields a card
# ready to dd. Use the `sdcard` target if you only want the image + flash hint.
instructions:=$(platform_dir)/README.md
platform: $(sdcard_img)
	@echo "==> SD card image ready: $(sdcard_img)"
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)
