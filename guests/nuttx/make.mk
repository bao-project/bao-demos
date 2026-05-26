nuttx_src:=$(wrkdir_src)/nuttx
nuttx_repo:=https://github.com/apache/nuttx.git
nuttx_version:=nuttx-12.12.0
nuttx_patches:=$(wildcard $(bao_demos)/guests/nuttx/patches/nuttx/$(nuttx_version)/*.patch)

nuttx_apps_src:=$(wrkdir_src)/apps
nuttx_apps_repo:=https://github.com/apache/nuttx-apps.git
nuttx_apps_version:=nuttx-12.12.0
nuttx_apps_patches:=$(wildcard $(bao_demos)/guests/nuttx/patches/apps/$(nuttx_apps_version)/*.patch)

# For e3650, we use out-of-tree board configurations from guests/nuttx
nuttx_board_src:=$(bao_demos)/guests/nuttx/boards/$(PLATFORM)
nuttx_kconfig_dir:=$(wrkdir_bin)/nuttx-kconfig
nuttx_kconfig_bin:=$(nuttx_kconfig_dir)/bin
nuttx_kconfig_stamp:=$(nuttx_kconfig_dir)/.installed

nuttx_build_dir:=$(wrkdir_demo_imgs)/nuttx_build
nuttx_board_profile:=bao_guest

nuttx_bin:=$(nuttx_build_dir)/nuttx.bin

nuttx_cc_name:=$(CROSS_COMPILE)gcc
nuttx_cxx_name:=$(CROSS_COMPILE)g++
nuttx_cc_path:=$(strip $(shell command -v $(nuttx_cc_name) 2>/dev/null))
nuttx_cxx_path:=$(strip $(shell command -v $(nuttx_cxx_name) 2>/dev/null))
nuttx_toolchain_bindir:=$(patsubst %/,%,$(dir $(nuttx_cc_path)))

ifeq ($(nuttx_cc_path),)
$(error Could not find $(nuttx_cc_name) in PATH. Please install the Arm GNU toolchain or export PATH to include it)
endif

ifeq ($(nuttx_cxx_path),)
$(error Could not find $(nuttx_cxx_name) in PATH. Please install the Arm GNU toolchain or export PATH to include it)
endif

$(nuttx_src):
	git clone --depth 1 --branch $(nuttx_version) $(nuttx_repo) $@
ifneq ($(strip $(nuttx_patches)),)
	git -C $@ apply $(nuttx_patches)
endif

$(nuttx_apps_src):
	git clone --depth 1 --branch $(nuttx_apps_version) $(nuttx_apps_repo) $@
ifneq ($(strip $(nuttx_apps_patches)),)
	git -C $@ apply $(nuttx_apps_patches)
endif

$(nuttx_kconfig_stamp):
	python3 -c "import olddefconfig, savedefconfig, setconfig"
	mkdir -p $(nuttx_kconfig_bin)
	printf '%s\n' '#!/usr/bin/env python3' 'from olddefconfig import main' 'raise SystemExit(main())' > $(nuttx_kconfig_bin)/olddefconfig
	printf '%s\n' '#!/usr/bin/env python3' 'from savedefconfig import main' 'raise SystemExit(main())' > $(nuttx_kconfig_bin)/savedefconfig
	printf '%s\n' '#!/usr/bin/env python3' 'from setconfig import main' 'raise SystemExit(main())' > $(nuttx_kconfig_bin)/setconfig
	chmod +x $(nuttx_kconfig_bin)/olddefconfig $(nuttx_kconfig_bin)/savedefconfig $(nuttx_kconfig_bin)/setconfig
	touch $@

define configure-nuttx
	mkdir -p $(nuttx_build_dir)
	env PATH="$(nuttx_kconfig_bin):$(nuttx_toolchain_bindir):$${PATH}" \
		cmake -S $(nuttx_src) -B $(nuttx_build_dir) \
		-DBOARD_CONFIG:STRING="$(nuttx_board_src)/configs/$(nuttx_board_profile)" \
		-DCMAKE_C_COMPILER=$(nuttx_cc_path) \
		-DCMAKE_CXX_COMPILER=$(nuttx_cxx_path) \
		-DCMAKE_ASM_COMPILER=$(nuttx_cc_path)
endef

$(nuttx_build_dir): $(nuttx_src) $(nuttx_apps_src) $(nuttx_kconfig_stamp)
	$(call configure-nuttx)

define build-nuttx
$(strip $1): $(nuttx_build_dir)
	env PATH="$(nuttx_kconfig_bin):$(nuttx_toolchain_bindir):$${PATH}" cmake --build $(nuttx_build_dir)
	cp $(nuttx_bin) $$@
endef
