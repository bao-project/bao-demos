# RH850-U2A16 FreeRTOS+Baremetal DEMO
After building the Bao and the guests, follow these steps.

<!--- instruction#1 -->
If you used the `A) Use automated make` option to build the demo images, first export the following variables:

```shell
export BAO_DEMOS=$(realpath .)
export BAO_DEMOS_WRKDIR=$BAO_DEMOS/wrkdir
export BAO_DEMOS_WRKDIR_SRC=$BAO_DEMOS_WRKDIR/srcs
export BAO_DEMOS_WRKDIR_PLAT=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM
export BAO_DEMOS_WRKDIR_IMGS=$BAO_DEMOS_WRKDIR_PLAT/$DEMO
```

<!--- instruction#2 -->
## Generate HEX files

```shell
${CROSS_COMPILE}objcopy -O ihex ${BAO_DEMOS_WRKDIR_SRC}/freertos/build/${PLATFORM}/freertos.elf ${BAO_DEMOS_WRKDIR_IMGS}/freertos.hex
${CROSS_COMPILE}objcopy -O ihex ${BAO_DEMOS_WRKDIR_SRC}/baremetal/build/${PLATFORM}/baremetal.elf ${BAO_DEMOS_WRKDIR_IMGS}/baremetal.hex
```

## Connect the uart
Connect to the UARTs assigned to the guests (baud 500000). For example:

```shell
screen /dev/ttyACM0 500000
screen /dev/ttyUSB0 500000
```

> :information_source: By default, FreeRTOS uses RLIN35 as its UART, while the Baremetal app uses RLIN34. RH850-based boards may expose only one UART through the onboard USB interface. In such cases, a separate UART-to-USB adapter must be connected to the second UART pins (TX/RX). In the board we use, RLIN35 is connected to the onboard USB interface and RLIN34 is exposed through the P6_12 (TX) and P6_15 (RX) pins.

<!--- instruction#3 -->
## Flash the guest and bao binaries

```shell
rfp-cli -device RH850/U2x -tool e2 \
    -osc 40.0 \
    -auth id FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \
    -program -file $BAO_DEMOS_WRKDIR_IMGS/freertos.hex

rfp-cli -device RH850/U2x -tool e2 \
    -osc 40.0 \
    -auth id FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \
    -program -file $BAO_DEMOS_WRKDIR_IMGS/baremetal.hex

rfp-cli -device RH850/U2x -tool e2 \
    -osc 40.0 \
    -auth id FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \
    -program -bin 0x0 $BAO_DEMOS_WRKDIR_IMGS/bao.bin -run
```
<!--- instruction#end -->