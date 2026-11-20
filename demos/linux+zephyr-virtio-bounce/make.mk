# Zephyr's upstream VirtIO MMIO transport and virtio-console driver are
# available since Zephyr 4.5. The demo builds the bao/demo-v4.5.0-virtio-bounce
# branch of the Bao fork of Zephyr. It carries two fixes submitted upstream on
# top of Zephyr main right after the v4.5.0 release (the virtio-console driver
# must complete the feature negotiation before setting up the virtqueues, and
# the PLIC driver must use the supervisor hart contexts in supervisor mode),
# followed by the restricted DMA pool proof of concept: the VirtIO transport
# allocates the virtqueues from the memory region named by the virtio-mmio node
# and bounces the driver buffers through it.
zephyr_repo:=https://github.com/bao-project/zephyr.git
zephyr_version:=bao/demo-v4.5.0-virtio-bounce
zephyr_src:=$(wrkdir_src)/zephyr-virtio-bounce

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
# The Zephyr application is the one of the linux+zephyr-virtio demo, unchanged.
# Only its configuration (prj.conf, board fragments and devicetree overlays)
# is specific to this demo.
zephyr_app:=$(bao_demos)/demos/linux+zephyr-virtio/zephyr/app
zephyr_app_cfg:=$(bao_demos)/demos/$(DEMO)/zephyr/config

include $(bao_demos)/guests/zephyr/make.mk

linux_backend_image:=$(wrkdir_demo_imgs)/linux_backend.bin
linux_backend_dts:=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-backend.dts
$(eval $(call build-linux, $(linux_backend_image), $(linux_backend_dts)))

zephyr_image:=$(wrkdir_demo_imgs)/zephyr.bin
$(eval $(call build-zephyr, $(zephyr_image)))

guest_images:=$(linux_backend_image) $(zephyr_image)
