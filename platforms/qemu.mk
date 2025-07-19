qemu_arch:=$(ARCH)
qemu_repo:=https://github.com/qemu/qemu.git
qemu_version:=v10.0.2 
qemu_cmd:=qemu-system-$(qemu_arch)

ifeq ($(shell which $(qemu_cmd)),)
qemu_src:=$(wrkdir_src)/qemu
qemu_cmd:=$(qemu_src)/build/$(qemu_cmd)
$(qemu_src):
	git clone --depth 1 --branch $(qemu_version) $(qemu_repo) $(qemu_src)

$(qemu_cmd): | $(qemu_src)
	cd $(qemu_src) && ./configure --target-list=$(qemu_arch)-softmmu \
		--enable-slirp
	$(MAKE) -C $(qemu_src) -j$(nproc)

qemu: $(qemu_cmd)
else
# TODO: add version check if qemu is already installed
qemu:
endif

all:
	@printf "\nTo start qemu execute \"make PLATFORM=$(PLATFORM) DEMO=$(DEMO) run\"\n\n"

.PHONY: qemu
