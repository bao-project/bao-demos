# Linux+Zephyr VirtIO Console Demo with Bounce Buffers

This demo features a dual-guest configuration where Linux acts as a VirtIO
backend and Zephyr as a VirtIO console frontend, connected through Bao's
Remote I/O infrastructure. It is a Zephyr counterpart to the FreeRTOS VirtIO
console frontend of the [VirtIO demo](../virtio/README.md), available on the
`qemu-riscv64-virt` and `qemu-aarch64-virt` platforms.

It is a variant of the
[Linux+Zephyr VirtIO console demo](../linux+zephyr-virtio/README.md) where
Zephyr no longer shares all of its RAM with the Linux backend: only a 64 KiB
region holding the virtqueues and bounce copies of the VirtIO buffers is
shared. It also gives Zephyr a VirtIO network device and a VirtIO block
device, each with a shared region of its own (1 MiB and 256 KiB).

The Zephyr guest uses the upstream Zephyr VirtIO MMIO transport and
virtio-console driver (available since Zephyr v4.5.0), running as a regular
Bao VM: in RISC-V supervisor mode on top of the SBI implemented by Bao, or at
EL1 on Arm. No Bao-specific driver is needed on the Zephyr side.

The VirtIO device is emulated in the Linux backend by the
[Bao VirtIO device model](https://github.com/bao-project/bao-virtio-dm), which
exposes the Zephyr console as a pseudo-terminal. The Zephyr application:

- periodically writes messages from two threads to the VirtIO console;
- prints on its UART console every line received from the VirtIO console;
- forwards every character typed on its UART console to the VirtIO console.

The second VirtIO device is a network interface, driven by Zephyr's upstream
virtio-net driver and backed in Linux by a TAP interface. The backend bridges
it with its own network interface, so Zephyr sits on the same network at the
static address **192.168.42.16**. As in the [VirtIO demo](../virtio/README.md),
each Remote I/O device has its own shared memory region: Bao maps a region
along with its device, so two devices of a VM cannot name the same one. Each
`virtio,mmio` node therefore references a `restricted-dma-pool` of its own.
The network interface is set up when Zephyr boots, so Zephyr only starts once
the backend is running.

The third VirtIO device is a block device, driven by Zephyr's virtio-blk disk
driver and backed by a 1 MiB disk image the backend creates when it starts
(`/tmp/bao-virtio-blk.img`). Its first sector holds a message, which the
application prints on the UART console, and the application answers in the
second sector. It has a pool of its own too.

The platform's first UART is assigned to Zephyr. The Linux backend is
accessible through QEMU's virtio console (see the platform instructions for
[qemu-riscv64-virt](../../platforms/qemu-riscv64-virt/README.md) and
[qemu-aarch64-virt](../../platforms/qemu-aarch64-virt/README.md)) and via
ssh at the static address **192.168.42.15** (`ssh root@localhost -p 5555` from
the host).

---

**NOTE**

The Bao backend can only access the shared memory region of the Remote I/O
device, while the Zephyr VirtIO drivers assume the device can access any
memory the driver hands it. This demo builds a proof-of-concept Zephyr branch
where the shared region is declared as a `restricted-dma-pool` node and the
`virtio,mmio` node names it with a `memory-region` property: the virtqueues
are allocated from it and the driver buffers are bounced through it. Zephyr's
image and all of its RAM stay in the VM's private memory, the image is not
built in XIP mode, and the virtio-console driver and the application are
unchanged. See `zephyr/config/overlays/<arch>/app.overlay` and
`configs/<platform>.c`.

---

## Build

### Zephyr

This demo requires Zephyr v4.5.0 or newer (which needs Python 3.12 or newer
on the host) with two fixes that have been submitted upstream:

- the virtio-console driver must complete the VirtIO feature negotiation
  (FEATURES_OK) before setting up the virtqueues, as required by the Bao VirtIO
  device model;
- on RISC-V, the PLIC driver must use the supervisor-mode hart contexts when
  Zephyr runs in supervisor mode. Bao only emulates the supervisor contexts, so
  without it the VirtIO interrupts never reach Zephyr.

It also requires the restricted DMA pool support of the VirtIO transport,
which is a proof of concept not yet submitted upstream (the
`poc/virtio-bounce-buffers` branch of the
[Bao fork of Zephyr](https://github.com/bao-project/zephyr) tracks it on top
of upstream Zephyr main).

The demo uses the `bao/demo-v4.5.0-virtio-bounce` branch of the fork, which
carries the two fixes on top of upstream Zephyr main right after the v4.5.0
release, followed by the proof of concept. On `qemu-riscv64-virt` it also uses a dedicated board,
`baovm_qemu-riscv64-virt-plic` (in `guests/zephyr/boards/riscv`), where Zephyr
runs in supervisor mode on top of the SBI implemented by Bao and uses the PLIC,
unlike the platform's default `baovm_qemu-riscv64-virt` board, which targets
the AIA and the Zephyr snapshot used by the other demos. On `qemu-aarch64-virt`
the platform's default `baovm_qemu-aarch64-virt` board is used.

Follow the instructions in [Zephyr](../../guests/zephyr/README.md) with the
following variables, cloning Zephyr from the Bao fork instead of the upstream
repository (there are no patches to apply to this revision). The application
is the one of the Linux+Zephyr VirtIO console demo; only its configuration
(`prj.conf`, board fragments and devicetree overlays, in `zephyr/config`) is
specific to this demo, so in the `cmake` command add
`-DAPPLICATION_CONFIG_DIR=$BAO_DEMOS/demos/$DEMO/zephyr/config` and pass the
overlay of that directory,
`-DDTC_OVERLAY_FILE=$BAO_DEMOS/demos/$DEMO/zephyr/config/overlays/$ARCH/app.overlay`:

```
export BAO_DEMOS_ZEPHYR_VRS=bao/demo-v4.5.0-virtio-bounce
export BAO_DEMOS_ZEPHYR_SRC=$BAO_DEMOS_WRKDIR_SRC/zephyr-virtio-bounce
# qemu-riscv64-virt only; keep BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM elsewhere
export BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM-plic
export BAO_DEMOS_ZEPHYR_APP=$BAO_DEMOS/demos/linux+zephyr-virtio/zephyr/app
git init $BAO_DEMOS_ZEPHYR_SRC
git -C $BAO_DEMOS_ZEPHYR_SRC remote add origin \
    https://github.com/bao-project/zephyr.git
git -C $BAO_DEMOS_ZEPHYR_SRC fetch --depth 1 origin $BAO_DEMOS_ZEPHYR_VRS
git -C $BAO_DEMOS_ZEPHYR_SRC checkout FETCH_HEAD
```

### Linux

The Linux backend needs the I/O dispatcher kernel module and the Bao VirtIO
device model, which are built as Buildroot packages. Add the following to the
Buildroot defconfig (`guests/linux/buildroot/riscv64.config`):

```
BR2_PACKAGE_BAO_VIRTIO_DM=y
BR2_PACKAGE_HOST_RUSTC=y
BR2_PACKAGE_HOST_RUST_BIN=y
```

Add the VirtIO kernel configuration fragment to the Linux configuration:

```
export BAO_DEMOS_LINUX_CFG_FRAG="$BAO_DEMOS_LINUX_CFG_FRAG \
    $BAO_DEMOS/guests/linux/configs/virtio.config"
```

And add this demo's root filesystem overlay, which contains the device model
configuration (`/etc/bao-virtio-console.yaml`) and an init script that launches
the backend at boot:

```
export BAO_DEMOS_BUILDROOT_OVERLAY="$BAO_DEMOS/guests/linux/buildroot/overlay \
    $BAO_DEMOS/demos/linux+zephyr-virtio-bounce/overlay"
```

Then follow the instructions in [Linux](../../guests/linux/README.md), using
`devicetrees/$PLATFORM/linux-backend.dts` as the device tree and naming the
resulting image `linux_backend.bin`.

### Bao

On `qemu-aarch64-virt`, follow the general instructions to build Bao.

On `qemu-riscv64-virt`, this demo relies on Bao's PLIC emulation, so it uses
Bao v2.0.0 (where the platform is built with the PLIC) instead of the revision
used by the other demos. Follow the general instructions to build Bao, but
check out v2.0.0 (commit `0af4a1ab558ad60c9658af4f43756c4536dd3141`); the
Makefile clones it into `$BAO_DEMOS_WRKDIR_SRC/bao-v2.0.0`. Since Bao uses the
PLIC, QEMU must be started with the plain `virt` machine, i.e. without the
`aia=aplic-imsic,aia-guests=1` machine options the other demos use on this
platform. This is what `make run` does.

<!--- instruction#1 -->
## Run

When the Linux backend boots, it loads the I/O dispatcher module and launches
the VirtIO device model. Until then, the Zephyr guest waits on the UART console
after printing:

```
Initializing the virtio console (waiting for the backend)...
```

In the Linux backend, check the device model log to find the pseudo-terminal
assigned to the Zephyr console:

```
cat /var/log/bao-virtio-dm.log
```

You should see output similar to:

```
virtio-console device id 0 at /dev/pts/0
```

Connect to it with `picocom`:

```
picocom /dev/pts/0
```

The messages sent by the Zephyr threads will show up:

```
Thread1: 0
Thread2: 0
Thread1: 1
Thread2: 1
```

Lines typed in `picocom` are printed by Zephyr on its UART console, and
characters typed on the Zephyr UART console are echoed in `picocom`.

The network interface of Zephyr answers from the backend VM:

```
ping 192.168.42.16
```

When it starts, Zephyr prints the message it read from the block device:

```
message from the virtio block device: Hello from the disk image of the Linux backend
```

And its answer is in the second sector of the disk image of the backend VM:

```
dd if=/tmp/bao-virtio-blk.img bs=512 skip=1 count=1 2> /dev/null | tr -d '\000'
```

<!--- instruction#end -->
