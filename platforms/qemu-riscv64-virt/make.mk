ARCH:=riscv64

include $(bao_demos)/platforms/qemu.mk
include $(bao_demos)/platforms/opensbi.mk

opensbi_image:=$(wrkdir_demo_imgs)/opensbi.bin
$(eval $(call build-opensbi-payload, $(opensbi_image), $(bao_image)))

platform: $(opensbi_image)

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
run: qemu platform
	$(call print-instructions, $(instructions), 1, true)
	$(qemu_cmd) -nographic\
		-M virt -cpu rv64,priv_spec=v1.13.0,sstc=true -m 4G -smp 4\
		-bios $(opensbi_image)\
		-device virtio-net-device,netdev=net0\
		-netdev user,id=net0,net=192.168.42.0/24,hostfwd=tcp:127.0.0.1:5555-:22\
		-device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3\
		-S
