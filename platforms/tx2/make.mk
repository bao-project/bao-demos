ARCH:=aarch64

atf_image:=$(wrkdir_plat_imgs)/bl31.bin
include $(bao_demos)/platforms/atf.mk
$(atf_image): $(atf_src)
	$(MAKE) -C $(atf_src) bl31 PLAT=tegra TARGET_SOC=t186
	cp $(atf_src)/build/tegra/t186/release/bl31.bin $@

nvidia_tools:=$(wrkdir_src)/nvidia-tools
nvidia_tools_flash:=$(nvidia_tools)/Linux_for_Tegra/
wrkdirs+=$(nvidia_tools)
nvidia_tools_flash_link:=https://developer.nvidia.com/embedded/l4t/r32_release_v5.1/r32_release_v5.1/t186/tegra186_linux_r32.5.1_aarch64.tbz2
nvidia_tools_flash_ar:=$(nvidia_tools)/tegra186_linux_r32.5.1_aarch64.tbz2

environment+=BAO_DEMOS_NVIDIA_TOOLS=$(nvidia_tools)

$(nvidia_tools_flash_ar):
	wget -P $(nvidia_tools) $(nvidia_tools_flash_link)

$(nvidia_tools_flash): $(nvidia_tools_flash_ar)
	tar xfvm $(nvidia_tools_flash_ar) -C $(nvidia_tools)

flash_image:=$(wrkdir_plat_imgs)/tos.img
$(flash_image): $(nvidia_tools_flash) $(atf_image)
	$(nvidia_tools_flash)/nv_tegra/tos-scripts/gen_tos_part_img.py\
		--monitor $(atf_image) $(flash_image)

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md

platform: $(bao_image) $(flash_image) 
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, false)
	$(call print-instructions, $(instructions), 4, true)