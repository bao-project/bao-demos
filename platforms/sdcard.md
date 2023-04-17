## Prepare SD card

First make sure you save all the files currently in the card.

In this example the sdcard is `/dev/mmcblk0` and the partitions are
`/dev/mmcblk0p1`, `/dev/mmcblk0p2`, etc. Other common names might be TODO.

### i) Make sure all partitions are unmounted

```
umount /dev/mmcblk0*
```

### ii) Delete all partitions

```
sudo fdisk /dev/mmcblk0
```

Then run the commands:

* Press `d` until there are no more partitions (if it asks you for the
partition, press `return` for the default)
* Press `w` write changes and exit

### iii) Create partition

```
sudo fdisk /dev/mmcblk0
```

Then run the commands:

* `o` to create a new empty DOS partition table
* `n` to create a new partition. Select the following options:
    *  `p` to make it a primary partition
    *  the automatically assigned partition number by pressing `return`
    *  `16384` (this gap is needed for some of the selected boards)
    *  the max default size by pressing `return`
    *  if it asks you to remove the file system signature press `y`
* `a` to make the partition bootable
* `t` to set the partition type:
    * type `c` for W95 FAT32 (LBA)
* `w` to write changes and exit


### iv) Format partition

Format the created partition to a fat filesystem:

```
sudo mkfs.fat /dev/mmcblk0p1 -n boot
```

Remove and insert the sd card to automatically mount it. Setup an environment
variable to the mount path:

---

**NOTE**

This guide expects for the sd card mount point to be at **/media/$USER/boot**.

---

Setup environment variables pointing to the sd card device, partition and mount
point, which in our example are:

```
export BAO_DEMOS_SDCARD_DEV=/dev/mmcblk0
export BAO_DEMOS_SDCARD=/media/$USER/boot
```
