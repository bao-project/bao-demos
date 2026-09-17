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

If you are targetting an Arm MPU platform set, you need to set the images variable:

For FVP-R:

```
export FVPR_VM_IMAGES="$BAO_DEMOS_WRKDIR_IMGS/zephyr.bin@0x24000000 \
    $BAO_DEMOS_WRKDIR_IMGS/baremetal.bin@0x10000000"
```

For MPS3-AN536:

```
export MPS3_VM_IMAGES="$BAO_DEMOS_WRKDIR_IMGS/baremetal.bin@0x32200000 \
    $BAO_DEMOS_WRKDIR_IMGS/zephyr.bin@0x40000000"
```

To build the baremetal app, in case you are targeting an MPU platform, set the
required parameters.

For FVP-R:

```
export BAREMETAL_PARAMS="$BAREMETAL_PARAMS MEM_BASE=0x10000000"
```

For MPS3-AN536:

```
export BAREMETAL_PARAMS="$BAREMETAL_PARAMS MEM_BASE=0x32200000 SHMEM_BASE=0x70000000 SHMEM_SIZE=0x200000"
```

The Zephyr board for MPS3-AN536 is `baovm_mps3-an536`.

### TC4D7 (TriCore)

Unlike the MPU platforms above, TC4D7 doesn't need the `*_VM_IMAGES`/
`BAREMETAL_PARAMS` env vars -- both guests' addresses are fixed in
[`configs/tc4dx.c`](configs/tc4dx.c) and get embedded directly into `bao.elf`
at build time (loaded via TRACE32, see [platforms/tc4dx](../../platforms/tc4dx/README.md)).

The board only has one ASCLIN pinned by default (ASCLIN0), which this demo
gives to Zephyr; the baremetal guest instead uses ASCLIN1, selected by
building bao-baremetal-guest with `TC4DX_ASCLIN=1` (already wired up in this
demo's `make.mk` for `PLATFORM=tc4dx`, no manual step needed).

The Zephyr board is `kit_a3g_tc4d7_lite/tc4d7xp/cpu0` (no custom `baovm_*`
board -- it ships inside the `hal_infineon` module and is resolved via
`ZEPHYR_MODULES`). Zephyr's guest source is a fork
([linumiz/zephyr-tricore](https://github.com/linumiz/zephyr-tricore),
`dev/ifx/tricore-upstream`) rather than upstream Zephyr, since TriCore
architecture support isn't upstream yet -- see the TODO comment in
[`guests/zephyr/make.mk`](../../guests/zephyr/make.mk) for why that's
currently pointed at a local mirror instead of a GitHub URL.

Follow the instructions to build [Zephyr](../../guests/zephyr/README.md) 
and the [baremetal app](../../guests/baremetal/README.md).
