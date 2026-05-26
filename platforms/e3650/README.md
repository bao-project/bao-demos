# E3650 Platform Support

This directory contains the platform-specific configurations and build rules for the **Semidrive E3650** platform.

## Build Instructions

To build a demo, use the standard `make` command from the root `bao-demos` directory. You must specify `PLATFORM=e3650` and the corresponding `CROSS_COMPILE`.

### Prerequisites
*   `arm-none-eabi-` toolchain (AArch32)
*   `python3`
*   `kconfiglib` Python package for the NuttX guest demos

For demos that include NuttX guests, the build system generates local
wrappers for the upstream `kconfiglib` command-line tools under
`wrkdir/bin/nuttx-kconfig/bin`.

### Building Baremetal
```bash
make PLATFORM=e3650 DEMO=baremetal CROSS_COMPILE=arm-none-eabi-
```

The build compiles Bao and the guest images automatically. Packaging them into a
`.pac` file fetches the E3650 packaging helper automatically and uses any
required Semidrive tooling/materials available under that helper tree.

## Output Artifacts

The build system fetches the packaging helper automatically, but it does not
populate vendor-provided Semidrive PAC assets that may be missing.

*   **Output Location**: `wrkdir/imgs/e3650/<DEMO_NAME>/bao_pack_output/bao.pac`
*   **Intermediate Binaries**: `wrkdir/imgs/e3650/<DEMO_NAME>/*.bin`

## Flashing

Flash the bootloader from your packaging tool package
(`$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/bootloader/e3650_bootloader.pac`) and
the generated `bao.pac` package into the E3650 board using Semidrive's
SDFactoryTool. Access to these Semidrive tools typically requires an active
support agreement or direct contact with [Semidrive](https://www.semidrive.com/).

<!--- instruction#1 -->
The E3650 packaging helper is downloaded automatically.

Follow these steps:

1. If packaging reports missing PAC dependencies, place these files under:

	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool

	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/atb_signer
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/bin2hex
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/pactool
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/flashloader/E3650/flashloader.out
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/keys/TestRSA2048_ossl.pem

	Set executable permission on:
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/atb_signer
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/bin2hex
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/pactool

2. If you do not already have these files, contact Semidrive to obtain:

	SDToolBox, used to flash the bootloader and `bao.pac`
	E3_SSDK_PTG5.2_Source_Code.tar.gz, used to extract:
	- tools/common/image_gen/builtin_tools/sdtools/linux/atb_signer
	- tools/common/image_gen/builtin_tools/sdtools/linux/bin2hex
	- tools/common/image_gen/builtin_tools/sdtools/linux/pactool
	- tools/common/flashloader/E3650/flashloader.out
	- tools/common/image_gen/res_default/keys/TestRSA2048_ossl.pem

3. After all required files are in place, press Enter to continue the build.
<!--- instruction#2 -->

Provide any missing Semidrive PAC assets under the cloned helper tree.

The build currently checks for these required paths:

	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/atb_signer
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/bin2hex
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/builtin_tools/pactool
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/flashloader/E3650/flashloader.out
	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/keys/TestRSA2048_ossl.pem

The packaging helper entry point is:

	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/bao_pac.sh
<!--- instruction#3 -->

PAC packaging completed.

Generated package:

	$BAO_DEMOS_WRKDIR_IMGS/bao_pack_output/bao.pac

Intermediate images used for packaging are under:

	$BAO_DEMOS_WRKDIR_IMGS

To flash the board, use Semidrive's SDFactoryTool to flash bootloader first:

	$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/bootloader/simple_bootloader.pac

and then flash generated `bao.pac`.

Contact Semidrive to obtain SDToolBox if you do not have it.

<!--- instruction#end -->
