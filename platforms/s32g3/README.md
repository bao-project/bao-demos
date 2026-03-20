# NXP S32G3

## 1) Setup firmware

Creat a directory to put the imx build tools:

```bash
export BAO_DEMOS_NXP_TOOLS=$BAO_DEMOS_WRKDIR_SRC/nxp-tools
```

### 1.1) DDR FW

Create a folder for NXP tools:
```bash
export BAO_DEMOS_NXP_TOOLS=$BAO_DEMOS_WRKDIR_SRC/nxp-tools
mkdir -p $BAO_DEMOS_NXP_TOOLS
```

Download the DDR firmware from NXP:
1. Log in into your NXP account and go to [S32G3 BSP Download page](https://nxp.flexnetoperations.com/control/frse/download?agree=Accept&element=7739171)
2. Download `s32g3_linuxbsp_46.0.0_binaries.tgz`
3. Place it in your nxp-tools folder:
```bash
mv ~/Downloads/s32g3_linuxbsp_46.0.0_binaries.tgz $BAO_DEMOS_NXP_TOOLS/
```

Unpack the firmware:
```bash
cd $BAO_DEMOS_NXP_TOOLS
tar -xzf s32g3_linuxbsp_46.0.0_binaries.tgz
```

Verify the DDR firmware binary exists:
```bash
ls -lh $BAO_DEMOS_NXP_TOOLS/s32g3_linuxbsp_46.0_binaries/s32g399ardb3/ddr_fw_ecc_on.bin
```

After this, the DDR firmware is ready for the ATF build.

### 1.2) Build U-boot

Setup the u-boot directory variable:

```bash
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot
```

Download, configure it:

```bash
git clone https://github.com/nxp-auto-linux/u-boot $BAO_DEMOS_UBOOT\
    --depth 1 --branch bsp46.0-2022.04
cd $BAO_DEMOS_UBOOT
make s32g399ardb3_defconfig
```

Now you need to set the Kconfig options:

* CONFIG_AUTOBOOT=n

You can do it via using an interface such as `menuconfig` or just write them 
directly to the config file:

```bash
echo "CONFIG_AUTOBOOT=n\n" >> $BAO_DEMOS_UBOOT/.config
```

And build it:

```bash
make -j $(nproc)
```

And copy the image to the platform's working directory:

```bash
cp $BAO_DEMOS_UBOOT/u-boot.bin $BAO_DEMOS_WRKDIR_PLAT
cp $BAO_DEMOS_UBOOT/u-boot-nodtb.bin $BAO_DEMOS_WRKDIR_PLAT
```

### 1.3) Build TF-A

Setup the TF-A directory variable:

```bash
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware
```

Download and build it:

```bash
git clone https://github.com/nxp-auto-linux/arm-trusted-firmware\
    $BAO_DEMOS_ATF --branch bsp46.0-2.10.25 --depth 1
cd $BAO_DEMOS_ATF
make PLAT=s32g399ardb3 BL33=$BAO_DEMOS_WRKDIR_PLAT/u-boot-nodtb.bin BL33DIR=$BAO_DEMOS_UBOOT DDR_FW_BIN_PATH=$BAO_DEMOS_NXP_TOOLS/s32g3_linuxbsp_46.0_binaries/s32g399ardb3/ddr_fw_ecc_on.bin
```

And copy the images to the platform's working directory:

```bash
cp $BAO_DEMOS_ATF/build/imx8qm/release/bl2_w_dtb.s32 $BAO_DEMOS_WRKDIR_PLAT
cp $BAO_DEMOS_ATF/build/imx8qm/release/fip.bin $BAO_DEMOS_WRKDIR_PLAT
```

**Note**: The TF-A compilation produces two binary artifacts used in the boot process: bl2_w_dtb.s32 and
fip.bin. The fip.bin binary artifact contains the second TF-A boot stage (BL31) and the U-Boot
binary (BL33). Hence, the TF-A build process depends on U-Boot being available. To that end, the BL33
parameter to the make command-line must be the path to the u-boot-nodtb.bin located in the directory
where U-Boot has been built as part of the prerequisites. 

<!--- instruction#1 -->
## 2) Setup SD card

Finally, [prepare your sd card](./../../platforms/s32g3/sdcard.md), install the 
firmware on it, and copy bao's final image to it:

```bash
sudo dd\
    if=$BAO_DEMOS_WRKDIR_IMGS/bl2_w_dtb.s32\
    of=/dev/mmcblk0\
    conv=notrunc,fsync seek=512 skip=512 oflag=seek_bytes iflag=skip_bytes
sudo dd\
    if=$BAO_DEMOS_WRKDIR_IMGS/fip.bin\
    of=/dev/mmcblk0p1\
    conv=notrunc,fsync
sudo cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD
sync
umount $BAO_DEMOS_SDCARD
```

<!--- instruction#2 -->
## 3) Setup board

Make sure you have the board configured to boot from SD card. Checkout out the
[Getting Started with the S32G Reference Design Board 3 for Vehicle Network Processing](https://www.nxp.com/document/guide/getting-started-with-the-s32g-reference-design-board-3-for-vehicle-network-processing:GS-S32G-VNP-RDB3) 
guide for more information on the boot switch setup.

Insert the sd card in the board's sd slot.

Connect to the  UART via the USB cable. If you are using the dual guest 
configuration, connect to both available UARTs (in our setup /dev/ttyUSB0)
with baud 115200.

Open a new terminal and connect to it. For example:

```bash
screen /dev/ttyUSB0 115200
```
 
Reset the board.

You should be presented with the u-boot prompt. 

<!--- instruction#3 -->
## 4) Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until 
you get the u-boot prompt. Then load the bao image, and jump to it:

```bash
fatload mmc 0:2 0xF0000000 bao.bin && go 0xF0000000
```

You should see a message from Bao followed by the guests' output on the UART
console.

At this point, depending on your demo, you might be able connect to one of the 
guests via ssh by connecting to the board's ethernet RJ45 socket.

<!--- instruction#end -->
