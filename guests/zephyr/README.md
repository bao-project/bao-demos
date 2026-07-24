# Zephyr Guest

---
**NOTE**

RISC-V S-mode support (and the AIA drivers the RISC-V boards rely on) is not
part of any Zephyr release yet, so all platforms pin a snapshot of upstream
main. Zephyr main requires Python 3.12 or newer on the PATH to build.

---

Setup the environment variables for Zephyr:

```
export BAO_DEMOS_ZEPHYR_VRS=99e635ace5aa2d9b6d069dd3a3b94018d17742d9
export BAO_DEMOS_ZEPHYR_CMSIS_COMMIT=512cc7e895e8491696b61f7ba8066b4a182569b8
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
git init $BAO_DEMOS_ZEPHYR_CMSIS_SRC
git -C $BAO_DEMOS_ZEPHYR_CMSIS_SRC remote add origin \
    https://github.com/zephyrproject-rtos/cmsis.git
git -C $BAO_DEMOS_ZEPHYR_CMSIS_SRC fetch --depth 1 origin \
    $BAO_DEMOS_ZEPHYR_CMSIS_COMMIT
git -C $BAO_DEMOS_ZEPHYR_CMSIS_SRC checkout FETCH_HEAD
git init $BAO_DEMOS_ZEPHYR_SRC
git -C $BAO_DEMOS_ZEPHYR_SRC remote add origin \
    https://github.com/zephyrproject-rtos/zephyr.git
git -C $BAO_DEMOS_ZEPHYR_SRC fetch --depth 1 origin $BAO_DEMOS_ZEPHYR_VRS
git -C $BAO_DEMOS_ZEPHYR_SRC checkout FETCH_HEAD
```

---
**NOTE**

For NXP S32Z270, we need to specify the rtu0 as a board qualifier and also fetch the `hal_nxp`
module at the revision the pinned Zephyr expects.

```
export BAO_DEMOS_ZEPHYR_BOARD=baovm_$PLATFORM/s32z270/rtu0
export BAO_DEMOS_ZEPHYR_NXP_SRC=$BAO_DEMOS_WRKDIR_SRC/nxp
export BAO_DEMOS_ZEPHYR_NXP_VRS=6c77523dfdfc5a1ecefbafb31feee6499a5de8ec
git init $BAO_DEMOS_ZEPHYR_NXP_SRC
git -C $BAO_DEMOS_ZEPHYR_NXP_SRC remote add origin \
    https://github.com/bao-project/hal_nxp.git
git -C $BAO_DEMOS_ZEPHYR_NXP_SRC fetch --depth 1 origin $BAO_DEMOS_ZEPHYR_NXP_VRS
git -C $BAO_DEMOS_ZEPHYR_NXP_SRC checkout FETCH_HEAD
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
