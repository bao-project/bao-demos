# Qemu RV64 virt

## 1) Download build and install RV64 qemu

---

**NOTE**

If you already have qemu-system-riscv64 installed or you don't want to compile 
it but install it directly using a package manager or some other method, please
make sure you are using version 7.2.0 or higher. If so, you can skip this step.

---

```
export BAO_DEMOS_QEMU=$BAO_DEMOS_WRKDIR_SRC/qemu-$ARCH
git clone https://github.com/qemu/qemu.git $BAO_DEMOS_QEMU --depth 1\
   --branch v7.2.0
cd $BAO_DEMOS_QEMU
./configure --target-list=riscv64-softmmu --enable-slirp
make -j$(nproc)
sudo make install
```

## 2) Compile OpenSBI

```
export BAO_DEMOS_OPENSBI=$BAO_DEMOS_WRKDIR_SRC/opensbi
git clone https://github.com/bao-project/opensbi.git $BAO_DEMOS_OPENSBI\
    --depth 1 --branch bao/demo-next
make -C $BAO_DEMOS_OPENSBI PLATFORM=generic \
    FW_PAYLOAD=y \
    FW_PAYLOAD_FDT_ADDR=0x80100000\
    FW_PAYLOAD_PATH=$BAO_DEMOS_WRKDIR_IMGS/bao.bin
cp $BAO_DEMOS_OPENSBI/build/platform/generic/firmware/fw_payload.elf\
    $BAO_DEMOS_WRKDIR_IMGS/opensbi.elf
```

## 3) Run QEMU

```
 qemu-system-riscv64 -nographic\
    -M virt -cpu rv64 -m 4G -smp 4\
    -bios $BAO_DEMOS_WRKDIR_IMGS/opensbi.elf\
    -device virtio-net-device,netdev=net0 \
    -netdev user,id=net0,net=192.168.42.0/24,hostfwd=tcp:127.0.0.1:5555-:22\
    -device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3 -S
```

<!--- instruction#1 -->
## 4) Setup connections and jump to Bao

Qemu will let you know in which pseudo-terminals it placed the virtio serial.
For example:

```
char device redirected to /dev/pts/4 (label serial3)
```

Open a new terminal and connect to it. For example:

```
screen /dev/pts/4
```

Finally, start the emulation by pressing `Ctrl-a` then `c` in the terminal you 
launched qemu to access the monitor console and running:

```
(qemu) cont
```

Then, press `Ctrl-a` and `c` again to be able to interact with the guest 
through the serial console.

If you are running a Linux guest you can also connect via ssh to it by opening 
a new terminal and running:

```
ssh root@localhost -p 5555
```

When you want to leave QEMU press `Ctrl-a` then `x`.
 
<!--- instruction#end -->
