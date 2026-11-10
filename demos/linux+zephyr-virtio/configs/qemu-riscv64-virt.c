#include <config.h>

VM_IMAGE(linux_backend_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_backend.bin))
VM_IMAGE(zephyr_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/zephyr.bin))

struct config config = {

    CONFIG_HEADER

    /* Shared memory used by the Remote I/O (VirtIO) console device */
    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x01000000, },
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            /* Linux: VirtIO backend */
            .image = {
                .base_addr = 0x90200000,
                .load_addr = VM_IMAGE_OFFSET(linux_backend_image),
                .size = VM_IMAGE_SIZE(linux_backend_image)
            },

            .entry = 0x90200000,
            .cpu_affinity = 0x1,

            .platform = {
                .cpu_num = 1,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x90000000,
                        .size = 0x40000000,
                        .place_phys = true,
                        .phys = 0x90000000,
                    },
                },

                .remio_dev_num = 1,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 0,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 36,
                        .shmem = {
                            .base = 0xD0000000,
                            .size = 0x01000000,
                            .shmem_id = 0,
                        }
                    },
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[])
                {
                    {
                        /* QEMU virtio-mmio devices (network and console) */
                        .pa = 0x10001000,
                        .va = 0x10001000,
                        .size = 0x8000,
                        .interrupt_num = 8,
                        .interrupts = (irqid_t[]) {1, 2, 3, 4, 5, 6, 7, 8}
                    },
                },

                .arch = {
                    .irqc.plic = {
                        .base = 0xC000000,
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
                        .size = 0x02000000,
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

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* UART (ns16550) */
                        .pa = 0x10000000,
                        .va = 0x10000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {10}
                    },
                },

                .arch = {
                    .irqc.plic = {
                        .base = 0xC000000,
                    }
                }
            },
        },
    },
};
