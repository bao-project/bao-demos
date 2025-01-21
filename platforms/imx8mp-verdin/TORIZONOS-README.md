# Toradex Verdin-IMX8MP Torizon OS Deployment

Follow the [Torizon OS guest setup steps](./../../guests/torizonos/README.md)
before proceeding with the platform-specific instructions below.

<!--- instruction#1 -->
## 1. Copy Bao image to the SD card

Copy bao's final image to the sdcard:

```shell
cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD
```

<!--- instruction#2 -->
## 2. Reset and run u-boot commands

After validating Torizon OS boots, reboot the board and quickly press any key
to skip autoboot.

```
fatload mmc 1 0x80200000 bao.bin && go 0x80200000
```

You should see a message from Bao followed by the FreeRTOS output on the UART
console.

At this point, connect to TorizonOS via ssh by connecting to the board's
ethernet RJ45 socket.
Update access permissions to the bao driver:

``` shell
sudo chmod 666 /dev/baoipc0
```

You can communicate with freeRTOS through /dev/baoipc0. For example:

``` shell
echo "Hello from TorizonOS" > /dev/baoipc0
cat /dev/baoipc0
```

<!--- instruction#end -->

