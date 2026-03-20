ARCH:=aarch32
ARCH_PROFILE:=armv8-r

# PAC Tool Configuration
instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
pac_tool_dir:=$(wrkdir_src)/e3650_pac_tool
wrkdirs+=$(pac_tool_dir)

pac_tool_repo:=https://github.com/bao-project/e3650_pac_tool.git
pac_tool_branch:=main
pac_tool:=$(pac_tool_dir)/bao_pac.sh

# Packaging needs Semidrive-provided vendor assets that are not
# redistributed here.

$(pac_tool):
	rm -rf $(pac_tool_dir)
	git clone --recursive --branch $(pac_tool_branch) $(pac_tool_repo) $(pac_tool_dir)

.PHONY: pac_vendor_assets

pac_vendor_assets: $(pac_tool)
	$(if $(NO_INSTRUCTIONS),@:, $(call print-instructions, $(instructions), 1, true))
	@if [ -n "$(BAO_DEMOS_E3_SSDK)" ]; then \
		ssdk_dir="$(BAO_DEMOS_E3_SSDK)"; \
	else \
		read -p "Path to your extracted E3_SSDK_PTG5.2_Source_Code: " ssdk_dir; \
	fi; \
	ssdk_common="$$ssdk_dir/ssdk/tools/common"; \
	mkdir -p $(pac_tool_dir)/builtin_tools $(pac_tool_dir)/flashloader/E3650 $(pac_tool_dir)/keys; \
	cp "$$ssdk_common/image_gen/builtin_tools/sdtools/linux/atb_signer" $(pac_tool_dir)/builtin_tools/; \
	cp "$$ssdk_common/image_gen/builtin_tools/sdtools/linux/bin2hex" $(pac_tool_dir)/builtin_tools/; \
	cp "$$ssdk_common/image_gen/builtin_tools/sdtools/linux/pactool" $(pac_tool_dir)/builtin_tools/; \
	cp "$$ssdk_common/flashloader/E3650/flashloader.out" $(pac_tool_dir)/flashloader/E3650/; \
	cp "$$ssdk_common/image_gen/res_default/keys/TestRSA2048_ossl.pem" $(pac_tool_dir)/keys/; \
	chmod +x $(pac_tool_dir)/builtin_tools/atb_signer \
	    $(pac_tool_dir)/builtin_tools/bin2hex \
	    $(pac_tool_dir)/builtin_tools/pactool

# PAC Image Generation
pac_image:=$(wrkdir_demo_imgs)/bao_pack_output/bao.pac

$(pac_image): $(bao_image) $(guest_images) pac_vendor_assets
	$(pac_tool) $(bao_image) $(guest_images)

platform: $(pac_image)
	$(call print-instructions, $(instructions), 2, false)
