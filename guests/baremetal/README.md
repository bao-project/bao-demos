# Bao Bare-Metal Guest

Setup an environment variable for the baremetal app source code:

```
export BAO_DEMOS_BAREMETAL=$BAO_DEMOS_WRKDIR_SRC/baremetal
```

Clone and build the bao bare-metal guest application:

```
git clone https://github.com/bao-project/bao-baremetal-guest.git\
    --branch demo $BAO_DEMOS_BAREMETAL
make -C $BAO_DEMOS_BAREMETAL PLATFORM=$PLATFORM $BAREMETAL_PARAMS
```

Copy the resulting binary to the final image's directory:

```
cp $BAO_DEMOS_BAREMETAL/build/$PLATFORM/baremetal.bin $BAO_DEMOS_WRKDIR_IMGS
```
