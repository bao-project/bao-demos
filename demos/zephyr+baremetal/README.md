# Zephyr+Baremetal Demo

This Demo features two guests, Zephyr and a baremetal app, that can communicate
via a shared memory object which includes a shared memory region and a
notification mechanism in the form of hardware interrupts. 

If Zephyr supports SMP for the taarget architecture, half of the cores are
assigned to Zephyr and half to the baremetal app. If not, one core is assigned
to Zephyr and the remaining ones to the baremetal app. 

This demo is only available in platforms featuring at least two UART
peripherals where one is assigned to the baremetal app and the other to Zephyr.
Each time the baremetal app receives a UART interrupt it will update the shared
memory region with a message.

Zephyr is configured with its shell enabled plus aditional an aditional
'baoipc' command used for interacting with the baremetal app via the shared
memory object. To read the baremetal app message run:

```
baoipc read 0
```

To write a message in the shared memory and notify the baremetal app run:

```
baoipc write_notify 0 "Hello, Bao!"
```

When receiving the notification, the baremetal app should print the message
written by Zephyr.

Follow the instructions to build [Zephyr](../../guests/zephyr/README.md) 
and the [baremetal app](../../guests/baremetal/README.md).
