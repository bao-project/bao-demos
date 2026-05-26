# NuttX+Baremetal Demo

This demo features two guests, NuttX and a bare-metal app, connected through a
shared-memory mailbox.

On `PLATFORM=e3650`, Bao starts the guests with this split:
- NuttX on CPUs `0-1` (affinity `0x3`)
- bare-metal app on CPUs `2-3` (affinity `0xc`)

Both guests map the same mailbox region at `0x00CD0000` and exchange messages
through sequence counters and text buffers.

The bare-metal guest continuously polls the mailbox and updates heartbeat/status
information. On the NuttX side, the `shmdemo` app is enabled in the default
`bao_guest` defconfig and provides a simple shell interface.

After booting into NSH, you can run:

```
shmdemo dump
```

to inspect mailbox state, or:

```
shmdemo send hello from nuttx
```

to send a message to the bare-metal guest.

To sample the mailbox repeatedly:

```
shmdemo watch 10 500
```

This demo uses different UARTs for each guest on e3650:
- NuttX console: UART18 (`0xF8970000`)
- bare-metal console: UART15 (`0xF8D60000`)

Build both guests by following:
- [NuttX guest instructions](../../guests/nuttx/README.md)
- [Bare-metal guest instructions](../../guests/baremetal/README.md)
