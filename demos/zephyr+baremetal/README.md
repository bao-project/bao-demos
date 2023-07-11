# Zephyr+Baremetal Demo

This demo features two guests, Zephyr and a baremetal app, that can communicate
via a shared memory object which includes a shared memory region and a
notification mechanism in the form of hardware interrupts. 

If Zephyr supports SMP for the target architecture, half of the cores are
assigned to Zephyr and half to the baremetal app. If not, one core is assigned
to Zephyr and the remaining ones to the baremetal app. 

This demo is only available in platforms featuring at least two UART
peripherals where one is assigned to the baremetal app and the other to Zephyr.
Each time the baremetal app receives a UART interrupt it will update the shared
memory region with a message.

Zephyr is configured with its shell enabled plus an additional 'baoipc' command
used for interacting with the baremetal app via the shared memory object. To 
read the baremetal app message run:

```
baoipc read 0
```

To write a message in the shared memory and notify the baremetal app run:

```
baoipc write_notify 0 "Hello, Bao!"
```

When receiving the notification, the baremetal app should print the message
written by Zephyr.

Configure the baremetal app for communication:

```
export BAREMETAL_PARAMS="DEMO_IPC=y"
```

If you are targetting an MPU platform (i.e. fvp-r), set:

```
export FVPR_VM_IMAGES="$BAO_DEMOS_WRKDIR_IMGS/zephyr.bin@0x24000000 \
    $BAO_DEMOS_WRKDIR_IMGS/baremetal.bin@0x10000000"
```

To build the baremetal app, in case you are targeting an MPU platform (e.g.
fvp-r), set:

```
export BAREMETAL_PARAMS="$BAREMETAL_PARAMS MEM_BASE=0x10000000"
```

Follow the instructions to build [Zephyr](../../guests/zephyr/README.md) 
and the [baremetal app](../../guests/baremetal/README.md).
