# Nvidia Tegra TX2

---

**NOTE**

In this guide, we build and flash the TF-A firmware in order to control SMMU 
stream ids of peripherals used by some virtual machines, which are not 
programmed by default. If you are targeting a demo where no virtual machine 
features DMA-capable peripherals (e.g. baremetal demo) you can use the 
pre-flashed firmware and skip directly to [step 4](#4-Setup-SD-card).

---

## 1) Build TF-A

Download and build TF-A:

```
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware 
git clone https://github.com/bao-project/arm-trusted-firmware.git\
    $BAO_DEMOS_ATF --branch bao/demo-next --depth 1
cd $BAO_DEMOS_ATF
make PLAT=tegra TARGET_SOC=t186 bl31
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_ATF/build/tegra/t186/release/bl31.bin $BAO_DEMOS_WRKDIR_PLAT
```

## 2) Prepare firmware with new TF-A image

For this you will first need to download Nvidia's flashing tools part of the 
TX2's [L4T Driver Package (BSP)][tegra-bsp]. Then extract it:

```
export BAO_DEMOS_NVIDIA_TOOLS=$BAO_DEMOS_WRKDIR_SRC/nvidia-tools
mkdir -p $BAO_DEMOS_NVIDIA_TOOLS
wget -P $BAO_DEMOS_NVIDIA_TOOLS/\
    https://developer.nvidia.com/embedded/l4t/r32_release_v5.1/\
r32_release_v5.1/t186/tegra186_linux_r32.5.1_aarch64.tbz2
tar xfvm $BAO_DEMOS_NVIDIA_TOOLS/tegra186_linux_r32.5.1_aarch64.tbz2\
    -C $BAO_DEMOS_NVIDIA_TOOLS
```

Create the Trusted OS image for the flash tool:

```
$BAO_DEMOS_NVIDIA_TOOLS/Linux_for_Tegra/nv_tegra/tos-scripts/gen_tos_part_img.py\
    --monitor $BAO_DEMOS_WRKDIR_PLAT/bl31.bin $BAO_DEMOS_WRKDIR_PLAT/tos.img       
```

<!--- instruction#1 -->
## 3) Flash the firmware

Set your board to recovery mode by:

1) Power it off completely. If needed unplug and replug the power cord.
2) Press down the Recovery button. While pressing the Recovery button, press
the Power button. Wait for the board to turn on.
3) Connect to the board using USB through the J28 micro-USB port.
4) Flash the board:

``` 
cd $BAO_DEMOS_NVIDIA_TOOLS/Linux_for_Tegra
sudo ./flash.sh -k secure-os --image $BAO_DEMOS_WRKDIR_PLAT/tos.img\ 
    --bup jetson-tx2 mmcblk0p1
```

If all goes well you should see the message:

```
*** The [secure-os] has been updated successfully. ***
```

<!--- instruction#2 -->
## 4) Setup SD card

After [preparing your sd card](../../platforms/sdcard.md), copy bao's final
image to it:

```
cp $BAO_DEMOS_WRKDIR_IMGS/bao.bin $BAO_DEMOS_SDCARD
umount $BAO_DEMOS_SDCARD
```

<!--- instruction#3 -->
## 5) Setup board

Insert the sd card in the board's sd slot.

Connect to the TX2's UART using a USB-to-TTL adapter. Use a terminal 
application such as `screen`. For example:

```
screen /dev/ttyUSB0 115200
```

Turn on/reset your board.

<!--- instruction#4 -->
## 6) Run u-boot commands

You will get u-boot's prompt. Load the bao image, and jump to it:

```
fatload mmc 1 0xa0000000 bao.bin; go 0xa0000000
```

You should see the firmware, bao and its guests printing on the UART.

At this point, depending on your demo, you might be able connect to one of the 
guests via ssh by connecting to the board's ethernet RJ45 socket.

<!--- instruction#end -->

<!-- Links -->

[tegra-bsp]: https://developer.nvidia.com/embedded/linux-tegra
