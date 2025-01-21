
TORIZONOS_MACHINE=verdin-imx8mp
UBOOT_DEFCONFIG=verdin-imx8mp_defconfig

toradex-easy-installer-version=Verdin-iMX8MP_ToradexEasyInstaller_6.8.1+build.9
toradex-easy-installer-link:=https://tezi.toradex.com/artifactory/tezi-oe-prod-frankfurt/kirkstone-6.x.y/release/9/verdin-imx8mp/tezi/tezi-run/oedeploy/$(toradex-easy-installer-version).zip
toradex-easy-installer:=$(toradex_tools)/$(toradex-easy-installer-version)

environment+=BAO_DEMOS_TORADEX_EASY_INSTALLER=$(toradex-easy-installer)
environment+=BAO_DEMOS_TORADEX_EASY_INSTALLER_VERSION=$(toradex-easy-installer-version)

$(bao_src):
	@git clone --branch $(bao_version) $(bao_repo) $(bao_src)
	@cd $(bao_src) && git apply $(bao_demos)/platforms/$(PLATFORM)/0001-feat-armv8-add-configurable-SMC-passthrough.patch

BAO_SRC_DEFINED = 1
