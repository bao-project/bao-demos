ARCH:=aarch32
ARCH_PROFILE:=armv8-r

# PAC Tool Configuration
pac_tool_dir:=$(wrkdir_src)/e3650_pac_tool
wrkdirs+=$(pac_tool_dir)
pac_tool_repo:=https://github.com/leon6002/e3650_pac_tool.git
pac_tool_branch:=main

# Target to fetch the PAC tool
$(pac_tool_dir)/bao_pac.sh:
	rm -rf $(pac_tool_dir)
	git clone --recursive --branch $(pac_tool_branch) $(pac_tool_repo) $(pac_tool_dir)

# PAC Image Generation
pac_image:=$(wrkdir_demo_imgs)/bao_2vms.pac
bao_bin:=$(wrkdir_demo_imgs)/bao.bin
guest_bin:=$(wrkdir_demo_imgs)/baremetal_vm0.bin

$(pac_image): $(bao_image) $(guest_images) $(pac_tool_dir)/bao_pac.sh
	$(pac_tool_dir)/bao_pac.sh $(bao_image) $(guest_images)

platform: $(pac_image)

