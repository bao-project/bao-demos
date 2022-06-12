# Bao Hypervisor Demo Guide

This tutorial provides a step-by-step guide on how to run different demo 
configurations of the Bao hypervisor featuring multiple guest operating 
systems and targeting several supported platforms. The available demos are:

* [Single-guest Baremetal](demos/baremetal/README.md)
* [Dual-guest Linux+FreeRTOS](demos/linux+freertos/README.md)

---

**NOTE**

This tutorial assumes you are running a standard Linux distro (e.g. 
Debian) and using bash.

If you have any doubts, questions, feedback, or suggestions regarding 
this guide, please raise an issue in GitHub or contact us via 
info@bao-project.org.

If you run into any problem while following this guide, we ask you to raise 
an issue on Github, but first please make sure you are using the same or
newer/compatible versions of the tools and software listed in 
[Appendix II](#Appendix-II) (not all are needed for all target platforms). 

---

## -1. Install dependencies

```
sudo apt install build-essential bison flex git libssl-dev ninja-build \
    u-boot-tools pandoc
```

## 0. Download and setup the toolchain

Download the latest bare-metal cross-compile toolchain for your target 
architecture:

a) For Armv8-A, use the **aarch64-none-elf-** toolchain.

Download it from the [Arm Developer's][arm-toolchains]  website.

b) For RISC-V, use the **riscv64-unknown-elf-** toolchain.

Download it from [SiFive's Freedom Tools][riscv-toolchains] github reposiroty.

Install the toolchain. Then, set the **CROSS_COMPILE** environment variable 
with the reference toolchain prefix path:

```
export CROSS_COMPILE=/path/to/toolchain/install/dir/bin/your-toolchain-prefix-
```

## 1. Setup base environment

Clone this repo and cd to it:

```
git clone https://github.com/bao-project/bao-demos
cd bao-demos
```

Depending on your target platform and demo, setup the **PLATFORM** and **DEMO** 
environment variables using the IDs in [Appendix I](#Appendix-I). For example, 
for a system configuration targeting the ZCU102 board and featuring a dual-guest
Linux+FreeRTOS demo:

```
export PLATFORM=zcu102
export DEMO=linux+freertos
```

At this point you have two options:

## A) Use automated make

Just execute:

```
make -j$(nproc)
```

And all the needed source and images will be automatically downloaded and built. 
The makefiles will also print some instructions for you to carry out when it is 
not possible to automate a given step for some reason (e.g. download behind 
authentication wall). It will also print the instructions on how to deploy the
images on your target platform. To quiet instructions pass `NO_INSTRUCTIONS=1` 
to make.

---

**WARNING**

The makefiles will automatically accept end-user license agreements (EULAs) on 
your behalf for some of the downloaded firmware. If you wish to be prompted 
to accept them manually, pass `ALWAYS_ASK=1` to make.

---

If you are targetting an emulator platform like QEMU, after building 
you can start it with:

```
make run
```

In this case, if you don't have qemu for the target architecture installed, 
it will build it for you.

---

**NOTE**

These makefiles are intended **ONLY to automate** this guide's steps; not to be 
used as any kind of build system during development.

---

## B) Follow the step-by-step guide

As an alternative, we provide a step-by-step guide that you can follow to build 
all the necessary software and deploy it to your target platform.

### B.1) Setup ARCH manually

Setup the *ARCH* environment variable manually according to 
[Appendix I](#Appendix-I). For example, for the ZCU102 platform:

```
export ARCH=aarch64
```

### B.2) Create working directory

Create the working directories where you'll place the source code and
final images:

```
export BAO_DEMOS=$(realpath .)
export BAO_DEMOS_WRKDIR=$BAO_DEMOS/wrkdir
export BAO_DEMOS_WRKDIR_SRC=$BAO_DEMOS_WRKDIR/srcs
export BAO_DEMOS_WRKDIR_PLAT=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM
export BAO_DEMOS_WRKDIR_IMGS=$BAO_DEMOS_WRKDIR_PLAT/$DEMO
mkdir -p $BAO_DEMOS_WRKDIR
mkdir -p $BAO_DEMOS_WRKDIR_SRC
mkdir -p $BAO_DEMOS_WRKDIR_IMGS
```

### B.3) Build guests

Build guests according to the target demo:

* [Single Baremetal Guest](demos/baremetal/README.md)
* [Dual-guest Linux+FreeRTOS](demos/linux+freertos/README.md)


### B.4) Build Bao

Clone Bao's repo to the the working directory:

```
export BAO_DEMOS_BAO=$BAO_DEMOS_WRKDIR_SRC/bao
git clone https://github.com/bao-project/bao-hypervisor $BAO_DEMOS_BAO\
    --branch v0.1.0
```

Copy your config to the working directory:

```
mkdir -p $BAO_DEMOS_WRKDIR_IMGS/config
cp -L $BAO_DEMOS/demos/$DEMO/configs/$PLATFORM.c\
    $BAO_DEMOS_WRKDIR_IMGS/config/$DEMO.c
```

Build it:

```
make -C $BAO_DEMOS_BAO\
    PLATFORM=$PLATFORM\
    CONFIG_REPO=$BAO_DEMOS_WRKDIR_IMGS/config\
    CONFIG=$DEMO\
    CONFIG_BUILTIN=y\
    CPPFLAGS=-DBAO_DEMOS_WRKDIR_IMGS=$BAO_DEMOS_WRKDIR_IMGS
```

And copy the resulting binary into the final image directory:

```
cp $BAO_DEMOS_BAO/bin/$PLATFORM/builtin-configs/$DEMO/bao.bin\
    $BAO_DEMOS_WRKDIR_IMGS
```

### B.5) Build Firmware and Deploy

Build the firmware and deploy the system according to the target platform:

#### AArch64 platforms:
* [Xilinx ZCU102/4](platforms/zcu104/README.md)
* [NXP i.MX8QM](platforms/imx8qm/README.md)
* [Nvidia TX2](platforms/tx2/README.md)
* [Raspberry 4 Model B](platforms/rpi4/README.md)
* [QEMU virt](platforms/qemu-aarch64-virt/README.md)

#### RISC-V platforms:
* [QEMU virt](platforms/qemu-riscv64-virt/README.md)

---

## Appendix I

| | PLATFORM | ARCH
|--|--|--|
| Xilinx ZCU102 | zcu102 | aarch64
| Xilinx ZCU104 | zcu104 | aarch64
| NXP i.MX8QM | imx8qm | aarch64
| Nvidia TX2 | tx2 | aarch64
| Raspberry 4 Model B | rpi4 | aarch64
| QEMU Aarch64 virt | qemu-aarch64-virt | aarch64
| QEMU RV64 virt | qemu-riscv64-virt | riscv
<!-- TODO -->
<!-- | NXP i.MX8MQ | imx8mq | -->
<!-- | Avnet Ultra96 | ultra96 | -->
<!-- | Rocket on ZynqMP | rocket-fpga | -->
<!-- | Rocket on Firesim | rocket-firesim | -->
<!-- | Hikey 960 | hikey960 | -->
<!-- | Rock 960 | rock960 | -->

| | DEMO |
|--|--|
| Baremetal guest | baremetal |
| Linux+FreeRTOS | linux+freertos |

<!-- TODO: Add Accepted Platform/Demos table -->

---

## Appendix II

| Tool | Version |
|--|--|
| aarch64-none-elf-gcc | 9.2.1 |
| riscv64-unknown-elf-gcc | 10.1.0 |
| make | 4.2.1 |
| dtc | 1.5.0 |
| gcc | 9.3.0 |
| mkimage | 20.10 |
| cmake | 3.16.3 |
| ninja | 1.10.1 |


<!-- Links -->

[arm-toolchains]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads
[riscv-toolchains]: https://github.com/sifive/freedom-tools/releases
