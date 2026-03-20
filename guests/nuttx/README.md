# Nuttx Guest

Setup the environment variables for Nuttx:

```
export BAO_DEMOS_NUTTX_VRS=nuttx-12.12.0
export BAO_DEMOS_NUTTX_SRC=$BAO_DEMOS_WRKDIR_SRC/nuttx
export BAO_DEMOS_NUTTX_APPS_SRC=$BAO_DEMOS_WRKDIR_SRC/apps
export BAO_DEMOS_NUTTX_BUILD=$BAO_DEMOS_WRKDIR_PLAT/nuttx_build
export BAO_DEMOS_NUTTX_BOARD_ROOT=$BAO_DEMOS/guests/nuttx
export BAO_DEMOS_NUTTX_PATCHES=$BAO_DEMOS_NUTTX_BOARD_ROOT/patches/nuttx/$BAO_DEMOS_NUTTX_VRS
export BAO_DEMOS_NUTTX_APPS_PATCHES=$BAO_DEMOS_NUTTX_BOARD_ROOT/patches/apps/$BAO_DEMOS_NUTTX_VRS
export BAO_DEMOS_NUTTX_BOARD_CONFIG=$BAO_DEMOS_NUTTX_BOARD_ROOT/boards/$PLATFORM/configs/bao_guest
export BAO_DEMOS_NUTTX_KCONFIG_BIN=$BAO_DEMOS_WRKDIR_BIN/nuttx-kconfig/bin
export BAO_DEMOS_NUTTX_TOOLCHAIN_BINDIR=$(dirname "$(which ${CROSS_COMPILE}gcc)")
export BAO_DEMOS_NUTTX_ENV="PATH=$BAO_DEMOS_NUTTX_KCONFIG_BIN:$BAO_DEMOS_NUTTX_TOOLCHAIN_BINDIR:$PATH"
```

Clone Nuttx kernel and apps module:

```
git clone https://github.com/apache/nuttx.git --depth 1 \
    --branch $BAO_DEMOS_NUTTX_VRS $BAO_DEMOS_NUTTX_SRC

git clone https://github.com/apache/nuttx-apps.git --depth 1 \
    --branch $BAO_DEMOS_NUTTX_VRS $BAO_DEMOS_NUTTX_APPS_SRC
```

---
**NOTE**

The out-of-tree board/chip support in this repository currently targets `PLATFORM=e3650`
with the `bao_guest` board profile.

Before configuring NuttX, install helper wrappers expected by the CMake flow:

```
python3 -m pip install --user kconfiglib
mkdir -p $BAO_DEMOS_NUTTX_KCONFIG_BIN
printf '%s\n' '#!/usr/bin/env python3' 'from olddefconfig import main' 'raise SystemExit(main())' > $BAO_DEMOS_NUTTX_KCONFIG_BIN/olddefconfig
printf '%s\n' '#!/usr/bin/env python3' 'from savedefconfig import main' 'raise SystemExit(main())' > $BAO_DEMOS_NUTTX_KCONFIG_BIN/savedefconfig
printf '%s\n' '#!/usr/bin/env python3' 'from setconfig import main' 'raise SystemExit(main())' > $BAO_DEMOS_NUTTX_KCONFIG_BIN/setconfig
chmod +x $BAO_DEMOS_NUTTX_KCONFIG_BIN/olddefconfig \
    $BAO_DEMOS_NUTTX_KCONFIG_BIN/savedefconfig \
    $BAO_DEMOS_NUTTX_KCONFIG_BIN/setconfig
```
---

Then, patch nuttx and apps and build:

```
git -C $BAO_DEMOS_NUTTX_SRC apply $BAO_DEMOS_NUTTX_PATCHES/*
git -C $BAO_DEMOS_NUTTX_APPS_SRC apply $BAO_DEMOS_NUTTX_APPS_PATCHES/*

env $BAO_DEMOS_NUTTX_ENV cmake -S $BAO_DEMOS_NUTTX_SRC \
    -B $BAO_DEMOS_NUTTX_BUILD \
    -DBOARD_CONFIG:STRING="$BAO_DEMOS_NUTTX_BOARD_CONFIG" \
    -DCMAKE_C_COMPILER=$(which ${CROSS_COMPILE}gcc) \
    -DCMAKE_CXX_COMPILER=$(which ${CROSS_COMPILE}g++) \
    -DCMAKE_ASM_COMPILER=$(which ${CROSS_COMPILE}gcc)

env $BAO_DEMOS_NUTTX_ENV cmake --build $BAO_DEMOS_NUTTX_BUILD
```

Finally, copy the image to the images directory:

```
cp $BAO_DEMOS_NUTTX_BUILD/nuttx.bin $BAO_DEMOS_WRKDIR_IMGS
```

For the `nuttx+baremetal` demo, NuttX enables the `shmdemo` app by default. Once NSH
starts, you can inspect mailbox status with `shmdemo dump` or send a message to the
bare-metal guest with `shmdemo send hello from nuttx`.
