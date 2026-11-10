# Linux+Zephyr VirtIO Console Demo

This demo features a dual-guest configuration where Linux acts as a VirtIO
backend and Zephyr as a VirtIO console frontend, connected through Bao's
Remote I/O infrastructure. It is a Zephyr counterpart to the FreeRTOS VirtIO
console frontend of the [VirtIO demo](../virtio/README.md), available on the
`qemu-riscv64-virt` and `qemu-aarch64-virt` platforms.

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

The platform's first UART is assigned to Zephyr. The Linux backend is
accessible through QEMU's virtio console (see the platform instructions for
[qemu-riscv64-virt](../../platforms/qemu-riscv64-virt/README.md) and
[qemu-aarch64-virt](../../platforms/qemu-aarch64-virt/README.md)) and via
ssh at the static address **192.168.42.15** (`ssh root@localhost -p 5555` from
the host).

---

**NOTE**

The Zephyr VirtIO drivers assume the device can access any memory the driver
hands it, while the Bao backend can only access the shared memory region of the
Remote I/O device. For this reason, the Zephyr image is built in XIP mode: the
code and read-only data are loaded in the VM's private memory region, while all
of Zephyr's RAM (data, bss, stacks and heap, including the virtqueues and the
console buffers) is placed in the shared memory region. See
`zephyr/app/overlays/<arch>/app.overlay` and `configs/<platform>.c`.

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

Until they are part of a Zephyr release, the demo uses the `bao/demo-v4.5.0`
branch of the [Bao fork of Zephyr](https://github.com/bao-project/zephyr),
which carries these two fixes on top of upstream Zephyr main right after the
v4.5.0 release. On `qemu-riscv64-virt` it also uses a dedicated board,
`baovm_qemu-riscv64-virt-plic` (in `guests/zephyr/boards/riscv`), where Zephyr
runs in supervisor mode on top of the SBI implemented by Bao and uses the PLIC,
unlike the platform's default `baovm_qemu-riscv64-virt` board, which targets
the AIA and the Zephyr snapshot used by the other demos. On `qemu-aarch64-virt`
the platform's default `baovm_qemu-aarch64-virt` board is used.

Follow the instructions in [Zephyr](../../guests/zephyr/README.md) with the
following variables, cloning Zephyr from the Bao fork instead of the upstream
repository (there are no patches to apply to this revision):

```
export BAO_DEMOS_ZEPHYR_VRS=067b2fe70264af4dd182328f57405b73d69bddba
export BAO_DEMOS_ZEPHYR_SRC=$BAO_DEMOS_WRKDIR_SRC/zephyr-bao-demo-v4.5.0
# qemu-riscv64-virt only; keep BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM elsewhere
export BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM-plic
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
    $BAO_DEMOS/demos/linux+zephyr-virtio/overlay"
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

<!--- instruction#end -->
