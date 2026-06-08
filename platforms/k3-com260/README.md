# SpacemiT K3 - CoM260 Kit

Bao on the SpacemiT K3 (octa-core X100, RISC-V, AIA + Sstc) on the CoM260 Kit
(a.k.a. Banana Pi BPI-SM10). Boot chain: `BootROM -> FSBL -> OpenSBI -> U-Boot -> Bao`.

> For the demos including Linux, Linux is NOT the generic kernel from the
> demo guide: it is SpacemiT's vendor tree
> (`https://github.com/spacemit-com/linux-6.18.git`, branch `k3-br-v1.0.y`).

## 1) Compile OpenSBI

The FSBL loads OpenSBI as the dynamic firmware, wrapped in SpacemiT's
`fw_dynamic.itb` FIT, built from their OpenSBI tree:

```
export BAO_DEMOS_OPENSBI=$BAO_DEMOS_WRKDIR_SRC/opensbi
git clone https://github.com/spacemit-com/opensbi.git $BAO_DEMOS_OPENSBI\
    --depth 1 --branch k3-br-v1.0.0
make -C $BAO_DEMOS_OPENSBI PLATFORM=generic PLATFORM_DEFCONFIG=k3_defconfig\
    CROSS_COMPILE=$OPENSBI_CROSS_COMPILE
cp $BAO_DEMOS_OPENSBI/build/platform/generic/firmware/fw_dynamic.itb\
    $BAO_DEMOS_WRKDIR_PLAT
```

## 2) Compile U-Boot

A single U-Boot build produces everything else the K3 boot flow needs:
`u-boot.itb` (the main U-Boot FIT), `u-boot-env-default.bin` (the default
environment image), `FSBL.bin` (the signed SPL the BootROM loads) and
`bootinfo_block.bin` (the block that points the BootROM at the FSBL):

```
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot-k3-br-v1.0.0
git clone https://github.com/spacemit-com/uboot-2022.10.git $BAO_DEMOS_UBOOT\
    --depth 1 --branch k3-br-v1.0.0
make -C $BAO_DEMOS_UBOOT ARCH=riscv CROSS_COMPILE=$OPENSBI_CROSS_COMPILE\
    k3_defconfig
make -C $BAO_DEMOS_UBOOT ARCH=riscv CROSS_COMPILE=$OPENSBI_CROSS_COMPILE\
    -j$(nproc)
cp $BAO_DEMOS_UBOOT/u-boot.itb $BAO_DEMOS_UBOOT/u-boot-env-default.bin\
    $BAO_DEMOS_UBOOT/FSBL.bin $BAO_DEMOS_UBOOT/bootinfo_block.bin\
    $BAO_DEMOS_WRKDIR_PLAT
```

> U-Boot needs the Linux-targeting toolchain: the bare-metal one cannot link
> its EFI applications.

## 3) Package Bao and the boot script as FITs

This board's U-Boot has the legacy image format disabled, so both Bao and the
boot script are delivered as FITs: `bao.itb` (Bao + the guests, fixed
load/entry `0x1_40000000`) and `boot.scr` (the script that loads + `bootm`'s
`bao.itb`). `mkimage` resolves the `/incbin/` in each `.its` relative to the
`.its` file's directory, so stage them next to `bao.bin` first:

```
cp $BAO_DEMOS/platforms/k3-com260/bao.its\
    $BAO_DEMOS/platforms/k3-com260/boot.cmd\
    $BAO_DEMOS/platforms/k3-com260/boot_scr.its $BAO_DEMOS_WRKDIR_IMGS
cd $BAO_DEMOS_WRKDIR_IMGS
mkimage -f bao.its bao.itb
mkimage -f boot_scr.its boot.scr
```

## 4) Patch the U-Boot environment

The vendor environment's `bootcmd` only knows how to boot grub or a raw Linux
kernel; it never sources `boot.scr`. Rebuild the env image with a `bootcmd`
that does (the image format is a 4-byte CRC32 followed by NUL-separated
variables; `mkenvimage` regenerates it):

```
cd $BAO_DEMOS_WRKDIR_PLAT
tail -c +5 u-boot-env-default.bin | tr '\0' '\n' | grep -aE '^[[:alnum:]_]+='\
    | sed '/^bootcmd=/d' > u-boot-env-bao.txt
echo 'bootcmd=mmc dev 0; if part number mmc 0 bootfs bootpart && load mmc 0:${bootpart} 0x140000000 boot.scr; then source 0x140000000; else run autoboot; fi'\
    >> u-boot-env-bao.txt
mkenvimage -s 0x4000 -p 0xff -o u-boot-env-bao.bin u-boot-env-bao.txt
```

<!--- instruction#1 -->
## 5) Flash the SD card

If you used the automated make, it already assembled a flashable SD-card image
(`sdcard.img`), so just write it to a microSD card (find the DISK node with
`lsblk` - NOT a partition; `dd` to the wrong disk destroys it):

```
sudo umount /dev/sdX* 2>/dev/null || true
sudo dd if=${BAO_DEMOS_WRKDIR_IMGS}/sdcard.img of=/dev/sdX bs=4M status=progress conv=fsync
sync
```

### …or build the card by hand

The image is just a GPT with one `bootfs` partition plus firmware blobs `dd`'d to
fixed offsets (the K3 BootROM/SPL find them by offset, not via the table - do not
move them). This is exactly the `$(sdcard_img)` recipe in `make.mk`; run the same
commands with `of=/dev/sdX` to write a card directly.

```
CARD=/dev/sdX                                  # the DISK node from lsblk

# GPT + one 256 MiB bootfs partition at 12 MiB (sectors 24576..548863):
sudo sgdisk --zap-all "$CARD"
sudo sgdisk --clear -n 1:24576:548863 -c 1:bootfs "$CARD"

# firmware to fixed offsets (seek is KiB, bs=1024):
sudo dd if=${BAO_DEMOS_WRKDIR_PLAT}/u-boot-env-bao.bin                of="$CARD" bs=1024 seek=640  conv=notrunc,fsync    # env      @640K
sudo dd if=${BAO_DEMOS_WRKDIR_PLAT}/bootinfo_block.bin                of="$CARD" bs=1024 seek=1024 conv=notrunc,fsync    # bootinfo @1M
sudo dd if=${BAO_DEMOS_WRKDIR_PLAT}/FSBL.bin                          of="$CARD" bs=1024 seek=1536 conv=notrunc,fsync    # fsbl     @1536K
sudo dd if=${BAO_DEMOS_WRKDIR_PLAT}/fw_dynamic.itb                    of="$CARD" bs=1024 seek=7168 conv=notrunc,fsync    # opensbi  @7M
sudo dd if=${BAO_DEMOS_WRKDIR_PLAT}/u-boot.itb                        of="$CARD" bs=1024 seek=8192 conv=notrunc,fsync    # uboot    @8M

# bootfs filesystem with bao.itb + boot.scr (+ u-boot.itb):
sudo partprobe "$CARD"; sudo mkfs.ext4 -F -L bootfs "${CARD}1"   # ${CARD}p1 on mmc/nvme
m=$(mktemp -d); sudo mount "${CARD}1" "$m"
sudo cp ${BAO_DEMOS_WRKDIR_IMGS}/bao.itb ${BAO_DEMOS_WRKDIR_IMGS}/boot.scr \
        ${BAO_DEMOS_WRKDIR_PLAT}/u-boot.itb "$m"/
sync; sudo umount "$m"; rmdir "$m"
```

On a card that already has a working K3 layout, skip the firmware and just
refresh `bootfs` with the new `bao.itb` + `boot.scr`:

```
m=$(mktemp -d); sudo mount "${CARD}1" "$m"                       # ${CARD}p1 on mmc/nvme
sudo cp ${BAO_DEMOS_WRKDIR_IMGS}/bao.itb ${BAO_DEMOS_WRKDIR_IMGS}/boot.scr "$m"/
sync; sudo umount "$m"; rmdir "$m"
```

Finally, insert the microSD card into the board's SD card slot.

<!--- instruction#2 -->
## 6) Connect the board

**Serial (console)** - UART0 at **115200 8N1**. Use the onboard USB-serial debug
port (enumerates as e.g. `/dev/ttyUSB0`), or an external 3.3 V USB-TTL/FTDI on the
debug header (cross the data lines, leave VCC unconnected):

```
  board UART0 TX  ->  FTDI RX        screen /dev/ttyUSB0 115200
  board UART0 RX  ->  FTDI TX
  board GND       <-> FTDI GND
```

For the **baremetal** demo this is the guest console. For **linux+freertos** this
is the **FreeRTOS** console (Linux has no serial console).

> Confirm the exact debug-header pinout against the CoM260 user guide (it is
> board-revision specific):
> https://spacemit.com/community/document/info?lang=en&nodepath=hardware/eco/k3_com260/com260_user_guide.md

<!--- instruction#3 -->
## 7) Boot Bao

Power on / reset: Bao boots unattended. On the serial console you can watch the
whole chain unfold:

1. The BootROM reads the bootinfo block from the card and loads the **FSBL**,
   which brings up DDR.
2. The FSBL loads **OpenSBI** (the resident M-mode firmware) and **U-Boot**
   from their fixed offsets.
3. U-Boot's `bootcmd` loads `boot.scr` from the `bootfs` partition and sources
   it; the script stages `bao.itb` in DRAM and `bootm` unpacks **Bao** to its
   fixed load address `0x140000000` and jumps to it.
4. Bao prints its banner, copies the guest image(s) to their memory regions,
   and starts the VMs.

You should then see the guests' output on the UART console (and, depending on
the demo, be able to reach a guest over the network): follow your specific
demo's instructions for what to expect and how to interact with it.

<!--- instruction#end -->
