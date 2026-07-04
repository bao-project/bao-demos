# Kria KR260

This guide describes how to boot Bao on the AMD Kria KR260.
The final Bao payload is wrapped as a U-Boot image (`bao.img`) and booted with
`bootm`.

## 1) Prepare a U-Boot image

Create the final Bao U-Boot image:

```bash
mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000 \
    -e 0x200000 -d $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_WRKDIR_IMGS/bao.img
```

<!--- instruction#1 -->
## 2) Setup boot media

Do **not** copy `BOOT.BIN` to the SD card for KR260. The K26 Starter Kits,
including KR260, have factory-programmed boot firmware stored in the SOM QSPI
device and maintained through the Kria boot firmware update flow.

If boot firmware needs to be updated, use either:

- `xmutil bootfw_update` from Linux, which writes a new `BOOT.BIN` into the
  non-active A/B QSPI slot and marks it active for the next boot.
- The Boot Image Recovery Tool, which is an Ethernet-based recovery application
  for manually updating the A/B boot firmware partitions in QSPI.

To use the Boot Image Recovery Tool on KR260:

1. Connect the PC to Ethernet port **J10C** on the KR260; this is the required
   recovery port for KR260.
2. Set the PC to a static IP in the `192.168.0.x` subnet, but not
   `192.168.0.111`.
3. Hold the firmware update button while powering on the board.
4. Open `http://192.168.0.111` in a web browser.
5. Use the recovery GUI to update either the A or B boot firmware partition with
   the desired `BOOT.BIN` file.

For normal Bao boot on KR260, place only `bao.img` on the USB drive.

<!--- instruction#2 -->
## 3) Setup board

Ensure the board has valid boot firmware already programmed in QSPI. For KR260,
Bao is loaded from USB at the U-Boot prompt, while boot firmware remains managed
separately in QSPI.

Connect to the KR260 UART console at 115200 baud and power on or reset the board.

<!--- instruction#3 -->
## 4) Run U-Boot commands

Quickly press any key to skip autoboot. If needed, press `ctrl-c` until you get
the U-Boot prompt. Then load the Bao image from USB and boot it:

```bash
usb start; fatload usb 0 0x200000 bao.img; bootm start 0x200000; bootm loados; bootm go
```

You should see Bao and its guests printing on the UART.

At this point, depending on your demo, you might be able to connect to one of
the guests via SSH through the board Ethernet interface.

<!--- instruction#end -->