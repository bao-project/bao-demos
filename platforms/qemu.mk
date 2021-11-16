ifeq ($(ARCH),riscv)
qemu_arch:=riscv64
else
qemu_arch:=$(ARCH)
endif

qemu_repo:=https://github.com/qemu.git
qemu_version:=v5.2.0
qemu_cmd:=qemu-system-$(qemu_arch)

#override the qemu repo to our patched repo
#TODO: remove this when small riscv bugs on qemu are fixed
qemu_repo:=https://github.com/josecm/qemu.git
qemu_cur_ver:=josecm/hyp

ifeq ($(shell which $(qemu_cmd)),)
qemu_src:=$(wrkdir_src)/qemu
qemu_cmd:=$(qemu_src)/build/$(qemu_cmd)
$(qemu_src):
	git clone --depth 1 --branch $(qemu_cur_ver) $(qemu_repo) $(qemu_src)

$(qemu_cmd): | $(qemu_src)
	cd $(qemu_src) && ./configure --target-list=$(qemu_arch)-softmmu
	$(MAKE) -C $(qemu_src) -j$(nproc)

qemu: $(qemu_cmd)
else
# TODO: add version check if qemu is already installed
qemu:
endif

all:
	@printf "\nTo start qemu execute \"make PLATFORM=$(PLATFORM) DEMO=$(DEMO) run\"\n\n"

.PHONY: qemu
