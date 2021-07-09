# Rocket/Chypyard on Xilinx FPGA

<!--- instruction#1 -->

## 0) Setup environment

You will need to [install the necessary Chipyard and Rocket-chip dependencies](https://github.com/chipsalliance/rocket-chip#install-necessary-dependencies) such as Chisel to build Rocket RTL. 

If you don't have Vivado already installed, do it by following 
[Installing the Vivado Design Suite Tools](https://www.xilinx.com/content/dam/xilinx/support/documentation/sw_manuals/xilinx2020_2/ug973-vivado-release-notes-install-license.pdf). Then, make 
sure the Vivado tools binary directory is in your **PATH** environment variable.

<!--- instruction#end -->

## 1) Generate the Rocket/Chipyard SoC RTL design

Clone and setup the Chipyard/Rocket-Chip repos:

```
export BAO_DEMOS_CHIPYARD=$BAO_DEMOS_WRKDIR_SRC/chipyard
export BAO_DEMOS_ROCKETCHIP=$BAO_DEMOS_CHIPYARD/generators/rocket-chip
git clone https://github.com/ucb-bar/chipyard.git $BAO_DEMOS_CHIPYARD
cd $BAO_DEMOS_CHIPYARD
git checkout 64632c8
./scripts/init-submodules-no-riscv-tools.sh
git apply $BAO_DEMOS/platforms/$PLATFORM/patches/0001-add-rocket-hyp-fpga-support.patch
git -C generators/boom apply $BAO_DEMOS/platforms/$PLATFORM/patches/0001-boom-add-usehyp-option.patch
git -C generators/ariane apply $BAO_DEMOS/platforms/$PLATFORM/patches/0001-ariane-add-usehyp-option.patch
cd $BAO_DEMOS_ROCKETCHIP
git remote add hyp https://github.com/josecm/rocket-chip.git
git fetch hyp
git checkout hyp
```

Build the bootrom:

```
make -C $BAO_DEMOS_CHIPYARD/bootromFPGA
```

Generate the verilog:

```
export BAO_DEMOS_ROCKET_CONFIG=RocketHypConfig$(echo $PLATFORM | awk '{split($0,A,"-"); print A[length(A)]}')
make -C $BAO_DEMOS_CHIPYARD/sims/vcs verilog SUB_PROJECT=rocket \
    CONFIG=$BAO_DEMOS_ROCKET_CONFIG
```

## 2) Build the design using Vivado

Now run the Vivado tcl scripts which will create a Vivado project and run all
the build steps.

First set the number of theads you want vivado to use on the 
build in the **VIVADO_CORES** environment variable. For example, to use all 
available cores:

```
export VIVADO_CORES=$(nproc)
```

Then run the tcl scripts to create and build the vivado designs:

```
export BAO_DEMOS_VIVADO_SCRIPTS=$BAO_DEMOS/platforms/$PLATFORM/scripts
vivado -nolog -nojournal -mode batch -source $BAO_DEMOS_VIVADO_SCRIPTS/create_ip.tcl
vivado -nolog -nojournal -mode batch -source $BAO_DEMOS_VIVADO_SCRIPTS/create_design.tcl
vivado -nolog -nojournal -mode batch -source $BAO_DEMOS_VIVADO_SCRIPTS/build.tcl
```

## 3) Compile OpenSBI

Clone and build OpenSBI firmware that runs in machine mode:

```
export BAO_DEMOS_OPENSBI=$BAO_DEMOS_WRKDIR_SRC/opensbi
git clone https://github.com/bao-project/opensbi.git $BAO_DEMOS_OPENSBI\
    --depth 1 --branch bao/rocket
make -C $BAO_DEMOS_OPENSBI PLATFORM=$PLATFORM \
    FW_PAYLOAD=y \
    FW_PAYLOAD_FDT_ADDR=0x40100000\
    FW_PAYLOAD_PATH=$BAO_DEMOS_WRKDIR_IMGS/bao.bin
cp $BAO_DEMOS_OPENSBI/build/platform/$PLATFORM/firmware/fw_payload.bin\
    $BAO_DEMOS_WRKDIR_IMGS/opensbi.bin
```

<!--- instruction#2 -->

## 4) Setup board

First make sure you have the board's boot mode configured as JTAG. If you 
are not sure how, check the the board's user guide provided by Xilinx.
Then, connect to the baord via the  available micro-USB JTAG/UART port.

Connect to both UARTs available on the board (baud 115200) (in our setup
*/dev/ttyUSB1* and */dev/ttyUSB2* for ZCU104) and connect to them using a tool
such as screen. For example:

```
screen /dev/ttyUSB1 115200
```

Turn on/reset your board.

<!--- instruction#end -->

## 5) Flash bitsream and run

Execute the deploy script using the xilinx command line tools which will load
the system image and flash the bitsteam:

```
xsct  $BAO_DEMOS_VIVADO_SCRIPTS/deploy.tcl $BAO_DEMOS_WRKDIR_IMGS/opensbi.bin
```

You should start seeing OpenSBI, Bao and the guests' ouputs.