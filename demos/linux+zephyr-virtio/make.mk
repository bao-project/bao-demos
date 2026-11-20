# Zephyr's upstream VirtIO MMIO transport and virtio-console driver are
# available since Zephyr 4.5. The demo builds the bao/demo-v4.5.0 branch of the
# Bao fork of Zephyr, which carries two fixes submitted upstream on top of
# Zephyr main right after the v4.5.0 release: the virtio-console driver must
# complete the feature negotiation before setting up the virtqueues, and the
# PLIC driver must use the supervisor hart contexts in supervisor mode.
zephyr_repo:=https://github.com/bao-project/zephyr.git
zephyr_version:=067b2fe70264af4dd182328f57405b73d69bddba
zephyr_src:=$(wrkdir_src)/zephyr-bao-demo-v4.5.0

ifeq ($(PLATFORM),qemu-riscv64-virt)
# Zephyr relies on Bao's PLIC emulation on this platform: use the dedicated
# PLIC-based board (the default one targets the AIA and the Zephyr snapshot
# used by the other demos), build Bao v2.0.0 and run QEMU without the AIA.
zephyr_board:=baovm_$(PLATFORM)-plic
bao_version:=0af4a1ab558ad60c9658af4f43756c4536dd3141
bao_src:=$(wrkdir_src)/bao-v2.0.0
qemu_machine:=virt
endif

# Linux is the VirtIO backend: build the I/O dispatcher driver and the
# VirtIO device model, and add the demo files to the root filesystem.
linux_virtio_backend:=y
buildroot_overlay_extra:=$(bao_demos)/demos/$(DEMO)/overlay

include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/zephyr/make.mk

linux_backend_image:=$(wrkdir_demo_imgs)/linux_backend.bin
linux_backend_dts:=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-backend.dts
$(eval $(call build-linux, $(linux_backend_image), $(linux_backend_dts)))

zephyr_image:=$(wrkdir_demo_imgs)/zephyr.bin
$(eval $(call build-zephyr, $(zephyr_image)))

guest_images:=$(linux_backend_image) $(zephyr_image)
