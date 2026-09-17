zephyr_src:=$(wrkdir_src)/zephyr
zephyr_repo:=https://github.com/zephyrproject-rtos/zephyr.git
# RISC-V S-mode support (and the AIA drivers the RISC-V boards rely on) is
# not part of any Zephyr release yet, so all platforms pin a snapshot of
# upstream main.
zephyr_version:=99e635ace5aa2d9b6d069dd3a3b94018d17742d9
zephyr_cmsis_repo:=https://github.com/zephyrproject-rtos/cmsis.git
zephyr_cmsis_src:=$(wrkdir_src)/cmsis
zephyr_cmsis_commit:=512cc7e895e8491696b61f7ba8066b4a182569b8
zephyr_hal_nxp_src:=$(wrkdir_src)/nxp
zephyr_hal_nxp_repo:=https://github.com/bao-project/hal_nxp.git
zephyr_hal_nxp_version:=6c77523dfdfc5a1ecefbafb31feee6499a5de8ec
zephyr_hal_infineon_src:=$(wrkdir_src)/infineon
zephyr_hal_infineon_repo:=https://github.com/zephyrproject-rtos/hal_infineon.git
# zephyrproject-rtos/hal_infineon#51 (AURIX TriCore HAL, not merged yet) --
# resolved SHA, since PR refs can move.
zephyr_hal_infineon_version:=46478f60f1ac74bb5b5a2e64e2c7870e35e4f1a5
zephyr_build:=$(wrkdir_demo_imgs)/zephyr_build
zephyr_board_root:=$(bao_demos)/guests/zephyr
zephyr_board:=baovm_$(PLATFORM)
zephyr_app:=$(bao_demos)/demos/$(DEMO)/zephyr/app
zephyr_bin:=$(zephyr_build)/zephyr/zephyr.bin
zephyr_env:=ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
	CROSS_COMPILE=$(shell which $(CROSS_COMPILE)gcc | sed 's/.\{3\}$$//')
zephyr_modules:=$(zephyr_cmsis_src)
zephyr_overlay:=overlays/$(ARCH)/app.overlay
zephyr_conf:=

ifeq ($(PLATFORM),s32z270)
zephyr_modules:=$(zephyr_modules);$(zephyr_hal_nxp_src)
zephyr_hal_nxp_dep:=$(zephyr_hal_nxp_src)
zephyr_board:=baovm_$(PLATFORM)/$(PLATFORM)/rtu0
zephyr_overlay:=overlays/$(ARCH)/app.overlay;boards/$(PLATFORM).overlay
endif

ifeq ($(PLATFORM),tc4dx)
# TC4D7/TriCore isn't upstream yet -- lives on a fork (linumiz/zephyr-tricore,
# dev/ifx/tricore-upstream), pointed at a local mirror since that branch gets
# rebased periodically and can drop commits a shallow fetch depends on.
zephyr_repo:=$(bao_demos)/dev/zephyr-tricore
zephyr_version:=e8e299cd89580e156a60fc39279c75a829dcd84b
# No CMSIS (ARM-only) or custom baovm_* board: the board ships inside
# hal_infineon, found via ZEPHYR_MODULES.
zephyr_modules:=$(zephyr_hal_infineon_src)
zephyr_hal_infineon_dep:=$(zephyr_hal_infineon_src)
zephyr_board:=kit_a3g_tc4d7_lite/tc4d7xp/cpu0
zephyr_conf:=$(bao_demos)/guests/zephyr/boards/tricore/bao_guest.conf
endif

# Must come after every PLATFORM override above (s32z270/tc4dx) that touches
# zephyr_version -- := expands immediately, so this has to be last to pick up
# the right patches dir per platform.
zephyr_patches:=$(wildcard $(bao_demos)/guests/zephyr/patches/$(zephyr_version)/*.patch)

$(zephyr_src):
	git init -q $@
	git -C $@ remote add origin $(zephyr_repo)
	git -C $@ fetch --depth 1 origin $(zephyr_version)
	git -C $@ checkout -q FETCH_HEAD
	git -C $(zephyr_src) apply $(zephyr_patches)

zephyr_bin:=$(zephyr_build)/zephyr/zephyr.bin

$(zephyr_cmsis_src):
	git init -q $@
	git -C $@ remote add origin $(zephyr_cmsis_repo)
	git -C $@ fetch --depth 1 origin $(zephyr_cmsis_commit)
	git -C $@ checkout -q FETCH_HEAD

$(zephyr_hal_nxp_src):
	git init -q $@
	git -C $@ remote add origin $(zephyr_hal_nxp_repo)
	git -C $@ fetch --depth 1 origin $(zephyr_hal_nxp_version)
	git -C $@ checkout -q FETCH_HEAD

$(zephyr_hal_infineon_src):
	git init -q $@
	git -C $@ remote add origin $(zephyr_hal_infineon_repo)
	git -C $@ fetch --depth 1 origin $(zephyr_hal_infineon_version)
	git -C $@ checkout -q FETCH_HEAD

$(zephyr_build): $(zephyr_src) $(zephyr_cmsis_src) $(zephyr_hal_nxp_dep) $(zephyr_hal_infineon_dep)
	$(zephyr_env) cmake -DCMAKE_PREFIX_PATH=$(zephyr_src)/share/zephyr-package \
		-DZEPHYR_MODULES="$(zephyr_modules)" -B$(zephyr_build) -GNinja \
		-DBOARD_ROOT=$(zephyr_board_root) -DBOARD=$(zephyr_board) \
		-DDTC_OVERLAY_FILE="$(zephyr_overlay)" \
		$(if $(zephyr_conf),-DEXTRA_CONF_FILE=$(zephyr_conf)) \
		$(zephyr_app)

define build-zephyr
$(strip $1): $(zephyr_build)
	$(zephyr_env) ninja -C $(zephyr_build)
	cp $(zephyr_bin) $$@
endef
