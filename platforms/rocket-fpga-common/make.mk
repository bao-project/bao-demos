
ARCH:=riscv
board:=$(lastword $(subst -, ,$(PLATFORM)))
instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md

chipyard_repo:=https://github.com/ucb-bar/chipyard.git
chipyard_commit:=64632c8
chipyard_src:=$(wrkdir_src)/chipyard
chipyard_patches:=$(bao_demos)/platforms/$(PLATFORM)/patches
rocketchip_src:=$(chipyard_src)/generators/rocket-chip
rocketchip_repo:=https://github.com/josecm/rocket-chip.git 
bootrom_src:=$(chipyard_src)/bootromFPGA
bootrom_img:=$(bootrom_src)/bootrom_zynqmp.img
rocket_config:=RocketHypConfig$(board)
verilog_srcs:=$(chipyard_src)/sims/vcs/generated-src/freechips.rocketchip.system.TestHarness.$(rocket_config)

VIVADO_CORES:=$(shell nproc)
vivado_scripts:=$(bao_demos)/platforms/$(PLATFORM)/scripts

ip_project:=$(wrkdir_plat_imgs)/rocket_ip
project_name:=rocket_system_$(board)
system_project:=$(wrkdir_plat_imgs)/$(project_name)
bitstream:=$(wrkdir_plat_imgs)/$(project_name)/$(project_name).bit

environment+=BAO_DEMOS_CHIPYARD=$(chipyard_src)
environment+=BAO_DEMOS_ROCKET_CONFIG=$(rocket_config)
environment+=VIVADO_CORES=$(VIVADO_CORES)

$(chipyard_src):
	$(call print-instructions, $(instructions), 1, false)
	git clone $(chipyard_repo) $(chipyard_src)
	git -C $(chipyard_src) checkout $(chipyard_commit)
	cd $(chipyard_src) && ./scripts/init-submodules-no-riscv-tools.sh
	git -C $(chipyard_src) apply $(chipyard_patches)/0001-add-rocket-hyp-fpga-support.patch
	git -C $(chipyard_src)/generators/boom apply $(chipyard_patches)/0001-boom-add-usehyp-option.patch
	git -C $(chipyard_src)/generators/ariane apply $(chipyard_patches)/0001-ariane-add-usehyp-option.patch
	git -C $(rocketchip_src) remote add hyp $(rocketchip_repo) 
	git -C $(rocketchip_src) fetch hyp
	git -C $(rocketchip_src) checkout hyp

$(bootrom_img): | $(chipyard_src)
	make -C $(bootrom_src)

$(verilog_srcs): | $(chipyard_src) $(bootrom_img)
	make -C $(chipyard_src)/sims/vcs verilog SUB_PROJECT=rocket \
		CONFIG=$(rocket_config)

$(ip_project): | $(verilog_srcs)
	$(environment) vivado -nolog -nojournal -mode batch -source $(vivado_scripts)/create_ip.tcl

$(system_project): | $(ip_project)
	$(environment) vivado -nolog -nojournal -mode batch -source $(vivado_scripts)/create_design.tcl

$(bitstream): | $(system_project)
	$(environment) vivado -nolog -nojournal -mode batch -source $(vivado_scripts)/build.tcl

include $(bao_demos)/platforms/opensbi.mk
opensbi_image:=$(wrkdir_demo_imgs)/opensbi.bin
$(eval $(call build-opensbi-payload, $(opensbi_image), $(bao_image), $(PLATFORM), 0x40100000))

platform: $(bitstream) $(opensbi_image)

run: $(bitstream) $(opensbi_image)
	$(call print-instructions, $(instructions), 2, false)
	$(environment) xsct $(vivado_scripts)/deploy.tcl $(opensbi_image)

all:
	@printf "\nTo load system image and flash the bitstream execute \"make PLATFORM=$(PLATFORM) DEMO=$(DEMO) run\"\n\n"