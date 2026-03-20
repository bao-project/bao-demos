#include <config.h>


struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { 
            .base = 0x70000000,
            .size = 0x00010000, 
        }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            .image = VM_IMAGE_LOADED(0x28000000, 0x28000000, 50 * 1024 * 1024),

            .entry = 0x28000000,

            .platform = {
                .cpu_num = 2,

                .mmu = true,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x28000000,
                        .size = 0x40000000,
                    },
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

                .dev_num = 3,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* UART2, PL011 */
                        .pa = 0x9c0c0000,
                        .va = 0x9c0c0000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {40} 
                    },
                    {
                        /* smsc,lan91c111t */
                        .pa = 0x09a000000,
                        .va = 0x09a000000,
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
                        .gicd_addr = 0xAF000000,
                        .gicr_addr = 0xAF100000,
                    }
                }
            },
        },
        {
            .image = VM_IMAGE_LOADED(0x24000000, 0x24000000, 1 * 1024 * 1024),

            .entry = 0x24000000,

            .platform = {
                .cpu_num = 2,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x20000000,
                        .size = 0x08000000,
                    },
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
                        /* UART1, PL011 */
                        .pa = 0x9c0b0000,
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
                        .gicd_addr = 0xAF000000,
                        .gicr_addr = 0xAF100000,
                    }
                }
            },
        }
    }
};
