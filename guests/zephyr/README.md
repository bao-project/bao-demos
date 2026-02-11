# Zephyr Guest

Setup the environment variables for Zephyr:

```
export BAO_DEMOS_ZEPHYR_VRS=v4.1.0
export BAO_DEMOS_ZEPHYR_SRC=$BAO_DEMOS_WRKDIR_SRC/zephyr
export BAO_DEMOS_ZEPHYR_CMSIS_SRC=$BAO_DEMOS_WRKDIR_SRC/cmsis
export BAO_DEMOS_ZEPHYR_BUILD=$BAO_DEMOS_WRKDIR_PLAT/zephyr_build
export BAO_DEMOS_ZEPHYR_ENV="ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
    CROSS_COMPILE=$(which ${CROSS_COMPILE}gcc | sed 's/.\{3\}$//')"
export BAO_DEMOS_ZEPHYR_BOARD_ROOT=$BAO_DEMOS/guests/zephyr
export BAO_DEMOS_ZEPHYR_PATCHES=$BAO_DEMOS_ZEPHYR_BOARD_ROOT/patches/$BAO_DEMOS_ZEPHYR_VRS
export BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM
export BAO_DEMOS_ZEPHYR_APP=$BAO_DEMOS/demos/$DEMO/zephyr/app
```

Clone zephyr kernel and necessary modules:

```
git clone https://github.com/zephyrproject-rtos/cmsis.git --depth 1 \
    $BAO_DEMOS_ZEPHYR_CMSIS_SRC
git clone https://github.com/zephyrproject-rtos/zephyr.git --depth 1 \
    --branch $BAO_DEMOS_ZEPHYR_VRS $BAO_DEMOS_ZEPHYR_SRC
```

---
**NOTE**

For NXP S32Z270, we need to specify the rtu0 as a board qualifier and also clone the `hal_nxp`
module on the specific revision that works with Zephyr v4.1.0.

```
export BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM/s32z270/rtu0
export BAO_DEMOS_ZEPHYR_NXP_SRC=$BAO_DEMOS_WRKDIR_SRC/nxp
git clone https://github.com/bao-project/hal_nxp.git --depth 1 \
    --branch v4.1.0 $BAO_DEMOS_ZEPHYR_NXP_SRC
```
---

Then, patch Zephyr and build.

```
git -C $BAO_DEMOS_ZEPHYR_SRC apply $BAO_DEMOS_ZEPHYR_PATCHES/*

env $BAO_DEMOS_ZEPHYR_ENV cmake -GNinja \
    -DCMAKE_PREFIX_PATH=$BAO_DEMOS_ZEPHYR_SRC/share/zephyr-package \
	-DZEPHYR_MODULES="$BAO_DEMOS_ZEPHYR_CMSIS_SRC;$BAO_DEMOS_ZEPHYR_NXP_SRC" \
    -B$BAO_DEMOS_ZEPHYR_BUILD \
	-DBOARD_ROOT=$BAO_DEMOS_ZEPHYR_BOARD_ROOT \
    -DBOARD=$BAO_DEMOS_ZEPHYR_BOARD \
    -DDTC_OVERLAY_FILE="app.overlay;boards/$PLATFORM.overlay" \
	$BAO_DEMOS_ZEPHYR_APP

ninja -C $BAO_DEMOS_ZEPHYR_BUILD
```

Finally, copy the image to the images directory:

```
cp $BAO_DEMOS_ZEPHYR_BUILD/zephyr/zephyr.bin $BAO_DEMOS_WRKDIR_IMGS
```
