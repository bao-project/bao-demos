# NXP S32Z270

The Lauterbach TRACE32 debugger is used to program the board. A `.cmm` script is available to load
and run the target demo.

---

<!--- instruction#1 -->
## 1) Setup board

Make sure you have the JTAG probe connected to the board (J129). Follow the table below to ensure
the proper hardware configuration that allows to run the demos smoothly:

| Jumper | Position |
| ------ |--------- |
| J124   | 1-2      |
| J125   | 2-3      |
| J247   | 1-2      |
| J248   | 1-2      |

| Switch     | SW4(0-7) | SW7(24-31) | SW5(8-15) | SW6(16-23) |
| ---------- |--------- |----------- |---------- |----------- |
| BOOT_CFG   | 00100000 | 00001100   | 00011000  | 00010010   |

**NOTE**

Other jumpers and switches are left as by default.

### 1.1) Setup terminals

Connect a micro-USB cable to the J119 USB port. This outputs Bao's serial output. Use a terminal
application such as `screen`. For example:

```
screen /dev/ttyUSB0 115200
```

Connect a USB-to-TTL adapter to the J61 and J37 headers to provide an extra UART interface for
guests. The wiring should be set up as follows:

| Header | Pin | Adapter |
| -------|-----|-------- |
| J61    | 2   | GND     |
| J37    | 11  | RX      |
| J37    | 8   | TX      |

Open a terminal application such as `screen` for guest's output. For example:

```
screen /dev/ttyUSB1 115200
```

<!--- instruction#2 -->
## 2) Load the demo using TRACE32

Run the following command, depending on the demo:

**Baremetal:**
```
t32marm -s ./platforms/$PLATFORM/t32.cmm $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_WRKDIR_IMGS/baremetal.bin
```

**Zephyr+Baremetal**
```
t32marm -s ./platforms/$PLATFORM/t32.cmm $BAO_DEMOS_WRKDIR_IMGS/bao.bin  $BAO_DEMOS_WRKDIR_IMGS/baremetal.bin $BAO_DEMOS_WRKDIR_IMGS/zephyr.bin
```

On TRACE32 you an add symbol files by using the command `Data.Load.Elf "<path/to/elf/file>"
/NoCODE`.

<!--- instruction#end -->

<!--- instruction#3 -->
## 3) Run the script

Click the `GO` button on the List window to run the demo.

<!--- instruction#end -->
