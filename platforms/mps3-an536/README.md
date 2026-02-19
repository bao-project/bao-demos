# Qemu Arm MPS3-AN536

## 1) Download build and install Arm QEMU w/ mp3-an546 platform

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

## 4) Run QEMU

```
qemu-system-arm -nographic \
    -M mps3-an536 -cpu cortex-r52 -smp 2 -m 2G \
    -device loader,file=$(trampoline_img)	 \
    -device loader,file=$(bao_image),addr=$(BAO_BASE_ADDR) \
    -serial null -serial null -serial mon:stdio -serial pty \
    $(foreach image_addr, $(qemu_loader_data), -device loader,file=$(word 1, $(subst @, ,$(image_addr))),addr=$(word 2, $(subst @, ,$(image_addr)))) -S
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
