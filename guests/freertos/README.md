# FreeRTOS Guest

Setup an environment variable for the FreeRTOS repo:

```shell
export BAO_DEMOS_FREERTOS=$BAO_DEMOS_WRKDIR_SRC/freertos
```

Then clone and build the FreeRTOS:

```shell
git clone https://github.com/bao-project/freertos-over-bao.git \
    $BAO_DEMOS_FREERTOS
(cd $BAO_DEMOS_FREERTOS && \
git checkout a4787e224ac20ab2fc4601d1a7f6bbe7ada84b9b && \
git submodule update --init --recursive)
make -C $BAO_DEMOS_FREERTOS PLATFORM=$PLATFORM $FREERTOS_PARAMS
```

Finally, copy the FreeRTOS image to the final guest image directory:

```shell
cp $BAO_DEMOS_FREERTOS/build/$PLATFORM/freertos.bin $BAO_DEMOS_WRKDIR_IMGS
```
