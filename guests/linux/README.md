# Linux Guest

## 1. Download the Linux kernel source

Setup linux environment variables. Start by the demo resource directory:

```
export BAO_DEMOS_LINUX=$BAO_DEMOS/guests/linux
```

Setup repo and version. Specifically for the NXP iMX platforms use:

```
export BAO_DEMOS_LINUX_REPO=https://source.codeaurora.org/external/imx/linux-imx
export BAO_DEMOS_LINUX_VERSION=rel_imx_5.4.24_2.1.0
```

For all other platforms clone the latest mainline Linux release:

```
export BAO_DEMOS_LINUX_REPO=https://github.com/torvalds/linux.git
export BAO_DEMOS_LINUX_VERSION=v6.1
```

Setup an environment variable pointing to Linux's source code:

```
export BAO_DEMOS_LINUX_SRC=$BAO_DEMOS_WRKDIR_SRC/linux-$BAO_DEMOS_LINUX_VERSION
```

And make a shallow clone of the target repo:

```
git clone $BAO_DEMOS_LINUX_REPO $BAO_DEMOS_LINUX_SRC\
    --depth 1 --branch $BAO_DEMOS_LINUX_VERSION
cd $BAO_DEMOS_LINUX_SRC
git apply $BAO_DEMOS_LINUX/patches/$BAO_DEMOS_LINUX_VERSION/*.patch
```

Finally, setup and environment variable pointing to the target architecture and 
platform specific config to be used by buildroot:

```
export BAO_DEMOS_LINUX_CFG_FRAG=$(ls $BAO_DEMOS_LINUX/configs/base.config\
    $BAO_DEMOS_LINUX/configs/$ARCH.config\
    $BAO_DEMOS_LINUX/configs/$PLATFORM.config\
    $BAO_DEMOS_LINUX/configs/$DEMO.config 2> /dev/null)
```

## Use Buildroot to build Linux with a built-in initramfs

Setup buildroot environment variables:

```
export BAO_DEMOS_BUILDROOT=$BAO_DEMOS_WRKDIR_SRC/\
buildroot-$ARCH-$BAO_DEMOS_LINUX_VERSION
export BAO_DEMOS_BUILDROOT_DEFCFG=$BAO_DEMOS_LINUX/buildroot/$ARCH.config
export LINUX_OVERRIDE_SRCDIR=$BAO_DEMOS_LINUX_SRC
```

Clone the latest buildroot at the latest stable version:

```
git clone https://github.com/buildroot/buildroot.git $BAO_DEMOS_BUILDROOT\
    --depth 1 --branch 2022.11
cd $BAO_DEMOS_BUILDROOT
```

Use our provided buildroot defconfig, which itselfs points to the a Linux 
kernel defconfig and patches (mainly for the inter-vm communication drivers)
and build:

```
make defconfig BR2_DEFCONFIG=$BAO_DEMOS_BUILDROOT_DEFCFG
make linux-reconfigure all
mv $BAO_DEMOS_BUILDROOT/output/images/Image\
    $BAO_DEMOS_BUILDROOT/output/images/Image-$PLATFORM
```

## Build the device tree and wrap it with the kernel image

---

**NOTE**

If your target demo features multiple Linux virtual machines, you will have to 
repeat this last step for each of these, which should correspond to 
different *.dts* files in *$BAO_DEMOS/$DEMO/devicetrees*.

---

The device tree(s) for your target demo are available in 
*$BAO_DEMOS/$DEMO/devicetrees/$PLATFORM*. For a device tree file named 
*linux.dts* define a virtual machine variable:

```
export BAO_DEMO_LINUX_VM=linux
```

Then build:

```
dtc $BAO_DEMOS/demos/$DEMO/devicetrees/$PLATFORM/$BAO_DEMO_LINUX_VM.dts >\
    $BAO_DEMOS_WRKDIR_IMGS/$BAO_DEMO_LINUX_VM.dtb
```

Wrap the kernel image and device tree blob in a single binary:

```
make -C $BAO_DEMOS_LINUX/lloader\
    ARCH=$ARCH\
    IMAGE=$BAO_DEMOS_BUILDROOT/output/images/Image-$PLATFORM\
    DTB=$BAO_DEMOS_WRKDIR_IMGS/$BAO_DEMO_LINUX_VM.dtb\
    TARGET=$BAO_DEMOS_WRKDIR_IMGS/$BAO_DEMO_LINUX_VM
```

---

**NOTE**

The password for `root` is `root`.

---
