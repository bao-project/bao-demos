ARCH:=aarch32
ARCH_PROFILE:=armv8-r

include $(bao_demos)/platforms/qemu.mk

trampoline_dir:=$(platform_dir)/trampline
trampoline_img:=$(wrkdir_plat_imgs)/trampline.bin
BAO_BASE_ADDR:=0x20000000

trampoline:
	make -C $(trampoline_dir) BAO_BASE_ADDR:=$(BAO_BASE_ADDR) OUTPUT_DIR:=$(wrkdir_plat_imgs) TRAMPLINE_IMG:=$(trampoline_img) 

platform: trampoline

run: qemu platform
	$(qemu_cmd) -nographic \
		-M mps3-an536 -cpu cortex-r52 -smp 2 -m 2G \
		-device loader,file=$(trampoline_img)	 \
		-device loader,file=$(bao_image),addr=$(BAO_BASE_ADDR) \
		-serial null -serial null -serial mon:stdio -serial pty \
		$(foreach image_addr, $(qemu_loader_data), -device loader,file=$(word 1, $(subst @, ,$(image_addr))),addr=$(word 2, $(subst @, ,$(image_addr)))) -S

.PHONY: run
