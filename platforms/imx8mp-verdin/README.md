# Toradex Verdin-IMX8MP

<!--- instruction#1 -->
## 1) Setup SD card

Prepare an SD card [prepare your sd card](./../../platforms/sdcard.md), and
copy bao's final image to it:


```
cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD
umount $BAO_DEMOS_SDCARD
```

<!--- instruction#2 -->
## 2) Setup board

Insert the sd card in the board's sd slot.

Connect to the UART via the USB cable (in our setup /dev/ttyUSB3) with baud
115200.

Open a new terminal and connect to it. For example:

```
screen /dev/ttyUSB3 115200
```

Reset the board.

You should be presented with the u-boot prompt.

<!--- instruction#3 -->
## 3) Run u-boot commands

Quickly press any key to skip autoboot. If not possibly press `ctrl-c` until
you get the u-boot prompt. Then load the bao image, and jump to it:

```
fatload mmc 1 0x60200000 bao.bin && go 0x60200000
```

You should see a message from Bao followed by the guests' output on the UART
console.

At this point, depending on your demo, you might be able connect to one of the
guests via ssh by connecting to the board's ethernet RJ45 socket.

<!--- instruction#end -->
