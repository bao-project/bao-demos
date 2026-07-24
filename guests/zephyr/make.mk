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
zephyr_build:=$(wrkdir_demo_imgs)/zephyr_build
zephyr_board_root:=$(bao_demos)/guests/zephyr
zephyr_board:=baovm_$(PLATFORM)
zephyr_app:=$(bao_demos)/demos/$(DEMO)/zephyr/app
zephyr_bin:=$(zephyr_build)/zephyr/zephyr.bin
zephyr_env:=ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
	CROSS_COMPILE=$(shell which $(CROSS_COMPILE)gcc | sed 's/.\{3\}$$//')
zephyr_patches:=$(wildcard $(bao_demos)/guests/zephyr/patches/$(zephyr_version)/*.patch)
zephyr_modules:=$(zephyr_cmsis_src)
zephyr_overlay:=overlays/$(ARCH)/app.overlay

ifeq ($(PLATFORM),s32z270)
zephyr_modules:=$(zephyr_modules);$(zephyr_hal_nxp_src)
zephyr_hal_nxp_dep:=$(zephyr_hal_nxp_src)
zephyr_board:=baovm_$(PLATFORM)/$(PLATFORM)/rtu0
zephyr_overlay:=overlays/$(ARCH)/app.overlay;boards/$(PLATFORM).overlay
endif

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

$(zephyr_build): $(zephyr_src) $(zephyr_cmsis_src) $(zephyr_hal_nxp_dep)
	$(zephyr_env) cmake -DCMAKE_PREFIX_PATH=$(zephyr_src)/share/zephyr-package \
		-DZEPHYR_MODULES="$(zephyr_modules)" -B$(zephyr_build) -GNinja \
		-DBOARD_ROOT=$(zephyr_board_root) -DBOARD=$(zephyr_board) \
		-DDTC_OVERLAY_FILE="$(zephyr_overlay)" \
		$(zephyr_app)

define build-zephyr
$(strip $1): $(zephyr_build)
	$(zephyr_env) ninja -C $(zephyr_build)
	cp $(zephyr_bin) $$@
endef
