#include <config.h>

VM_IMAGE(linux_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux.bin))
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin))

struct config config = {

    CONFIG_HEADER

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x80200000,
            .cpu_affinity = 0x7,

            .platform = {
                .cpu_num = 3,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80200000,
                        .size = 0x20000000,
                        .place_phys = 1,
                        .phys = 0x80200000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },
                .dev_num = 8,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}
                    },
                    {
                        /* gpr */
                        .pa =   0x30340000,
                        .va =   0x30340000,
                        .size = 0x00010000,
                    },
                    {
                        /* anatop */
                        .pa =   0x30360000,
                        .va =   0x30360000,
                        .size = 0x00010000,
                    },
                    {
                        /* clk */
                        .pa = 0x30380000,
                        .va = 0x30380000,
                        .size = 0x10000,
                        .interrupt_num = 2,
                        .interrupts = (irqid_t[]) {117, 118},
                    },
                    {
                        /* gpi1 */
                        .pa = 0x30200000,
                        .va = 0x30200000,
                        .size = 0x10000,
                        .interrupt_num = 2,
                        .interrupts = (irqid_t[]) {96, 97},
                    },
                    {
                        /* iomuxc */
                        .pa = 0x30330000,
                        .va = 0x30330000,
                        .size = 0x10000,
                    },
                    {
                        /* ocotp */
                        .pa = 0x30350000,
                        .va = 0x30350000,
                        .size = 0x10000,
                    },
                    {
                        /* eqos */
                        .pa = 0x30bf0000,
                        .va = 0x30bf0000,
                        .size = 0x10000,
                        .interrupt_num = 3,
                        .interrupts = (irqid_t[]) {167, 166, 42},
                    },
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x38800000,
                       .gicr_addr = 0x38880000
                    },
                }
            },
        },
        {
            .image = {
                .base_addr = 0x0,
                .load_addr = VM_IMAGE_OFFSET(freertos_image),
                .size = VM_IMAGE_SIZE(freertos_image)
            },

            .entry = 0x0,
            .cpu_affinity = 0x8,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x8000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* uart 3 */
                        .pa = 0x30880000,
                        .va = 0xff000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {60}
                    },
                    {
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts =
                            (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicr_addr = 0xF9020000,
                    }
                }
            },
        }
    },
};
