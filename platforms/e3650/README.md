# Semidrive E3650

## 1) Get the PAC packaging tool

Bao images for the E3650 are packaged into a `.pac` file with `e3650_pac_tool`,
a helper script that wraps Semidrive's proprietary signing and packaging
tools. Clone it:

```
export BAO_DEMOS_PAC_TOOL=$BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool
git clone --recursive --branch main \
    https://github.com/bao-project/e3650_pac_tool.git $BAO_DEMOS_PAC_TOOL
```

<!--- instruction#1 -->
## 2) Provide the Semidrive vendor assets

The packaging helper itself is open source, but packaging also depends on the
following Semidrive-provided vendor assets. They are not redistributed here
and will always be missing on a fresh checkout:

	builtin_tools/atb_signer
	builtin_tools/bin2hex
	builtin_tools/pactool
	flashloader/E3650/flashloader.out
	keys/TestRSA2048_ossl.pem

If you don't already have these files, contact [Semidrive](https://www.semidrive.com/)
to request `E3_SSDK_PTG5.2_Source_Code.tar.gz`, which contains them under:

	ssdk/tools/common/image_gen/builtin_tools/sdtools/linux/atb_signer
	ssdk/tools/common/image_gen/builtin_tools/sdtools/linux/bin2hex
	ssdk/tools/common/image_gen/builtin_tools/sdtools/linux/pactool
	ssdk/tools/common/flashloader/E3650/flashloader.out
	ssdk/tools/common/image_gen/res_default/keys/TestRSA2048_ossl.pem

While you're at it, also request `SDToolBox` from Semidrive; it is used
further ahead, together with SDFactoryTool, to flash the bootloader and
`bao.pac`.

<!--- instruction#end -->

With the `E3_SSDK_PTG5.2_Source_Code` archive extracted, export its path.

```
export BAO_DEMOS_E3_SSDK=/path/to/E3_SSDK_PTG5.2_Source_Code
```

Now, copy the vendor assets into the packaging helper:

```
ssdk_common=$BAO_DEMOS_E3_SSDK/ssdk/tools/common

mkdir -p $BAO_DEMOS_PAC_TOOL/builtin_tools \
    $BAO_DEMOS_PAC_TOOL/flashloader/E3650 $BAO_DEMOS_PAC_TOOL/keys

cp $ssdk_common/image_gen/builtin_tools/sdtools/linux/atb_signer \
    $BAO_DEMOS_PAC_TOOL/builtin_tools/
cp $ssdk_common/image_gen/builtin_tools/sdtools/linux/bin2hex \
    $BAO_DEMOS_PAC_TOOL/builtin_tools/
cp $ssdk_common/image_gen/builtin_tools/sdtools/linux/pactool \
    $BAO_DEMOS_PAC_TOOL/builtin_tools/
cp $ssdk_common/flashloader/E3650/flashloader.out \
    $BAO_DEMOS_PAC_TOOL/flashloader/E3650/
cp $ssdk_common/image_gen/res_default/keys/TestRSA2048_ossl.pem \
    $BAO_DEMOS_PAC_TOOL/keys/

chmod +x $BAO_DEMOS_PAC_TOOL/builtin_tools/atb_signer \
    $BAO_DEMOS_PAC_TOOL/builtin_tools/bin2hex \
    $BAO_DEMOS_PAC_TOOL/builtin_tools/pactool
```

## 3) Create the bao.pac file

Generate `bao.pac` from Bao's image and the guest image(s) built for your
chosen demo.

For `DEMO=baremetal`:

```
$BAO_DEMOS_PAC_TOOL/bao_pac.sh $BAO_DEMOS_WRKDIR_IMGS/bao.bin \
    $BAO_DEMOS_WRKDIR_IMGS/baremetal.bin
```

For `DEMO=nuttx+baremetal`:

```
$BAO_DEMOS_PAC_TOOL/bao_pac.sh $BAO_DEMOS_WRKDIR_IMGS/bao.bin \
    $BAO_DEMOS_WRKDIR_IMGS/nuttx.bin
```

For this demo, both the NuttX and bare-metal guest images are already linked
into `bao.bin` (see `demos/nuttx+baremetal/configs/e3650.c`). `nuttx.bin` is
passed here only to satisfy `bao_pac.sh`'s required `<guest.bin>` argument;
`baremetal.bin` does not need to be passed separately.

The package is written to `$BAO_DEMOS_WRKDIR_IMGS/bao_pack_output/bao.pac`.

<!--- instruction#2 -->
## 4) Flash the bootloader and Bao

Flashing the bootloader and Bao uses a SEGGER J-Trace (or J-Link) probe.
Connect the JTAG probe to the J401 connector, and a USB cable to the J1401
UART interface.

Set the SW601 switch to debug mode, then power on the board with the SW201
switch.

Using Semidrive's SDFactoryTool, flash the following two files in order,
repeating the steps below for each:

	1. $BAO_DEMOS_WRKDIR_SRC/e3650_pac_tool/bootloader/simple_bootloader.pac
	2. $BAO_DEMOS_WRKDIR_IMGS/bao_pack_output/bao.pac

For each file:

1. Press "Load packet":
   - Select "Multi Pac" and "Load pac in safe mode"
   - Select the "NVM" memory and open the file
2. Press "OK".
3. Press "Start Downloading" and wait for the status to read "Finish".

---

**NOTE**

Access to Semidrive's tools (SDFactoryTool, SDToolBox) typically requires an
active support agreement or direct contact with
[Semidrive](https://www.semidrive.com/).

---

<!--- instruction#end -->
