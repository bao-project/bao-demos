# Qemu RV64 virt

## 1) Download build and install RV64 qemu

---

**NOTE**

For now you really need to use our patched QEMU because currently RISC-V QEMU 
is missing a couple small features from the latest draft Hypervisor 
specification that Bao depends on to run correctly. We have submitted patches 
to fix these issues which we hope will be merged and available in the next 
release.

---

```
export BAO_DEMOS_QEMU=$BAO_DEMOS_WRKDIR_SRC/qemu-$ARCH
git clone https://github.com/josecm/qemu.git $BAO_DEMOS_QEMU --depth 1\
    --branch josecm/hyp
cd $BAO_DEMOS_QEMU
./configure --target-list=riscv64-softmmu
make -j$(nproc)
sudo make install
```

## 2) Compile OpenSBI

```
export BAO_DEMOS_OPENSBI=$BAO_DEMOS_WRKDIR_SRC/opensbi
git clone https://github.com/bao-project/opensbi.git $BAO_DEMOS_OPENSBI\
    --depth 1
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
    -M virt -cpu rv64,priv_spec=v1.10.0,x-h=true -m 4G -smp 4 -serial pty\
    -bios $BAO_DEMOS_WRKDIR_IMGS/opensbi.elf\
    -device virtio-net-device,netdev=net0 -netdev user,id=net0,hostfwd=tcp:127.0.0.1:5555-:22\
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