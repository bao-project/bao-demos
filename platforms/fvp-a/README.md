# FVP-A

## 1) Download and install the FVP model

---

**NOTE**

If you already have FVP_Base_RevC-2xAEMvA installed please make sure you are
using version 11.28_23 or higher. If so, you can skip this step

---

Download and extract the model:

```
curl -L https://developer.arm.com/-/cdn-downloads/permalink/FVPs-Architecture/FM-11.28/FVP_Base_RevC-2xAEMvA_11.28_23_Linux64.tgz | tar xz -C $BAO_DEMOS_WRKDIR_PLAT
export PATH=$PATH:$BAO_DEMOS_WRKDIR_PLAT/Base_RevC_AEMvA_pkg/models/Linux64_GCC-9.3
```

## 2) Build U-boot

Download and configure it:

```
export BAO_DEMOS_UBOOT=$BAO_DEMOS_WRKDIR_SRC/u-boot
git clone https://github.com/u-boot/u-boot.git $BAO_DEMOS_UBOOT --depth 1\
   --branch v2025.07
cd $BAO_DEMOS_UBOOT
make vexpress_aemv8a_semi_defconfig
```

Now you need to set the Kconfig options:

* CONFIG_AUTOBOOT=n

You can do it via using an interface such as `menuconfig` or just write them 
directly to the config file:

```
echo "CONFIG_AUTOBOOT=n\n" >> $BAO_DEMOS_UBOOT/.config
```

And build it:


```
make -j$(nproc)
```

And copy the image to the platform's working directory:

```
cp $BAO_DEMOS_UBOOT/u-boot.bin $BAO_DEMOS_WRKDIR/imgs/$PLATFORM
```

## 3) Build TF-A


```
export BAO_DEMOS_ATF=$BAO_DEMOS_WRKDIR_SRC/arm-trusted-firmware-$(ARCH)
git clone https://github.com/bao-project/arm-trusted-firmware.git\
   $BAO_DEMOS_ATF --branch bao/demo-next --depth 1
cd $BAO_DEMOS_ATF
make PLAT=fvp bl1 fip BL33=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM/u-boot.bin \
    QEMU_USE_GIC_DRIVER=QEMU_GICV3 ARCH=$ARCH
cp build/fvp/release/bl1.bin $BAO_DEMOS_WRKDIR/imgs/$PLATFORM
cp build/fvp/release/fip.bin $BAO_DEMOS_WRKDIR/imgs/$PLATFORM
```

## 4) Run FVP


```
FVP_Base_RevC-2xAEMvA \
    -C cluster0.NUM_CORES=4 \
	-C cache_state_modelled=0 \
	-C bp.refcounter.use_real_time=1 \
	-C bp.exclusive_monitor.monitor_access_level=1 \
	-C cluster0.supports_multi_threading=0 \
	-C cluster0.mpidr_layout=0 \
	-C cluster1.NUM_CORES=0 \
	-C pctl.startup=0.0.0.0 \
	-C pctl.Affinity-shifted=0 \
	-C pctl.CPU-affinities='0.0.0.0, 0.0.0.1, 0.0.0.2, 0.0.0.3' \
	-C gic_distributor.CPU-affinities='0.0.0.0, 0.0.0.1, 0.0.0.2, 0.0.0.3' \
	-C gic_distributor.reg-base-per-redistributor='0.0.0.0=0x2f100000,0.0.0.1=0x2f120000,0.0.0.2=0x2f140000,0.0.0.3=0x2f160000' \
	-C bp.smsc_91c111.enabled=true -C bp.hostbridge.userNetworking=true \
	-C bp.hostbridge.userNetSubnet=192.168.42.0/24 -C bp.hostbridge.userNetPorts=127.0.0.1:5555=22 \
    --data=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM/bl1.bin@0x0  \
    --data=$BAO_DEMOS_WRKDIR/imgs/$PLATFORM/fip.bin@0x08000000 \
	--data=$BAO_DEMOS_WRKDIR_IMGS/bao.bin@0x90000000
```


<!--- instruction#1 -->
## 5) Jump to Bao (Only for AArch64)

Once U-boot initializes, jump to Bao's load address:

```
go 0x90000000
```

---

**NOTE**

The FVP AEM models are quite slow. Demos featuring heavier guests like Linux
might take some time to fully boot up.

---

<!--- instruction#end -->
