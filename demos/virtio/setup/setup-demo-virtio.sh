#!/bin/sh

# Setup the virtual network
/etc/setup-virtual-network.sh

# Load the I/O dispatcher kernel module
/etc/setup-iodispatcher.sh

# Launch the VirtIO backends
/etc/setup-virtio-backends.sh