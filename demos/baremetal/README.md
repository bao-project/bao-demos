# Bare metal application Demo

This demo features a multi-threaded bare metal application running over Bao
on all available cores. The application prints to the first available UART.

It configures a periodic timer interrupt in the first core and, each time this 
interrupt is triggered, it forwards an IPI to the next core. Each time a core 
receives an IPI it will forward it to the next core, until there are no more 
cores to interrupt.

The application also setups an RX UART interrupt that might be forwarded to just 
one or multiple cores, depending on the capabilities of the platform's interrupt 
controller. Each time it receives a character, it will print a message.

Follow the [build instructions](../../guests/baremetal/README.md) to compile the
bare metal application to your target platform.
