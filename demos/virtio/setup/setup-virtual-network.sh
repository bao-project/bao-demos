#!/bin/sh

# Bring down eth0
ip link set dev eth0 down

# Ensure /dev/net/tun exists
modprobe tun
mkdir -p /dev/net
mknod /dev/net/tun c 10 200
chmod 0666 /dev/net/tun

# Create TAP devices
ip tuntap add dev tap0 mode tap
ip tuntap add dev tap1 mode tap

# Create bridge
ip link add name bao-br0 type bridge
ip addr add 192.168.42.12/24 dev bao-br0

# Add interfaces to bridge
ip link set dev eth0 master bao-br0
ip link set dev tap0 master bao-br0
ip link set dev tap1 master bao-br0

# Bring up bridge and member interfaces
ip link set dev bao-br0 up
ip link set dev eth0 up
ip link set dev tap0 up
ip link set dev tap1 up
