# Linux+FreeRTOS Demo

This demo features a dual guest configuration, i.e., Linux and FreeRTOS, 
connected through an inter-VM communication object. This object is 
implemented through a shared memory region and a doorbell mechanism in 
the form of hardware interrupts.

One of the platform's cores is assigned to FreeRTOS while all remaining cores
are used by Linux. The platform's first available UART is also assigned to
FreeRTOS. Any additional UART device is assigned to Linux. The Linux guest is 
also accessible via ssh at the static address **192.168.42.15**.

You can send messages to FreeRTOS by writing to `/dev/baoipc0`. For example:

```
echo "Hello, Bao!" > /dev/baoipc0
```

The FreeRTOS guest will also send a message to Linux each time it receives a 
character in its UART. However, the Linux inter-VM is not configured for Linux
to asynchronously react to it and output this message. You can checkout the last
FreeRTOS message by reading `/dev/baoipc0`:

```
cat /dev/baoipc0
```

If you are targetting an MPU platform (i.e. fvp-r), set:

```
export FVPR_VM_IMAGES="$BAO_DEMOS_WRKDIR_IMGS/freertos.bin@0x10000000 \
    $BAO_DEMOS_WRKDIR_IMGS/linux.bin@0x20000000"
```

To build FreeRTOS, in case you are targeting an MPU platform (e.g. fvp-r), set:

```
export FREERTOS_PARAMS="MEM_BASE=0x10000000"
```

Otherwise set:

```
export FREERTOS_PARAMS="STD_ADDR_SPACE=y"
```

And follow the instructions in [FreeRTOS](../../guests/freertos/README.md). To
build linux follow [Linux](../../guests/linux/README.md).
