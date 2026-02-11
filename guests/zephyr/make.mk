zephyr_src:=$(wrkdir_src)/zephyr
zephyr_repo:=https://github.com/zephyrproject-rtos/zephyr.git
zephyr_version:=v4.1.0
zephyr_cmsis_repo:=https://github.com/zephyrproject-rtos/cmsis.git
zephyr_cmsis_src:=$(wrkdir_src)/cmsis
zephyr_cmsis_commit:=5a00331455dd74e31e80efa383a489faea0590e3
zephyr_hal_nxp_src:=$(wrkdir_src)/nxp
zephyr_hal_nxp_repo:=https://github.com/bao-project/hal_nxp.git
zephyr_build:=$(wrkdir_demo_imgs)/zephyr_build
zephyr_board_root:=$(bao_demos)/guests/zephyr
zephyr_board:=baovm_$(PLATFORM)
zephyr_app:=$(bao_demos)/demos/$(DEMO)/zephyr/app
zephyr_bin:=$(zephyr_build)/zephyr/zephyr.bin
zephyr_env:=ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
	CROSS_COMPILE=$(shell which $(CROSS_COMPILE)gcc | sed 's/.\{3\}$$//')
zephyr_patches:=$(wildcard $(bao_demos)/guests/zephyr/patches/$(zephyr_version)/*.patch)
zephyr_modules:=$(zephyr_cmsis_src)

ifeq ($(PLATFORM),s32z270)
zephyr_modules:=$(zephyr_modules);$(zephyr_hal_nxp_src)
zephyr_hal_nxp_dep:=$(zephyr_hal_nxp_src)
zephyr_board:=baovm_$(PLATFORM)/$(PLATFORM)/rtu0
zephyr_overlay:=app.overlay;boards/$(PLATFORM).overlay
endif

$(zephyr_src):
	git clone --branch $(zephyr_version) --depth 1 $(zephyr_repo) $@
	git -C $(zephyr_src) apply $(zephyr_patches)

zephyr_bin:=$(zephyr_build)/zephyr/zephyr.bin

$(zephyr_cmsis_src):
	git clone $(zephyr_cmsis_repo) $@
	git -C $(zephyr_cmsis_src) checkout $(zephyr_cmsis_commit)

$(zephyr_hal_nxp_src):
	git clone $(zephyr_hal_nxp_repo) --branch $(zephyr_version) --depth 1 $@

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
