#!/bin/sh

# Only run setup if /dev/bao-io-dispatcher exists
if [ -e /dev/bao-io-dispatcher ]; then
    # Setup the virtual network
    /etc/setup-virtual-network.sh

    # Launch the VirtIO backends
    /etc/setup-virtio-backends.sh
else
    echo "Device /dev/bao-io-dispatcher not found — skipping backend setup."
fi
