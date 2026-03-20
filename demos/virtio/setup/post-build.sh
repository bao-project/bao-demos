#!/bin/sh

SCRIPT_DIR=$(dirname $0)

# Append the main setup script to the /etc/inittab file
echo "::sysinit:/etc/setup-demo-virtio.sh" >> ${TARGET_DIR}/etc/inittab