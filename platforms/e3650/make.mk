ARCH:=aarch32
ARCH_PROFILE:=armv8-r

# PAC Tool Configuration
instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
pac_tool_dir:=$(wrkdir_src)/e3650_pac_tool
wrkdirs+=$(pac_tool_dir)
# todo: move this repo to bao org and update the URL?
pac_tool_repo:=https://github.com/leon6002/e3650_pac_tool.git
pac_tool_branch:=main
pac_tool:=$(pac_tool_dir)/bao_pac.sh
pac_vendor_files:=builtin_tools/atb_signer builtin_tools/bin2hex
pac_vendor_files+=builtin_tools/pactool flashloader/E3650/flashloader.out
pac_vendor_files+=keys/TestRSA2048_ossl.pem
pac_missing_marker:=$(pac_tool_dir)/.missing_assets

# The packaging helper itself can be fetched automatically. Users only need to
# supplement any vendor-provided Semidrive assets that are not present.
$(pac_tool):
	rm -rf $(pac_tool_dir)
	git clone --recursive --branch $(pac_tool_branch) $(pac_tool_repo) $(pac_tool_dir)

.PHONY: pac_tool_check
.PHONY: pac_tool_missing

pac_tool_missing:
	@if [ -f "$(pac_missing_marker)" ]; then cat "$(pac_missing_marker)"; fi
	$(if $(NO_INSTRUCTIONS),@:, $(call print-instructions, $(instructions), 1, false))
	$(if $(NO_INSTRUCTIONS),@:, $(call print-instructions, $(instructions), 2, true))

pac_tool_check: $(pac_tool)
	@while true; do \
		rm -f "$(pac_missing_marker)"; \
		missing=0; \
		for file in $(pac_vendor_files); do \
			if [ ! -e "$(pac_tool_dir)/$$file" ]; then \
				echo "Missing PAC dependency: $(pac_tool_dir)/$$file" >> "$(pac_missing_marker)"; \
				missing=1; \
			fi; \
		done; \
		if [ $$missing -eq 0 ]; then \
			break; \
		fi; \
		$(MAKE) --no-print-directory pac_tool_missing; \
		if [ -n "$(NO_INSTRUCTIONS)" ]; then \
			echo "Unable to continue until the missing Semidrive PAC assets are provided."; \
			exit 1; \
		fi; \
	done

# PAC Image Generation
pac_image:=$(wrkdir_demo_imgs)/bao_2vms.pac
bao_bin:=$(wrkdir_demo_imgs)/bao.bin
guest_bin:=$(wrkdir_demo_imgs)/baremetal_vm0.bin

$(pac_image): $(bao_image) $(guest_images) pac_tool_check
	$(pac_tool) $(bao_image) $(guest_images)

platform: $(pac_image)
	$(call print-instructions, $(instructions), 3, true)
