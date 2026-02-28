# BeagleBone AI-64

## 0. We need both the ARMv8 AArch64 and the ARMv7 GCC toolchains, with Linux hard-float ABI toolchain for ARMv7.

Download them from the [Arm Developer's][arm-toolchains] website. Post that:

```bash
export CROSS_COMPILE32=/path/to/toolchain/install/dir/bin/arm-linux-gnueabihf-
export CROSS_COMPILE64=/path/to/toolchain/install/dir/bin/aarch64-none-elf-
```

## 1. Build TF-A

```bash
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware

git clone https://github.com/bao-project/arm-trusted-firmware.git\
    $BAO_DEMOS_ATF --branch bao/demo --depth 1
cd $BAO_DEMOS_ATF

make CROSS_COMPILE=$CROSS_COMPILE64 PLAT=k3 SPD=opteed TARGET_BOARD=generic

cp $BAO_DEMOS_ATF/build/k3/generic/release/bl31.bin $BAO_DEMOS_WRKDIR_PLAT
```

## 2. Build OP-TEE

```bash
export BAO_DEMOS_OPTEE=$BAO_DEMOS_WRKDIR_SRC/optee_os

git clone https://github.com/OP-TEE/optee_os.git\
    $BAO_DEMOS_OPTEE --branch master --depth 1
cd $BAO_DEMOS_OPTEE

make CROSS_COMPILE=$CROSS_COMPILE32 CROSS_COMPILE64=$CROSS_COMPILE64\
    CFG_ARM64_core=y PLATFORM=k3-j721e

cp $BAO_DEMOS_OPTEE/out/arm-plat-k3/core/tee-raw.bin $BAO_DEMOS_WRKDIR_PLAT
```

## 3. Setup TI-FW

```bash
export BAO_DEMOS_TIFW=$BAO_DEMOS_WRKDIR_SRC/ti-linux-firmware

git clone https://github.com/TexasInstruments/ti-linux-firmware.git\
    $BAO_DEMOS_TIFW --branch ti-linux-firmware --depth 1
```

## 4. Setup u-boot

```bash
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot

git clone https://github.com/u-boot/u-boot.git $BAO_DEMOS_UBOOT\
    --branch master --depth 1
cd $BAO_DEMOS_UBOOT
```

## 5. Build u-boot for the R5F core

```bash
make j721e_beagleboneai64_r5_defconfig
make CROSS_COMPILE=$CROSS_COMPILE32 BINMAN_INDIRS=$BAO_DEMOS_TIFW

cp $BAO_DEMOS_UBOOT/tiboot3-j721e-gp-evm.bin $BAO_DEMOS_WRKDIR_PLAT/tiboot3.bin
cp $BAO_DEMOS_UBOOT/sysfw-j721e-gp-evm.itb $BAO_DEMOS_WRKDIR_PLAT/sysfw.itb
```

## 6. Build u-boot for the A72 core

```bash
make j721e_beagleboneai64_a72_defconfig
make CROSS_COMPILE=$CROSS_COMPILE64 BINMAN_INDIRS=$BAO_DEMOS_TIFW \
    BL31=$BAO_DEMOS_WRKDIR_PLAT/bl31.bin \
    TEE=$BAO_DEMOS_WRKDIR_PLAT/tee-raw.bin

cp $BAO_DEMOS_UBOOT/tispl.bin_unsigned $BAO_DEMOS_WRKDIR_PLAT/tispl.bin
cp $BAO_DEMOS_UBOOT/u-boot.img_unsigned $BAO_DEMOS_WRKDIR_PLAT/u-boot.img
```

<!--- instruction#1 -->

## 7. Setup uSD card

Post [preparing the uSD card](../sdcard.md):

```bash
cp $BAO_DEMOS_WRKDIR_PLAT/tiboot3.bin $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_PLAT/sysfw.itb $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_PLAT/tispl.bin $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_PLAT/u-boot.img $BAO_DEMOS_SDCARD

cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD

umount $BAO_DEMOS_SDCARD
```

<!--- instruction#2 -->

## 8. Setup board

- Insert the uSD card in the board's SD/MMC card interface.
- The boot media is set via a "BOOT" button on the board. Please refer to the [board schematics][board-schematics] for more details.

  | BOOT switch position | primary boot media | secondary boot media |
  | -------------------- | ------------------ | -------------------- |
  | not pressed          | eMMC               | uSD card             |
  | pressed              | uSD card           | uSD card             |

- To switch to SD card boot mode, hold the BOOT button while powering on with Type-C power supply, then release when power LED lights up.
- Connect to the board's main serial debug port(uart0) using a USB2TTL adapter, and then, turn on the board.

<!--- instruction#3 -->

9. Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until
you get the u-boot prompt. Then load the bao image, and jump to it:

```
fatload mmc ${mmcdev} 0x82000000 bao.bin; go 0x82000000
```

You should see the firmware, bao and its guests printing on the UART.

At this point, depending on your demo, you might be able connect to one of the
guests via ssh by connecting to the board's ethernet RJ45 socket.

<!--- instruction#end -->

[board-schematics]: https://git.beagleboard.org/beagleboard/beaglebone-ai-64/-/blob/main/BeagleBone_AI-64_SCH.pdf
[arm-toolchains]: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
