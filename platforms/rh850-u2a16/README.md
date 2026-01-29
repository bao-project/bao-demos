# RH850-U2A16

## 1) Download and install the gcc toolchain

You can download and compile the toolchain from the official GCC repository.

A toolchain that has been used and verified is [gcc 14.2.0](https://github.com/bao-project/gcc-v850-elf-toolchain/releases/download/v14.2.0/gcc-14.2.0-v850-elf.tar.gz).


## 2) Download and install rfp-cli

You can download `rfp-cli` for linux [here](https://www.renesas.com/rfp).
Add it to your PATH.

```
rfp-cli --version
Renesas Flash Programmer CLI V1.11
Module Version: V3.18.00.000
```

## 3) Build a demo
For example to build the baremetal demo:

```sh
export CROSS_COMPILE=v850-elf-
export DEMO=baremetal
export PLATFORM=rh850-u2a16

make
```

## 4) Flash bao.bin and guests to board

You must use the `rfp-cli` tool to program the `bao.bin` and guest images.
Please take a look at the corresponding demo instructions for this platform.
* [baremetal](baremetal.md)
