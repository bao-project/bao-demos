#include <config.h>

VM_IMAGE(linux1, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux1.bin));
VM_IMAGE(linux2, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux2.bin));


struct config config = {
    
    CONFIG_HEADER

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },
    
    .vmlist_size = 2,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(linux1),
                .size = VM_IMAGE_SIZE(linux1)
            },

            .entry = 0x80200000,
            .cpu_affinity = 0b000001,

            .platform = {
                .cpu_num = 1,   
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x80000000,
                        .size = 0x40000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x60000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {184,185}
                    }
                },

                .dev_num = 5,
                .devs =  (struct dev_region[]) {    
                    {   
                        /* lpuart0 */
                        .pa = 0x5a060000,
                        .va = 0x5a060000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {377}                           
                    },
                    {   
                        /* lpuart0 clock generator */
                        .pa = 0x5a460000,
                        .va = 0x5a460000,
                        .size = 0x10000                        
                    },
                    {   
                        /* edma0 channels 12 and 13 */
                        .pa = 0x5a2c0000,
                        .va = 0x5a2c0000,
                        .size = 0x20000,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {466, 467},    
                        /**
                         * this streamid must be configured in atf or uboot 
                         * through a call to scfw
                         */
                        .id = 0x1,
                    },
                    {
                        /* message unit 1 */
                        .pa = 0x5d1c0000,
                        .va = 0x5d1c0000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {209},    
                    },
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {27}                         
                    }
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x51a00000,
                       .gicr_addr = 0x51b00000
                    },
                },
            },
        },
        { 
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(linux2),
                .size = VM_IMAGE_SIZE(linux2)
            },

            .entry = 0x80200000,
            .cpu_affinity = 0b111110,

            .platform = {
                .cpu_num = 5,   
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x80000000,
                        .size = 0x40000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x60000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {184,185}
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {    
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {27}                         
                    },
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x51a00000,
                       .gicr_addr = 0x51b00000
                    },
                },   
            },
        }
    },
};
