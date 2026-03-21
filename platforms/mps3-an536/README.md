# Qemu Arm MPS3-AN536

## 1) Download build and install Arm QEMU w/ mps3-an536 platform

---

**NOTE**

If you already have qemu-system-arm installed or you don't want to compile
it but install it directly using a package manager or some other method, please
make sure you are using version 10.2.0 or higher. If so, you can skip this step.

---

```
export BAO_DEMOS_QEMU=$BAO_DEMOS_WRKDIR_SRC/qemu-$ARCH
git clone https://github.com/qemu/qemu.git $BAO_DEMOS_QEMU --depth 1\
   --branch v10.2.0
cd $BAO_DEMOS_QEMU
./configure --target-list=arm-softmmu --enable-slirp
make -j$(nproc)
sudo make install
```

## 2) Build the trampoline

```
cd $BAO_DEMOS/platforms/mps3-an536/trampline
make BAO_BASE_ADDR=0x20000000 OUTPUT_DIR=$BAO_DEMOS_WRKDIR_PLAT
```

## 3) Run QEMU

```
qemu-system-arm -nographic \
    -M mps3-an536 -cpu cortex-r52 -smp 2 -m 2G \
    -device loader,file=$BAO_DEMOS_WRKDIR_PLAT/trampline.bin \
    -device loader,file=$BAO_DEMOS_WRKDIR_IMGS/bao.bin,addr=0x20000000 \
    -serial null -serial null -serial mon:stdio -serial pty \
    $(printf -- "-device loader,file=%s " $(echo $MPS3_VM_IMAGES | sed 's/@/,addr=/g')) -S
```

`$MPS3_VM_IMAGES` is demo-specific. See the demo's README for the value to use.

<!--- instruction#1 -->
## 4) Setup connections and resume QEMU

QEMU will print the path of the pseudo-terminal it opened for the guest serial.
For example:

```
char device redirected to /dev/pts/4 (label serial3)
```

If you are running the zephyr+baremetal demo, open a new terminal and connect
to it. For example:

```
screen /dev/pts/4
```

Finally, resume QEMU execution. Press `Ctrl-A C` to enter the QEMU monitor,
then type:

```
c
```

When you want to leave QEMU press `Ctrl-A X`.

<!--- instruction#end -->
