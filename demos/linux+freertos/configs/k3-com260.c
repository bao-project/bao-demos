#include <config.h>

VM_IMAGE(linux_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux.bin))
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin))

struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, },
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        /* --- Linux: ethernet only, headless (network access) -------------- */
        {
            .image = {
                .base_addr = 0x180000000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image),
            },
            .entry = 0x180000000,
            .cpu_affinity = 0x7, /* harts 0-2 */

            .platform = {
                .cpu_num = 3,

                .region_num = 1,
                .regions = (struct vm_mem_region[]) {
                    {
                        .base = 0x180000000,
                        .size = 0x40000000, /* 1 GiB */
                        .place_phys = true,
                        .phys = 0x180000000,
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) { 52 },
                    },
                },

                .dev_num = 5,
                .devs = (struct vm_dev_region[]) {
                    {
                        /* eth1 (gmac) + the wake irq */
                        .pa = 0xcac82000,
                        .va = 0xcac82000,
                        .size = 0x2000,
                        .interrupt_num = 2,
                        .interrupts = (irqid_t[]) { 133, 277 },
                    },
                    {
                        /* syscon_apbc (clock/reset provider for gpio + pinctrl) */
                        .pa = 0xd4015000,
                        .va = 0xd4015000,
                        .size = 0x1000,
                    },
                    {
                        /* syscon_apmu (gmac clocks + reset); reg base page-aligned */
                        .pa = 0xd4282000,
                        .va = 0xd4282000,
                        .size = 0x1000,
                    },
                    {
                        /* pinctrl (muxes the gmac1 RGMII/MDIO pads) */
                        .pa = 0xd401e000,
                        .va = 0xd401e000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) { 60 },
                    },
                    {
                        /* gpio (gmac PHY reset line) + its syscon views */
                        .pa = 0xd4019000,
                        .va = 0xd4019000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) { 58 },
                    }
                },

                .arch = {
                    .irqc.aia.aplic.base = 0xe0804000,
                    .irqc.aia.imsic.base = 0xe0400000,
                },
            },
        },
        /* --- FreeRTOS: owns the single UART console ----------------------- */
        {
            .image = {
                /* built with STD_ADDR_SPACE: linked at and entered from 0x0 */
                .base_addr = 0x0,
                .load_addr = VM_IMAGE_OFFSET(freertos_image),
                .size = VM_IMAGE_SIZE(freertos_image),
            },
            .entry = 0x0,
            .cpu_affinity = 0x8, /* hart 3 */

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions = (struct vm_mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x8000000, /* 128 MiB */
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) { 52 },
                    },
                },

                .dev_num = 1,
                .devs = (struct vm_dev_region[]) {
                    {
                        .pa = 0xd4017000, 
                        .va = 0xd4017000, 
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) { 42 },
                    },
                },

                .arch = {
                    .irqc.aia.aplic.base = 0xe0804000,
                    .irqc.aia.imsic.base = 0xe0400000,
                },
            },
        },
    },
};
