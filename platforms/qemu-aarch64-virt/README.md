# Qemu Aarch64 virt

## 1) Download build and install Aarch64 qemu

---

**NOTE**

If you already have qemu-system-aarch64 installed or you don't want to compile 
it but install it directly using a package manager or some other method, please
make sure you are using version 7.2.0 or higher. If so, you can skip this step.

---

```
export BAO_DEMOS_QEMU=$BAO_DEMOS_WRKDIR_SRC/qemu-$ARCH
git clone https://github.com/qemu/qemu.git $BAO_DEMOS_QEMU --depth 1\
   --branch v7.2.0
cd $BAO_DEMOS_QEMU
./configure --target-list=aarch64-softmmu --enable-slirp
make -j$(nproc)
sudo make install
```

## 2) Build U-boot


```
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot
git clone https://github.com/u-boot/u-boot.git $BAO_DEMOS_UBOOT --depth 1\
   --branch v2022.10
cd $BAO_DEMOS_UBOOT
make qemu_arm64_defconfig
```

Now you need to set the Kconfig options:

* CONFIG_TFABOOT=y
* CONFIG_SYS_TEXT_BASE=0x60000000

You can do it via using an interface such as `menuconfig` or just write them 
directly to the config file:

```
echo "CONFIG_TFABOOT=y" >> .config
echo "CONFIG_SYS_TEXT_BASE=0x60000000" >> .config
```

Then build it:

```
make -j$(nproc)
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_UBOOT/u-boot.bin $BAO_DEMOS_WRKDIR/imgs/$PLATFORM
```

## 3) Build TF-A

```
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware
git clone https://github.com/bao-project/arm-trusted-firmware.git\
   $BAO_DEMOS_ATF --branch bao/demo-next --depth 1
cd $BAO_DEMOS_ATF
make PLAT=qemu bl1 fip BL33=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM/u-boot.bin\
   QEMU_USE_GIC_DRIVER=QEMU_GICV3
dd if=$BAO_DEMOS_ATF/build/qemu/release/bl1.bin\
   of=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM/flash.bin
dd if=$BAO_DEMOS_ATF/build/qemu/release/fip.bin\
   of=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM/flash.bin seek=64 bs=4096 conv=notrunc
```

## 4) Run QEMU

```
qemu-system-aarch64 -nographic\
   -M virt,secure=on,virtualization=on,gic-version=3 \
   -cpu cortex-a53 -smp 4 -m 4G\
   -bios $BAO_DEMOS_WRKDIR/imgs/$PLATFORM/flash.bin \
   -device loader,file="$BAO_DEMOS_WRKDIR_IMGS/bao.bin",addr=0x50000000,force-raw=on\
   -device virtio-net-device,netdev=net0\
   -netdev user,id=net0,net=192.168.42.0/24,hostfwd=tcp:127.0.0.1:5555-:22\
   -device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3
```

You should see TF-A and U-boot initialization messages.

<!--- instruction#1 -->
## 5) Setup connections and jump to Bao

Qemu will let you know in which pseudo-terminals it placed the virtio serial. 
For example:

```
char device redirected to /dev/pts/4 (label serial3)
```

If you are running more than one guest, open a new terminal and connect to it. 
For example:

```
screen /dev/pts/4
```

If you are running a Linux guest you can also connect via ssh to it by opening 
a new terminal and running:

```
ssh root@localhost -p 5555
```

Finally, make u-boot jump to where the bao image was loaded:

```
go 0x50000000
```

When you want to leave QEMU press `Ctrl-a` then `x`.

<!--- instruction#end -->
