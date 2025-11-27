#include <config.h>

VM_IMAGE(linux_backend_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_backend.bin))
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin))
VM_IMAGE(linux_frontend1_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_frontend1.bin))
VM_IMAGE(linux_frontend2_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_frontend2.bin))

struct config config = {

    CONFIG_HEADER

    .shmemlist_size = 5,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x01000000, },
        [1] = { .size = 0x01000000, },
        [2] = { .size = 0x01000000, },
        [3] = { .size = 0x01000000, },
        [4] = { .size = 0x01000000, },
    },

    .vmlist_size = 4,
    .vmlist = (struct vm_config[]) {
        {
            .image = {
                .base_addr = 0x40000000,
                .load_addr = VM_IMAGE_OFFSET(linux_backend_image),
                .size = VM_IMAGE_SIZE(linux_backend_image)
            },

            .entry = 0x40000000,
            .cpu_affinity = 0x1,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x40000000,
                        .size = 0x40000000,
                        .place_phys = true,
                        .phys = 0x40000000,
                    },
                },

                .remio_dev_num = 5,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 0,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 40,
                        .shmem = {
                            .base = 0xD0000000,
                            .size = 0x01000000,
                            .shmem_id = 0,
                        }
                    },
                    {
                        .bind_key = 1,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 41,
                        .shmem = {
                            .base = 0xD1000000,
                            .size = 0x01000000,
                            .shmem_id = 1,
                        }
                    },
                    {
                        .bind_key = 2,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 42,
                        .shmem = {
                            .base = 0xD2000000,
                            .size = 0x01000000,
                            .shmem_id = 2,
                        }
                    },
                    {
                        .bind_key = 3,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 43,
                        .shmem = {
                            .base = 0xD3000000,
                            .size = 0x01000000,
                            .shmem_id = 3,
                        }
                    },
                    {
                        .bind_key = 4,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 44,
                        .shmem = {
                            .base = 0xD4000000,
                            .size = 0x01000000,
                            .shmem_id = 4,
                        }
                    }
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[])
                {
                    {
                        /* GENET */
                        .pa = 0xfd580000,
                        .va = 0xfd580000,
                        .size = 0x10000,
                        .interrupt_num = 2,
                        .interrupts = (irqid_t[]) {189, 190}
                    },
                    {
                        .interrupt_num = 1,
                        .interrupts =
                            (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xff841000,
                        .gicc_addr = 0xff842000,
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

            .cpu_affinity = 0x2,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x8000000
                    }
                },

                .remio_dev_num = 1,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 0,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003e00,
                        .interrupt = 52,
                        .shmem = {
                            .base = 0xD0000000,
                            .size = 0x01000000,
                            .shmem_id = 0,
                        }
                    },
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* UART1 */
                        .pa = 0xfe215000,
                        .va = 0xff000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {125}
                    },
                    {
                        .interrupt_num = 1,
                        .interrupts =
                            (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicc_addr = 0xF9020000,
                    }
                }
            },
        },
        {
            .image = {
                .base_addr = 0x20000000,
                .load_addr = VM_IMAGE_OFFSET(linux_frontend1_image),
                .size = VM_IMAGE_SIZE(linux_frontend1_image)
            },

            .entry = 0x20000000,
            .cpu_affinity = 0x4,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x20000000,
                        .size = 0x40000000,
                    },
                },

                .remio_dev_num = 2,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 1,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003e00,
                        .interrupt = 79,
                        .shmem = {
                            .base = 0xD1000000,
                            .size = 0x01000000,
                            .shmem_id = 1,
                        }
                    },
                    {
                        .bind_key = 2,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003c00,
                        .interrupt = 78,
                        .shmem = {
                            .base = 0xD2000000,
                            .size = 0x01000000,
                            .shmem_id = 2,
                        }
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        .interrupt_num = 1,
                        .interrupts =
                            (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xff841000,
                        .gicc_addr = 0xff842000,
                    }
                }
            },
        },
        {
            .image = {
                .base_addr = 0x20000000,
                .load_addr = VM_IMAGE_OFFSET(linux_frontend2_image),
                .size = VM_IMAGE_SIZE(linux_frontend2_image)
            },

            .entry = 0x20000000,
            .cpu_affinity = 0x8,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x20000000,
                        .size = 0x40000000,
                    },
                },

                .remio_dev_num = 2,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 3,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003e00,
                        .interrupt = 79,
                        .shmem = {
                            .base = 0xD3000000,
                            .size = 0x01000000,
                            .shmem_id = 3,
                        }
                    },
                    {
                        .bind_key = 4,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003c00,
                        .interrupt = 78,
                        .shmem = {
                            .base = 0xD4000000,
                            .size = 0x01000000,
                            .shmem_id = 4,
                        }
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        .interrupt_num = 1,
                        .interrupts =
                            (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xff841000,
                        .gicc_addr = 0xff842000,
                    }
                }
            },
        },
    },
};
