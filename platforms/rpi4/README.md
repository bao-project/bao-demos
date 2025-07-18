# Raspberry Pi 4 Model B

## 1) Get firmware

Download the latest firmware files for Raspberry Pi:

```
export BAO_DEMOS_FW=$BAO_DEMOS_WRKDIR_PLAT/firmware 
git clone https://github.com/raspberrypi/firmware.git $BAO_DEMOS_FW\
    --depth 1 --branch 1.20210201
```

## 2) Build U-boot

```
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot
git clone https://github.com/u-boot/u-boot.git $BAO_DEMOS_UBOOT\
    --depth 1 --branch v2022.10
cd $BAO_DEMOS_UBOOT
make rpi_4_defconfig
```

Then build it:

```
make -j $(nproc)
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_UBOOT/u-boot.bin $BAO_DEMOS_WRKDIR_PLAT
```

## 3) Build TF-A

```
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware 
git clone https://github.com/bao-project/arm-trusted-firmware.git\
    $BAO_DEMOS_ATF --branch bao/demo-next --depth 1
cd $BAO_DEMOS_ATF
make PLAT=rpi4
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_ATF/build/rpi4/release/bl31.bin $BAO_DEMOS_WRKDIR_PLAT
```


<!--- instruction#1 -->
## 3) Setup SD card

After [preparing your sd card](../../platforms/sdcard.md), copy the firmware 
and bao's final image to it:

```
cp -rf $BAO_DEMOS_WRKDIR_PLAT/firmware/boot/* $BAO_DEMOS_SDCARD
cp $BAO_DEMOS/platforms/rpi4/config.txt $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_PLAT/bl31.bin $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_PLAT/u-boot.bin $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD
umount $BAO_DEMOS_SDCARD
```


<!--- instruction#2 -->
## 4) Setup board

Insert the sd card in the board's sd slot.

Connect to the Raspberry Pi's UART using a USB-to-TTL adapter to connect to the
Raspberry Pi's GPIO header UART pins. Use a terminal application such as 
`screen`. For example:

```
screen /dev/ttyUSB0 115200
```

Turn on/reset your board.


<!--- instruction#3 -->
## 5) Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until 
you get the u-boot prompt. Then load the bao image, and jump to it:

```
fatload mmc 0 0x200000 bao.bin; go 0x200000
```

You should see the firmware, bao and its guests printing on the UART.

At this point, depending on your demo, you might be able connect to one of the 
guests via ssh by connecting to the board's ethernet RJ45 socket.


<!--- instruction#end -->
