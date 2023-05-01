# Linux+Zephyr Demo

This demo duplicates the behavior of the Linux+FreeRTOS demo, with Zephyr
taking the place of FreeRTOS. It features a dual guest configuration, i.e.,
Linux and Zephyr, connected through an inter-VM communication object. This
object is implemented through a shared memory region and a doorbell mechanism
in the form of hardware interrupts.

If Zephyr supports SMP for the target architecture, half of the cores are
assigned to Zephyr and half to Linux. If not, one core is assigned to Zephyr
and the remaining ones to Linux. The platform's first available UART is also
assigned to Zephyr. Any additional UART device is assigned to Linux. The Linux
guest is also accessible via ssh at the static address **192.168.42.15**.

You can send messages to Zephyr by writing to `/dev/baoipc0`. For example:

```
echo "Hello, Bao!" > /dev/baoipc0
```

The Zephyr guest will also send a message to Linux each time it receives a 
character in its UART. However, the Linux inter-VM is not configured for Linux
to asynchronously react to it and output this message. You can checkout the last
Zephyr message by reading `/dev/baoipc0`:

```
cat /dev/baoipc0
```

If you are targetting an MPU platform (i.e. fvp-r), set:

```
export FVPR_VM_IMAGES="$BAO_DEMOS_WRKDIR_IMGS/zephyr.bin@0x24000000 \
    $BAO_DEMOS_WRKDIR_IMGS/linux.bin@0x28000000"
```

Follow the instructions to build [Zephyr](../../guests/zephyr/README.md) 
and [Linux](../../guests/linux/README.md).
