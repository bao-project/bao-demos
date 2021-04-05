# Dual-Linux, pseudo IVSHMEM-NET Demo

This demo features two Linux guests connected via ab inter-VM communication 
object (i.e. shared memory plus a doorbell mechanism) communication using a 
port of the IVSHMEM-NET driver which exposes the object as a network interface. 

While one of the guests is assigned the platform's main UART, the second Linux 
instance is "hidden" from the world and only reachable through this
communication mechanism via ssh at the static address *192.168.42.16*.

Follow the [instructions to build Linux](../../guests/linux/README.md).
