#include <config.h>

VM_IMAGE(linux_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/u-boot.bin))
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin))

struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            .image = {
                .base_addr = 0x40200000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x40200000,

            .platform = {
                .cpu_num = 3,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x40000000,
                        .size = 0x40000000,
                        .place_phys = 1,
                        .phys = 0x40000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        // This interrupt was used for uart, since we are no
                        // longer using we use it here
                        .interrupts = (irqid_t[]) {60}
                    }
                },
                .dev_num = 6,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}
                    },
                    {
                        .pa =   0x00000000,
                        .va =   0x00000000,
                        .size = 0x30880000,
                    },
                    /* Hole for UART3 */
                    {
                        .pa =   0x30881000,
                        .va =   0x30881000,
                        .size = 0x38800000 - 0x30881000,
                    },
                    /* Hole for gic */
                    {
                        .pa =   0x38900000,
                        .va =   0x38900000,
                        .size = 0x40000000 - 0x38900000,
                    },
                    {
                        .interrupt_num = 158,
                        .interrupts = (irqid_t[]) {32, 33, 34, 35, 36, 37, 38,
                            39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
                            52, 53, 54, 55, 56, 57, 58, 59, /*60,*/ 61, 62, 63, 64,
                            65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
                            78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
                            91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102,
                            103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
                            113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
                            123, 124, 125, 126, 127, 128, 129, 130, 131, 132,
                            133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
                            143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
                            153, 154, 155, 156, 157, 158, 159, 160, 161, 162,
                            163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
                            173, 174, 175, 176, 177, 178, 179, 180, 181, 182,
                            183, 184, 185, 186, 187, 188, 189, 190, 191},
                    },
                    {
                        /* OCRAM */
                        .pa = 0x900000,
                        .va = 0x900000,
                        .size = 0x70000
                    }
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x38800000,
                       .gicr_addr = 0x38880000
                    },
                    .smc_passthrough = {
                        .num = 6,
                        .smcs = (size_t[]) {0xc2000000, 0xc2000004, 0xc2000005, 0x80000000, 0x80000001, 0x84000050}
                    }
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
        },
    },
};


