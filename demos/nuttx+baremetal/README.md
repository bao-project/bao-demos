# NuttX+Baremetal Demo

This demo features two guests, NuttX and a baremetal app, connected through a
shared memory mailbox. Unlike the interrupt-driven inter-VM demos, this
mailbox is polled by both guests, which exchange messages through sequence
counters and text buffers.

This demo is only available on the `e3650` platform. Bao assigns NuttX to
CPUs `0-1` (affinity `0x3`) and the baremetal app to CPUs `2-3` (affinity
`0xc`). Each guest is also given its own UART: NuttX uses UART18
(`0xF8970000`) and the baremetal app uses UART15 (`0xF8D60000`).

NuttX is configured with its shell (NSH) enabled plus an additional
`shmdemo` command used for interacting with the baremetal app via the shared
memory mailbox. After booting into NSH, you can inspect the mailbox state by
running:

```
shmdemo dump
```

To send a message to the baremetal guest, run:

```
shmdemo send hello from nuttx
```

To sample the mailbox repeatedly, run:

```
shmdemo watch 10 500
```

The baremetal guest continuously polls the mailbox and prints heartbeat and
status updates to its own console.

Follow the instructions to build [NuttX](../../guests/nuttx/README.md) and
the [baremetal app](../../guests/baremetal/README.md).
