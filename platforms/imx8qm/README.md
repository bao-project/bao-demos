# NXP IMX8QM

## 1) Setup firmware

Creat a directory to put the imx build tools:

```
export BAO_DEMOS_NXP_TOOLS=$BAO_DEMOS_WRKDIR_SRC/nxp-tools
mkdir -p $BAO_DEMOS_NXP_TOOLS/scfw
```

### 1.1) SCFW

```
wget -P $BAO_DEMOS_NXP_TOOLS\
    https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/imx-sc-firmware-1.5.0.bin
cd $BAO_DEMOS_NXP_TOOLS
chmod +x imx-sc-firmware-1.5.0.bin
./imx-sc-firmware-1.5.0.bin
```

### 1.2) SECO

Download the and install the seco binary:

```
wget -P $BAO_DEMOS_NXP_TOOLS\
    https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/imx-seco-3.6.3.bin
cd $BAO_DEMOS_NXP_TOOLS
chmod a+x imx-seco-3.6.3.bin
./imx-seco-3.6.3.bin
```

## 1.3) Build U-boot

Setup the u-boot directory variable:

```
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot
```

Download, configure and build it:

```
git clone https://github.com/u-boot/u-boot.git $BAO_DEMOS_UBOOT\
    --depth 1 --branch v2022.10
cd $BAO_DEMOS_UBOOT
make imx8qm_mek_defconfig
make -j $(nproc)
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_UBOOT/u-boot.bin $BAO_DEMOS_WRKDIR_PLAT
```

### 1.4) Build TF-A

Setup the TF-A directory variable:

```
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware
```

Download and build it:

```
git clone https://github.com/bao-project/arm-trusted-firmware.git\
    $BAO_DEMOS_ATF --branch bao/demo --depth 1
cd $BAO_DEMOS_ATF
make PLAT=imx8qm bl31
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_ATF/build/imx8qm/release/bl31.bin $BAO_DEMOS_WRKDIR_PLAT
```


### 1.5) NXP's mkimage tool

```
git clone https://source.codeaurora.org/external/imx/imx-mkimage\
    $BAO_DEMOS_NXP_TOOLS/imx-mkimage --depth 1 --branch rel_imx_5.4.24_2.1.0
```

Go to imx-mkimage target platform directory and copy over the scfw seco firmware
as well as the TF-A's bl31 and uboot binaries. Then build firmware binary.

```
cd $BAO_DEMOS_NXP_TOOLS/imx-mkimage
cp $BAO_DEMOS_NXP_TOOLS/imx-sc-firmware-1.5.0/mx8qm-mek-scfw-tcm.bin\
    iMX8QM/scfw_tcm.bin
cp $BAO_DEMOS_NXP_TOOLS/imx-seco-3.6.3/firmware/seco/mx8qmb0-ahab-container.img\
    iMX8QM
cp $BAO_DEMOS_WRKDIR_PLAT/bl31.bin iMX8QM
cp $BAO_DEMOS_WRKDIR_PLAT/u-boot.bin iMX8QM
make SOC=iMX8QM flash_b0
cp iMX8QM/flash.bin $BAO_DEMOS_WRKDIR_IMGS
```

<!--- instruction#1 -->
## 2) Setup SD card

Finally, [prepare your sd card](./../../platforms/sdcard.md), install the 
firmware on it, and copy bao's final image to it:

---

**WARNING**

Make sure the sd card is properly inserted and $BAO_DEMOS_SDCARD_DEV
exists (for example using `ls $BAO_DEMOS_SDCARD_DEV`) before running `dd`! 

Otherwise it will simply create the file and not actually write to the card. If
this happens you might need to delete it by using 
`sudo rm $BAO_DEMOS_SDCARD_DEV`.

---

```
test -e $BAO_DEMOS_SDCARD_DEV &&\
    sudo dd if=$BAO_DEMOS_WRKDIR_IMGS/flash.bin\
    of=$BAO_DEMOS_SDCARD_DEV bs=1k seek=32 || echo Failed flashing sd card!
cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD 
umount $BAO_DEMOS_SDCARD
```

<!--- instruction#2 -->
## 3) Setup board

Make sure you have the board configured to boot from SD card. Checkout out the
[Get Started with the i.MX 8QuadMax MEK](https://www.nxp.com/document/guide/get-started-with-the-i-mx-8quadmax-mek:GS-iMX-8QM-MEK) 
guide for more information on the boot switch setup.

Insert the sd card in the board's sd slot.

Connect to the  UART via the USB cable. If you are using the dual guest 
configuration, connect to both available UARTs (in our setup /dev/ttyUSB0)
with baud 115200.

Open a new terminal and connect to it. For example:

```
screen /dev/ttyUSB0 115200
```
 
Reset the board.

You should be presented with the u-boot prompt. 

<!--- instruction#3 -->
## 4) Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until 
you get the u-boot prompt. Then load the bao image, and jump to it:

```
fatload mmc 1 0x80200000 bao.bin && go 0x80200000
```

You should see a message from Bao followed by the guests' output on the UART
console.

At this point, depending on your demo, you might be able connect to one of the 
guests via ssh by connecting to the board's ethernet RJ45 socket.

<!--- instruction#end -->
