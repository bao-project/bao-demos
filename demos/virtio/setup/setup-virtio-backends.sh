#!/bin/sh

# Launch the VirtIO backends
nohup bao-virtio-dm --config /etc/config-virtio-demo-vm1.yaml > /etc/bao-vm1.log 2>&1 &
nohup bao-virtio-dm --config /etc/config-virtio-demo-vm2.yaml > /etc/bao-vm2.log 2>&1 &
nohup bao-virtio-dm --config /etc/config-virtio-demo-vm3.yaml > /etc/bao-vm3.log 2>&1 &
