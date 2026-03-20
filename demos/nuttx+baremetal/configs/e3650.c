#include <config.h>

VM_IMAGE(nuttx_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/nuttx.bin))
VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin))

struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = {
            .base = 0x00CD0000,
            .size = 0x00020000,
        }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            .image = VM_IMAGE_BUILTIN(nuttx_image, 0x00BA0000),
            .entry = 0x00BA0064,
            .cpu_affinity = 0x3,
            .platform = {
                .cpu_num = 2,
                .region_num = 1,
                .regions = (struct vm_mem_region[]) {
                    {
                        .base = 0x00BA0000,
                        .size = 0x00110000,
                    }
                },
                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x00CD0000,
                        .size = 0x00020000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (unsigned[]) {52}
                    }
                },
                .dev_num = 1,
                .devs = (struct vm_dev_region[]) {
                    {
                        /* UART18 - NuttX console output */
                        .pa = 0xF8970000,
                        .va = 0xF8970000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (unsigned[]) { 117 },
                    },
                },
                .arch = {
                    .gic = {
                        .gicd_addr = 0xF4000000,
                        .gicr_addr = 0xF4100000,
                    }
                }
            }
        },
        {
            .image = VM_IMAGE_BUILTIN(baremetal_image, 0x00CB0000),
            .entry = 0x00CB0000,
            .cpu_affinity = 0xc,
            .platform = {
                .cpu_num = 2,
                .region_num = 1,
                .regions = (struct vm_mem_region[]) {
                    {
                        .base = 0x00CB0000,
                        .size = 0x00020000,
                    }
                },
                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x00CD0000,
                        .size = 0x00020000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (unsigned[]) {52}
                    }
                },
                .dev_num = 1,
                .devs = (struct vm_dev_region[]) {
                    {
                        /* UART15 bare-metal console output */
                        .pa = 0xF8D60000,
                        .va = 0xF8D60000,
                        .size = 0x10000,
                    },
                },
                .arch = {
                    .gic = {
                        .gicd_addr = 0xF4000000,
                        .gicr_addr = 0xF4100000,
                    }
                }
            }
        }
    },
};
