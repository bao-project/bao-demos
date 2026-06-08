# SPDX-License-Identifier: Apache-2.0
# Boot the Bao hypervisor on the SpacemiT K3 (CoM260 Kit) from U-Boot.
# Compiled to boot.scr as a FIT (mkimage -f boot_scr.its boot.scr; this U-Boot
# rejects legacy uImages) and placed on the bootfs partition next to bao.itb.

# Staging address for the FIT file; must not overlap the kernel's fixed load
# address 0x140000000 (baked into bao.itb). Both are well inside DRAM.
setenv bao_stage 0x150000000

setenv bao_mmc 0
mmc dev ${bao_mmc}

if part number mmc ${bao_mmc} bootfs bootpart; then
	echo "Bao: bootfs is mmc ${bao_mmc}:${bootpart}"
else
	echo "Bao: 'bootfs' not found by name; using part 7"
	setenv bootpart 7
fi

echo "Bao: loading bao.itb -> ${bao_stage}"
load mmc ${bao_mmc}:${bootpart} ${bao_stage} bao.itb

# Bao consumes only a0 (boot hart id); it ignores the FDT in a1, but bootm's FDT
# stage still needs a valid tree, so pass U-Boot's resident control FDT.
echo "Bao: booting..."
bootm ${bao_stage} - ${fdtcontroladdr}
