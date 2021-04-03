# Xilinx ZCU10X

<!--- instruction#1 -->
## 1) Get firmware

Download latest pre-built Zynq UltraScale+ MPSoC Firmware for your target 
platform (you will need to sign-in to Xilinx account and provide further 
personal information) to $BAO_DEMOS_WRKDIR_SRC:

* [ZCU102](https://www.xilinx.com/member/forms/download/xef.html?filename=2020.2-zcu102-release.tar.xz)
* [ZCU104](https://www.xilinx.com/member/forms/download/xef.html?filename=2020.2-zcu104-release.tar.xz)

<!--- instruction#end -->

Extract it  to $BAO_DEMOS_WRKDIR_SRC: 

```
tar xvfm $BAO_DEMOS_WRKDIR_SRC/2020.2-$PLATFORM-release.tar.xz\
    -C $BAO_DEMOS_WRKDIR_PLAT --wildcards "*BOOT.BIN" --strip-components=1
```

Alternatively, you can [build the firmware from scratch][firmware-from-scratch]. 

## 2) Prepare a U-boot image

Run mkimage to create the final system image:

```
mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
    -e 0x200000 -d $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_WRKDIR_IMGS/bao.img
```

<!--- instruction#2 -->
## 3) Setup SD card

After [preparing your sd card](../../platforms/sdcard.md), copy the firmware and 
bao's final image to it:

```
cp $BAO_DEMOS_WRKDIR_PLAT/BOOT.BIN $BAO_DEMOS_SDCARD
cp $BAO_DEMOS_WRKDIR_IMGS/bao.img $BAO_DEMOS_SDCARD
umount $BAO_DEMOS_SDCARD
```

<!--- instruction#3 -->
## 4) Setup board

First make sure you have the board configured to boot from the SD card. If you 
are not sure how, check the MPSoC Device Configuration section in the board's 
User Guide:

- [ZCU102](https://www.xilinx.com/support/documentation/boards_and_kits/zcu102/ug1182-zcu102-eval-bd.pdf) 
- [ZCU104](https://www.xilinx.com/support/documentation/boards_and_kits/zcu104/ug1267-zcu104-eval-bd.pdf)

Insert the sd card in the board's sd slot and connect to the baord via the 
available micro-USB JTAG/UART port.

Connect to both UARTs available on the zcu (baud 115200). In our setup:

* /dev/ttyUSB1 and /dev/ttyUSB2 for ZCU104
* /dev/ttyUSB0 and /dev/ttyUSB1 for ZCU102

Open a new terminal and connect to it. For example:

```
screen /dev/ttyUSB1 115200
```

Turn on/reset your board.

<!--- instruction#4 -->
## 5) Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until 
you get the u-boot prompt. Then load the bao image, and jump to it:

```
fatload mmc 0 0x200000 bao.img; bootm start 0x200000; bootm loados; bootm go
```

You should see the firmare, bao and its guests printing on the UARTs.

At this point, depending on your demo, you might be able connect to one of the 
guests via ssh by connecting to the board's ethernet RJ54 socket.

<!--- instruction#end -->

<!-- Links -->

[firmware-from-scratch]: https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18841722/ZCU102+Image+creation+in+OSL+flow