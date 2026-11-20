#include <config.h>

VM_IMAGE(linux_backend_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_backend.bin))
VM_IMAGE(zephyr_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/zephyr.bin))

struct config config = {

    CONFIG_HEADER

    /* Shared memory used by the Remote I/O (VirtIO) devices. The first
     * region holds all the RAM of Zephyr, so the buffers of every device are
     * in it. Bao maps one region along with each device and two devices of a
     * VM cannot name the same one, so the network and block devices name
     * placeholder regions: their backends reach the first region through the
     * device tree of the backend VM */
    .shmemlist_size = 3,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x01000000, },
        [1] = { .size = 0x00001000, },
        [2] = { .size = 0x00001000, },
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            /* Linux: VirtIO backend */
            .image = {
                .base_addr = 0x80000000,
                .load_addr = VM_IMAGE_OFFSET(linux_backend_image),
                .size = VM_IMAGE_SIZE(linux_backend_image)
            },

            .entry = 0x80000000,
            .cpu_affinity = 0x1,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80000000,
                        .size = 0x40000000,
                        .place_phys = true,
                        .phys = 0x80000000,
                    },
                },

                .remio_dev_num = 3,
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
                            .size = 0x00001000,
                            .shmem_id = 1,
                        }
                    },
                    {
                        .bind_key = 2,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 42,
                        .shmem = {
                            .base = 0xD1001000,
                            .size = 0x00001000,
                            .shmem_id = 2,
                        }
                    },
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[])
                {
                    {
                        /* QEMU virtio-mmio devices (network and console) */
                        .pa = 0xa003000,
                        .va = 0xa003000,
                        .size = 0x1000,
                        .interrupt_num = 8,
                        .interrupts = (irqid_t[]) {72, 73, 74, 75, 76, 77, 78, 79}
                    },
                    {
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0x8000000,
                        .gicr_addr = 0x80A0000,
                    }
                }
            },
        },
        {
            /* Zephyr: VirtIO console frontend */
            .image = {
                .base_addr = 0x80000000,
                .load_addr = VM_IMAGE_OFFSET(zephyr_image),
                .size = VM_IMAGE_SIZE(zephyr_image)
            },

            .entry = 0x80000000,
            .cpu_affinity = 0x2,

            .platform = {
                .cpu_num = 1,

                /*
                 * Zephyr's code and read-only data live in this private
                 * region. Its RAM (data, bss, stacks and heap, including
                 * the virtqueues) is placed in the shared memory region
                 * below, so the backend can access the VirtIO buffers.
                 */
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80000000,
                        .size = 0x01000000,
                    }
                },

                .remio_dev_num = 3,
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
                    {
                        .bind_key = 1,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003c00,
                        .interrupt = 51,
                        .shmem = {
                            .base = 0xD1000000,
                            .size = 0x00001000,
                            .shmem_id = 1,
                        }
                    },
                    {
                        .bind_key = 2,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003a00,
                        .interrupt = 50,
                        .shmem = {
                            .base = 0xD1001000,
                            .size = 0x00001000,
                            .shmem_id = 2,
                        }
                    },
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* UART (PL011) */
                        .pa = 0x9000000,
                        .va = 0x9000000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {33}
                    },
                    {
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0x8000000,
                        .gicr_addr = 0x80A0000,
                    }
                }
            },
        },
    },
};
