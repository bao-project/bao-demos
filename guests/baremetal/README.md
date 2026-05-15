# Bao Bare-Metal Guest

Setup an environment variable for the baremetal app source code:

```shell
export BAO_DEMOS_BAREMETAL=$BAO_DEMOS_WRKDIR_SRC/baremetal
```

Clone and build the bao bare-metal guest application:

```shell
git clone https://github.com/bao-project/bao-baremetal-guest.git \
    $BAO_DEMOS_BAREMETAL
(cd $BAO_DEMOS_BAREMETAL && git checkout 5821efb7ae0cf5718aa8dcc6d735019eea212cb8)
make -C $BAO_DEMOS_BAREMETAL PLATFORM=$PLATFORM $BAREMETAL_PARAMS
```

Copy the resulting binary to the final image's directory:

```shell
cp $BAO_DEMOS_BAREMETAL/build/$PLATFORM/baremetal.bin $BAO_DEMOS_WRKDIR_IMGS
```
