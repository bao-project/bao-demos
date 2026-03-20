#include <config.h>

VM_IMAGE(linux_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux.bin))
VM_IMAGE(zephyr_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/zephyr.bin))

struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        { 
            .image = {
                .base_addr = 0xa0000000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image),
            },

            .entry = 0xa0000000,

            .platform = {
                .cpu_num = 3,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0xa0000000,
                        .size = 0x40000000,
                        .place_phys = true,
                        .phys = 0xa0000000,
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

                .dev_num = 3,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* UART1, PL011 */
                        .pa = 0x1c0a0000,
                        .va = 0x1c0a0000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {38} 
                    },
                    {   
                        /* smsc,lan91c111t */
                        .pa = 0x01a000000,
                        .va = 0x01a000000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {47} 
                    },
                    {   
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27} 
                    }
                },

                .arch = {
                    .gic = {
                        .gicr_addr = 0x2F100000,
                        .gicd_addr = 0x2F000000
                    }
                }
            },
        },
        { 
            .image = {
                .base_addr = 0x20000000,
                .load_addr = VM_IMAGE_OFFSET(zephyr_image),
                .size = VM_IMAGE_SIZE(zephyr_image)
            },

            .entry = 0x20000000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x20000000,
                        .size = 0x8000000
                    }
                },


                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x70000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* UART2, PL011 */
                        .pa = 0x1c0b0000,
                        .va = 0x9c0b0000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {39} 
                    },
                    {   
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27} 
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xaf000000,
                        .gicr_addr = 0xaf100000,
                    }
                }
            },
        },
    },
};
