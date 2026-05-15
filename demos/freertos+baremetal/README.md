# FreeRTOS + Baremetal Demo

This demo features two guests running on top of Bao: **FreeRTOS** and a **Baremetal app**. The two guests can communicate via a shared memory object which includes a shared memory region and a notification mechanism in the form of a hardware interrupt.

One of the platform's cores is assigned to FreeRTOS, while all remaining cores are assigned to the Baremetal app.

> :information_source: This demo is only available in platforms featuring at least two UART peripherals, where one is assigned to the FreeRTOS and the other to the Baremetal app.

The Baremetal app is configured with a periodic timer interrupt in the first vCPU that starts an IPI chain to the remaining vCPUs. Additionally, the Baremetal app configures an UART RX interrupt. Every time it receives a character, it prints a message and copies the received character to the shared memory region and sends a notification to FreeRTOS.

The FreeRTOS guest is configured with two tasks and an UART RX interrupt which prints a message every time a character is received via console. When FreeRTOS receives a notification from the Baremetal app, it parses the received character and performs the corresponding operation in a local counter:
- `i`/`I`: Increments the counter
- `d`/`D`: Decrements the counter
- `c`/`C`: Clears the counter
- *others*: unsupported option

## Building the Guests

To build FreeRTOS and the Baremetal app, some env variables require specific values according to your platform.

### RH850/U2A16

Follow the steps exactly in the order listed below:

1. To build FreeRTOS, run:

```shell
export APP_SRC_DIR="$BAO_DEMOS/demos/freertos+baremetal/freertos-app"
export RO_MEM_BASE=0x100000
export RO_MEM_SIZE=0x100000
export RW_MEM_BASE=0xfe100000
export RW_MEM_SIZE=0x80000
export SHMEM_BASE=0xfe480000
export SHMEM_SIZE=0x1000
```

2. Follow the instructions in [FreeRTOS](../../guests/freertos/README.md).


3. Then, to build the Baremetal app, run:

```shell
export APP_SRC_DIR="$BAO_DEMOS/demos/freertos+baremetal/baremetal-app"
export RO_MEM_BASE=0x200000
export RO_MEM_SIZE=0x100000
export RW_MEM_BASE=0xfe400000
export RW_MEM_SIZE=0x80000
export SHMEM_BASE=0xfe480000
export SHMEM_SIZE=0x1000
export CPPFLAGS="\
-DPLAT_UART_ADDR=0xFFD28400UL \
-DUART_IRQ_ID=434UL \
-DPLAT_OSTM_BASE=0xFFBF0100UL \
-DTIMER_IRQ_ID=200UL"
```

4. Follow the instructions in [Baremetal](../../guests/baremetal/README.md).

5. To avoid any conflict with further steps, unset the CPPFLAGS env variable
```shell
unset CPPFLAGS
```