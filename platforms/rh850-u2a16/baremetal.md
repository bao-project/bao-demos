# RH850-U2A16 berametal DEMO
After building the demo follow these steps.

<!--- instruction#1 -->
## Generate baremetal HEX file

```
${CROSS_COMPILE}objcopy -O ihex ${BAO_DEMOS_WRKDIR_SRC}/baremetal/build/${PLATFORM}/baremetal.elf ${BAO_DEMOS_WRKDIR_IMGS}/baremetal.hex
```

<!--- instruction#2 -->
## Connect the uart
Connect to the UART available on the RH850-U2A16 (baud 500000). For example:

```
screen /dev/ttyACM0 500000
```

<!--- instruction#3 -->
## Flash the guest and bao binaries

```
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
