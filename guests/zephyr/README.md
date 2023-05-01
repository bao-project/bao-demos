# Zephyr Guest

Setup the environment variables for Zephyr:

```
export BAO_DEMOS_ZEPHYR_VRS=v3.3.0
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

Then clone and build it:

```
git clone https://github.com/zephyrproject-rtos/cmsis.git --depth 1 \
    $BAO_DEMOS_ZEPHYR_CMSIS_SRC
git clone https://github.com/zephyrproject-rtos/zephyr.git --depth 1 \
    --branch $BAO_DEMOS_ZEPHYR_VRS $BAO_DEMOS_ZEPHYR_SRC

git -C $BAO_DEMOS_ZEPHYR_SRC apply $BAO_DEMOS_ZEPHYR_PATCHES/*

env $BAO_DEMOS_ZEPHYR_ENV cmake -GNinja \
    -DCMAKE_PREFIX_PATH=$BAO_DEMOS_ZEPHYR_SRC/share/zephyr-package \
	-DZEPHYR_MODULES=$BAO_DEMOS_ZEPHYR_CMSIS_SRC \
    -B$BAO_DEMOS_ZEPHYR_BUILD \
	-DBOARD_ROOT=$BAO_DEMOS_ZEPHYR_BOARD_ROOT \
    -DBOARD=$BAO_DEMOS_ZEPHYR_BOARD \
	$BAO_DEMOS_ZEPHYR_APP

ninja -C $BAO_DEMOS_ZEPHYR_BUILD
```

Finally, copy the image to the images directory:

```
cp $BAO_DEMOS_ZEPHYR_BUILD/zephyr/zephyr.bin $BAO_DEMOS_WRKDIR_IMGS
```
