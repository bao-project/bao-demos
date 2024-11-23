#!/bin/sh

# Down eth0
ifconfig eth0 down

# Create two tap devices for the two VMs
ip tuntap add dev tap0 mode tap
ip tuntap add dev tap1 mode tap

# Create a bridge bao-br0
brctl addbr bao-br0

# Assign IP address to bridge bao-br0
ifconfig bao-br0 192.168.42.12 netmask 255.255.255.0

# Add interfaces to bridge bao-br0
brctl addif bao-br0 eth0
brctl addif bao-br0 tap0
brctl addif bao-br0 tap1

# Bring up eth0
ifconfig eth0 up

# Configure tap0 and tap1 devices and bring it up
ifconfig tap0 192.168.42.13 up
ifconfig tap1 192.168.42.14 up
