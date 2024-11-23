# VirtIO Demo

This demo showcases a four-guest configuration, comprising three Linux VMs and one FreeRTOS VM. 
One Linux guest operates as the VirtIO backend, while the remaining three guests act as VirtIO frontends.
In this setup:
- Two Linux VirtIO frontend guests share access to the same network and storage interfaces.
- The FreeRTOS VirtIO frontend guest is equipped with a VirtIO console.

Access details:
- The backend VM can be accessed via SSH at the static IP address **192.168.42.15**.
- Once the VirtIO backend device is initialized in the backend VM, the Linux frontend VMs 
will become accessible via SSH at static IP addresses **192.168.42.16** and **192.168.42.17**, respectively.

To build FreeRTOS, set:

```
export FREERTOS_PARAMS="STD_ADDR_SPACE=y"
```

And follow the instructions in [FreeRTOS](../../guests/freertos/README.md). 
To build linux follow [Linux](../../guests/linux/README.md).

Additionally, you must build the I/O dispatcher system and the VirtIO device model.

## 1) Build the I/O dispatcher system

Setup environment variables:
```
export BAO_DEMOS_LINUX_DRIVERS=$BAO_DEMOS_WRKDIR_SRC/bao-linux-drivers
export BAO_DEMOS_LINUX_DRIVERS_REPO=https://github.com/bao-project/bao-linux-drivers.git
export BAO_DEMOS_LINUX_DRIVERS_BRANCH=linux-$BAO_DEMOS_LINUX_VERSION
export BAO_DEMOS_BUILDROOT_LINUX_BUILD=$(BAO_DEMOS_BUILDROOT)/output/build/linux-custom
export BAO_DEMOS_BUILDROOT_TOOLCHAIN_DIR=$(BAO_DEMOS_BUILDROOT)/output/host/bin
```

Clone Bao's Linux drivers repo into your working directory:
```
git clone --depth 1 --branch $BAO_DEMOS_LINUX_DRIVERS_BRANCH $BAO_DEMOS_LINUX_DRIVERS_REPO $BAO_DEMOS_LINUX_DRIVERS
```

To compile the I/O dispatcher kernel module, set up the target architecture and cross-compilers based on your target platform. 
Use the appropriate configuration below:

For aarch64:
```
export BAO_DEMOS_LINUX_DRIVERS_ARCH=arm64
export BAO_DEMOS_LINUX_DRIVERS_CC=$BAO_DEMOS_BUILDROOT_TOOLCHAIN_DIR/aarch64-buildroot-linux-gnu-
```

For riscv64:
```
export BAO_DEMOS_LINUX_DRIVERS_ARCH=riscv
export BAO_DEMOS_LINUX_DRIVERS_CC=$BAO_DEMOS_BUILDROOT_TOOLCHAIN_DIR/riscv64-buildroot-linux-gnu-
```

For arm:
```
export BAO_DEMOS_LINUX_DRIVERS_ARCH=arm32
export BAO_DEMOS_LINUX_DRIVERS_CC=$BAO_DEMOS_BUILDROOT_TOOLCHAIN_DIR/arm-buildroot-linux-gnueabi-
```

Run the following command to compile the I/O dispatcher kernel module:
```
make -C $BAO_DEMOS_BUILDROOT_LINUX_BUILD \
        M=$BAO_DEMOS_LINUX_DRIVERS/iodispatcher \
        CROSS_COMPILE=$BAO_DEMOS_LINUX_DRIVERS_CC \
        ARCH=$BAO_DEMOS_LINUX_DRIVERS_ARCH \
        modules
```

After successful compilation, copy the `iodispatcher.ko` file to the images directory:
```
cp $BAO_DEMOS_LINUX_DRIVERS/iodispatcher/iodispatcher.ko $BAO_DEMOS_WRKDIR_IMGS
```

## 2) Build the Bao VirtIO device model

Setup environment variables:
```
export BAO_DEMOS_BAO_VIRTIO_DM_URL=https://github.com/bao-project/bao-virtio-dm.git
export BAO_DEMOS_BAO_VIRTIO_DM_DIR=$BAO_DEMOS_WRKDIR/bao-virtio-dm
```

If your target is aarch64:
```
export BAO_DEMOS_BAO_VIRTIO_DM_TARGET=aarch64-unknown-linux-gnu
```

If your target is riscv64:
```
export BAO_DEMOS_BAO_VIRTIO_DM_TARGET=riscv64gc-unknown-linux-gnu
```

If your target is arm:
```
export BAO_DEMOS_BAO_VIRTIO_DM_TARGET=arm-unknown-linux-gnueabi
```

Export a variable pointing to the final binary:
```
export BAO_DEMOS_BAO_VIRTIO_DM_BIN=$BAO_DEMOS_BAO_VIRTIO_DM_DIR/target/$BAO_DEMOS_BAO_VIRTIO_DM_TARGET/release
```

Clone Bao's VirtIO device model repo:
```
git clone --depth 1 --branch main $BAO_DEMOS_BAO_VIRTIO_DM_URL $BAO_DEMOS_BAO_VIRTIO_DM_DIR
```

Build the device model:
```
cd $BAO_DEMOS_BAO_VIRTIO_DM_DIR && cargo build --target=$BAO_DEMOS_BAO_VIRTIO_DM_TARGET --release
```

After successful compilation, copy the `bao-virtio-dm` binary to the images directory:
```
cp $BAO_DEMOS_BAO_VIRTIO_DM_BIN/bao-virtio-dm $BAO_DEMOS_WRKDIR_IMGS
```

<!--- instruction#1 -->
## 3) Setup the storage images for the two frontend Linux guests

Setup environment variables:
```
export BAO_DEMOS_FRONTEND1_IMG=$BAO_DEMOS_WRKDIR_IMGS/ext4_frontend_vm1_image.img
export BAO_DEMOS_FRONTEND2_IMG=$BAO_DEMOS_WRKDIR_IMGS/ext4_frontend_vm2_image.img
export BAO_DEMOS_FRONTEND1_MP=$BAO_DEMOS_WRKDIR_SRC/ext4_frontend_vm1
export BAO_DEMOS_FRONTEND2_MP=$BAO_DEMOS_WRKDIR_SRC/ext4_frontend_vm2
```

Begin by creating two mount points on your host system:
```
mkdir -p $BAO_DEMOS_FRONTEND1_MP
mkdir -p $BAO_DEMOS_FRONTEND2_MP
```

Create two empty disk images, one for each Linux guest VM:
```
dd if=/dev/zero of=$BAO_DEMOS_FRONTEND1_IMG bs=1M count=1
dd if=/dev/zero of=$BAO_DEMOS_FRONTEND2_IMG bs=1M count=1
```

**Note**: The example above creates disk images with a size of 1MB. 
Adjust the count value to set a different size, considering the memory allocation for your VMs.

Format the disk images using the ext4 filesystem:
```
sudo mkfs.ext4 $BAO_DEMOS_FRONTEND1_IMG
sudo mkfs.ext4 $BAO_DEMOS_FRONTEND2_IMG
```

Mount the disk images to their respective mount points:
```
sudo mount -o loop $BAO_DEMOS_FRONTEND1_IMG $BAO_DEMOS_FRONTEND1_MP
sudo mount -o loop $BAO_DEMOS_FRONTEND2_IMG $BAO_DEMOS_FRONTEND2_MP
```

At this stage, you can customize the filesystems. Create directories, add files, or perform other modifications as required.

After making the necessary changes, unmount the images:
```
sudo umount $BAO_DEMOS_FRONTEND1_MP
sudo umount $BAO_DEMOS_FRONTEND2_MP
```

<!--- instruction#2 -->
## 4) Transfer files to the backend VM

All necessary files, such as storage images, binaries, and configuration files, can be efficiently transferred using a file system overlay.

Export the defined global variable:
```
export BAO_DEMOS_BUILDROOT_OVERLAY_DIR=$BAO_DEMOS_BUILDROOT-overlay
```

Create the subdirectories in the buildroot overlay directory:
```
mkdir -p $BAO_DEMOS_BUILDROOT_OVERLAY_DIR/bin $BAO_DEMOS_BUILDROOT_OVERLAY_DIR/etc
```

Copy all the files to the respective overlay directory:
```
cp $BAO_DEMOS_WRKDIR_IMGS/ext4_frontend_vm1_image.img $BAO_DEMOS_BUILDROOT_OVERLAY_DIR/etc
cp $BAO_DEMOS_WRKDIR_IMGS/ext4_frontend_vm2_image.img $BAO_DEMOS_BUILDROOT_OVERLAY_DIR/etc
cp -r $BAO_DEMOS/demos/virtio/setup/config* $BAO_DEMOS_BUILDROOT_OVERLAY_DIR/etc
cp -r $BAO_DEMOS/demos/virtio/setup/setup* $BAO_DEMOS_BUILDROOT_OVERLAY_DIR/etc
```

**Note**: 
The scripts and configuration files provide the necessary instructions to:
  - Set up the virtual network
  - Load the I/O dispatcher kernel module
  - Configure and launch the VirtIO backends

To insert the I/O dispatcher kernel module automatically when the system starts and run the demo, you can set up and export the following variable:
```
export BAO_DEMOS_BUILDROOT_POST_BUILD_SCRIPT=$BAO_DEMOS/demos/virtio/setup/post-build.sh
```

This will introduce a new entry into the system initialization routine within the `/etc/inittab` file, 
ensuring the automatic insertion of the I/O dispatcher kernel module and the execution of all setup scripts required to configure and run the VirtIO demo.

Lastly, rebuild the backend kernel with:
```
cd $BAO_DEMOS_BUILDROOT
make linux-reconfigure all
```

<!--- instruction#3 -->
### 5.1) Run the Demo (VirtIO Console)

Once the VirtIO console backend is launched, check the log file:
```
cat /etc/bao-vm1.log
```

You should see output similar to the following:
```
virtio-console device id 0 at /dev/pts/0
```

To interact with the FreeRTOS guest, connect to the VirtIO console using `picocom`:
```
picocom -b 115200 /dev/pts/0
```

This allows you to view FreeRTOS guest messages. 
Notwithstanding, any input typed into the backend console will trigger corresponding messages from the FreeRTOS guest.

<!--- instruction#4 -->
### 5.2) Run the Demo (VirtIO Network)

You can log into the two Linux frontend guests via `ssh` using their static IP addresses:

```
ssh root@192.168.42.16
ssh root@192.168.42.17
```

<!--- instruction#5 -->
### 5.3) Run the Demo (VirtIO Block)

After logging into each Linux frontend VM, mount the virtual storage filesystem by executing the following commands:
```
mkdir /mnt/ext4_mountpoint
mount /dev/vda /mnt/ext4_mountpoint
```

This will allow you to access the storage filesystem. 
You can verify changes made to the storage filesystem either from the backend VM or your laptop. 
Changes are persistent and will remain intact even after a reboot of the Linux frontend guests.

**Note**: To use a filesystem as the root file system, add the following parameters to the `bootargs` in the frontend device tree file:
```
root=/dev/vda rootwait
```

This will configure the Linux frontend to boot directly from the virtual storage.

<!--- instruction#end -->

---

dtc /home/joaopeixoto13/thesis/bao-demos/demos/virtio/devicetrees/zcu102/linux-backend.dts > /home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio/linux-backend.dtb

make -C guests/linux/lloader ARCH=aarch64 IMAGE=/home/joaopeixoto13/thesis/bao-demos/wrkdir/srcs/buildroot-aarch64-v6.1/output/images/Image-zcu102-test-2 DTB=/home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio/linux-backend.dtb CROSS_COMPILE=/home/joaopeixoto13/thesis/wrkdir/toolchains/arm-gnu-toolchain-13.2.Rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf- TARGET=/home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio/linux_backend

make -C guests/linux/lloader ARCH=aarch64 IMAGE=/home/joaopeixoto13/thesis/bao-demos/wrkdir/srcs/buildroot-aarch64-v6.11/output/images/Image-zcu102 DTB=/home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio/linux-backend.dtb CROSS_COMPILE=/home/joaopeixoto13/thesis/wrkdir/toolchains/arm-gnu-toolchain-13.2.Rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf- TARGET=/home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio/linux_backend

---

cp /home/joaopeixoto13/thesis/bao-demos/wrkdir/srcs/freertos/build/zcu102/freertos.bin wrkdir/imgs/zcu102/virtio/

make -C wrkdir/srcs/freertos PLATFORM=zcu102 CROSS_COMPILE=/home/joaopeixoto13/thesis/wrkdir/toolchains/arm-gnu-toolchain-13.2.Rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf- STD_ADDR_SPACE=y

rm -rf wrkdir/srcs/bao/bin/ wrkdir/srcs/bao/build/

make -C /home/joaopeixoto13/thesis/bao-demos/wrkdir/srcs/bao        PLATFORM=zcu102        CONFIG_REPO=/home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio/config        CONFIG=virtio         CPPFLAGS=-DBAO_DEMOS_WRKDIR_IMGS=/home/joaopeixoto13/thesis/bao-demos/wrkdir/imgs/zcu102/virtio

---

cp /home/joaopeixoto13/thesis/bao-demos/wrkdir/srcs/bao/bin/zcu102/virtio/bao.bin /media/joaopeixoto13/boot/

umount /media/joaopeixoto13/boot
